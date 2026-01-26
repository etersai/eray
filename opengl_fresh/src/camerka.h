#ifndef ERAY_CAMERKA_H_
#define ERAY_CAMERKA_H_
#include "lamath.h"

typedef struct {
    float yaw;   // azimuth (left/right)
    float pitch; // polar   (up/down)
    vecf3 pos;
} Camerka;

#endif /* ERAY_CAMERKA_H_ */
