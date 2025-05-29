#ifndef METEO_MATH_H
#define METEO_MATH_H
float fahrenheit(float t_c);
float max_presure(float t_c);
float density_from_presure(float p, float t_c);
float max_density(float t_c);
float absolute_humidity(float rel, float t_c);
#endif