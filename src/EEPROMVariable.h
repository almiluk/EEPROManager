#ifndef EEPROM_VAR_H
#define EEPROM_VAR_H

#include <Arduino.h>
#include "EEPROManager.h"

/** EEPROM variable.
 * 
 * @tparam T The type of the user data to be stored in EEPROM.
*/
template<class T>
class EEPROMVar {
public:
	EEPROMVar(const char* path, T default_value);

	/**
	 * @brief Set new value to the variable and schedule its update in EEPROM.
	 * 
	 * @param new_value 
	 */
	void Update(T new_value);

	/**
	 * @brief Schedule update this variable value in EEPROM.
	 * 
	 */
	void Update();

	/**
	 * @brief Set new value to the variable and immediately update it in EEPROM.
	 * 
	 * @param new_value 
	 */
	void UpdateNow(T new_value);

	/**
	 * @brief Immediately update this variable value in EEPROM.
	 * 
	 */
	void UpdateNow();

	/**
	 * @brief Assign new value to the stored user data.
	 * 
	 * @param new_value The new value to be assigned.
	 * @return EEPROMVar<T>& The reference to this variable.
	 */
	EEPROMVar<T>& operator=(const T new_value);

	/**
	 * @brief Convert the variable to the user data, it stores.
	 * 
	 * @return The reference to the user data.
	 */
	operator T&();
private:
	uint16_t dataAddr;  /**< The address where user data is stored.*/
	T data;             /**< The user data to be stored.*/
};



template <class T>
inline EEPROMVar<T>::EEPROMVar(const char* path, const T default_value) {
    data = default_value;
    dataAddr = EEPROManager::GetOrAddVar(path, &data);
}

template <class T>
inline void EEPROMVar<T>::Update(T new_value) {
	data = new_value;
	Update();
}

template <class T>
inline void EEPROMVar<T>::Update() {
	EEPROManager::Update(dataAddr, data);
}

template <class T>
inline void EEPROMVar<T>::UpdateNow(T new_value) {
	data = new_value;
	UpdateNow();
}

template <class T>
inline void EEPROMVar<T>::UpdateNow()
{
	EEPROManager::UpdateNow(dataAddr, data);
}

template <class T>
inline EEPROMVar<T>& EEPROMVar<T>::operator=(const T new_value) {
    Update(new_value);
    return *this;
}

template <class T>
inline EEPROMVar<T>::operator T&() {
    return data;
}

#endif
