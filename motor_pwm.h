/*
 * motor_pwm.h
 *
 *  Created on: Sep 9, 2021
 *      Author: Ayhan Karaca
 */

#ifndef MODULES_INC_MOTOR_PWM_H_
#define MODULES_INC_MOTOR_PWM_H_

#include "stm32f407xx.h"

#include "../../Drivers/Modules/Inc/pid.h"

uint16_t esc1, esc2, esc3, esc4;

void pwm_timer3_init(void);
void enableMotors(void);
void disableMotors(void);
void escPowers(void);

#endif /* MODULES_INC_MOTOR_PWM_H_ */
