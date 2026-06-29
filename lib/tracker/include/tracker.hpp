/**
 * Tracker.hpp
 *
 * Classe de solarimetria para tracker solar de dois eixos.
 *
 * Convenções:
 *   beta  (inclinação) : 90 = placa horizontal, faixa [30, 150]
 *   gamma (azimute)    : Norte=0, Oeste=90, Sul=180, faixa [0, 180]
 *
 * Uso:
 *   Tracker tracker(latitude, longitude);
 *   tracker.atualizar(ano, mes, dia, hora, minuto, segundo); // tempo em UTC
 *   servo_beta.write(tracker.getBeta());
 *   servo_gamma.write(tracker.getGamma());
 */

#pragma once

#include <ctime>
#include <rtc.hpp> 

class Tracker {
private:
    double _latitude;
    double _longitude;

    int  _beta;
    int  _gamma;
    bool _sol_visivel;

    // Constantes internas
    static constexpr double PI            = 3.14159265358979323846;
    static constexpr double ELEV_MINIMA   = -0.833; // compensação de refração

    // Helpers
    static double toRad(double graus) { return graus * PI / 180.0; }
    static double toDeg(double rad)   { return rad * 180.0 / PI;   }
    static double clamp(double v, double lo, double hi);

    struct PosicaoSolar {
        double elevacao;
        double azimute;
        bool   acima_horizonte;
    };

    double       diasDesdeJ2000(const struct tm& utc) const;
    PosicaoSolar calcPosicao(const struct tm& utc)    const;
    int          calcBeta (double elevacao, bool flip) const;
    int          calcGamma(double azimute, bool& flip) const;

public:
    // Limites dos servos
    static constexpr int BETA_MIN  = 30;
    static constexpr int BETA_MAX  = 150;
    static constexpr int GAMMA_MIN = 0;
    static constexpr int GAMMA_MAX = 180;

    // Posição de repouso (sol abaixo do horizonte)
    static constexpr int BETA_REPOUSO  = 90;
    static constexpr int GAMMA_REPOUSO = 90;

    /**
     * @param latitude   Latitude em graus decimais  (+Norte / −Sul)
     * @param longitude  Longitude em graus decimais (+Leste / −Oeste)
     */
    Tracker(double latitude, double longitude);

    /**
     * Atualiza o tempo e recalcula beta e gamma.
     * Deve ser chamado a cada ciclo de controle (ex: a cada minuto).
     * O tempo deve ser fornecido em UTC.
     */
    void atualizar(const struct tempo &t);

    /** Retorna o ângulo de inclinação para o servo [30, 150]. */
    int getBeta()  const { return _beta;  }

    /** Retorna o ângulo de orientação para o servo [0, 180]. */
    int getGamma() const { return _gamma; }

    /** Retorna true se o sol está acima do horizonte. */
    bool solVisivel() const { return _sol_visivel; }
};
