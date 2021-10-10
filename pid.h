/*
 * pid.h
 *
 *  Created on: Sep 9, 2021
 *      Author: Ayhan Karaca
 */

#ifndef MODULES_INC_PID_H_
#define MODULES_INC_PID_H_

#define ROLL_PITCH_KP 0.2
#define ROLL_PITCH_KI 0.009
#define ROLL_PITCH_KD 700
#define YAW_KP 0.00
#define YAW_KI 0.00
#define YAW_KD 0.00
#define MAX_VALUE 400

#include "../../Drivers/Modules/Inc/map.h"
#include "../../Drivers/Modules/Inc/rc_commands.h"
#include "../../Drivers/Modules/Inc/mpu6050.h"

typedef struct {
	float pid_p;
	float pid_i;
	float pid_d;
	float pid_last_error;
	float pid_value;
}pid_values_t;

pid_values_t pid_roll, pid_pitch, pid_yaw;

void pid_calculation(void);
void resetPid_Values(void);

#endif /* MODULES_INC_PID_H_ */
