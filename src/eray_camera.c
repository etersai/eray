#include "eray_camera.h"
#include "eter_debug.h"

void camera_set_pos(eCamera* camera, fvec3 pos)
{
    debug_assert(camera, "WELCOMING!");
    camera->position.x = pos.x;
    camera->position.y = pos.y;
    camera->position.z = pos.z;
}

void camera_set_aspect_ratio(eCamera* camera, float aspect_ratio)
{
    assert(camera && "[TODO]"); 
    camera->aspect_ratio = aspect_ratio;
}

void camera_calulcate_pixel_ndc(eCamera* camera, int rasterX, int rasterY, int raster_width, int raster_height)
{
    assert(camera && "[TODO]");
    float pixel_ndc_x;
    float pixel_ndc_y;
    float pixel_screen_space_x;
    float pixel_screen_space_y;
    
    // Normalized NDC
    pixel_ndc_x = (rasterX + 0.5f) / raster_width; 
    pixel_ndc_y = (rasterY + 0.5f) / raster_height; 

    pixel_screen_space_x = (2*pixel_ndc_x-1)*camera->aspect_ratio;
    pixel_screen_space_y = 1-(2*pixel_ndc_y); // flip Y axis so that y goes up.

    // [TODO] add more vectors to my lib. 
    
    debug_log_format("[%f, %f]\n", pixel_screen_space_x, pixel_screen_space_y);
}


