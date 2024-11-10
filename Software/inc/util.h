#ifndef UTIL_H
#define UTIL_H

#include "defines.h"

bool calculateFilteredRaw(uint16_t *adc_raw_arr, uint16_t *adc_filtered_raw, uint8_t size);
bool checkValuesNearWithHystresis(int32_t value, int32_t refValue, int16_t hysteresis);


#endif // UTIL_H
