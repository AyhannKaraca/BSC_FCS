/*
 * pid.c
 *
 *  Created on: Sep 9, 2021
 *      Author: Ayhan Karaca
 *
 *      NOTLAR:
 *      Bu bölümde pid hesaplamalarını içeren fonksiyonu
 *      ve pid değerlerin resetleyen fonksiyonu yazdım.
 */

//a_channels[0] = roll (x) ,channles[1] = pitch(y)

#include "pid.h"

float error=0;

void pid_calculation(void){
if(control_num != FAILSAFE_ALERT && arm == ARM){
//roll pid
error = angle_roll_output - a_channels[0];

pid_roll.pid_p = error*ROLL_PITCH_KP;

pid_roll.pid_i += (error*ROLL_PITCH_KI);

if(pid_roll.pid_i > MAX_VALUE){
	pid_roll.pid_i = MAX_VALUE;
}
else if(pid_roll.pid_i < -MAX_VALUE){
	pid_roll.pid_i = -MAX_VALUE;
}

pid_roll.pid_d = (error - pid_roll.pid_last_error)*ROLL_PITCH_KD;

pid_roll.pid_value = pid_roll.pid_p + pid_roll.pid_i + pid_roll.pid_d;

if(pid_roll.pid_value > MAX_VALUE){
	pid_roll.pid_value = MAX_VALUE;
}
else if(pid_roll.pid_value < -MAX_VALUE){
	pid_roll.pid_value = -MAX_VALUE;
}

pid_roll.pid_last_error = error;


//pitch pid
//https://github.com/berkaysaka/ArduinoQuadcopterFlightController/blob/master/lessons/4_PID/pid_balancing_seesaw/pid.ino
error = angle_pitch_output; //- a_channels[1];

pid_pitch.pid_p = error*ROLL_PITCH_KP;

pid_pitch.pid_i += (error*ROLL_PITCH_KI);

if(pid_pitch.pid_i > MAX_VALUE){
	pid_pitch.pid_i = MAX_VALUE;
}
else if(pid_pitch.pid_i < -MAX_VALUE){
	pid_pitch.pid_i = -MAX_VALUE;
}

//pid_pitch.pid_d = (error - pid_pitch.pid_last_error)*ROLL_PITCH_KD;
pid_pitch.pid_d = ((error - pid_pitch.pid_last_error)/0.004)*ROLL_PITCH_KD;

pid_pitch.pid_value = pid_pitch.pid_p + pid_pitch.pid_i + pid_pitch.pid_d;

if(pid_pitch.pid_value > MAX_VALUE){
	pid_pitch.pid_value = MAX_VALUE;
}
else if(pid_pitch.pid_value < -MAX_VALUE){
	pid_pitch.pid_value = -MAX_VALUE;
}

pid_pitch.pid_last_error = error;


//yaw pid
error = angle_yaw_output - a_channels[2];

pid_yaw.pid_p = error*YAW_KP;

pid_yaw.pid_i += (error*YAW_KI);

if(pid_yaw.pid_i > MAX_VALUE){
	pid_yaw.pid_i = MAX_VALUE;
}
else if(pid_yaw.pid_i < -MAX_VALUE){
	pid_yaw.pid_i = -MAX_VALUE;
}

pid_yaw.pid_d = (error - pid_yaw.pid_last_error)*YAW_KD;

pid_yaw.pid_value = pid_yaw.pid_p + pid_yaw.pid_i + pid_yaw.pid_d;

if(pid_yaw.pid_value > MAX_VALUE){
	pid_yaw.pid_value = MAX_VALUE;
}
else if(pid_yaw.pid_value < -MAX_VALUE){
	pid_yaw.pid_value = -MAX_VALUE;
}

pid_yaw.pid_last_error = error;

}
else
{
resetPid_Values();
return;
}
}

void resetPid_Values(void){
	pid_roll.pid_p = 0;
	pid_roll.pid_i = 0;
	pid_roll.pid_d = 0;
	pid_roll.pid_value = 0;
	pid_roll.pid_last_error = 0;
	pid_pitch.pid_p = 0;
	pid_pitch.pid_i = 0;
	pid_pitch.pid_d = 0;
	pid_pitch.pid_value = 0;
	pid_pitch.pid_last_error = 0;
	pid_yaw.pid_p = 0;
	pid_yaw.pid_i = 0;
	pid_yaw.pid_d = 0;
	pid_yaw.pid_value = 0;
	pid_yaw.pid_last_error = 0;
}
