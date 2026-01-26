#ifndef ERAY_CAMERKA_H_
#define ERAY_CAMERKA_H_
#include "lamath.h"

typedef struct {
    float yaw;   // azimuth (left/right)
    float pitch; // polar   (up/down)
    vecf3 camera_direction_hmm;
    vecf3 pos;
} Camerka;

matf4x4 camerka_view_matrix(const Camerka* camerka);
vecf3 camerka_direction(const Camerka* camerka);

#endif /* ERAY_CAMERKA_H_ */
