#include "camerka.h"
#include "lamath.h"

matf4x4 camerka_view_matrix(const Camerka* camerka)
{
    matf4x4 view = {0};
    vecf3 direction = {0};

    direction.x = cos(deg_to_rad(camerka->yaw)) * cos(deg_to_rad(camerka->pitch));
    direction.y = sin(deg_to_rad(camerka->pitch));
    direction.z = sin(deg_to_rad(camerka->yaw)) * cos(deg_to_rad(camerka->pitch));
    direction = vecf3_norm(direction);
    direction = vecf3_add(camerka->pos, direction);

    lamath_lookat_matrix(&view, camerka->pos, direction, (vecf3){0.0f, 1.0f, 0.0f});
    return view;
}

vecf3 camerka_direction(const Camerka* camerka)
{
    vecf3 direction = {0};
    direction.x = cos(deg_to_rad(camerka->yaw)) * cos(deg_to_rad(camerka->pitch));
    direction.y = sin(deg_to_rad(camerka->pitch));
    direction.z = sin(deg_to_rad(camerka->yaw)) * cos(deg_to_rad(camerka->pitch));
    return direction;
    LOG_VEC(direction);
}
