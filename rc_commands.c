/*
 * rc_commands.c
 *
 *  Created on: Sep 7, 2021
 *      Author: Ayhan Karaca
 *
 *      NOTLAR:
 *      Bu bölümde kumandadan gelen verilerin okunduğu fonksiyonu ve failsafe
 *      fonksiyonu yazmaya çalıştım.(işe yarıyor ama nekadar kullanışlı bilmiyorum.)
 *      haberleşme sbus protokolünde gerçekleşiyor.
 *      veriler DMA kullanılarak alınıyor.
 *
 */

#include "rc_commands.h"
//PA10 --> Rx
// program başlangıcında default değerlere getirmek için fonksiyon.

void commandsInit(void){
	channels[0] = 1500; //roll
	channels[1] = 1500; //pitch
	channels[2] = 1000; //throttle
	channels[3] = 1500; //yaw
	channels[4] = 1000; //arm switch
	channels[5] = 1000;
	channels[6] = 1000;
	channels[7] = 1000;
	channels[8] = 1000;
	channels[9] = 1000;
}

/*
 * alıcı ile verici arasında iletişim koparsa yada kumanda bozulur ve veriler yanlış gönderilirse
 * motorları durduran bir fonksiyon.
 * bu fonksiyondronun failsafe durumunda motorları durdurmayıp, acil iniş yapmasını sağlayacak şekilde
 * geliştirilebilir.
 */
void failsafe_Control(void){

	  cnt = 0; //(1)*
	  HAL_UART_Receive_DMA(&huart1, (uint8_t *)sbusData, 24);
	  delay_us(1000);

	  if(cnt == 0){
		  failsafe++; //(2)*
	  }
	  else if(cnt != 0)
	  {
		  failsafe = FAILSAFE_OK; //(5)*
	  	  control_num = FAILSAFE_OK;
	  }
	  if(failsafe == LOST_SIGNAL_LIMIT){ //(3)*
		  //(4)*
		  channels[0] = 1500; //roll
		  channels[1] = 1500; //pitch
		  channels[2] = 1000; //throttle
		  channels[3] = 1500; //yaw
		  channels[4] = 1000; //arm switch
		  channels[5] = 1000;
		  channels[6] = 1000;
		  channels[7] = 1000;
		  channels[8] = 1000;
		  channels[9] = 1000;

		  resetPid_Values(); // pid değerleri resetlenmeli. çünkü hesaplanan son pid değerleri hala motorları döndürecek kadar büyük olabilir.
		  arm = DISARM; // arm durumu disarm.

		  // esc değerleri 1000 yani motorlar duruyor.
		  esc1 = 1000;
		  esc2 = 1000;
		  esc3 = 1000;
		  esc4 = 1000;

		  control_num = FAILSAFE_ALERT;
		  disableMotors(); /*
		  	  	  	  	  	*motorlar disable. (failsafe durumunda sadece bu fonksiyonu çağırmam yeterli olabilirdi.
		  	  	  	  	  	*ama ben motorları durduracak nekadar kod satırı varsa yazmak istdim.)
		  	  	  	  	  	*/
	  }
	  if(control_num == FAILSAFE_OK){
		  enableMotors();	/*
		   	   	   	   	     * control_num gereksiz değil. bu lazımdı. motorları enable etmek için failsafe değişkenini kontrol edemezdim
		  	  	  	  	     * cünkü failsafe durumundayken (bağlantı koptuğunda) failsafe değeri sürekli artarak taşıyor. ve her taşmasında
		  	  	  	  	     * bütün değerleri alıyor(0 dan 255 e kadar bütün değerler).
		  	  	  	  	     * böylece failsafe durumundayken mutlaka enable oluyor. bana artmayan bir değişken lazmdı.
						     */
}
}
/*
 * arm fonksiyonu
 * eğer drone arm durumunda değilse diğer bir if yapısına gir
 * KUMANDADAN GELEN VERİLER İLK ARM DURUMUNA GÖRE VERİLER GÜVENLİYSE ARM DURUMUNA GEÇ
 * PİD DEĞERLERİNİ RESETLE
 * DRONE DİSARM DURUMUNDA DEĞİLSE DİĞER İF YAPISINA GİRER
 * ARM SWİTCHİ KAPALIYSA DİSARM DURUMUNA GEÇER
 */
