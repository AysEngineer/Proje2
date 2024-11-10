#include "ntc.h"

#define NTC_ARRAY_SIZE                          240
#define NTC_ARRAY_ONE_ROW_COUNT                 20
#define NTC_MAX_ERROR_COUNT                     15
#define NTC_ADC_SAMPLE_COUNT                    6

//-20 derece
#define NTC_MAX_READ_VALUE                      3863
//120 derece
#define NTC_MIN_READ_VALUE                      243


const uint16_t ntcArr[NTC_ARRAY_SIZE] = {
    3696, 3687, 3677, 3667, 3656, 3646, 3635, 3624, 3613, 3602, 3591, 3579, 3567, 3555, 3543, 3531, 3518, 3505, 3492, 3479,      //-9.5, -9   ...... -0.5,  0
    3466, 3452, 3438, 3425, 3410, 3396, 3382, 3367, 3352, 3337, 3322, 3306, 3291, 3275, 3259, 3243, 3226, 3210, 3193, 3176,     //0.5,  1    ...... 9.5,  10
    3159, 3142, 3124, 3107, 3089, 3071, 3053, 3035, 3016, 2998, 2979, 2960, 2941, 2922, 2903, 2883, 2864, 2844, 2824, 2805,     //10.5, 11   ...... 19.5, 20
    2785, 2765, 2744, 2724, 2704, 2683, 2663, 2642, 2622, 2601, 2580, 2559, 2538, 2517, 2496, 2475, 2454, 2433, 2412, 2391,     //20.5, 21   ...... 29.5, 30
    2369, 2348, 2327, 2306, 2284, 2263, 2242, 2221, 2200, 2178, 2157, 2136, 2115, 2094, 2073, 2052, 2031, 2010, 1990, 1969,     //30.5, 31   ...... 39.5, 40
    1948, 1928, 1907, 1887, 1867, 1846, 1826, 1806, 1786, 1766, 1747, 1727, 1708, 1688, 1669, 1650, 1631, 1612, 1593, 1574,     //40.5, 41   ...... 49.5, 50
    1556, 1537, 1519, 1501, 1483, 1465, 1447, 1430, 1412, 1395, 1378, 1361, 1344, 1327, 1311, 1294, 1278, 1262, 1246, 1230,     //50.5, 51   ...... 59.5, 60
    1215, 1199, 1184, 1169, 1154, 1139, 1124, 1109, 1095, 1081, 1067, 1052, 1039, 1025, 1012, 998 , 985 , 972 , 959 , 946 ,     //60.5, 61   ...... 69.5, 70
    934 , 921 , 909 , 897 , 885 , 873 , 861 , 850 , 838 , 827 , 816 , 805 , 794 , 783 , 772 , 762 , 752 , 741 , 731 , 721 ,     //70.5, 71   ...... 79.5, 80
    712 , 702 , 692 , 683 , 673 , 664 , 655 , 646 , 637 , 629 , 620 , 612 , 603 , 595 , 587 , 579 , 571 , 563 , 555 , 548 ,     //80.5, 81   ...... 89.5, 90
    540 , 533 , 526 , 518 , 511 , 504 , 497 , 491 , 484 , 477 , 471 , 464 , 458 , 452 , 446 , 439 , 434 , 428 , 422 , 416 ,     //90.5, 91   ...... 99.5, 100
    410 , 405 , 399 , 394 , 389 , 383 , 378 , 373 , 368 , 363 , 358 , 353 , 348 , 344 , 339 , 335 , 330 , 326 , 321 , 317       //100.5, 101 ...... 109.5, 110
};



typedef struct {
    //Ilk filtrelenmis sicaklik hesaplandi mi?
    bool firstTempCalculated;
    //Ntc hata sayisi max NTC_MAX_ERROR_COUNT olabilir.
    uint8_t errorCounter;

    //Her adc olcumunda bir artar ve NTC_ADC_SAMPLE_COUNT ulastiginda adcFilteredRaw hesaplanir.
    uint8_t adcSampleCount;
    //Her adc olcumunda eleman eklenir.
    uint16_t adcRaw[NTC_ADC_SAMPLE_COUNT];
    //adcRaw arrayinin icinde toplanan degerlerin filtrelenmis degeri
    uint16_t adcFilteredRaw;

    //adcFilteredRaw'den olculen sicaklik degeri filteredTemp degerinden buyukse 1 artar
    uint8_t upperCounter;
    //adcFilteredRaw'den olculen sicaklik degeri filteredTemp degerinden kucukse 1 azal�r
    uint8_t lowerCounter;
    
    //Diger kutuphanelerin kullanabilecegi gercek filtrelenmis sicaklik degeri
    int16_t filteredTemp;

    //15 sn boyunca gecerli bir sicaklik okuyamazsa hata verir.
    SystemTick validTempEndTime;
} _Ntc;

