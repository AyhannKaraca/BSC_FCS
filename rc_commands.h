/*
 * rc_commands.h
 *
 *  Created on: Sep 7, 2021
 *      Author: Ayhan Karaca
 */

#ifndef MODULES_INC_RC_COMMANDS_H_
#define MODULES_INC_RC_COMMANDS_H_

#define ARM 2
#define DISARM 0
#define SBUS_VERIFICATION 0x0F
#define FAILSAFE_ALERT 13
#define FAILSAFE_OK 0
#define LOST_SIGNAL_LIMIT 50

#include "stm32f4xx_hal.h"
#include "../../Drivers/Modules/Inc/delay.h"
#include "../../Drivers/Modules/Inc/map.h"
#include "stm32f407xx.h"
#include "../../Drivers/Modules/Inc/motor_pwm.h"

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

volatile uint8_t cnt; // Dma yada interrupt callback fonksiyonu içinde ve aynı zamanda main fonk. içinde kullandığım değişkenleri volatile tanımlıyorum.
uint8_t failsafe, control_num;
char sbusData[24];
uint16_t channels[10];
int8_t a_channels[3];
uint8_t arm;

void commandsInit(void);
void failsafe_Control(void);
void arm_Drone(void);

#endif /* MODULES_INC_RC_COMMANDS_H_ */