void arm_Drone(void){

	if(arm != ARM){
	if(channels[4]>1950 && channels[3] > 1485 && channels[3] < 1515 && channels[2] < 1015 && channels[1] > 1485 && channels[1] < 1515 && channels[0] > 1485 && channels[0] < 1515){
		arm = ARM;
		resetPid_Values();
}
}
	if(arm != DISARM){
	if(channels[4]<1015){
		arm = DISARM;
}
}
}

/*if(channels[2] < 1050 && channels[3] < 1050){
	arm = 1;
}


  if(arm == 1 && channels[2] < 1050 && channels[3] > 1450){
    arm = 2;
    resetPid_Values();
 }

  if(arm == 2 && channels[2] < 1050 && channels[3] > 1950){
	  arm = 0;
	  //resetPid_Values();
  }
  }*/


/*
 * DMA DEN SBUS VERİSİ OKUMA.
 * (DMA DE SORUN VAR. BU FONKSİYONU WHİLE LOOP UN İÇİNDE ÇAĞIRMAMA GEREK KALMADAN VERİLERİ ALABİLMEM GEREKİYOR
 * FAKAT HAL DMADE SORUN VAR. HAL LİBRARY KULLANMADAN FONKSİYONU YENİDEN YAZ! )
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
        /*
    	 * sbus verileri 25 bytedan oluşur.
    	 * ilk byte ı doğrulama değeri. Ve bu değer 15 olmalı.
    	 * eğer 15 değilse gelen veriler yanlış demektir.
    	 * sonraki gelen 22 byte 16 kanal için gelen verileri ifade eder.
    	 * her kanalı 11 bit değeri oluşturur.
    	 * 23. byte failsafe, ve dijital kanalların bitlerinden oluşur.
    	 * 24. byte kuyruk. değeri 0.
    	 * Protokolün failsafe özelliğini anlayamadım yada kullanmayı başaramadım.
    	 * bu yüzden kendime göre bir failsafe algoritması geliştirdim.
    	 */
  if(sbusData[0] == SBUS_VERIFICATION){

	  /*
	   * ilk gelen data low byte sonra gelen data high byte.
	   * sbusData[1] = low byte , sbusData[2] = high byte
	   * high byte ı 8 bit sola kaydırarak low byte a yer açmış olduk.
	   * sonra gelen low byte ile 8 bit kaydırılmış high byte ı or operatörü ile
	   * topladık. Elde ettiğimiz data 16 bit değerinde oldu.
	   * ama bize 11 byte ı lazım. dolayısıyla high byte ın en son 5 bitini 0 yapmamız
	   * gerekiyor. bu yüzden toplamdan elde edilen 16 bitlik değer 0x07FF ile çarpılarak
	   * son 5 biti 0 lanıyor. Ve elimizde 11 bit kalıyor.
	   * 0x07FF = 0b0000011111111111
	   * böylece ilk kanal için gelen veriyi elde etmiş olduk.
	   * ikinci kanal için; ilk kanalın high byte nın ilk üç bitini kullanmıştık. bu yüzden
	   * kullandığımız byte ları düşürmek için ilk kanalın high byte nı 3 bit sağa kaydıracağız.
	   * ilk kanalın high byte ının 3 bit sağa kaydırılmış hali ikinci kanalın low bytı olacak.
	   * ikinci kanalın low byte ı 5 bit değerinde. bize 6 bit daha lazım. onuda sonraki gelen
	   * byte ı 5 bit sola kaydırarak elde edeceğiz.ve bu iki değeri orladık. sonraki gelen byte ı 5 bit sola kaydırarak
	   * low byte için 5 bit yer acmıs olduk. o yeride yukarıda anlattığım 5 bit ile doldurduk. sonra 0x07FF ile çarparak
	   * ilk 11 bit haric kalan bütün bitleri 0 ladık.
	   * diğer kanallarda aynı mantık ile hesaplanıyor.
 	   */
  	  channels[0]  = ((sbusData[1]|sbusData[2]<< 8) & 0x07FF);
  	  channels[1]  = ((sbusData[2]>>3|sbusData[3]<<5) & 0x07FF);
  	  channels[2]  = ((sbusData[3]>>6|sbusData[4]<<2|sbusData[5]<<10) & 0x07FF);
  	  channels[3]  = ((sbusData[5]>>1|sbusData[6]<<7) & 0x07FF);
  	  channels[4]  = ((sbusData[6]>>4|sbusData[7]<<4) & 0x07FF);
  	  channels[5]  = ((sbusData[7]>>7|sbusData[8]<<1|sbusData[9]<<9) & 0x07FF);
  	  channels[6]  = ((sbusData[9]>>2|sbusData[10]<<6) & 0x07FF);
  	  channels[7]  = ((sbusData[10]>>5|sbusData[11]<<3) & 0x07FF);
  	  channels[8]  = ((sbusData[12]|sbusData[13]<< 8) & 0x07FF);
  	  channels[9]  = ((sbusData[13]>>3|sbusData[14]<<5) & 0x07FF);

  	  // gelen değerler 1000 ile 2000 arasına ölçeklendiriliyor.
  	  channels[0] = map(channels[0],240,1807,1000,2000);
  	  channels[1] = map(channels[1],240,1807,1000,2000);
  	  channels[2] = map(channels[2],240,1807,1000,2000);
  	  channels[3] = map(channels[3],240,1807,1000,2000);
  	  channels[4] = map(channels[4],240,1807,1000,2000);
  	  channels[5] = map(channels[5],240,1807,1000,2000);
  	  channels[6] = map(channels[6],240,1807,1000,2000);
  	  channels[7] = map(channels[7],240,1807,1000,2000);
  	  channels[8] = map(channels[8],240,1807,1000,2000);
  	  channels[9] = map(channels[9],240,1807,1000,2000);

  	  // pid error hesaplamaları için r p y değerlerinin açıya çevriliyor.
  	  a_channels[0] = map(channels[0],1000,2000,-45,45);
  	  a_channels[1] = map(channels[1],1000,2000,-45,45);
  	  a_channels[2] = map(channels[3],1000,2000,-45,45);



  }

  	  cnt = 1; /*
 	  	  		* bu değişken oluşturduğum failsafe algoritmasını bir parçası. Alıcıyla verici arasında iletişim
  	  	  		* koptuğunda, bu fonksiyon çağırılmıyor. ve cnt değeri sürekli olarak failsafe() fonksiyonunun
  	  	  		* başında belirlediğim 0 değerini alıyor.(1)* Ve failsafe değeri artmaya başlıyor.(2)* failsafe değeri belli
  	  	  		* bir değere ulaştığında failsafe modu devreye giriyor(3)* ve motorlar duruyor.(4)*
  	  	  		* iletişim sağlandığında ve sbus data frame başlığı 15 ise
  	  	  		* bu değer 1 oluyor. ve failsafe değeri 0 lanıyor.(5)*
  	  	  		*/
  	  /*
  	   * sbus başlığı 15 değilse bi önceki değerler geçerli olur. eğer kumanda bozulursa ve
  	   * sbus başlığı sürekli olarak 15 den farklı ise(böyle birşey mümkünmü bilmiyorum) failsafe durumuna girmeden
  	   * kumanda bozulmadan hemen önceki gönderilen değerler program tarafından sanki yeni değermiş
  	   * gibi algılanacak ve failsafe durumuna girmeyecek. Bunu engellemek için aşağıda bir if bloğu daha yazdım.
  	   */
  	if(sbusData[0] != SBUS_VERIFICATION){
  		cnt = 0;
  	}
}
