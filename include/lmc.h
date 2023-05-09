/**
 * This file contains the API used to read data from the LMC.
 */

#ifndef LMC_H
#define LMC_H

#include <stdbool.h>
#include "LeapC.h"

typedef struct lmc* lmc_t;
typedef union lmc_hand_evt_u* lmc_hand_evt_t;
/**
 * Represents a full structure containing the hand data.
 */
typedef union lmc_hand_evt_min_u* lmc_hand_evt_min_t;
typedef void (*on_hand_evt_cb)(const lmc_t, const lmc_hand_evt_t);

/**
 * Represents a minimal structure containing the hand data.
 * 
 * The key difference between this and the full structure is that this only
 * contains the joint angles and palm distance.
 */
typedef void (*on_hand_evt_min_cb)(const lmc_t, const lmc_hand_evt_min_t);

/**
 * Connects to a leap device.
 * 
 * @return An instance to the LMC device, NULL if connection was unsuccessful.
 */
lmc_t lmc_connect(bool verbosity);

/**
 * Serializes a minimal event into a byte stream.
 *
 * @param lmc The LMC instance.
 * @param evt The event to serialize.
 * @param out_buf The output buffer to write to.
 * @param sz The size of the output buffer.
 */
void lmc_serialize_min_evt(const lmc_t lmc, const lmc_hand_evt_min_t evt,
	                       unsigned char** out_buf, size_t* sz);

/**
 * Starts a tracking loop.
 * 
 * @param lmc The LMC instance.
 * @param big_hand_cb Optional callback fired with the lmc_hand_evt_t data.
 *                    You can ignore this if you are not interested in parsing
 *                    the full hand data by passing in NULL.
 * @param small_hand_cb Optional callback fired with the lmc_hand_evt_min_t
 *                      data. Ignore if uninterested by passing NULL.
 */
void lmc_listen_for_hand(lmc_t lmc, on_hand_evt_cb big_hand_cb,
	                     on_hand_evt_min_cb small_hand_cb);

/**
 * Stops the current tracking loop. Can (and should) be called from a
 * separate thread.
 * 
 * @param lmc The LMC instance.
 */
void lmc_stop_listening(lmc_t lmc);

/**
 * Closes the LMC instance and cleans up any memory.
 * 
 * @param lmc The LMC instance.
 */
void lmc_close(lmc_t lmc);

#endif // LMC_H