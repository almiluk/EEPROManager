#include "EEPROManager.h"
#include <CRC32.h>

uint16_t EEPROManager::lastVarInfoAddr = 0;
bool EEPROManager::isInited = false;
EEPROManager::VariableInfo EEPROManager::lastVarInfo = EEPROManager::VariableInfo();

// FOR init()
//uint16_t EEPROManager::firstVarInfoAddr  = EEPROManager::startAddr + 2; // 2 - sizeof control value

void EEPROManager::init() {
    uint16_t control_value = readValue<uint16_t>(startAddr);

    if(control_value != CONTROL_VALUE) {
        // the very first run
        DEBUG_PRINTLN("The very first run");

        // write control value
        //updateValue(startAddr, (uint16_t)CONTROL_VALUE);

        control_value = CONTROL_VALUE;
        updateBytes(startAddr, (uint8_t*)&control_value, sizeof(startAddr));

        updateValue(startAddr + 2, (uint8_t)0);
    } else {
        DEBUG_PRINTLN("Not very first run");
        uint8_t variable_exists =  readValue<uint16_t>(startAddr + 2);

        if (variable_exists == VARIABLE_EXISTS_VALUE) {
            DEBUG_PRINTLN("Variables exists:");
            uint16_t addr = FIRST_VAR_ADDR;

            while(addr != 0) {
                lastVarInfo = readValue<VariableInfo>(addr);
                lastVarInfoAddr = addr;
                addr = lastVarInfo.NextVarAddr;
                DEBUG_PRINT(lastVarInfoAddr); DEBUG_PRINT(' ');
                DEBUG_PRINT(lastVarInfo.PathHash); DEBUG_PRINT(' ');
                DEBUG_PRINT(lastVarInfo.DataSize); DEBUG_PRINT(' ');
                DEBUG_PRINTLN(lastVarInfo.NextVarAddr);
            }
        }
    }

    isInited = true;
}

void EEPROManager::updateBytes(const uint16_t addr, const uint8_t *data, const uint16_t size)
{
	
    #if defined(ESP8266) || defined(ESP32)
        for(uint16_t i = 0; i < size; i++) {
            EEPROM.write(addr + i, data[i]);
        }
        EEPROM.commit();
    #else
        for(uint16_t i = 0; i < size; i++) {
            EEPROM.update(addr + i, data[i]);
        }
    #endif
}

void EEPROManager::readBytes(const uint16_t addr, uint8_t *data, const uint16_t size) {
    for (uint16_t i = 0; i < size; i++) {
        data[i] = EEPROM.read(addr + i);
    }
}

uint16_t EEPROManager::getDataAddr(const uint16_t var_info_addr) {
	return var_info_addr + varMetaDataSize;
}

uint16_t EEPROManager::getNewVarInfoAddr() {
    if (lastVarInfoAddr == 0) {
        // it's the first creating variable
        return FIRST_VAR_ADDR;
    }

	return getDataAddr(lastVarInfoAddr) + lastVarInfo.DataSize;
}
