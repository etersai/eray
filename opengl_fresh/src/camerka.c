#include <assert.h>
typedef struct {
    vecf3 pos;
    vecf3 target;
} Camerka;

static inline void camerka_update_pos(Camerka* cam, vecf3 vec)
{
    assert(cam);
    cam->pos = vec;
}

static inline void camerka_update_aim(Camerka* cam, vecf3 vec)
{
    assert(cam);
    cam->target = vec;
}
