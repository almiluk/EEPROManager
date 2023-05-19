#include "EEPROManager.h"

#include <CRC32.h>
#include "EEPROMReaderWriter.h"
#include "helpers.h"

uint16_t EEPROManager::lastVarInfoAddr = 0;
bool EEPROManager::isInited = false;
EEPROManager::VariableInfo EEPROManager::lastVarInfo = EEPROManager::VariableInfo();
EEPROMReaderWriter* EEPROManager::eepromRW = nullptr;


void EEPROManager::init() {
    #if defined(ESP8266) || defined(ESP32)
        EEPROM.begin(4096);
        eepromRW = new EEPROMReaderWriterESP();
    #else
        eepromRW = new EEPROMReaderWriterAVR();
    #endif

    
    int32_t current_compilation_time = compilationUnixTime();
    auto meta_data = readValue<MetaData>(startAddr);

    bool same_compilation_times = true;
    delay(5);

    #if !defined NOT_CLEAR_EEPROM_ON_FLASHING
        same_compilation_times = current_compilation_time == meta_data.CompileTimestamp;
    #endif
	
    if(meta_data.ControlValue == CONTROL_VALUE && same_compilation_times) {
        DEBUG_PRINTLN("Not very first run. No variables.");
    } else if (meta_data.ControlValue == (uint16_t)~CONTROL_VALUE && same_compilation_times) {
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
        UpdateNow(startAddr, meta_data);
        DEBUG_PRINT("Compile timestamp: "); DEBUG_PRINTLN(meta_data.CompileTimestamp);
    }

    isInited = true;
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

    UpdateNow(startAddr, meta_data);
}
