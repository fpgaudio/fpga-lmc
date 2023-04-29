#ifndef FPGA_SERIAL_H
#define FPGA_SERIAL_H

#include "struct_expand.h"
#include <stdio.h>

typedef PACK(struct {
	float x;
	float y;
	float z;
}) vec_s;

typedef PACK(struct {
	float metacarpophalangeal_angle;
	float proximal_interphalangeal_angle;
	float distal_interphalangeal_angle;
}) finger_data_s;

typedef PACK(struct {
	// This vector contains the distance from the camera sensor in mm, such
	// that the XY plane is facing the user. In other words:
	// X is left-right
	// Y is down-up
	// Z is out-in
	// if the LMC is placed on the table with the LED facing the user
	// and the ranges are - towards + in the current labelling
	vec_s palm_position;
	// The normal of the palm, with the vector facing away from the open palm
	// Same coordinate system as palm position
	vec_s palm_normal;

	float palm_distance;

	finger_data_s index;
	finger_data_s middle;
	finger_data_s ring;
	finger_data_s pinky;
}) hand_evt_s;

inline void DumpEvt(const hand_evt_s* evt) {
	printf("{ "
		"\"px\": %f, "
		"\"py\": %f, "
		"\"pz\": %f, "
		"\"nx\": %f, "
		"\"ny\": %f, "
		"\"nz\": %f, "
		"\"pd\": %f, "
		"\"index\": { "
		" \"angle_metacrapophalengeal\": %f "
		" \"angle_proximal_interphalangeal\": %f "
		" \"angle_distal_interphalangeal\": %f "
		"}, "
		"\"middle\": { "
		" \"angle_metacrapophalengeal\": %f "
		" \"angle_proximal_interphalangeal\": %f "
		" \"angle_distal_interphalangeal\": %f "
		"}, "
		"\"ring\": { "
		" \"angle_metacrapophalengeal\": %f "
		" \"angle_proximal_interphalangeal\": %f "
		" \"angle_distal_interphalangeal\": %f "
		"}, "
		"\"pinky\": { "
		" \"angle_metacrapophalengeal\": %f "
		" \"angle_proximal_interphalangeal\": %f "
		" \"angle_distal_interphalangeal\": %f "
		"}"
		"}",
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

#endif
