#include "KalmanFilter.h"
#include <math.h>

// Tracks vertical position, velocity, accelerometer bias, and a slow barometer
// altitude bias. Barometer observation model: z = h + p_bias + noise.

namespace {

enum {
  IDX_H = 0,
  IDX_V = 1,
  IDX_ABIAS = 2,
  IDX_PBIAS = 3,
  STATE_DIM = 4
};

float clampf_local(float v, float lo, float hi) {
  if(v < lo) return lo;
  if(v > hi) return hi;
  return v;
}

} // namespace

void KalmanFilter::setup(float altCov, float accCov, float biasCov, float pBiasCov) {
  this->altCov = altCov;
  this->accCov = accCov;
  this->biasCov = biasCov;
  this->pBiasCov = pBiasCov;

  h = 0;
  v = 0;
  bias = 0;
  p_bias = 0;

  for(int r = 0; r < STATE_DIM; r++) {
    for(int c = 0; c < STATE_DIM; c++) {
      P_[r][c] = 0.0f;
    }
  }

  P_[IDX_H][IDX_H] = 1.0f;
  P_[IDX_V][IDX_V] = 1.0f;
  P_[IDX_ABIAS][IDX_ABIAS] = 100000.0f;
  P_[IDX_PBIAS][IDX_PBIAS] = 0.25f;
}

void KalmanFilter::propagate(float a, float dt) {
  // Predict state
  float accel = a - bias;
  h += v * dt + 0.5f * accel * dt * dt;
  v += accel * dt;

  accCov = clampf_local(fabsf(accel) / 50.0f, 0.01f, 0.50f);

  // Predict State Covariance matrix
  float dt2div2 = dt*dt/2.0f;
  float dt3div2 = dt2div2*dt;
  float dt4div4 = dt2div2*dt2div2;
  float F[STATE_DIM][STATE_DIM] = {
    {1.0f, dt, -dt2div2, 0.0f},
    {0.0f, 1.0f, -dt, 0.0f},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f}
  };
  float FP[STATE_DIM][STATE_DIM] = {};
  float nextP[STATE_DIM][STATE_DIM] = {};

  for(int r = 0; r < STATE_DIM; r++) {
    for(int c = 0; c < STATE_DIM; c++) {
      for(int k = 0; k < STATE_DIM; k++) {
        FP[r][c] += F[r][k] * P_[k][c];
      }
    }
  }
  for(int r = 0; r < STATE_DIM; r++) {
    for(int c = 0; c < STATE_DIM; c++) {
      for(int k = 0; k < STATE_DIM; k++) {
        nextP[r][c] += FP[r][k] * F[c][k];
      }
    }
  }

  for(int r = 0; r < STATE_DIM; r++) {
    for(int c = 0; c < STATE_DIM; c++) {
      P_[r][c] = nextP[r][c];
    }
  }

  P_[IDX_H][IDX_H] += dt4div4*accCov;
  P_[IDX_H][IDX_V] += dt3div2*accCov;
  P_[IDX_V][IDX_H] += dt3div2*accCov;
  P_[IDX_V][IDX_V] += dt*dt*accCov;
  P_[IDX_ABIAS][IDX_ABIAS] += biasCov * dt;
  P_[IDX_PBIAS][IDX_PBIAS] += pBiasCov * dt;
}

float KalmanFilter::innovation(float z) const {
  return z - h - p_bias;
}

float KalmanFilter::innovationVariance() const {
  return P_[IDX_H][IDX_H]
    + P_[IDX_H][IDX_PBIAS]
    + P_[IDX_PBIAS][IDX_H]
    + P_[IDX_PBIAS][IDX_PBIAS]
    + altCov;
}

void KalmanFilter::update(float z) {
  // Error
  float innov = innovation(z);
  float sInv = 1.0f / innovationVariance();

  // Kalman gains
  float K[STATE_DIM] = {};
  for(int r = 0; r < STATE_DIM; r++) {
    K[r] = (P_[r][IDX_H] + P_[r][IDX_PBIAS]) * sInv;
  }

  // Update state
  h += K[IDX_H] * innov;
  v += K[IDX_V] * innov;
  bias += K[IDX_ABIAS] * innov;
  p_bias += K[IDX_PBIAS] * innov;

  // Update state covariance matrix
  float HP[STATE_DIM] = {};
  for(int c = 0; c < STATE_DIM; c++) {
    HP[c] = P_[IDX_H][c] + P_[IDX_PBIAS][c];
  }
  for(int r = 0; r < STATE_DIM; r++) {
    for(int c = 0; c < STATE_DIM; c++) {
      P_[r][c] -= K[r] * HP[c];
    }
  }
}
