#include "eray_camera.h"
#include "eter_debug.h"

void camera_set_pos(eCamera* camera, fvec3 pos)
{
    debug_assert(camera, "Camer null");
    camera->position.x = pos.x;
    camera->position.y = pos.y;
    camera->position.z = pos.z;
}

void camera_set_focal_length(eCamera* camera, float focal_lenth)
{
    debug_assert(camera, "Camer null");
    camera->focal_length = focal_lenth;
}

// void camera_calulcate_pixel_ndc(eCamera* camera, int rasterX, int rasterY, int raster_width, int raster_height)
// {
//     debug_assert(camera, "Camer null");
//     float pixel_ndc_x;
//     float pixel_ndc_y;
//     float pixel_screen_space_x;
//     float pixel_screen_space_y;
//
//     // Normalized NDC
//     pixel_ndc_x = (rasterX + 0.5f) / raster_width; 
//     pixel_ndc_y = (rasterY + 0.5f) / raster_height; 
//
//     pixel_screen_space_x = (2*pixel_ndc_x-1)*camera->aspect_ratio;
//     pixel_screen_space_y = 1-(2*pixel_ndc_y); // flip Y axis so that y goes up.
//
//     // [TODO] add more vectors to my lib. 
//
//     debug_log_format("[%f, %f]\n", pixel_screen_space_x, pixel_screen_space_y);
// }


