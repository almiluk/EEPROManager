// For arduino only, not for ESP
#define DEBUG_EEPROM

#include <EEPROMVariable.h>


void setup() {
    Serial.begin(115200);

    EEPROMVar<uint16_t> boot_cnt("boot_cnt", 1);

    Serial.println(boot_cnt);


    boot_cnt = boot_cnt + 1;

    //EEPROM.put(0, "000000000000000000000000000");
}

void loop() {

}
