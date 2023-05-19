#ifndef EEPROM_HELPERS_H
#define EEPROM_HELPERS_H

#include <stdint.h>
#include <time.h>


/** Get the firmware compilation time in unix time format. 
 * 
 *  @return The firmware compilation time in unix time format. 
*/
int32_t compilationUnixTime();

#endif
