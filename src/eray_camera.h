#ifndef ERAY_CAMERA_H_
#define ERAY_CAMERA_H_

#define ETER_VEC_STRUCT
#include "eter_math.h"

typedef struct {
    fvec3 position;
    float focal_length;

    //float field_of_view;
    //float aspect_ratio;
    //float angle_of_view;
} eCamera;

void camera_set_pos(eCamera* camera, fvec3 pos);
void camera_set_focal_length(eCamera* camera, float focal_lenth);


#endif /* ETER_CAMERA_H_ */
