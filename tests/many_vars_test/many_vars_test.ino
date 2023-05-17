#define DEBUG_EEPROM

#include <EEPROMVariable.h>
#include <EEPROM.h>


void setup() {
    Serial.begin(115200);

    EEPROMVar<uint16_t> boot_cnt1("boot_cnt1", 1);
	EEPROMVar<uint16_t> boot_cnt2("boot_cnt2", 2);
	EEPROMVar<uint16_t> boot_cnt3("boot_cnt3", 3);
	EEPROMVar<uint16_t> boot_cnt4("boot_cnt4", 4);

    Serial.println(boot_cnt1);
	Serial.println(boot_cnt2);
	Serial.println(boot_cnt3);
	Serial.println(boot_cnt4);


    boot_cnt1 = boot_cnt1 + 1;
	boot_cnt2 = boot_cnt2 + 1;
	boot_cnt3 = boot_cnt3 + 1;
	boot_cnt4 = boot_cnt4 + 1;

    //EEPROM.put(0, "000000000000000000000000000");
}

void loop() {

}
