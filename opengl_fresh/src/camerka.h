#ifndef ERAY_CAMERKA_H_
#define ERAY_CAMERKA_H_
#include "lamath.h"

typedef struct {
    float yaw;   // azimuth (left/right)
    float pitch; // polar   (up/down)
    vecf3 orientation;
    vecf3 pos;
} Camerka;

#define camerka_set_pos(camerka, pos) do { (camerka).pos = (pos); } while (0)
#define camerka_set_orientation(camerka, orientation) do { (camerka).orientation = (orientation); } while (0)

matf4x4 camerka_view_matrix(const Camerka* camerka);
vecf3 camerka_orientation(const Camerka* camerka);

#endif /* ERAY_CAMERKA_H_ */
