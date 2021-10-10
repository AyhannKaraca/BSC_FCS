/*
 * mpu6050.c
 *
 *  Created on: Sep 7, 2021
 *      Author: Ayhan Karaca
 *
 *      NOTLAR:
 *      Bu bölümde mpu6050 gyro sensörü için init fonksiyonu yazdım.
 *      init fonksiyonu içerisinde sensörün kalibrasyon ayarlarının yapıldığı
 *      ayrı bir fonksiyon daha var.
 *      Sensörden gelen işlenmemiş verilerin alındığı bir fonksiyon ve bu verilerin
 *      belirli hesaplamalarda kullanılan ve gerçek açı değerlerinin elde edildiği ayrı bir fonksiyon daha yazdım.
 */

#include "mpu6050.h"

//PB7 --> SDA

void MPU6050_Init(void){
	// MPU6050 Sensörünün WHO_AM_I registerını okudu.
	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADRS, WHO_AM_I, 1, &check, 1, 1000);
	// eğer check değeri doğrulama değeri ile eşitse diğer işlemelere geç.
	if (check == MPU6050_VERIFICATION){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, SET); // mpu6050 sensörünün sağlam çalıştığını işaret eden led.
		data = 0x00;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADRS, PWR_MGMT_1, 1, &data, 1, 1000); // wakeup the sensor.
	//	data = 0x07;
	//	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADRS, SMPRT_DIV, 1, &data, 1, 1000); // örnekleme hızı 1khz seçildi.
	//	data = 0x03;
	//	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADRS, CONFIG, 1, &data, 1, 1000); //filre ayarları yapıldı.
		data = 0x10;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADRS, ACCEL_CONFIG, 1, &data, 1,1000); // acc +-8g olarak ayarlama.
		data = 0x08;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADRS, GYRO_CONFIG, 1, &data, 1, 1000); // gyro +-500d/s olarak ayarlama.

		MPU6050_Calibration(); // kalibrasyon yapılıyor.
		time = gettick(); // karta güç verildiği andan itibaren geçen us cinsinden zaman, time değişkeni içine atıldı.
	}
}

/*
 * mpu6050 registerları 1 byte büyüklüğünde. mpu6050 datasheet bilgilerine göre
 * sensörden gelen acc ve gyro değerlerini 2 byte şeklinde okumak gerekiyor.
 * örnek olarak x ekseni için acc değerini 2 ayrı register adresinden okuduğumuz değerleri
 * birleştirerek elde edeceğiz. yani elde ettiğimiz sonuç 16 bit değerinde olacak.
 */
void MPU6050_raw_data(void){

	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADRS, DATA_STRT_REG, 1, mpu6050_datas, 14, 1000); /*
																						*belirttiğim adresten başlayarak ondan sonra sırayla gelen 13 adresteki değerleri
																						*mpu6050_datas dizi değişkeni içerisine doldur.
																						*/

	/*
	 * ilk gelen veri high byte sonra gelen veri low byte. high byteı 8 bit sola kaydırarak
	 * sonradan gelen low byte için 8 bitlik yer açmış olduk ve byteları | operatörü ile topladık.(değişken 16 bit).
	 */
	raw_acc_x = mpu6050_datas[0] << 8 | mpu6050_datas[1];
	raw_acc_y = mpu6050_datas[2] << 8 | mpu6050_datas[3];
	raw_acc_z = mpu6050_datas[4] << 8 | mpu6050_datas[5];

	raw_temperature = mpu6050_datas[6] << 8 | mpu6050_datas[7];
	temperature = (raw_temperature/340)+36.53; //datasheet de verilen sıcaklık hesaplama şekli.

	raw_gyro_x = mpu6050_datas[8] << 8 | mpu6050_datas[9];
	raw_gyro_y = mpu6050_datas[10] << 8 | mpu6050_datas[11];
	raw_gyro_z = mpu6050_datas[12] << 8 | mpu6050_datas[13];
}
/*
 * sensörden gelen verileri 2000 kere okuyup ortalamasını alan fonksiyon.
 * örnek olarak sensörü hareketsiz biçimde sabit ve her iki eksende
 * 0 derecede olacak şekilde bıraktığımız zaman değerlerin 0 gelmesini beklerken
 * belli bir aralıkta farklı değerler geldiğini görüyoruz.
 * Bu değerlerin ortalaması alınarak daha sonra okunacak olan ham verilerden
 * çıkartılacak olması değerleri her zaman 0 yapmasada 0 a yakınlaştıracak.
 * Acc için Z ekseninde kalibrasyon yapmayacağız. çünkü z eksenine daima etki eden bir kuvvet var; Yerçekimi.
 */