//Systemde bir tane ntc var.
_Ntc _ntc;

void _ntc_calculate_filtered_temp(_Ntc *ntc);

void _ntc_reset_to_defaults(_Ntc *ntc) {
    ntc->firstTempCalculated = false;
    ntc->errorCounter = 0;
    ntc->adcSampleCount = 0;
    ntc->upperCounter = 0;
    ntc->lowerCounter = 0;
    ntc->filteredTemp = 0;
    copyAndIncSystemTick(&(ntc->validTempEndTime), SECONDS(15));
}

void ntcInit() {
    _ntc_reset_to_defaults(&_ntc);
}

bool isNTCReadyToRead() {
    return _ntc.errorCounter < NTC_MAX_ERROR_COUNT && _ntc.firstTempCalculated == true;
}

bool isNTCOK() {
    return _ntc.errorCounter < NTC_MAX_ERROR_COUNT || isSystemTickElapsed(_ntc.validTempEndTime);
}

int16_t getNTCTemp() {
    return _ntc.filteredTemp;
}

uint16_t getNTCFilteredRaw() {
	return _ntc.adcFilteredRaw;
}


/**
 * Returns true if error occured
*/
bool _ntc_update_error_counter(_Ntc *ntc, uint16_t adc_raw) {
    if ( adc_raw >= NTC_MAX_READ_VALUE || adc_raw <= NTC_MIN_READ_VALUE ) {
        if ( ntc->errorCounter < NTC_MAX_ERROR_COUNT ) {
            ntc->errorCounter++;
        }
        return true;
    }
    return false;
}

void insertNTCAdcRaw(uint16_t adc_raw) {
    if ( _ntc_update_error_counter(&_ntc, adc_raw) ) {
        _ntc.adcSampleCount = 0;
        return;
    }
    _ntc.adcRaw[_ntc.adcSampleCount++] = adc_raw;
    if ( _ntc.adcSampleCount == NTC_ADC_SAMPLE_COUNT ) {
        if ( calculateFilteredRaw(_ntc.adcRaw, &_ntc.adcFilteredRaw, NTC_ADC_SAMPLE_COUNT) == true ) {
            _ntc_calculate_filtered_temp(&_ntc);
            copyAndIncSystemTick(&_ntc.validTempEndTime, SECONDS(15));
        }
        _ntc.adcSampleCount = 0;
    }
}

