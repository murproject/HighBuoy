#include <stdint.h>

class PidRegulator {
public:
    struct PidConfig {
        double p = 0.0;
        double i = 0.0;
        double d = 0.0;
    };

    struct PidOutput {
        double p_part;
        double i_part;
        double d_part;

        PidOutput(double p, double i, double d) : p_part(p), i_part(i), d_part(d) {}
    };

    PidRegulator(PidConfig &new_config);
    PidRegulator(double p, double i, double d);

    void setConfig(PidConfig &new_config);
    void setConfig(double p, double i, double d);
    PidConfig getConfig();

    void reset();

    PidOutput apply(double error);

private:
    PidConfig m_config;

    double calc_p_part();
    double calc_i_part();
    double calc_d_part();

    int64_t m_last_timestamp = 0;
    int64_t m_cur_timestamp = 0;

    double m_last_error = 0.0;
    double m_cur_error = 0.0;

    double m_integral = 0.0;
    double m_dt = 0.0;
};