void MPU6050_Calibration(void){

	for (int i = 0; i < 2000; i++) {
		if (i % 125 == 0) {
			HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14); //kalibrasyon işlemi boyunca led toggle olsun.
		}
		MPU6050_raw_data();

		gyro_x_cal += raw_gyro_x;
		gyro_y_cal += raw_gyro_y;
		gyro_z_cal += raw_gyro_z;

		acc_x_cal += raw_acc_x;
		acc_y_cal += raw_acc_y;

		delay_us(1000);
	}
	/*
	 * toplanan değerler toplama sayısına bölünerek ortalama değerler kaydedildi.
	 * bundan sonraki her ham veri okumasında bu değerler okunan verilerden çıkarılacak.
	 */
	gyro_x_cal /= 2000;
	gyro_y_cal /= 2000;
	gyro_z_cal /= 2000;

	acc_x_cal/=2000;
	acc_y_cal/=2000;
}


	//gelen verilerin hesaplamalara sokulup gerçek açı değerlerinin elde edildiği fonsksiyon.
void MPU6050_Gyro_data(void){

	MPU6050_raw_data();

	raw_gyro_x -= gyro_x_cal;
	raw_gyro_y -= gyro_y_cal;
	raw_gyro_z -= gyro_z_cal;

	raw_acc_x -= acc_x_cal;
	raw_acc_y -= acc_y_cal;

	pitch += raw_gyro_x * 0.0000611;
    roll += raw_gyro_y * 0.0000611;

	pitch += roll * sin(raw_gyro_z * 0.000001066);
	roll -= pitch * sin(raw_gyro_z * 0.000001066);

	acc_total_vector = sqrt((raw_acc_x*raw_acc_x)+(raw_acc_y*raw_acc_y)+(raw_acc_z*raw_acc_z));
	angle_pitch_acc = asin((float)raw_acc_y/acc_total_vector)* 57.296;
	angle_roll_acc = asin((float)raw_acc_x/acc_total_vector)* -57.296;

	//Eğer acc ı kalibre ettiysen bu iki satıra ihtiyacın yok.
	/*
	angle_pitch_acc -= 0;
	angle_roll_acc -= 0;
	*/

	if(set_gyro_angles){
	  pitch = pitch * 0.9996 + angle_pitch_acc * 0.0004;
	  roll = roll * 0.9996 + angle_roll_acc * 0.0004;
	  }
	else{
		pitch = angle_pitch_acc;
		roll = angle_roll_acc;
		set_gyro_angles = true;
		}

	    angle_roll_output = angle_roll_output * 0.9 + pitch * 0.1;
		angle_pitch_output = angle_pitch_output * 0.9 + roll * 0.1;
		angle_yaw_output += raw_gyro_z * 0.0000611;

		if(angle_pitch_output < 0.001 && angle_pitch_output > -0.001)
			angle_pitch_output = 0.00;

		if(angle_roll_output < 0.001 && angle_roll_output > -0.001)
			angle_roll_output = 0.00;

		if(angle_yaw_output < 0.001 && angle_yaw_output > -0.001)
			angle_yaw_output = 0.00;

	while(gettick() - time < 4000);
	time = gettick();
}
