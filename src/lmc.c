#include "lmc.h"
#include "ExampleConnection.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "leap_math.h"
#include "win_ex.h"
#include <time.h>

typedef PACK(struct {
    float x;
    float y;
    float z;
}) lmc_vec_s;

typedef PACK(struct {
    float metacarpophalangeal_angle;
    float proximal_interphalangeal_angle;
    float distal_interphalangeal_angle;
}) lmc_finger_data_s;

typedef PACK(struct {
    // This vector contains the distance from the camera sensor in mm, such
    // that the XY plane is facing the user. In other words:
    // X is left-right
    // Y is down-up
    // Z is out-in
    // if the LMC is placed on the table with the LED facing the user
    // and the ranges are - towards + in the current labelling
    lmc_vec_s palm_position;
    // The normal of the palm, with the vector facing away from the open palm
    // Same coordinate system as palm position
    lmc_vec_s palm_normal;

    float palm_distance;

    lmc_finger_data_s index;
    lmc_finger_data_s middle;
    lmc_finger_data_s ring;
    lmc_finger_data_s pinky;
}) lmc_hand_evt_s;

typedef PACK(struct {
    int32_t distance;
    int32_t indexAngle;
    int32_t middleAngle;
    int32_t ringAngle;
    int32_t pinkyAngle;
}) lmc_hand_evt_small_s;

union lmc_hand_evt_u {
    lmc_hand_evt_s asStruct;
    unsigned char asBytes[sizeof(lmc_hand_evt_s)];
};

union lmc_hand_evt_min_u {
    lmc_hand_evt_small_s asStruct;
    unsigned char asBytes[sizeof(lmc_hand_evt_small_s)];
};

struct lmc {
    bool verbose;
    clock_t last_sample_time;
    bool should_listen;
};

static void print_event(const lmc_hand_evt_s* evt) {
    printf("{ "
        "\"px\": %f, "
        "\"py\": %f, "
        "\"pz\": %f, "
        "\"nx\": %f, "
        "\"ny\": %f, "
        "\"nz\": %f, "
        "\"pd\": %f, "
        "\"index\": {"
        " \"angle_metacrapophalengeal\": %f "
        " \"angle_proximal_interphalangeal\": %f "
        " \"angle_distal_interphalangeal\": %f "
        "}, "
        "\"middle\": {"
        " \"angle_metacrapophalengeal\": %f "
        " \"angle_proximal_interphalangeal\": %f "
        " \"angle_distal_interphalangeal\": %f "
        "}, "
        "\"ring\": {"
        " \"angle_metacrapophalengeal\": %f "
        " \"angle_proximal_interphalangeal\": %f "
        " \"angle_distal_interphalangeal\": %f "
        "}, "
        "\"pinky\": {"
        " \"angle_metacrapophalengeal\": %f "
        " \"angle_proximal_interphalangeal\": %f "
        " \"angle_distal_interphalangeal\": %f "
        "}"
        "}\n",
        evt->palm_position.x,
        evt->palm_position.y,
        evt->palm_position.z,
        evt->palm_normal.x,
        evt->palm_normal.y,
        evt->palm_normal.z,
        evt->palm_distance,
        evt->index.metacarpophalangeal_angle,
        evt->index.proximal_interphalangeal_angle,
        evt->index.distal_interphalangeal_angle,
        evt->middle.metacarpophalangeal_angle,
        evt->middle.proximal_interphalangeal_angle,
        evt->middle.distal_interphalangeal_angle,
        evt->ring.metacarpophalangeal_angle,
        evt->ring.proximal_interphalangeal_angle,
        evt->ring.distal_interphalangeal_angle,
        evt->pinky.metacarpophalangeal_angle,
        evt->pinky.proximal_interphalangeal_angle,
        evt->pinky.distal_interphalangeal_angle
    );
}

