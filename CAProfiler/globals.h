#ifndef GLOBALS_H
#define GLOBALS_H

#ifndef HIGHBYTE
    #define HIGHBYTE(x)  x >> 8 
#endif

#ifndef LOWBYTE
    #define LOWBYTE(x)   x & 0xFF 
#endif

#define FILETIME_TO_SECOND                      10000000 

#define MAX_INI_VALUE_CHAR                      512

#define FLAG_OS_WIN9X                           1
#define FLAG_OS_WINNT                           2
    
//#define _NO_AVAIL_CARD

enum APDU_TYPE_CASE {
	CASE1,	// 4 bytes total, header (CLA, INS, P1, P2), no LC, no LE
	CASE2,  // 5 bytes total, header (CLA, INS, P1, P2), no LC, LE
	CASE3,  // 5 + LC bytes total, header (CLA, INS, P1, P2), LC, data in (LC bytes), no LE
	CASE4   // 6 + LC bytes total, header (CLA, INS, P1, P2), LC, data in (LC bytes), LE
};

typedef struct CARDAPDU {
	unsigned char   cla;
	unsigned char   ins;
	unsigned char   p1;
	unsigned char   p2;
	unsigned char   lc;
	unsigned char   le;
	unsigned char   DataIn[256];
	unsigned char   DataOut[256];
	unsigned short  sw;
	APDU_TYPE_CASE  apduType;

	CARDAPDU() {
		clear();
	}
	void clear() {
		cla = 0;
		ins = 0;
		p1 = 0;
		p2 = 0;
		lc = 0;
		le = 0;
		memset(DataIn, 0, sizeof(DataIn));
		memset(DataOut, 0, sizeof(DataOut));
		sw = 0x9000;
		apduType = CASE3;
	}

} CARDAPDU;

typedef struct _INT_DATA_BLOB {
    int*    pData;
    DWORD   dwMaxLen;
    DWORD   dwActLen;

    _INT_DATA_BLOB(void) {
        pData = NULL;
        dwMaxLen = 0;
        dwActLen = 0;
    }

} INT_DATA_BLOB;

typedef struct _FLOAT_DATA_BLOB {
    float*  pData;
    DWORD   dwMaxLen;
    DWORD   dwActLen;

    _FLOAT_DATA_BLOB(void) {
        pData = NULL;
        dwMaxLen = 0;
        dwActLen = 0;
    }

} FLOAT_DATA_BLOB;

#define SLOT_ANY_AVAILABLE	-1

#endif