void _ntc_calculate_filtered_temp(_Ntc *ntc) {
    uint8_t adc_array_range = 0xFF;
    //Array tablosuna gore hangi aralikta oldugunu hesapla.
    if ( ntc->adcFilteredRaw < NTC_MAX_READ_VALUE && ntc->adcFilteredRaw > 3696 )  adc_array_range = 12;
    else if ( ntc->adcFilteredRaw <= 3696 && ntc->adcFilteredRaw >= 3479 )      adc_array_range = 0;
    else if ( ntc->adcFilteredRaw <= 3478 && ntc->adcFilteredRaw >= 3176 )      adc_array_range = 1;
    else if ( ntc->adcFilteredRaw <= 3175 && ntc->adcFilteredRaw >= 2805 )      adc_array_range = 2;
    else if ( ntc->adcFilteredRaw <= 2804 && ntc->adcFilteredRaw >= 2391 )      adc_array_range = 3;
    else if ( ntc->adcFilteredRaw <= 2390 && ntc->adcFilteredRaw >= 1969 )      adc_array_range = 4;
    else if ( ntc->adcFilteredRaw <= 1968 && ntc->adcFilteredRaw >= 1574 )      adc_array_range = 5;
    else if ( ntc->adcFilteredRaw <= 1573 && ntc->adcFilteredRaw >= 1230 )      adc_array_range = 6;
    else if ( ntc->adcFilteredRaw <= 1229 && ntc->adcFilteredRaw >= 946  )      adc_array_range = 7;
    else if ( ntc->adcFilteredRaw <= 945 && ntc->adcFilteredRaw >= 721  )       adc_array_range = 8;
    else if ( ntc->adcFilteredRaw <= 720 && ntc->adcFilteredRaw >= 548  )       adc_array_range = 9;
    else if ( ntc->adcFilteredRaw <= 547 && ntc->adcFilteredRaw >= 416  )       adc_array_range = 10;
    else if ( ntc->adcFilteredRaw <= 415 && ntc->adcFilteredRaw >= 317  )       adc_array_range = 11;
    else if ( ntc->adcFilteredRaw <= 316 && ntc->adcFilteredRaw > NTC_MIN_READ_VALUE )   adc_array_range = 13;  
    
    //Aralik bulunamadiysa hata durumuna gec
    if ( adc_array_range == 0xFF ) {
        if ( ntc->errorCounter < NTC_MAX_ERROR_COUNT ) {
            ntc->errorCounter++;
        }
        return;
    }
    ntc->errorCounter = 0;
    
    uint8_t adc_array_starting_number = adc_array_range * NTC_ARRAY_ONE_ROW_COUNT;
    uint8_t adc_array_ending_number = adc_array_starting_number + NTC_ARRAY_ONE_ROW_COUNT;
    uint16_t i = adc_array_starting_number;

    int16_t calc_temp = 0;
    if ( adc_array_range < 12 ) {
        //Array tablosundaki yerini bul
        for ( ; i < adc_array_ending_number; i++ ) {
            if ( ntc->adcFilteredRaw == ntcArr[i] ) {
                break;
            }
            if ( ntc->adcFilteredRaw > ntcArr[i] && i != 0 ) {
                if ( ntc->adcFilteredRaw - ntcArr[i] > ntcArr[i - 1] - ntc->adcFilteredRaw ) {
                    i--;
                }
                break;
            }                
        }
        //Sicaklik degerini hesapla.(Tum yazilimdaki sicaklik degerleri x2 seklindedir.)
        calc_temp = -19 + 1 * i;
    } else if ( adc_array_range == 12 ) {
        calc_temp = -20;
    } else if ( adc_array_range == 13 ) {
        calc_temp = 221;
    }

    //ilk sicaklik hesaplanmadiysa olculen degeri filteredTemp'e ata.
    if ( ntc->firstTempCalculated == false ) {
        ntc->firstTempCalculated = true;
        ntc->filteredTemp = calc_temp;
    }
    if ( calc_temp > ntc->filteredTemp ) { //Hesaplanan deger onceki degerden buyukse upperCounter'i arttir.
        if ( calc_temp > ntc->filteredTemp + 7 ) {
            ntc->filteredTemp = calc_temp;
            ntc->upperCounter = 0;
        } else if ( calc_temp > ntc->filteredTemp + 2 ) {
            ntc->upperCounter += 4;
        } else {
            ntc->upperCounter++;
        }
        ntc->lowerCounter = 0;
    } else if ( calc_temp < ntc->filteredTemp ) { //Hesaplanan deger onceki degerden kucukse lowerCounter'i arttir.
        if ( calc_temp + 7 < ntc->filteredTemp ) {
            ntc->filteredTemp = calc_temp;
            ntc->lowerCounter = 0;
        } else if ( calc_temp + 2 < ntc->filteredTemp ) {
            ntc->lowerCounter += 4;
        } else {
            ntc->lowerCounter++;
        }          
        ntc->upperCounter = 0;
    } else {
        ntc->lowerCounter = 0;
        ntc->upperCounter = 0;
    }

    //filteredTemp degerini duruma gore guncelle
    if ( ntc->upperCounter >= 4 ) { //normalde 4 debug icin 1 yazildi
        ntc->filteredTemp++;
        ntc->upperCounter = 0;
    } else if ( ntc->lowerCounter >= 4 ) {//normalde 4 debug icin 1 yazildi
        ntc->filteredTemp--;
        ntc->lowerCounter = 0;
    }


}
