#include "helpers.h"

#include <string.h>
#include <time.h>

int32_t compilationUnixTime() {
	const char* strM = "JanFebMarAprMayJunJulAugSepOctNovDec";
	const char* sysT = __TIME__;
	const char* sysD = __DATE__;
	const int build_datetime[6]{ (sysT[6] - 48) * 10 + (sysT[7] - 48), (sysT[3] - 48) * 10 + (sysT[4] - 48), (sysT[0] - 48) * 10 + (sysT[1] - 48), (sysD[4] - 48) * 10 + (sysD[5] - 48), ((int)memmem(strM,36,sysD,3) + 3 - (int)&strM[0]) / 3 - 1, (sysD[9] - 48) * 10 + (sysD[10] - 48) + 130 };
	
	tm time;
	time.tm_sec = build_datetime[0];
	time.tm_min = build_datetime[1];
	time.tm_hour = build_datetime[2];
	time.tm_mday = build_datetime[3];
	time.tm_mon = build_datetime[4];
	time.tm_year = build_datetime[5];

	return mktime(&time);
}
