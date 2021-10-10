/*
 * delay.h
 *
 *  Created on: Sep 7, 2021
 *      Author: Ayhan Karaca
 */

#ifndef MODULES_INC_DELAY_H_
#define MODULES_INC_DELAY_H_

#include "stm32f407xx.h"

uint32_t curr_time;

void delay_us(uint16_t us);
void timer7_init(void);
void timer6_init(void);
uint32_t gettick(void);

#endif /* MODULES_INC_DELAY_H_ */
