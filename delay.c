/*
 * delay.c
 *
 *  Created on: Sep 7, 2021
 *      Author: Ayhan Karaca
 *
 *      NOTLAR:
 *      Bu bölümde microsec mertebesinde zaman sayımı için fonksiyon yazdım. Bunun için timer7 kullandım.
 *      Güncel zamanı microsec cinsinden bulmak için fonksiyon yazdım. Bunun için timer6 kullandım.
 */

#include "delay.h"

void timer7_init(void){
	RCC -> APB1ENR |= RCC_APB1ENR_TIM7EN;
	TIM7 -> PSC = 84-1;
	TIM7 -> ARR = 0xFFFF;
	TIM7 -> CR1 |= TIM_CR1_CEN;
	while(!(TIM7 -> SR & TIM_SR_UIF));
}

void timer6_init(void){
	RCC -> APB1ENR |= RCC_APB1ENR_TIM6EN;
	TIM6 -> PSC = 84-1;
	TIM6 -> ARR = 0xFFFF;
	TIM6 -> CR1 |= TIM_CR1_CEN;
	while(!(TIM6 -> SR & TIM_SR_UIF));
}


void delay_us(uint16_t us){
	TIM7 -> CNT = 0;
	while(TIM7 -> CNT < us);
}

uint32_t gettick(void){
	curr_time = TIM6 -> CNT;
	return curr_time;
}
