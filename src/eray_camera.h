#ifndef ERAY_CAMERA_H_
#define ERAY_CAMERA_H_

#define ETER_VEC_STRUCT
#include "eter_math.h"

typedef struct {
    fvec3 position;
    
    float field_of_view;
    float aspect_ratio;
    float focal_length; 
    float angle_of_view;
} eCamera;

void camera_set_pos(eCamera* camera, fvec3 pos);
void camera_set_aspect_ratio(eCamera* camera, float aspect_ratio);
void camera_calulcate_pixel_ndc(eCamera* camera, int rasterX, int rasterY, int raster_width, int raster_height);


#endif /* ETER_CAMERA_H_ */
