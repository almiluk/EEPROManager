#include "EEPROManager.h"
#include <CRC32.h>
#include "helpers.h"

uint16_t EEPROManager::lastVarInfoAddr = 0;
bool EEPROManager::isInited = false;
EEPROManager::VariableInfo EEPROManager::lastVarInfo = EEPROManager::VariableInfo();

// FOR init()
//uint16_t EEPROManager::firstVarInfoAddr  = EEPROManager::startAddr + 2; // 2 - sizeof control value

void EEPROManager::init() {
    auto meta_data = readValue<MetaData>(startAddr);
    int32_t current_compilation_time = compilationUnixTime();
	
    if(meta_data.ControlValue == CONTROL_VALUE && current_compilation_time == meta_data.CompileTimestamp) {
        DEBUG_PRINTLN("Not very first run. No variables.");
    } else if (meta_data.ControlValue == ~CONTROL_VALUE && current_compilation_time == meta_data.CompileTimestamp) {
        DEBUG_PRINTLN("Not very first run. Some variables exist.");

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
    } else {
        DEBUG_PRINTLN("The very first run.");

        // write control value
        //updateValue(startAddr, (uint16_t)CONTROL_VALUE);

        meta_data.ControlValue = CONTROL_VALUE;
        meta_data.CompileTimestamp = current_compilation_time;
        updateValue(startAddr, meta_data);
        DEBUG_PRINT("Compile timestamp: "); DEBUG_PRINTLN(meta_data.CompileTimestamp);
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

void EEPROManager::setVariablesExistence(bool vars_exist) {
    auto meta_data = readValue<MetaData>(startAddr);
    meta_data.ControlValue = vars_exist ? ~CONTROL_VALUE : CONTROL_VALUE;

    updateValue(startAddr, meta_data);
}
