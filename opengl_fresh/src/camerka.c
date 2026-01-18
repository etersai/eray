#include <assert.h>
typedef struct {
    vecf3 camera_pos_from;
    vecf3 camera_aim_to;
} Camerka;

static inline void camerka_update_pos(Camerka* cam, vecf3 vec)
{
    assert(cam);
    cam->camera_pos_from = vec;
}

static inline void camerka_update_aim(Camerka* cam, vecf3 vec)
{
    assert(cam);
    cam->camera_aim_to = vec;
}
