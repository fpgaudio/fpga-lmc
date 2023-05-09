/**
 * This interface contains the mathematics for leap quaternions.
 */

#ifndef LEAP_UTIL_H
#define LEAP_UTIL_H

#include "LeapC.h"

/**
 * Performs quaternion mulitplication.
 */
LEAP_QUATERNION leap_quat_mul(const LEAP_QUATERNION*, const LEAP_QUATERNION*);

/**
 * Performs quaternion conjugation.
 */
LEAP_QUATERNION leap_quat_conj(const LEAP_QUATERNION*);

/**
 * Calculates the Euclidan norm of a quaternion.
 */
float leap_quat_norm2(const LEAP_QUATERNION*);

/**
 * Normalizes a quaternion via Euclidean norm.
 */
LEAP_QUATERNION leap_quat_normalize2(const LEAP_QUATERNION*);

/**
 * Calculates the angle between finger bones.
 */
float leap_bones_angle(const LEAP_BONE*, const LEAP_BONE*);

#endif // LEAP_UTIL_H