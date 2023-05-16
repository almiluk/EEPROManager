#include "EEPROMReaderWriter.h"

#include <EEPROM.h>


void EEPROMReaderWriter::readBytes(const uint16_t addr, uint8_t *data, const uint16_t size) const {
	for (uint16_t i = 0; i < size; i++) {
        data[i] = EEPROM.read(addr + i);
    }
}

#if defined(ESP8266)

// EEPROMReaderWriterESP

void EEPROMReaderWriterESP::scheduleUpdate(UpdateInfo upd) {
	prepareUpdateBytes(upd);
	if (++updateCounter >= updateThreshold){
		updateCounter = 0;
		Serial.println(upd.Addr);
		commitUpdate();
	}
}

void EEPROMReaderWriterESP::updateNow(UpdateInfo upd) {
	updateBytes(upd);
}

void EEPROMReaderWriterESP::prepareUpdateBytes(UpdateInfo upd) {
	for (uint16_t i = 0; i < upd.Size; i++) {
		EEPROM.write(upd.Addr + i, upd.Data[i]);
	}
}

void EEPROMReaderWriterESP::commitUpdate() {
	EEPROM.commit();
}

void EEPROMReaderWriterESP::updateBytes(UpdateInfo upd) {
	prepareUpdateBytes(upd);
	commitUpdate();
}

#else

//EEPROMReaderWriterAVR

void EEPROMReaderWriterAVR::scheduleUpdate(UpdateInfo upd) {
	updateNow(upd);
}

void EEPROMReaderWriterAVR::updateNow(UpdateInfo upd) {
	updateBytes(upd);
}

void EEPROMReaderWriterAVR::updateBytes(UpdateInfo upd) {
	for (uint16_t i = 0; i < upd.Size; i++) {
		EEPROM.update(upd.Addr + i, upd.Data[i]);
	}
}

#endif
