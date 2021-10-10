/*
 * motor_pwm.c
 *
 *  Created on: Sep 9, 2021
 *      Author: Ayhan Karaca
 *
 *      NOTLAR:
 *      Bu bölümde motorlar için pwm sinyali üreten fonksiyonları yazdım. Bunun için timer3 kullandım.
 *      timer3 ayrıntılı init fonksiyonunu yazdım.
 *      Acil durumlarda yada istenmeyen durumlarda motorlara sadece 1000 (yani 0 throttle) vermek yerine
 *      timer3 kanallarını disable eden fonksiyon yazdım.(gerek varmıydı bilmiyorum)
 */

#include "motor_pwm.h"

//PC6=CH1 (SAĞ ÖN), PC7=CH2 (SAĞ ARKA), PC8=CH3 (SOL ARKA), PC9=CH4 (SOL ÖN)

void pwm_timer3_init(void){
	RCC -> AHB1ENR |= (1<<2); // C PORT CLOCK AKTİF ETME
	RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN; //tim3 clock enabled.
	//PC6,7,8,9 pinlerinin af olarak ayarlanması.
	GPIOC -> MODER |= (1<<13)|(1<<15)|(1<<17)|(1<<19);
	GPIOC -> MODER &= ~(1<<12);
	GPIOC -> MODER &= ~(1<<14);
	GPIOC -> MODER &= ~(1<<16);
	GPIOC -> MODER &= ~(1<<18);
	//PC6,7,8,9 pinlerinin push pull olarak ayarlanması.
	GPIOC -> OTYPER &= ~(1<<6);
	GPIOC -> OTYPER &= ~(1<<7);
	GPIOC -> OTYPER &= ~(1<<8);
	GPIOC -> OTYPER &= ~(1<<9);
	//PC6,7,8,9 pinlerinin nopullup down olarak ayarlanması.
	GPIOC -> PUPDR &= ~(1<<12);
	GPIOC -> PUPDR &= ~(1<<13);
	GPIOC -> PUPDR &= ~(1<<14);
	GPIOC -> PUPDR &= ~(1<<15);
	GPIOC -> PUPDR &= ~(1<<16);
	GPIOC -> PUPDR &= ~(1<<17);
	GPIOC -> PUPDR &= ~(1<<18);
	GPIOC -> PUPDR &= ~(1<<19);
	//PC6,7,8,9 pinlerinin hs olarak ayarlanması.
	GPIOC -> OSPEEDR  |= (1<<13)|(1<<15)|(1<<17)|(1<<19);
	GPIOC -> OSPEEDR &= ~(1<<12);
	GPIOC -> OSPEEDR &= ~(1<<14);
	GPIOC -> OSPEEDR &= ~(1<<16);
	GPIOC -> OSPEEDR &= ~(1<<18);
	//PC6,7,8,9 pinlerinin AF2 olarak ayarlanması.
	GPIOC -> AFR[0] |= (1<<25)|(1<<29);
	GPIOC -> AFR[1] |= (1<<1)|(1<<5);
	TIM3 -> CR1 |= TIM_CR1_CEN; // TIM5 ENABLE
	TIM3 -> EGR |= TIM_EGR_UG;
	// TIM5 CHANNEL1,2 PWM MODE
	TIM3 -> CCMR1 |= (1<<6)|(1<<5)|(1<<13)|(1<<14);
	TIM3 -> CCMR1 &= ~ (1<<4);
	TIM3 -> CCMR1 &= ~ (1<<12);
	//TIM5 CHANNEL3,4 PWM MODE
	TIM3 -> CCMR2 |= (1<<6)|(1<<5)|(1<<13)|(1<<14);
	TIM3 -> CCMR2 &= ~ (1<<4);
	TIM3 -> CCMR2 &= ~ (1<<12);

	/*
	 * motorlar 20 ms periyod ve 1000 ile 2000us arasında pwm sinyalleri ile çalışıyor.
	 * timer3 PSC ve ARR registerları ile bu değerleri ayarladım.
	 */
	TIM3 -> PSC = 83; //84,000,000/84 = 1MHz = 1us
	TIM3 -> ARR = 19999; // = 20000*1us= 20ms periyod.
}

	void enableMotors(void){
		//tim5 kanal cikislari aktif
		TIM3 -> CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
	}

	void disableMotors(void){
		//tim5 kanal cikislari aktif degil
		TIM3 -> CCER &= ~TIM_CCER_CC1E;
		TIM3 -> CCER &= ~TIM_CCER_CC2E;
		TIM3 -> CCER &= ~TIM_CCER_CC3E;
		TIM3 -> CCER &= ~TIM_CCER_CC4E;
	}

	void escPowers(void){

	// receiver throttle = channels[2]

		// eğer drone arm edilmiş ve failsafe durumunda değilse aşağıdaki satırlara geç
		if(arm == 2 && control_num != 13){
		/*
		 * eğer throttle değeri 1800 den büyük ise throttle değerini 1800 e eşitle.
		 * amaç pid hesaplamalarından gelen değerleri ekleyebilmek için alan bırakmak.
		 * (escye giden pwm sinyali 2000us den fazla olamaz.)
		 */
		if(channels[2] > 1800){
			channels[2] = 1800;
		}

		/*
		 * dronun dengede durabilmesi için kumandadan gelen veriler pid hesaplamaları için
		 * kullanılır. En sonunda gelen değerler throttle değeri ile ile toplanır veya çıkarılır ve escye
		 * gidecek olan pwm sinyali oluşur.
		 */
//		esc1 = channels[2] + pid_pitch.pid_value + pid_roll.pid_value - pid_yaw.pid_value;    //sağ ön
//		esc2 = channels[2] - pid_pitch.pid_value + pid_roll.pid_value + pid_yaw.pid_value;	 //sağ arka
//	 	esc3 = channels[2] - pid_pitch.pid_value - pid_roll.pid_value - pid_yaw.pid_value; 	 //sol arka
//		esc4 = channels[2] + pid_pitch.pid_value - pid_roll.pid_value + pid_yaw.pid_value; 	 //sol ön

//		aşağıdaki 2 satır test düzeneğinde test etmek için
		esc1 = channels[2] + pid_pitch.pid_value; //sağ pc6
		esc2 = channels[2] - pid_pitch.pid_value; //sol pc7
		// esc değeri 2000 den fazla ise esc değerini 2000e eşitle.
		if(esc1 > 2000) esc1 = 2000;
		if(esc2 > 2000) esc2 = 2000;
		if(esc3 > 2000) esc3 = 2000;
		if(esc4 > 2000) esc4 = 2000;
		/*
		 * esc değeri 1100 den küçük ise esc değerlerini 1100e eşitle.
		 * drone arm edildiğinde motorların dönmesini istediğim için en düşük değer 1100.
		 */
		if(esc1 < 1100) esc1 = 1100;
		if(esc2 < 1100) esc2 = 1100;
		if(esc3 < 1100) esc3 = 1100;
		if(esc4 < 1100) esc4 = 1100;

		/*
		 * esc değişkenlerine ihtiyacım yok. hesaplanan değerleri TIM3 -> CCR1 registerına gönderebilirim
		 * ama stmstudio da değerleri görebilmek için değerleri değişkenlerin içine gönderdim.
		 */
		TIM3 -> CCR1 = esc1;
		TIM3 -> CCR2 = esc2;
	//	TIM3 -> CCR3 = esc3;
	//	TIM3 -> CCR4 = esc4;
		}
		//yukarıdaki koşullar sağlanmassa motorlar duruyor.
		else
		{
		esc1 = 1000;
		esc2 = 1000;
		esc3 = 1000;
		esc4 = 1000;

		TIM3 -> CCR1 = esc1;
		TIM3 -> CCR2 = esc2;
		TIM3 -> CCR3 = esc3;
		TIM3 -> CCR4 = esc4;
		}
	}


