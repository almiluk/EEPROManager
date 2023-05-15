/*
    EEPROM manager - The library for simplifying the work with EEPROM
    GitHub: https://github.com/almiluk/EEPROManager
    Aleksandr Lukianov, almiluk@gmial.com
    MIT License
*/

#ifndef EEPROMANAGER_H
#define EEPROMANAGER_H

#define CONTROL_VALUE 0xABCD
#define DEBUG_EEPROM

#ifdef DEBUG_EEPROM
    #define DEBUG_PRINT(value) Serial.print(value)
    #define DEBUG_PRINTLN(value) Serial.println(value)
#else
    #define DEBUG_PRINT(value)
    #define DEBUG_PRINTLN(value)
#endif

#include <Arduino.h>
#include <EEPROM.h>
#include <CRC32.h>

/** EEPROM managing class.*/
class EEPROManager {
public:
    /** EEPROM variable.
     * 
     * @tparam T The type of the user data to be stored in EEPROM.
    */
    template<class T>
    class EEPROMVar {
    public:
        EEPROMVar(const char* path, T default_value);

        EEPROMVar<T>& operator=(const T new_value);
        operator T&() const;
    private:
        uint16_t dataAddr;  /**< The address where user data is stored.*/
        T data;             /**< The user data to be stored.*/
    };

private:
    /** Metainformation about an EEPROM variable for storing in EEPROM.*/
    struct VariableInfo {
        uint32_t PathHash = 0;      /**< The hash (basically CRC32) of variable path.*/
        uint16_t DataSize = 0;      /**< The size of stored user's data.*/
        uint16_t NextVarAddr = 0;   /**< The EEPROM address of the next variable info.*/

        VariableInfo() {};

        VariableInfo(uint32_t path_hash, uint16_t data_size, uint16_t next_var_addr) {
            PathHash = path_hash;
            DataSize = data_size;
            NextVarAddr = next_var_addr;
        };
    };

    /** General metainformation about EEPROM for storing in EEPROM.*/
    struct MetaData {
        uint16_t controlValue = 0;      /**< The special value allowing you to determine if the EEPROManager is initialized and stores any variables.*/
        int32_t compileTimestamp = 0;   /**< The unix timestamp of the firmware compilation moment. It's used to clear variables on reflashing.*/
    };

    /** Initialize manager.
     *  Read information about stored data from EEPROM 
     *  or write that information if it's the first run.
    */
    static void init();

    /** Get Variable by its path(name) or create a new variable. 
     *  If error occurs, returns 0.
     * 
     *  @param path The Path of the Variable.
     *  @param data The pointer to write data from the EEPROM to, if such variable exists. The pointer to data be written to the EEPROM, if such variable does not exist.
     *  @return The EEPROM address of Variable's data.
    */
    template<class T>
    static uint16_t getOrAddVar(const char* path, T* data);

    /** Get Variable's address and data by its path(name). 
     *  If Variable with that path doesn't exist, returns 0.
     * 
     *  @param path The Path of the Variable.
     *  @param data The pointer to write data from the EEPROM to.
     *  @return The EEPROM address of Variable's data.
    */
    template<class T>
    static uint16_t getVar(const char* path, T* data);

    /** Add to EEPROM new variable and return its address.
     *  If there is no enough space in the EEPROM, returns 0.
     *  NOTE: This method DOES NOT check if a variable with the same name already exists.
     * 
     *  @param path The Path of the Variable.
     *  @param data The pointer to the new data to be written.
     *  @return The EEPROM address of Variable's data.
    */
    template<class T>
    static uint16_t addVar(const char* path, const T* data);

    /** Update data of custom types in EEPROM.
     * 
     *  @param addr The address to update.
     *  @param data The new data to be written.
    */
    template<class T>
    static void updateValue(uint16_t addr, T data);

    /** Read data of custom types from EEPROM.
     * 
     *  @param addr The address to read from.
     *  @return Read data.
    */
    template<class T>
    static T readValue(const uint16_t addr);

    /** Update data in EEPROM.
     * 
     *  @param addr The address to update.
     *  @param data Pointer to the new data to be written.
     *  @param size The size of the data to be written.
    */
    static void updateBytes(const uint16_t addr, const uint8_t* data, const uint16_t size);

    /** Read data in EEPROM.
     * 
     *  @param addr The address to read from.
     *  @param data Pointer to write read data.
     *  @param size The size of the data to be read.
    */
    static void readBytes(const uint16_t addr, uint8_t* data, const uint16_t size);


    /** Get variable data address by its info address.
     * 
     *  @param var_info_addr The address of the variable info.
     *  @return The address of the variable data.
    */
    static uint16_t getDataAddr(const uint16_t var_info_addr);

    /** Get the address to write the new variable info to.
     * 
     * @return The address to write the new variable info to.
    */
    static uint16_t getNewVarInfoAddr();

    /** Set to meta info the information about whether at least one variable exists.
     * 
     * @param vars_exist The value indicating the existence of variables.
    */
    static void setVariablesExistence(bool vars_exist);

