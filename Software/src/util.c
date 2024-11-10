#include "util.h"

/*
 * Arrayin icindeki birbirine en yakin elemanlarin ortalamasini al 
 */
bool calculateFilteredRaw(uint16_t *adc_raw_arr, uint16_t *adc_filtered_raw, uint8_t size) {
    uint32_t totalSum = 0;
    uint8_t nearResult = 0;
    uint8_t totalSumCounter = 0;



    for ( uint8_t i = 0; i < size; i++ ) {
        totalSum += adc_raw_arr[i];
    }

    uint32_t tempHystersis = totalSum / ( size * 100 );  //ortalamanin yuzde biri hysteresis
    int16_t hystersis = 0;
    if ( tempHystersis > 100 )  hystersis = 100;
    else                        hystersis = (int16_t)tempHystersis;
    if ( hystersis < 10 )       hystersis = 10;

    totalSum = 0;
    nearResult = 0;
    totalSumCounter = 0;
    for ( uint8_t i = 0; i < size; i++ ) {
        nearResult = 0;
        for ( uint8_t j = 0; j < size; j++ ) {
            if ( checkValuesNearWithHystresis(adc_raw_arr[i], adc_raw_arr[j], hystersis)) {
                nearResult++;
            }
            if ( nearResult >= size - 2 ) {
                totalSum += adc_raw_arr[i];
                totalSumCounter++;
                break;
            }
        }
    }
    if ( size - totalSumCounter > 3 ) {
        return false;
    }
    *adc_filtered_raw = totalSum / totalSumCounter;

    return true;
}


/**
 * @{value} degeri @{refValue} degerinin hysteresis araliginda mi?  
*/
bool checkValuesNearWithHystresis(int32_t value, int32_t refValue, int16_t hysteresis) {
    return value > ( refValue - hysteresis ) && value < ( refValue + hysteresis );
}
