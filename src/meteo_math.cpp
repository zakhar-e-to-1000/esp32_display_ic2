#include "meteo_math.h"

#include <math.h>
float fahrenheit(float t_c) { return (t_c * 1.8) + 32; }

float max_presure(float t_c) {  // Tenes formula in kilopascals
  return 0.61078 * exp((17.21 * t_c) / (t_c + 237.3));
}

float density_from_presure(float p, float t_c) {
  float M = 18.01;
  float t_k = t_c + 273.15;
  float R = 8.31;
  return 1000 * (M * p) / (R * t_k);
}

float max_density(float t_c) {
  float p = max_presure(t_c);
  float M = 18.01;
  float t_k = t_c + 273.15;
  float R = 8.31;
  return 1000 * (M * p) / (R * t_k);
}

float absolute_humidity(float rel, float t_c) { return rel * max_density(t_c); }

float mm_hg(float pa) { return pa / 133; }
