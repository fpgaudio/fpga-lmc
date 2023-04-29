#include "fpga_serial.h"
#include "ExampleConnection.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

void LMCConnect() {
    OpenConnection();
    while (!IsConnected) {
        millisleep(100);
    }

    LEAP_DEVICE_INFO* deviceProps = GetDeviceProperties();
    if (deviceProps) {
        printf("Using device %s.\n", deviceProps->serial);
    }
}

char* GetCurrentTimeStr() {
    time_t rawTime;
    struct tm* currentTime;

    time(&rawTime);
    currentTime = localtime(&rawTime);

    // By default this returns a C string with a trailing newline that we
    // don't want.
    char* strTime = asctime(currentTime);
    strTime[strlen(strTime) - 1] = 0;

    return strTime;
}

LEAP_QUATERNION LeapQuatMul(LEAP_QUATERNION a, LEAP_QUATERNION b) {
    return (LEAP_QUATERNION) {
        .w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
        .x = a.w * b.x + a.x * b.w - a.y * b.z + a.z * b.y,
        .y = a.w * b.y + a.x * b.z + a.y * b.w - a.z * b.x,
        .z = a.w * b.z - a.x * b.y + a.y * b.x + a.z * b.w
    };
}

LEAP_QUATERNION LeapQuatConj(LEAP_QUATERNION a) {
    return (LEAP_QUATERNION) {
        .w = a.w,
        .x = -a.x,
        .y = -a.y,
        .z = -a.z
    };
}

float LeapQuat2Norm(LEAP_QUATERNION a) {
    return sqrt(pow(a.x, 2) + pow(a.y, 2) + pow(a.z, 2));
}

LEAP_QUATERNION LeapQuat2Normalize(LEAP_QUATERNION a) {
    const float norm = LeapQuat2Norm(a);
    return (LEAP_QUATERNION) {
        .x = a.x / norm,
        .y = a.y / norm,
        .z = a.z / norm,
        .w = a.w / norm
    };
}

float BoneAngle(LEAP_BONE bone1, LEAP_BONE bone2) {
    const LEAP_QUATERNION b1 = LeapQuat2Normalize(bone1.rotation);
    const LEAP_QUATERNION b2 = LeapQuat2Normalize(bone2.rotation);

    const LEAP_QUATERNION z = LeapQuatMul(LeapQuatConj(b1), b2);

    return 2 * atan2(LeapQuat2Norm(z), z.w);
}

void E_onHandFrame(const LEAP_HAND* pHand) {
    const char* ts = GetCurrentTimeStr();

    hand_evt_s evt = {
        .palm_position = {
            .x = pHand->palm.position.x,
            .y = pHand->palm.position.y,
            .z = pHand->palm.position.z
        },
        .palm_normal = {
            .x = pHand->palm.normal.x,
            .y = pHand->palm.normal.y,
            .z = pHand->palm.normal.z
        },
        .palm_distance = sqrt(
            pow(pHand->palm.position.x, 2) +
            pow(pHand->palm.position.y, 2) +
            pow(pHand->palm.position.z, 2)
        ),
        .index = {
            .metacarpophalangeal_angle = BoneAngle(pHand->index.metacarpal, pHand->index.proximal),
            .proximal_interphalangeal_angle = BoneAngle(pHand->index.proximal, pHand->index.intermediate),
            .distal_interphalangeal_angle = BoneAngle(pHand->index.intermediate, pHand->index.distal)
        },
        .middle = {
            .metacarpophalangeal_angle = BoneAngle(pHand->middle.metacarpal, pHand->middle.proximal),
            .proximal_interphalangeal_angle = BoneAngle(pHand->middle.proximal, pHand->middle.intermediate),
            .distal_interphalangeal_angle = BoneAngle(pHand->middle.intermediate, pHand->middle.distal)
        },
        .ring = {
            .metacarpophalangeal_angle = BoneAngle(pHand->ring.metacarpal, pHand->ring.proximal),
            .proximal_interphalangeal_angle = BoneAngle(pHand->ring.proximal, pHand->ring.intermediate),
            .distal_interphalangeal_angle = BoneAngle(pHand->ring.intermediate, pHand->ring.distal)
        },
        .pinky = {
            .metacarpophalangeal_angle = BoneAngle(pHand->pinky.metacarpal, pHand->pinky.proximal),
            .proximal_interphalangeal_angle = BoneAngle(pHand->pinky.proximal, pHand->pinky.intermediate),
            .distal_interphalangeal_angle = BoneAngle(pHand->pinky.intermediate, pHand->pinky.distal)
        },
    };

    printf("[%s] Found a hand (%i)\n",
        ts,
        pHand->id
    );
    printf("    Serialized: ");
    DumpEvt(&evt);
    printf("\n");
}

int main(int argc, char** argv) {
    LMCConnect();

    LEAP_TRACKING_EVENT* frame;
    int64_t prevFrameId = 0;
    for (frame = GetFrame();; frame = GetFrame()) {
        if (frame->tracking_frame_id <= prevFrameId) {
            continue;
        }
        prevFrameId = frame->tracking_frame_id;

        if (frame->nHands == 0) {
            continue;
        }

        E_onHandFrame(&frame->pHands[0]);
    }
}
