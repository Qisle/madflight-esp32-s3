/*==========================================================================================
alt_kalman3.h - madflight altitude estimator interface

MIT License

Copyright (c) 2025 https://madflight.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
===========================================================================================*/
#pragma once

#include "KalmanFilter.h"

class AltEst_Kalman3 : public AltEst {
protected:
  KalmanFilter filter;
  uint32_t tsa = 0;
  uint32_t tsb = 0;
  float bar_origin = 0;
  float bar_lpf = 0;
  bool bar_origin_valid = false;
  bool bar_lpf_valid = false;
  bool bar_correction_valid = false;

public:
  void setup(float alt) {
    //default parameters (see usage.txt)
    float altCov = 0.2;  //measured stdev BME280 = 0.4 [m] -> cov = 0.4*0.4 = 0.16
    float accCov = 0.01; //measured stdev MPU6500 @ 16G = 0.003 [G] = 0.03 [m/s2] -> cov = 0.03*0.03 = 0.0009
    float biasCov = 0.1; //estimated 3% * 9.81 = 0.3 [m/s2]  -> cov = 0.3*0.3 = 0.09
    float pBiasCov = 0.0004f; //baro bias random walk: 0.02 [m]/sqrt(s)

    Serial.printf("ALT: KALMAN3  altCov=%f accCov=%f biasCov=%f pBiasCov=%f\n", altCov, accCov, biasCov, pBiasCov);

    filter.setup(altCov, accCov, biasCov, pBiasCov);
    (void)alt;
    filter.h = 0;
    filter.v = 0;
    filter.bias = 0;
    filter.p_bias = 0;

    tsa = 0;
    tsb = 0;
    bar_origin = 0;
    bar_lpf = 0;
    bar_origin_valid = false;
    bar_lpf_valid = false;
    bar_correction_valid = false;
  }

  //a: accel up in [m/s^2], ts: timestamp in [us]
  void updateAccelUp(float a, uint32_t ts) {
    if(tsa!=0) {
      float dt = 1e-6 * (ts - tsa);
      filter.propagate(a, dt);
    }
    tsa = ts;
  };
  
  //altitude: barometric altitude in [m], ts: timestamp in [us]
  void updateBarAlt(float alt, uint32_t ts) {
    updateBarAlt(alt, ts, 0.0f, 0.0f, 0.0f);
  }

  void updateBarAlt(float alt, uint32_t ts, float collective, float hover, float propwash_comp_per_throttle) {
    const float bar_lpf_hz = 4.0f;
    const float gate_chi2_1d_95 = 3.84f;
    const float collective_safe = constrain(collective, 0.0f, 1.0f);
    const float propwash_comp = (collective_safe > 0.001f)
      ? propwash_comp_per_throttle * (collective_safe - constrain(hover, 0.0f, 1.0f))
      : 0.0f;
    const float baro_corrected = alt - propwash_comp;
    if(!bar_origin_valid) {
      bar_origin = baro_corrected;
      bar_origin_valid = true;
    }
    const float rel_alt = baro_corrected - bar_origin;

    float dt = 0;
    if(tsb != 0) dt = 1e-6f * (ts - tsb);
    tsb = ts;

    if(!bar_lpf_valid || dt <= 0 || dt > 0.5f) {
      bar_lpf = rel_alt;
      bar_lpf_valid = true;
    }else{
      const float rc = 1.0f / (2.0f * PI * bar_lpf_hz);
      float beta = dt / (rc + dt);
      beta = constrain(beta, 0.0f, 1.0f);
      bar_lpf += beta * (rel_alt - bar_lpf);
    }

    if(!bar_correction_valid) {
      filter.h = bar_lpf;
      filter.v = 0;
      filter.p_bias = 0;
      bar_correction_valid = true;
      return;
    }

    const float innovation = filter.innovation(bar_lpf);
    const float innovation_var = filter.innovationVariance();
    if(innovation_var > 0.0f && innovation * innovation <= gate_chi2_1d_95 * innovation_var) {
      filter.update(bar_lpf);
    }
  }
  
  float getH() {return filter.h;} //altitude estimate in [m]
  float getV() {return filter.v;} //vertical up speed (climb rate) estimate in [m/s]

  void toString(char *s) {
    int n = 0;
    n += sprintf(s+n, "alt.h:%.2f\t", filter.h);
    n += sprintf(s+n, "alt.v:%+.2f\t", filter.v);
    n += sprintf(s+n, "alt.abias:%+.2f\t", filter.bias);
    n += sprintf(s+n, "alt.pbias:%+.2f\t", filter.p_bias);
  }
};
