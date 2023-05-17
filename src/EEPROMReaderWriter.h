#ifndef EEPROM_READER_WRITER_H
#define EEPROM_READER_WRITER_H

#include <Arduino.h>

class EEPROMReaderWriter {

public:
	/**
	 * @brief Schedule the custom types data update.
	 * 
	 * @tparam T The type of the data to be updated.
	 * @param addr The address to update.
	 * @param data The new data to be written.
	 */
	template<class T>
	void ScheduleUpdate(const uint16_t addr, const T data);

	/**
	 * @brief Immediately update data of custom types in EEPROM.
	 * 
	 * @tparam T The type of the data to be updated.
	 * @param addr The address to update.
	 * @param data The new data to be written.
	 */
	template<class T>
	void UpdateNow(const uint16_t addr, const T data);

    /** Read data of custom types from EEPROM.
     * 
	 *  @tparam T The type of the data to be read.
     *  @param addr The address to read from.
     *  @return Read data.
    */
    template<class T>
    T ReadValue(const uint16_t addr) const;

protected:
	struct UpdateInfo {
		uint16_t Addr; /**< The address of the data for the update.*/
		uint8_t* Data; /**< The data to be written.*/
		uint16_t Size; /**< The size of the data.*/
	};

private:

	/**
	 * @brief Schedule the update of data in EEPROM.
	 * 
	 * @param upd The information about the update.
	 */
	virtual void scheduleUpdate(UpdateInfo upd) = 0;

	/**
	 * @brief Immediately update of data in EEPROM.
	 * 
	 * @param upd The information about the update.
	 */
	virtual void updateNow(UpdateInfo upd) = 0;


    /** Read data in EEPROM.
     * 
     *  @param addr The address to read from.
     *  @param data Pointer to write read data.
     *  @param size The size of the data to be read.
     */
    virtual void readBytes(const uint16_t addr, uint8_t* data, const uint16_t size) const;
};

template <class T>
inline void EEPROMReaderWriter::ScheduleUpdate(const uint16_t addr, const T data) {
	UpdateInfo upd = { addr, (uint8_t *)&data, sizeof(data)};
	scheduleUpdate(upd);
}

template <class T>
inline void EEPROMReaderWriter::UpdateNow(const uint16_t addr, const T data) {
	UpdateInfo upd = { addr, (uint8_t *)&data, sizeof(data)};
	updateNow(upd);
}

template <class T>
inline T EEPROMReaderWriter::ReadValue(const uint16_t addr) const {
	uint8_t* buf[sizeof(T)];
    readBytes(addr, (uint8_t*)buf, (uint16_t)sizeof(T));
    T data = *((T*)buf);
	return data;
}

#if defined(ESP8266)

class EEPROMReaderWriterESP : public virtual EEPROMReaderWriter {
private:
	/**
	 * @brief 
	 * 
	 * @param upd 
	 */
	virtual void scheduleUpdate(UpdateInfo upd) override;

	/**
	 * @brief 
	 * 
	 * @param upd 
	 */
	virtual void updateNow(UpdateInfo upd) override;

	/**
	 * @brief Update data in the EEPROM buffer (in RAM).
	 * 
	 * @param upd The information about the update.
	 */
	void prepareUpdateBytes(UpdateInfo upd); 

	/**
	 * @brief Write data from the buffer to the EEPROM.
	 * 
	 */
	void commitUpdate();

	/** Update data in EEPROM.
     * 
     *  @param upd The information about the update.
	 */
	void updateBytes(UpdateInfo upd);


	static const uint8_t updateThreshold = 3; 	/**< The number of update requests (scheduleUpdate calls), after which the update will be performed.*/
	uint8_t	updateCounter = 0;			/**< The number of update requests after the last update.*/
};

#else

class EEPROMReaderWriterAVR : public virtual EEPROMReaderWriter {
private:
	/**
	 * @brief 
	 * 
	 * @param upd 
	 */
	virtual void scheduleUpdate(UpdateInfo upd) override;

	/**
	 * @brief 
	 * 
	 * @param upd 
	 */
	virtual void updateNow(UpdateInfo upd) override;

    /** Update data in EEPROM.
     * 
     *  @param upd The information about the update.
     */
    void updateBytes(UpdateInfo upd);
};

#endif

#endif
