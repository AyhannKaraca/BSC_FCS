/*
 * map.c
 *
 *  Created on: Sep 7, 2021
 *      Author: Ayhan Karaca
 *
 *
 *      NOTLAR:
 *      Bu bölümde arduino da bulunan map fonksiyonunu yazdım.
 *
 */

#include "map.h"

/*
 *1000 ile 2000i -20 ile 20 arasinda bir degere donustur.
 *degerim 1000 ise -20 olacak. 2000 ise 20 olacak. 1500 ise 0 olur.
 *1000 deger araligini 40 birime sigdiracaz.                              -20    -19,84  ~0,04~  -19,80                  0    2,44  ~0,04~  2.48                      20
 *40/1000= 0.04 --> value-min_value*0.04+mapped_min_value.                  <-------.--------------.---------------------.-----.-------------.------------------------>
 *yani deger 1001 ise value = -19,96 olur.                               1000     1004           1005                  1500  1561           1562                      2000
 *örnek olarak:
 *value=1250
 *value - min_value * ((mapped_max_value-mapped_min_value)/max_value-min_value)+mapped_min_value
 *1250-1000 * ((20 - (-20))/2000-1000)+ (-20)
 *drone projesinde value = rc_values and z = output angle  olacak
 */

int16_t map(int16_t value, int16_t min_value, int16_t max_value, int16_t mapped_min_value, int16_t mapped_max_value){

	if(min_value<=value&&max_value>=value){
		mapped_value = (value - min_value) * (mapped_max_value-mapped_min_value)/(max_value-min_value)+(mapped_min_value);
		return mapped_value;
}
	else
	return 0;
}


