#include "PidRegulator.h"
#include "Arduino.h"

static const char* TAG = "PID";

PidRegulator::PidRegulator(PidRegulator::PidConfig &new_config) {
    setConfig(new_config);
}


PidRegulator::PidRegulator(double p, double i, double d) {
    setConfig(p, i, d);
}


void PidRegulator::setConfig(PidConfig &new_config) {
    m_config = new_config;
}


void PidRegulator::setConfig(double p, double i, double d) {
    PidConfig new_config{
        .p = p,
        .i = i,
        .d = d
    };

    setConfig(new_config);
}


PidRegulator::PidConfig PidRegulator::getConfig() {
    return m_config;
}


void PidRegulator::reset() {
    m_last_timestamp = millis();
    m_cur_timestamp = m_last_timestamp;

    m_cur_error = 0.0;
    m_last_error = 0.0;

    m_integral = 0.0;
    m_dt = 0.0;
}


PidRegulator::PidOutput PidRegulator::apply(double error) {
    m_last_timestamp = m_cur_timestamp;
    m_cur_timestamp = millis();                  
    m_dt = (m_cur_timestamp - m_last_timestamp);
    m_last_error = m_cur_error;
    m_cur_error = error;

    const double p = calc_p_part();
    const double i = calc_i_part();
    const double d = calc_d_part();

    return PidOutput(p, i, d);
}


double PidRegulator::calc_p_part() {
    return m_config.p * m_cur_error;
}


double PidRegulator::calc_i_part() {
    static const double max_integral = 50.0;
    if (m_integral < max_integral) {
        m_integral += (m_cur_error * m_dt) * 0.0001; 
    }

    m_integral = constrain(m_integral, -max_integral, max_integral);
    return m_config.i * m_integral;
}


double PidRegulator::calc_d_part() {
    double differential_part = (m_config.d * 100.0) * (m_cur_error - m_last_error) / m_dt;
    return differential_part;
}
