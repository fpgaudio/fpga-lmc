#include "leap_math.h"
#include <math.h>

LEAP_QUATERNION leap_quat_mul(const LEAP_QUATERNION* a, const LEAP_QUATERNION* b) {
    return (LEAP_QUATERNION) {
        .w = a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z,
        .x = a->w * b->x + a->x * b->w - a->y * b->z + a->z * b->y,
        .y = a->w * b->y + a->x * b->z + a->y * b->w - a->z * b->x,
        .z = a->w * b->z - a->x * b->y + a->y * b->x + a->z * b->w
    };
}

LEAP_QUATERNION leap_quat_conj(const LEAP_QUATERNION* a) {
    return (LEAP_QUATERNION) {
        .w = a->w,
        .x = -a->x,
        .y = -a->y,
        .z = -a->z
    };
}

float leap_quat_norm2(const LEAP_QUATERNION* a) {
    return sqrtf(powf(a->x, 2) + powf(a->y, 2) + powf(a->z, 2));
}

LEAP_QUATERNION leap_quat_normalize2(const LEAP_QUATERNION* a) {
    const float norm = leap_quat_norm2(a);
    return (LEAP_QUATERNION) {
        .x = a->x / norm,
        .y = a->y / norm,
        .z = a->z / norm,
        .w = a->w / norm
    };
}

float leap_bones_angle(const LEAP_BONE* bone1, const LEAP_BONE* bone2) {
    const LEAP_QUATERNION b1 = leap_quat_normalize2(&bone1->rotation);
    const LEAP_QUATERNION b2 = leap_quat_normalize2(&bone2->rotation);

    const LEAP_QUATERNION b1_conj = leap_quat_conj(&b1);
    const LEAP_QUATERNION z = leap_quat_mul(&b1_conj, &b2);

    const float l_norm = leap_quat_norm2(&z);
    return 2 * atan2f(l_norm, z.w);
}