static void on_raw_hand_frame(
    const lmc_t lmc, const LEAP_HAND* p_hand,
    on_hand_evt_cb big_hand_cb,
    on_hand_evt_min_cb small_hand_cb
) {
    union lmc_hand_evt_u evt = {
        .asStruct = {
            .palm_position = {
                .x = p_hand->palm.position.x,
                .y = p_hand->palm.position.y,
                .z = p_hand->palm.position.z
            },
            .palm_normal = {
                .x = p_hand->palm.normal.x,
                .y = p_hand->palm.normal.y,
                .z = p_hand->palm.normal.z
            },
            .palm_distance = sqrtf(
                powf(p_hand->palm.position.x, 2) +
                powf(p_hand->palm.position.y, 2) +
                powf(p_hand->palm.position.z, 2)
            ),
            .index = {
                .metacarpophalangeal_angle = leap_bones_angle(&p_hand->index.metacarpal, &p_hand->index.proximal),
                .proximal_interphalangeal_angle = leap_bones_angle(&p_hand->index.proximal, &p_hand->index.intermediate),
                .distal_interphalangeal_angle = leap_bones_angle(&p_hand->index.intermediate, &p_hand->index.distal)
            },
            .middle = {
                .metacarpophalangeal_angle = leap_bones_angle(&p_hand->middle.metacarpal, &p_hand->middle.proximal),
                .proximal_interphalangeal_angle = leap_bones_angle(&p_hand->middle.proximal, &p_hand->middle.intermediate),
                .distal_interphalangeal_angle = leap_bones_angle(&p_hand->middle.intermediate, &p_hand->middle.distal)
            },
            .ring = {
                .metacarpophalangeal_angle = leap_bones_angle(&p_hand->ring.metacarpal, &p_hand->ring.proximal),
                .proximal_interphalangeal_angle = leap_bones_angle(&p_hand->ring.proximal, &p_hand->ring.intermediate),
                .distal_interphalangeal_angle = leap_bones_angle(&p_hand->ring.intermediate, &p_hand->ring.distal)
            },
            .pinky = {
                .metacarpophalangeal_angle = leap_bones_angle(&p_hand->pinky.metacarpal, &p_hand->pinky.proximal),
                .proximal_interphalangeal_angle = leap_bones_angle(&p_hand->pinky.proximal, &p_hand->pinky.intermediate),
                .distal_interphalangeal_angle = leap_bones_angle(&p_hand->pinky.intermediate, &p_hand->pinky.distal)
            },
        }
    };

    union lmc_hand_evt_min_u minified = {
        .asStruct = {
            .distance = float2i32(evt.asStruct.palm_distance),
            .indexAngle = float2i32(evt.asStruct.index.metacarpophalangeal_angle),
            .middleAngle = float2i32(evt.asStruct.middle.metacarpophalangeal_angle),
            .ringAngle = float2i32(evt.asStruct.ring.metacarpophalangeal_angle),
            .pinkyAngle = float2i32(evt.asStruct.pinky.metacarpophalangeal_angle)
        }
    };

    if (lmc->verbose) {
        print_event(&evt.asStruct);
    }

    if (big_hand_cb != NULL) {
        big_hand_cb(lmc, &evt);
    }

    if (small_hand_cb != NULL) {
        small_hand_cb(lmc, &minified);
    }
}

lmc_t lmc_connect(bool verbosity) {
    struct lmc m_lmc = {
        .verbose = verbosity,
        .should_listen = true,
        .last_sample_time = clock()
    };

    OpenConnection();
    while (!IsConnected) {
        millisleep(100);
    }

    LEAP_DEVICE_INFO* deviceProps = GetDeviceProperties();
    if (!deviceProps) {
        fprintf(stderr, "Could not open the LMC device. Is it connected?\n");
        return NULL;
    }
    if (m_lmc.verbose) {
        printf("Using LMC: %s.\n", deviceProps->serial);
    }

    // Copy to the heap.
    lmc_t out = (lmc_t)malloc(sizeof(struct lmc));
    if (out == NULL) {
        fprintf(stderr, "Critical Error: OOM\n");
        return NULL;
    }
    memcpy(out, &m_lmc, sizeof(struct lmc));
    return out;
}

void lmc_close(lmc_t lmc) {
    CloseConnection();
    free(lmc);
}

void lmc_listen_for_hand(lmc_t lmc, on_hand_evt_cb big_hand_cb,
                         on_hand_evt_min_cb small_hand_cb) {
    long long last_frame_id = 0;
    lmc->should_listen = true;
    for (LEAP_TRACKING_EVENT* frame = GetFrame();
         lmc->should_listen;
         frame = GetFrame()) {
        if (frame == NULL) {
            if (lmc->verbose) {
                fprintf(stderr, "Received an empty frame. Ignoring.\n");
            }
            millisleep(100);
            continue;
        }

        if (frame->tracking_frame_id <= last_frame_id) {
            if (lmc->verbose) {
                fprintf(stderr, "Scanned a repeated frame. Ignoring.\n");
            }
            continue;
        }

        if (frame->nHands == 0) {
            continue;
        }

        const clock_t sample_time = clock();
        if (lmc->verbose) {
            const double sample_period =
                ((double)sample_time - (double)lmc->last_sample_time)
                / CLOCKS_PER_SEC;
            const double sample_frequency = 1.0 / sample_period;
            fprintf(stderr, "LMC Sample Rate: %.2fHz\n", sample_frequency);
        }
        lmc->last_sample_time = sample_time;
        on_raw_hand_frame(lmc, &frame->pHands[0], big_hand_cb, small_hand_cb);
    }
}

void lmc_serialize_min_evt(const lmc_t lmc, const lmc_hand_evt_min_t evt,
                           unsigned char** out_buf, size_t* sz) {
    (void)lmc;
    *out_buf = evt->asBytes;
    *sz = sizeof(union lmc_hand_evt_min_u);
}

void lmc_stop_listening(lmc_t lmc) {
    lmc->should_listen = false;
}