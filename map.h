/*
 * map.h
 *
 *  Created on: Sep 7, 2021
 *      Author: Ayhan Karaca
 */

#ifndef MODULES_INC_MAP_H_
#define MODULES_INC_MAP_H_

#include <stdint.h>

int16_t mapped_value;

int16_t map(int16_t value, int16_t min_value, int16_t max_value, int16_t mapped_min_value, int16_t mapped_max_value);

#endif /* MODULES_INC_MAP_H_ */