    static const uint16_t varMetaDataSize = sizeof(VariableInfo);
    static const uint16_t startAddr = 0;        /**< The initial EEPROM address for managing data.*/
    //static uint16_t firstVarInfoAddr;         /**< The address of the first variable info.*/
    static uint16_t lastVarInfoAddr;            /**< The address of the VariableInfo of the last written variable.*/
    static VariableInfo lastVarInfo;            /**< The VariableInfo of the last written variable.*/
    static bool isInited;                       /**< The value indicating whether this class is initialized.*/        

    #define FIRST_VAR_ADDR startAddr + sizeof(MetaData); // 2 - sizeof control value, 1 - size of variable existence value
};


// EEPROManager

template <class T>
inline uint16_t EEPROManager::getOrAddVar(const char* path, T* data) {
    // check if it's the first run
    if (!isInited) {
        init();
    }

    uint16_t addr = 0;

    if (lastVarInfoAddr != 0) {
        // at least one variable exists
        addr = getVar(path, data);
    }

    if (addr == 0) {
        addr = addVar(path, data);
    }

	return addr;
}

template <class T>
inline uint16_t EEPROManager::getVar(const char *path, T *data) {
    uint16_t addr = FIRST_VAR_ADDR; 

    uint32_t path_hash = CRC32::calculate(path, strlen(path));

    DEBUG_PRINT("Searching for "); DEBUG_PRINTLN(path);

    while(addr != 0) {
        DEBUG_PRINT("Checked address "); DEBUG_PRINTLN(addr);

		VariableInfo var_info = readValue<VariableInfo>(addr);
		if (var_info.PathHash == path_hash && var_info.DataSize == sizeof(T)) {
            DEBUG_PRINT("Found on address "); DEBUG_PRINTLN(addr);

            uint16_t data_addr = getDataAddr(addr);
            if (data) {
                *data = readValue<T>(data_addr);
                DEBUG_PRINTLN("Read data:"); DEBUG_PRINTLN(*data);
                return data_addr;
            }
        }
        addr = var_info.NextVarAddr;
    }

	return 0;
}

template <class T>
inline uint16_t EEPROManager::addVar(const char *path, const T *data) {
    uint16_t info_addr = getNewVarInfoAddr();
    uint32_t path_hash = CRC32::calculate(path, strlen(path));

    if (info_addr + sizeof(T) >= (uint16_t)EEPROM.length()) {
        // not enough memory
        DEBUG_PRINT("Not enough memory, addr: "); DEBUG_PRINT(info_addr);
        DEBUG_PRINT("data len: "); DEBUG_PRINTLN(sizeof(T));
        return 0;
    }

    VariableInfo var_info(path_hash, sizeof(T), 0);
    // write new variable info
    updateValue(info_addr, var_info);


    if(lastVarInfoAddr != 0) {
        // it's the first variable
        // update previous last variable info to write the address of the new variable info to it
        // if it is the first variable => lastVarInfoAddr == 0 => EEPROM data won't be updated by updateValue()
        lastVarInfo.NextVarAddr = info_addr;
        updateValue(lastVarInfoAddr, lastVarInfo);
    } else {
        setVariablesExistence(true);
        DEBUG_PRINTLN("The first variable written.");
    }

    // save new variable info as the last one
    lastVarInfoAddr = info_addr;
    lastVarInfo = var_info;

    // write default value
    uint16_t data_addr = getDataAddr(info_addr);
    updateValue(data_addr, *data);

    DEBUG_PRINT("Added the new variable on address "); DEBUG_PRINTLN(info_addr);

    return data_addr;
}

template<class T> 
inline void EEPROManager::updateValue(const uint16_t addr, const T data) {
    if (addr != 0) {
	    updateBytes(addr, (uint8_t *)&data, sizeof(data));
    }
}

template<> 
inline void EEPROManager::updateValue<EEPROManager::MetaData>(const uint16_t addr, const MetaData data) {
    updateBytes(addr, (uint8_t *)&data, sizeof(data));
}

template <class T>
inline T EEPROManager::readValue(const uint16_t addr) {
    uint8_t* buf[sizeof(T)];
    readBytes(addr, (uint8_t*)buf, (uint16_t)sizeof(T));
    T data = *((T*)buf);
	return data;
}

// EEPROManager::EEPROMVar

template <class T>
inline EEPROManager::EEPROMVar<T>::EEPROMVar(const char* path, const T default_value) {
    data = default_value;
    dataAddr = EEPROManager::getOrAddVar(path, &data);
}

template <class T>
inline EEPROManager::EEPROMVar<T>& EEPROManager::EEPROMVar<T>::operator=(const T new_value) {
	data = new_value;
    EEPROManager::updateValue(dataAddr, data);
    return *this;
}

template <class T>
inline EEPROManager::EEPROMVar<T>::operator T&() const {
    return data;
}

//template<class T>
//class EEPROMVar : public EEPROManager::EEPROMVar<T> {};

template<class T>
using EEPROMVar = EEPROManager::EEPROMVar<T>;

#endif
