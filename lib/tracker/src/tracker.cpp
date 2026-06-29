/**
 * Tracker.cpp
 *
 * Implementação da classe Tracker para solarimetria de dois eixos.
 *
 * Algoritmo: Jean Meeus, "Astronomical Algorithms" (2ª ed.)
 * Precisão: < 0,5° para 2000–2050.
 */

#include "tracker.hpp"
#include <cmath>

// -----------------------------------------------------------------------
// Construtor
// -----------------------------------------------------------------------

Tracker::Tracker(double latitude, double longitude)
    : _latitude(latitude)
    , _longitude(longitude)
    , _beta(BETA_REPOUSO)
    , _gamma(GAMMA_REPOUSO)
    , _sol_visivel(false)
{}

// -----------------------------------------------------------------------
// API pública
// -----------------------------------------------------------------------

void Tracker::atualizar(const struct tempo &t)
{
    struct tm utc = {};
    utc.tm_year = t.year - 1900;
    utc.tm_mon  = t.month - 1;      // tm_mon: 0–11
    utc.tm_mday = t.mday;
    utc.tm_hour = t.hour;
    utc.tm_min  = t.min;
    utc.tm_sec  = t.sec;

    PosicaoSolar pos = calcPosicao(utc);
    _sol_visivel = pos.acima_horizonte;

    if (!pos.acima_horizonte) {
        _beta  = BETA_REPOUSO;
        _gamma = GAMMA_REPOUSO;
        return;
    }

    bool flip = false;
    _gamma = calcGamma(pos.azimute, flip);
    _beta  = calcBeta (pos.elevacao, flip);
}

// -----------------------------------------------------------------------
// Helpers privados
// -----------------------------------------------------------------------

double Tracker::clamp(double v, double lo, double hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

/**
 * Dias julianos desde J2000.0.
 * Fórmula de Meeus, cap. 7 – válida para 1901–2099.
 */
double Tracker::diasDesdeJ2000(const struct tm& t) const
{
    int Y = t.tm_year + 1900;
    int M = t.tm_mon  + 1;
    int D = t.tm_mday;
    double UT = t.tm_hour + t.tm_min / 60.0 + t.tm_sec / 3600.0;

    if (M <= 2) { Y -= 1; M += 12; }
    int A = Y / 100;
    int B = 2 - A + A / 4;
    double JD = static_cast<int>(365.25 * (Y + 4716))
              + static_cast<int>(30.6001 * (M + 1))
              + D + B - 1524.5
              + UT / 24.0;
    return JD - 2451545.0;
}

/**
 * Calcula elevação e azimute solar.
 * Azimute: 0=Norte, 90=Leste, 180=Sul, 270=Oeste.
 */
Tracker::PosicaoSolar Tracker::calcPosicao(const struct tm& utc) const
{
    const double jd = diasDesdeJ2000(utc);
    const double T  = jd / 36525.0;  // séculos julianos

    // Longitude eclíptica solar
    const double L0    = std::fmod(280.46646 + 36000.76983 * T, 360.0);
    const double M     = std::fmod(357.52911 + 35999.05029 * T, 360.0);
    const double M_rad = toRad(M);

    const double C = (1.914602 - 0.004817 * T - 0.000014 * T * T) * std::sin(M_rad)
                   + (0.019993 - 0.000101 * T) * std::sin(2.0 * M_rad)
                   +  0.000289                  * std::sin(3.0 * M_rad);

    const double lon_sol = L0 + C;
    const double lon_rad = toRad(lon_sol);

    // Obliquidade da eclíptica
    const double eps_rad = toRad(23.439291 - 0.013004 * T);

    // Ascensão reta e declinação
    double asc_reta = toDeg(std::atan2(std::cos(eps_rad) * std::sin(lon_rad),
                                        std::cos(lon_rad)));
    if (asc_reta < 0.0) asc_reta += 360.0;

    const double dec_rad = std::asin(std::sin(eps_rad) * std::sin(lon_rad));

    // Tempo Sideral de Greenwich → ângulo horário local
    double GMST = 280.46061837 + 360.98564736629 * jd + 0.000387933 * T * T;
    GMST = std::fmod(GMST, 360.0);
    if (GMST < 0.0) GMST += 360.0;

    double LMST = std::fmod(GMST + _longitude, 360.0);
    if (LMST < 0.0) LMST += 360.0;

    double H = LMST - asc_reta;
    if (H < -180.0) H += 360.0;
    if (H >  180.0) H -= 360.0;

    const double H_rad   = toRad(H);
    const double lat_rad = toRad(_latitude);

    // Elevação
    const double sin_elev = std::sin(lat_rad) * std::sin(dec_rad)
                           + std::cos(lat_rad) * std::cos(dec_rad) * std::cos(H_rad);
    double elev = toDeg(std::asin(sin_elev));

    // Correção de refração atmosférica de Bennet
    if (elev > -1.5 && elev < 85.0) {
        const double refr = 1.02 / std::tan(toRad(elev + 10.3 / (elev + 5.11)));
        elev += refr / 60.0;
    }

    // Azimute geográfico (0=Norte, 90=Leste, 180=Sul, 270=Oeste)
    double cos_az = (std::sin(dec_rad) - std::sin(lat_rad) * sin_elev)
                  / (std::cos(lat_rad) * std::cos(toRad(elev)));
    cos_az = clamp(cos_az, -1.0, 1.0);

    double az = toDeg(std::acos(cos_az));
    if (H > 0.0) az = 360.0 - az; // sol no Oeste → az > 180°

    return { elev, az, elev >= ELEV_MINIMA };
}

/**
 * Calcula gamma (orientação, 0=Norte, 90=Oeste, 180=Sul).
 *
 * Sol no Oeste (az 180°–360°): gamma = 360° − az_solar, sem flip.
 * Sol no Leste (az   0°–180°): gamma = az_solar,         com flip.
 */
int Tracker::calcGamma(double azimute, bool& flip) const
{
    double gamma;
    if (azimute >= 180.0) {
        gamma = 360.0 - azimute;
        flip  = false;
    } else {
        gamma = azimute;
        flip  = true;
    }
    return static_cast<int>(std::round(clamp(gamma, GAMMA_MIN, GAMMA_MAX)));
}

/**
 * Calcula beta (inclinação, 90=horizontal).
 *
 * Sem flip: beta = 90° − elevação  (sol no Oeste, lado normal)
 * Com flip: beta = 90° + elevação  (sol no Leste, placa espelhada)
 */
int Tracker::calcBeta(double elevacao, bool flip) const
{
    const double beta = flip ? 90.0 + elevacao
                             : 90.0 - elevacao;
    return static_cast<int>(std::round(clamp(beta, BETA_MIN, BETA_MAX)));
}
