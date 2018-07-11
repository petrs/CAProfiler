#include "stdafx.h"
#include "PCSCMngr.h"

#include <string>
#include <iostream>
#include <map>
#include <list>
#include <iomanip>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const unsigned char APPLET_AID_VISA_PREFIX[]			= {0xa0, 0x00, 0x00, 0x00, 0x03};
const unsigned char APPLET_AID_MASTERCARD_PREFIX[]  	= {0xa0, 0x00, 0x00, 0x00, 0x04};
const unsigned char APPLET_AID_EMV_PREFIX[]				= {0xa0, 0x00, 0x00, 0x00, 0x42};
const unsigned char APPLET_AID_GEMALTO_PREFIX[]			= {0xa0, 0x00, 0x00, 0x00, 0x18};
const unsigned char APPLET_AID_SUN_PREFIX[]				= {0xa0, 0x00, 0x00, 0x00, 0x62};
const unsigned char APPLET_AID_PKCS15_PREFIX[]			= {0xa0, 0x00, 0x00, 0x00, 0x63};
const unsigned char APPLET_AID_MONEO_PREFIX[]			= {0xa0, 0x00, 0x00, 0x00, 0x69};
const unsigned char APPLET_AID_3GPP_ETSI_PREFIX[]		= {0xa0, 0x00, 0x00, 0x00, 0x87};
const unsigned char APPLET_AID_javacardforum_PREFIX[]	= {0xa0, 0x00, 0x00, 0x01, 0x32};
const unsigned char APPLET_AID_globalplatform_PREFIX[]  = {0xa0, 0x00, 0x00, 0x01, 0x51};
const unsigned char APPLET_AID_FIPS_PREFIX[]			= {0xa0, 0x00, 0x00, 0x01, 0x67};
const unsigned char APPLET_AID_NFCForum_PREFIX[]		= {0xD2, 0x76, 0x00, 0x00, 0x85};


const unsigned char APPLET_AID_GEMALTO_CM[]  = {0xA0, 0x00, 0x00, 0x00, 0x18, 0x43, 0x4D, 0x00};
const unsigned char APPLET_AID_GEMALTO_CM2[]  = {0xA0, 0x00, 0x00, 0x00, 0x18, 0x43, 0x4D};
const unsigned char APPLET_AID_JCOP_CM[]  = {0xa0, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00};


const unsigned char APPLET_AID_VISA_CAP[]  = {0xa0, 0x00, 0x00, 0x00, 0x03, 0x80, 0x02};
const unsigned char APPLET_AID_MASTERCARD[] = {0xa0, 0x00, 0x00, 0x00, 0x04, 0x80, 0x02};
const unsigned char APPLET_AID_MAESTRO[]   = {0xa0, 0x00, 0x00, 0x00, 0x04, 0x30, 0x60};
const unsigned char APPLET_AID_UNKNOWN2[]  = {0xa0, 0x00, 0x00, 0x01, 0x40, 0x80, 0x01};
const unsigned char APPLET_AID_VISA_CAP2[]  = {0xa0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10};


const unsigned char GENERATE_AC_INPUT_MASK__VISA[] =  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00};  
const unsigned char GENERATE_AC2_INPUT_MASK__VISA[] = {0x5a, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00};  

const unsigned char GENERATE_AC_INPUT_MASK__MAESTRO[] =  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  
const unsigned char GENERATE_AC2_INPUT_MASK__MAESTRO[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0x33, 0x80, 0x00, 0x00, 0x00, 0x00};  

enum CARD_ISSUER {
	ISSUER_UNKNOWN,
	ISSUER_VISA,
	ISSUER_MAESTRO,
	ISSUER_MASTERCARD
};
/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int String_SeparateHexValues(CString* pHexaString, CString separator) {
	int		status = STAT_OK;
	CString	newHexa;

    // EAT SPACES
	pHexaString->Replace(" ", "");
	for (int i = 0; i < pHexaString->GetLength(); i++) {
		if (i % 2 == 0) newHexa += separator;
		newHexa += pHexaString->GetAt(i);
	}

	*pHexaString = newHexa;

    pHexaString->TrimLeft(); pHexaString->TrimRight();

	return status;	
}

int BYTE_ConvertFromHexStringToArray(CString hexaString, BYTE* pArray, DWORD* pbArrayLen) {
    int         status = STAT_OK;
    DWORD       pos = 0;
    DWORD       pos2 = 0;
    CString     hexNum;
    DWORD       num;
    BYTE*       pTempArray = NULL;
    DWORD       tempArrayPos = 0;

    hexaString.TrimLeft(); hexaString.TrimRight();
    // SEPARATE HEX VALUES BY SPACE
    status = String_SeparateHexValues(&hexaString, " ");
    hexaString += " ";
    hexaString.GetLength();

    if (status == STAT_OK) {
        pTempArray = new BYTE[hexaString.GetLength()];
        memset(pTempArray, 0, hexaString.GetLength());

        pos = pos2 = 0;
        while ((pos = hexaString.Find(' ', pos2)) != -1) {
            hexNum = hexaString.Mid(pos2, pos - pos2);
            hexNum.TrimLeft(); hexNum.TrimRight();
            if (hexNum.GetLength() > 0) {
                num = strtol((LPCTSTR) hexNum, NULL, 16);
        
                if (num == 0xFF) pTempArray[tempArrayPos] = 0xFF;
                else pTempArray[tempArrayPos] = (BYTE) num & 0xFF;
                
                tempArrayPos++;
            }
            pos2 = pos + 1;
        }

        if (tempArrayPos > *pbArrayLen) {
            status = 1;
        }  
        else {
            memcpy(pArray, pTempArray, tempArrayPos);
        }
        *pbArrayLen = tempArrayPos;

        if (pTempArray) delete[] pTempArray;
    }

    return status;
}


int BYTE_ConvertFromArrayToHexString(const BYTE* pArray, DWORD pbArrayLen, CString* pHexaString) {
    int         status = STAT_OK;
    CString     hexNum;
    DWORD       i;

    *pHexaString = "";
    for (i = 0; i < pbArrayLen; i++) {
        hexNum.Format("%.2x", pArray[i]);
        hexNum += " ";

        *pHexaString += hexNum;
    }

    pHexaString->TrimRight(" ");

    return status;
}

CString BYTE_ConvertFromArrayToHexString(const BYTE* pArray, DWORD pbArrayLen) {
    int         status = STAT_OK;
    CString     hexaString;

	BYTE_ConvertFromArrayToHexString(pArray,pbArrayLen, &hexaString);

    return hexaString;
}

void SIMTest(CPCSCMngr* pScManager, const char* PIN, const char* Challenge) {
	int			status = STAT_OK;	
    CARDAPDU    apdu;
	CString		message;

    if (status == STAT_OK) {
	
		// SELECT
        apdu.cla = 0xa0;
        apdu.ins = 0xA4;
        apdu.p1 = 0x00;
        apdu.p2 = 0x00;
        apdu.le = 0x00;
        apdu.lc = 0x02;
		apdu.apduType = CASE3;

		apdu.DataIn[0] = 0x7f;
		apdu.DataIn[1] = 0x20;

		CPCSCMngr::PrintAPDU(&apdu, &message, TRUE, FALSE);
     	cout << endl << "-> " << message << endl;
		status = pScManager->ExchangeAPDU(&apdu);
		CPCSCMngr::PrintAPDU(&apdu, &message, FALSE);
		cout << "<- " << message << endl;

		if ((status & 0xff00) == 0x9f00) {
			// GET RESPONSE
			apdu.cla = 0xa0;
			apdu.ins = 0xc0;
			apdu.p1 = 0x00;
			apdu.p2 = 0x00;
			apdu.le = status & 0xff;
			apdu.apduType = CASE2;

			memset(apdu.DataIn, 0, sizeof(apdu.DataIn));
			
			CPCSCMngr::PrintAPDU(&apdu, &message, TRUE, FALSE);
     		cout << endl << "-> " << message << endl;
			if ((status = pScManager->ExchangeAPDU(&apdu)) == STAT_OK) {
				cout << "   GSM applet selected" << endl;
			}
			else {
				cout << "   ERROR: GSM applet not selected" << endl; 
			}
			CPCSCMngr::PrintAPDU(&apdu, &message, FALSE);
			cout << "<- " << message << endl;
		}
	}

	if (status == STAT_OK) {
		// VERIFY PIN 
        apdu.cla = 0xa0;
        apdu.ins = 0x20;
        apdu.p1 = 0x00;
        apdu.p2 = 0x01;
        apdu.le = 0x00;
        apdu.lc = 0x08;
		apdu.apduType = CASE3;

		memset(apdu.DataIn, 0xff, sizeof(apdu.DataIn));
		for (DWORD i = 0; i < strlen(PIN); i++) apdu.DataIn[i] = PIN[i];

		printf("PIN: %s\n", PIN);

		CPCSCMngr::PrintAPDU(&apdu, &message, TRUE, FALSE);
     	cout << endl << "-> " << message << endl;
		if ((status = pScManager->ExchangeAPDU(&apdu)) == STAT_OK) {
			cout << "   VERIFY PIN command OK" << endl;
		}
		else {
			cout << "   ERROR: VERIFY PIN command failed" << endl; 
		}
		CPCSCMngr::PrintAPDU(&apdu, &message, FALSE);
		cout << "<- " << message << endl;
/*
		// SELECT IMSI REQUEST
        apdu.cla = 0xa0;
        apdu.ins = 0xA4;
        apdu.p1 = 0x00;
        apdu.p2 = 0x00;
        apdu.le = 0x00;
        apdu.lc = 0x02;

		apdu.DataIn[0] = 0x6f;
		apdu.DataIn[1] = 0x07;

		pScManager->ExchangeAPDU(&apdu);


		// GET RESPONSE - read binary
        apdu.cla = 0xa0;
        apdu.ins = 0xc0;
        apdu.p1 = 0x00;
        apdu.p2 = 0x00;
        apdu.le = 0x00;
        apdu.lc = 0x0f;

		memset(apdu.DataIn, 0, sizeof(apdu.DataIn));
		// force length of the apdu - no Input data
		pScManager->ExchangeAPDU(&apdu, 5);
/**/
	}

	if (status == STAT_OK) {

		// RUN GSM ALG
        apdu.cla = 0xa0;
        apdu.ins = 0x88;
        apdu.p1 = 0x00;
        apdu.p2 = 0x00;
        apdu.le = 0x00;
        apdu.lc = 0x10;

		// GIVEN CHALLENGE
		memset(apdu.DataIn, 0, sizeof(apdu.DataIn));
		DWORD len = apdu.lc;
		printf("CHALLENGE: %s\n", Challenge);
		BYTE_ConvertFromHexStringToArray(Challenge, apdu.DataIn, &len);
		ASSERT(len == apdu.lc);

		CPCSCMngr::PrintAPDU(&apdu, &message, TRUE, FALSE);
     	cout << endl << "-> " << message << endl;
		status = pScManager->ExchangeAPDU(&apdu);
		CPCSCMngr::PrintAPDU(&apdu, &message, FALSE);
		cout << "<- " << message << endl;

		if ((status & 0xff00) == 0x9f00) {
			// GET RESPONSE - RUN GSM ALG
			apdu.cla = 0xa0;
			apdu.ins = 0xc0;
			apdu.p1 = 0x00;
			apdu.p2 = 0x00;
			apdu.le = status & 0xff;
			apdu.apduType = CASE2;

			CPCSCMngr::PrintAPDU(&apdu, &message, TRUE, FALSE);
     		cout << endl << "-> " << message << endl;
			if ((status = pScManager->ExchangeAPDU(&apdu)) == STAT_OK) {
				cout << "   GSM ALG SESSION KEY command OK" << endl;
			}
			else {
				cout << "   ERROR: GSM ALG SESSION KEY command failed" << endl; 
			}
			CPCSCMngr::PrintAPDU(&apdu, &message, FALSE);
			cout << "<- " << message << endl;
		}

		CString response;
		CPCSCMngr::BYTE_ConvertFromArrayToHexString(apdu.DataOut, 0x0c, &response);
		printf("RESPONSE: %s\n", response.GetString());
	}
}

int SelectAID(CPCSCMngr* pScManager, const BYTE* AID, BYTE AIDLen, const char* name) {
	CARDAPDU	apdu;
	int			status = STAT_OK;
	CString		message;

    apdu.cla = 0x00;
    apdu.ins = 0xA4;
    apdu.p1 = 0x04;
    apdu.p2 = 0x00;
    apdu.le = 0x00;
    apdu.lc = AIDLen;
	apdu.apduType = CASE4;

    memcpy(apdu.DataIn, AID, AIDLen);

	CPCSCMngr::PrintAPDU(&apdu, &message, TRUE, FALSE);
 	cout << endl << "-> " << message << endl;
	if ((status = pScManager->ExchangeAPDU(&apdu)) == STAT_OK) {
		cout << "   " << name << " applet found" << endl;
	}
	else {
		cout << "   " << name << " applet not found" << endl;
	}
	CPCSCMngr::PrintAPDU(&apdu, &message, FALSE);
	cout << "<- " << message << endl;

	return status;	
}

void InstalledAppletTest(CPCSCMngr* pScManager) {
    int					status = STAT_OK;
	map<CString, CString>		supportedAppletList;

    // CONNECT TO FIRST AVAILABLE READER AND EXCHANGE APDU COMMAND
    if (status == STAT_OK) {
		
		if (SelectAID(pScManager, APPLET_AID_VISA_PREFIX, sizeof(APPLET_AID_VISA_PREFIX), "APPLET_AID_VISA_PREFIX") == STAT_OK) supportedAppletList["APPLET_AID_VISA_PREFIX"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_VISA_PREFIX, sizeof(APPLET_AID_VISA_PREFIX));
		if (SelectAID(pScManager, APPLET_AID_MASTERCARD_PREFIX, sizeof(APPLET_AID_MASTERCARD_PREFIX), "APPLET_AID_MASTERCARD_PREFIX") == STAT_OK) supportedAppletList["APPLET_AID_MASTERCARD_PREFIX"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_MASTERCARD_PREFIX, sizeof(APPLET_AID_MASTERCARD_PREFIX));
		if (SelectAID(pScManager, APPLET_AID_EMV_PREFIX, sizeof(APPLET_AID_EMV_PREFIX), "APPLET_AID_EMV_PREFIX") == STAT_OK) supportedAppletList["APPLET_AID_EMV_PREFIX"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_EMV_PREFIX, sizeof(APPLET_AID_EMV_PREFIX));
		if (SelectAID(pScManager, APPLET_AID_GEMALTO_PREFIX, sizeof(APPLET_AID_GEMALTO_PREFIX), "APPLET_AID_GEMALTO_PREFIX") == STAT_OK) supportedAppletList["APPLET_AID_GEMALTO_PREFIX"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_GEMALTO_PREFIX, sizeof(APPLET_AID_GEMALTO_PREFIX));
		if (SelectAID(pScManager, APPLET_AID_SUN_PREFIX, sizeof(APPLET_AID_SUN_PREFIX), "APPLET_AID_SUN_PREFIX") == STAT_OK) supportedAppletList["APPLET_AID_SUN_PREFIX"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_SUN_PREFIX, sizeof(APPLET_AID_SUN_PREFIX));
		if (SelectAID(pScManager, APPLET_AID_PKCS15_PREFIX, sizeof(APPLET_AID_PKCS15_PREFIX), "APPLET_AID_PKCS15_PREFIX") == STAT_OK) supportedAppletList["APPLET_AID_PKCS15_PREFIX"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_PKCS15_PREFIX, sizeof(APPLET_AID_PKCS15_PREFIX));

		if (SelectAID(pScManager, APPLET_AID_MONEO_PREFIX, sizeof(APPLET_AID_MONEO_PREFIX), "APPLET_AID_MONEO_PREFIX") == STAT_OK) supportedAppletList["APPLET_AID_MONEO_PREFIX"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_MONEO_PREFIX, sizeof(APPLET_AID_MONEO_PREFIX));
		if (SelectAID(pScManager, APPLET_AID_3GPP_ETSI_PREFIX, sizeof(APPLET_AID_3GPP_ETSI_PREFIX), "APPLET_AID_3GPP_ETSI_PREFIX") == STAT_OK) supportedAppletList["APPLET_AID_3GPP_ETSI_PREFIX"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_3GPP_ETSI_PREFIX, sizeof(APPLET_AID_3GPP_ETSI_PREFIX));
		if (SelectAID(pScManager, APPLET_AID_javacardforum_PREFIX, sizeof(APPLET_AID_javacardforum_PREFIX), "APPLET_AID_javacardforum_PREFIX") == STAT_OK) supportedAppletList["APPLET_AID_javacardforum_PREFIX"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_javacardforum_PREFIX, sizeof(APPLET_AID_javacardforum_PREFIX));
		if (SelectAID(pScManager, APPLET_AID_globalplatform_PREFIX, sizeof(APPLET_AID_globalplatform_PREFIX), "APPLET_AID_globalplatform_PREFIX") == STAT_OK) supportedAppletList["APPLET_AID_globalplatform_PREFIX"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_globalplatform_PREFIX, sizeof(APPLET_AID_globalplatform_PREFIX));
		if (SelectAID(pScManager, APPLET_AID_FIPS_PREFIX, sizeof(APPLET_AID_FIPS_PREFIX), "APPLET_AID_FIPS_PREFIX") == STAT_OK) supportedAppletList["APPLET_AID_FIPS_PREFIX"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_FIPS_PREFIX, sizeof(APPLET_AID_FIPS_PREFIX));
		if (SelectAID(pScManager, APPLET_AID_NFCForum_PREFIX, sizeof(APPLET_AID_NFCForum_PREFIX), "APPLET_AID_NFCForum_PREFIX") == STAT_OK) supportedAppletList["APPLET_AID_NFCForum_PREFIX"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_NFCForum_PREFIX, sizeof(APPLET_AID_NFCForum_PREFIX));

		if (SelectAID(pScManager, APPLET_AID_VISA_CAP, sizeof(APPLET_AID_VISA_CAP), "AID_VISA_CAP") == STAT_OK) supportedAppletList["AID_VISA_CAP"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_VISA_CAP, sizeof(APPLET_AID_VISA_CAP));
		if (SelectAID(pScManager, APPLET_AID_VISA_CAP2, sizeof(APPLET_AID_VISA_CAP2), "APPLET_AID_VISA_CAP2") == STAT_OK) supportedAppletList["APPLET_AID_VISA_CAP2"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_VISA_CAP2, sizeof(APPLET_AID_VISA_CAP2));
		if (SelectAID(pScManager, APPLET_AID_MASTERCARD, sizeof(APPLET_AID_MASTERCARD), "AID_MASTERCARD") == STAT_OK) supportedAppletList["AID_MASTERCARD"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_MASTERCARD, sizeof(APPLET_AID_MASTERCARD));
		if (SelectAID(pScManager, APPLET_AID_MAESTRO, sizeof(APPLET_AID_MAESTRO), "AID_MAESTRO") == STAT_OK) supportedAppletList["AID_MAESTRO"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_MAESTRO, sizeof(APPLET_AID_MAESTRO));
		if (SelectAID(pScManager, APPLET_AID_UNKNOWN2, sizeof(APPLET_AID_UNKNOWN2), "AID_UNKNOWN2") == STAT_OK) supportedAppletList["AID_UNKNOWN2"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_UNKNOWN2, sizeof(APPLET_AID_UNKNOWN2));

		if (SelectAID(pScManager, APPLET_AID_GEMALTO_CM, sizeof(APPLET_AID_GEMALTO_CM), "APPLET_AID_GEMALTO_CM") == STAT_OK) supportedAppletList["APPLET_AID_GEMALTO_CM"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_GEMALTO_CM, sizeof(APPLET_AID_GEMALTO_CM));
		if (SelectAID(pScManager, APPLET_AID_GEMALTO_CM2, sizeof(APPLET_AID_GEMALTO_CM2), "APPLET_AID_GEMALTO_CM2") == STAT_OK) supportedAppletList["APPLET_AID_GEMALTO_CM2"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_GEMALTO_CM2, sizeof(APPLET_AID_GEMALTO_CM2));
		if (SelectAID(pScManager, APPLET_AID_JCOP_CM, sizeof(APPLET_AID_JCOP_CM), "APPLET_AID_JCOP_CM") == STAT_OK) supportedAppletList["APPLET_AID_JCOP_CM"] = BYTE_ConvertFromArrayToHexString(APPLET_AID_JCOP_CM, sizeof(APPLET_AID_JCOP_CM));
	}

	cout << endl << endl << "The following applets were detected:" << endl;
	if (supportedAppletList.size() == 0) {
		cout << "none" << endl;
	}
	map<CString,CString>::iterator	iter;
	for (iter = supportedAppletList.begin(); iter != supportedAppletList.end(); iter++) {
		cout << setw(30) << left << iter->first << " : " << iter->second << endl;
	}
}

void EMVTest(CPCSCMngr* pScManager, const BYTE* appletAID, BYTE appletAIDLen, const string& PIN, const string& challenge, string& response) {
    int			status =STAT_OK;
    CARDAPDU    apdu;
	CString		message;
	CARD_ISSUER	cardIssuer = ISSUER_UNKNOWN;

	// Detect card issuer by applet AID
	if (status == STAT_OK) {
		if (memcmp(APPLET_AID_VISA_CAP, appletAID, appletAIDLen) == 0) cardIssuer = ISSUER_VISA;
		if (memcmp(APPLET_AID_VISA_CAP2, appletAID, appletAIDLen) == 0) cardIssuer = ISSUER_VISA;
		if (memcmp(APPLET_AID_MAESTRO, appletAID, appletAIDLen) == 0) cardIssuer = ISSUER_MAESTRO;
		if (memcmp(APPLET_AID_MASTERCARD, appletAID, appletAIDLen) == 0) cardIssuer = ISSUER_MASTERCARD;

		if (cardIssuer == ISSUER_UNKNOWN) {
			cout << "Card issuer was not detected. Data template for AC generation may be wrong." << endl;
			cout << "0. quit (do not proceed)" << endl;
			cout << "1. VISA" << endl;
			cout << "2. MAESTRO" << endl;
			cout << "3. MASTERCARD" << endl;
			cout << "Select card issuer: ";
			int resp;
			cin >> resp;
			if (resp == 0) status = -1;
			else cardIssuer = static_cast<CARD_ISSUER>(resp);
		}
	}


	if (status == STAT_OK) {
        apdu.cla = 0x00;
        apdu.ins = 0xA4;
        apdu.p1 = 0x04;
        apdu.p2 = 0x00;
        apdu.le = 0x00;
        apdu.lc = appletAIDLen;
		apdu.apduType = CASE4;

        memcpy(apdu.DataIn, appletAID, appletAIDLen);

		CPCSCMngr::PrintAPDU(&apdu, &message, TRUE, FALSE);
     	cout << endl << "-> " << message << endl;
		if ((status = pScManager->ExchangeAPDU(&apdu)) == STAT_OK) {
			cout << "   EMV applet selected" << endl;
		}
		else {
			cout << "   ERROR: EMV applet not selected" << endl; 
		}
		CPCSCMngr::PrintAPDU(&apdu, &message, FALSE);
		cout << "<- " << message << endl;
	}

	if (status == STAT_OK) {
		// INIT TRANSACTION
		apdu.cla = 0x80;
		apdu.ins = 0xa8;
		apdu.p1 = 0x00;
		apdu.p2 = 0x00;
		apdu.lc = 0x02;
        apdu.le = 0x00;
		apdu.apduType = CASE4;

		apdu.DataIn[0] = 0x83;
		apdu.DataIn[1] = 0x00;


		CPCSCMngr::PrintAPDU(&apdu, &message, TRUE, FALSE);
     	cout << endl << "-> " << message << endl;
		if ((status = pScManager->ExchangeAPDU(&apdu)) == STAT_OK) {
			cout << "   INIT TRANSACTION OK" << endl; 
		}
		else {
			cout << "   ERROR: INIT TRANSACTION fail" << endl; 
		}
		CPCSCMngr::PrintAPDU(&apdu, &message, FALSE);
		cout << "<- "  << message << endl;
	}

	if (status == STAT_OK) {
		// GET STATIC DATA
		apdu.cla = 0x00;
		apdu.ins = 0xb2;
		apdu.p1 = 0x01;
		apdu.p2 = 0x0c;
		apdu.lc = 0x00;
        apdu.le = 0x00;
		apdu.apduType = CASE2;

		CPCSCMngr::PrintAPDU(&apdu, &message, TRUE, FALSE);
     	cout << endl << "-> " << message << endl;
		if ((status = pScManager->ExchangeAPDU(&apdu)) == STAT_OK) {
			cout << "   GET STATIC DATA OK" << endl; 
		}
		else {
			cout << "   ERROR: GET STATIC DATA fail" << endl; 
		}
		CPCSCMngr::PrintAPDU(&apdu, &message, FALSE);
		cout << "<- "  << message << endl;
	}

	if (status == STAT_OK) {
		// GET PIN COUNTER
		apdu.cla = 0x80;
		apdu.ins = 0xca;
		apdu.p1 = 0x9f;
		apdu.p2 = 0x17;
		apdu.lc = 0x00;
        apdu.le = 0x00;
		apdu.apduType = CASE2;

		CPCSCMngr::PrintAPDU(&apdu, &message, TRUE, FALSE);
     	cout << endl << "-> " << message << endl;
		if ((status = pScManager->ExchangeAPDU(&apdu)) == STAT_OK) {
			cout << "   GET PIN COUNTER OK" << endl; 
		}
		else {
			cout << "   ERROR: GET PIN COUNTER fail" << endl; 
		}
		CPCSCMngr::PrintAPDU(&apdu, &message, FALSE);
		cout << "<- "  << message << endl;
	}

	if (status == STAT_OK) {
		// VERIFY PIN
		apdu.cla = 0x00;
		apdu.ins = 0x20;
		apdu.p1 = 0x00;
		apdu.p2 = 0x80;
		apdu.lc = 0x08;
        apdu.le = 0x00;

		memset(apdu.DataIn, 0xff, apdu.lc);

		apdu.DataIn[0] = 0x24;

		int offset = 1;
		for (DWORD i = 0; i < PIN.length(); i++) {
			int pinDigit = atoi(CString(PIN[i]));
			if (i % 2 == 0) {
				apdu.DataIn[offset] = (pinDigit << 4) + 0xf;
			}
			else {
				apdu.DataIn[offset] -= 0xf;
				apdu.DataIn[offset] += pinDigit;
				offset++;
			}
		}
/*
		apdu.DataIn[0] = 0x24;
		apdu.DataIn[1] = 0x88;
		apdu.DataIn[2] = 0x84;
		apdu.DataIn[3] = 0xff;
		apdu.DataIn[4] = 0xff;
		apdu.DataIn[5] = 0xff;
		apdu.DataIn[6] = 0xff;
		apdu.DataIn[7] = 0xff;
/**/
		apdu.apduType = CASE3;

		CPCSCMngr::PrintAPDU(&apdu, &message, TRUE, FALSE);
     	cout << endl << "-> " << message << endl;
		if ((status = pScManager->ExchangeAPDU(&apdu, 0)) == STAT_OK) {
			cout << "   VERIFY PIN OK" << endl; 
		}
		else {
			cout << "   ERROR: VERIFY PIN fail" << endl; 
		}
		CPCSCMngr::PrintAPDU(&apdu, &message, FALSE);
		cout << "<- "  << message << endl;
	}


	if (status == STAT_OK) {
		// GENERATE AC
		apdu.cla = 0x80;
		apdu.ins = 0xae;
		apdu.p1 = 0x80;
		apdu.p2 = 0x00;
        apdu.le = 0x00;
		apdu.apduType = CASE4;

		BYTE	offset = 0;
		switch (cardIssuer) {
			case ISSUER_VISA: {
				apdu.lc = sizeof(GENERATE_AC_INPUT_MASK__VISA);
				memcpy(apdu.DataIn, GENERATE_AC_INPUT_MASK__VISA, sizeof(GENERATE_AC_INPUT_MASK__VISA));
				offset = sizeof(GENERATE_AC_INPUT_MASK__VISA);
				for (DWORD i = 0; i < challenge.length(); i++) {
					int challDigit = atoi(CString(challenge[i]));
					if (i % 2 == 0) {
						apdu.DataIn[offset] = (challDigit << 4) + 0xf;
					}
					else {
						apdu.DataIn[offset] -= 0xf;
						apdu.DataIn[offset] += challDigit;
						offset++;
					}
				}
				break;
			}
			case ISSUER_MAESTRO: {
				apdu.lc = sizeof(GENERATE_AC_INPUT_MASK__MAESTRO);
				memcpy(apdu.DataIn, GENERATE_AC_INPUT_MASK__MAESTRO, sizeof(GENERATE_AC_INPUT_MASK__MAESTRO));
				offset = sizeof(GENERATE_AC_INPUT_MASK__MAESTRO);
				for (DWORD i = 0; i < challenge.length(); i++) {
					int challDigit = atoi(CString(challenge[i]));
					if (i % 2 == 0) {
						apdu.DataIn[offset] = (challDigit << 4) + 0xf;
					}
					else {
						apdu.DataIn[offset] -= 0xf;
						apdu.DataIn[offset] += challDigit;
						offset++;
					}
				}

				// BUGBUG: What if challenge is not alligned to the whole bytes?
				apdu.DataIn[offset++] = 0x34; 
				apdu.DataIn[offset++] = 0x00; 
				apdu.DataIn[offset++] = 0x00; 
				break;
			}
			default: {
				status = -1;
			}
		}

		apdu.lc = offset;

		CPCSCMngr::PrintAPDU(&apdu, &message, TRUE, FALSE);
     	cout << endl << "-> " << message << endl;
		if ((status = pScManager->ExchangeAPDU(&apdu)) == STAT_OK) {
			cout << "   GENERATE AC OK" << endl; 
		}
		else {
			cout << "   ERROR: GENERATE AC fail" << endl; 
		}
		CPCSCMngr::PrintAPDU(&apdu, &message, FALSE);
		cout << "<- "  << message << endl;
	}

	if (status == STAT_OK) {
		// GENERATE AC2
		apdu.cla = 0x80;
		apdu.ins = 0xae;
		apdu.p1 = 0x00;
		apdu.p2 = 0x00;
        apdu.le = 0x00;
		apdu.apduType = CASE4;

		BYTE	offset = 0;
		switch (cardIssuer) {
			case ISSUER_VISA: {
				apdu.lc = sizeof(GENERATE_AC2_INPUT_MASK__VISA);
				memcpy(apdu.DataIn, GENERATE_AC2_INPUT_MASK__VISA, sizeof(GENERATE_AC2_INPUT_MASK__VISA));


				offset = sizeof(GENERATE_AC2_INPUT_MASK__VISA);
				for (DWORD i = 0; i < challenge.length(); i++) {
					int challDigit = atoi(CString(challenge[i]));
					if (i % 2 == 0) {
						apdu.DataIn[offset] = (challDigit << 4) + 0xf;
					}
					else {
						apdu.DataIn[offset] -= 0xf;
						apdu.DataIn[offset] += challDigit;
						offset++;
					}
				}
				break;
    		}
			case ISSUER_MAESTRO: {
				apdu.lc = sizeof(GENERATE_AC2_INPUT_MASK__MAESTRO);
				memcpy(apdu.DataIn, GENERATE_AC2_INPUT_MASK__MAESTRO, sizeof(GENERATE_AC2_INPUT_MASK__MAESTRO));
				offset = sizeof(GENERATE_AC2_INPUT_MASK__MAESTRO);

				// NOTE: Challenge is not inserted 

				break;
			}
			default: {
				status = -1;
			}
		}
		apdu.lc = offset;

		CPCSCMngr::PrintAPDU(&apdu, &message, TRUE, FALSE);
     	cout << endl << "-> " << message << endl;
		if ((status = pScManager->ExchangeAPDU(&apdu)) == STAT_OK) {
			cout << "   GENERATE AC2 OK" << endl; 
		}
		else {
			cout << "   ERROR: GENERATE AC2 fail" << endl; 
		}
		CPCSCMngr::PrintAPDU(&apdu, &message, FALSE);
		cout << "<- "  << message << endl;
	}

}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
        int					status = STAT_OK;
		CPCSCMngr           pcscMngr;
        int                 readerCount = 0;
        CString             tempCmdLine = GetCommandLine();
        CString             reader = "";
        int                 pos;
        int                 pos2;    
		string				PIN;
		string				challenge;
		string				appletAID;
		string				response;
		int					targetOperation = 0;
		BYTE				appletAIDBytes[16];
		DWORD				appletAIDBytesLen = 16;

        
        //
        // TARGET READER
        //
        if (status == STAT_OK) {
            if ((pos = tempCmdLine.Find("/reader:\"")) != -1) {
                pos += (int) strlen("/reader:\""); 
                pos2 = tempCmdLine.Find("\"", pos);
                reader = tempCmdLine.Mid(pos, pos2 - pos);
            }
            else {
                char                readers[1000]; 
                DWORD               readersLen = 1000;
                list<CString>       readersList;
                list<CString>::iterator iter;
                readerCount = 0;
				memset(readers, 0, readersLen);

                if ((status = pcscMngr.GetAvailableReaders(readers, &readersLen)) == STAT_OK) {
    		        // PARSE NULL SEPARATED ARRAY
                    cout << endl << "Available readers:" << endl;
                    pos = 0;
                    while ((DWORD) pos < readersLen) {
                        reader = &(readers[pos]);
                        if ((int)  strlen(reader) == 0) {
                            pos++;
                        }
                        else {
                            // DISPLAY READER
                            cout << (readerCount + 1) << ". " << reader << endl;

                            readersList.push_back(reader);

                            pos = pos + (DWORD) (int)  strlen(reader) + 1;
                            readerCount++; 
                        }
                    }

                    // SELECT TARGET READER
                    int     targetReader;
                    if (readerCount == 1) {
                        reader = *(readersList.begin());
                        cout << endl << "Only one reader detected, setting " << reader << " as target.";
                    }
                    else {
                        cout << endl << "Select index of target reader: ";
                        cin >> targetReader;
                        if (targetReader > 0 && targetReader <= readerCount) {  
                            // GET target-TH READER
                            iter = readersList.begin();
                            for (int i = 1; i < targetReader; i++) iter++;
                            reader = *iter;
                        }
                    }            
                    cout << endl << "INFO: You may use /reader:\"reader_name\" to provide this value\n";
                }
				else {
					cout << endl << "ERROR: Failed to obtain smartcard readers list, ending.\n";
				}
            }
        }    
        
        if (status == STAT_OK) {
            // OPEN SESSION    
            status = pcscMngr.OpenSession(reader);
            cout << "OpenSession(" << reader << ") finish with " << status << " " << ERROR_TO_STRING(status) << "\n";
        }


        if (status == STAT_OK) {
            if ((pos = tempCmdLine.Find("/OPER:\"")) != -1) {
                pos += (int)  strlen("/OPER:\""); 
                pos2 = tempCmdLine.Find("\"", pos);
                targetOperation = atoi(tempCmdLine.Mid(pos, pos2 - pos));
            }
            else {
				cout << endl << endl << "Available operations: \n";
                cout << "1. Test installed applets (/oper:\"1\")\n"; 
                cout << "2. Get EMV ACC challenge (/oper:\"2\")\n"; 
                cout << "3. Get SIM challenge (/oper:\"3\")\n"; 

                cout << endl << "Select index of target operation: ";
				cin >> targetOperation;
                cout << endl << "INFO: Use /OPER:\"operation_index\" switch to provide target operation.\n";
            }
		}

        if (status == STAT_OK) {
			switch (targetOperation) {
				case 1: {
					// no additional data required
					break;
				}
				case 2: {
					if ((pos = tempCmdLine.Find("/APPLET_AID:\"")) != -1) {
						pos += (int)  strlen("/APPLET_AID:\""); 
						pos2 = tempCmdLine.Find("\"", pos);
						appletAID = tempCmdLine.Mid(pos, pos2 - pos);
					}
					else {
						cout << "Known AIDs are :" << endl; 
						cout << "a0000000038002 (VISA)" << endl;
						cout << "a0000000031010 (VISA)" << endl;
						cout << "a0000000043060 (MAESTRO)" << endl;
						cout << "a0000001408001 (?)" << endl;

						cout << "Insert applet AID (hexadecimal, no separation between characters) : ";

						cin >> appletAID;


						cout << endl << "Applet AID :  " << appletAID << endl;
		                cout << endl << "INFO: Use /APPLET_AID:\"appletAID\" switch to provide applet AID.\n";
					}
					if (BYTE_ConvertFromHexStringToArray(CString(appletAID.c_str()), appletAIDBytes, &appletAIDBytesLen) != 0) {
						status = -1;
						cout << "ERROR: Applet AID incorrectly inserted: " << appletAID << endl;
					}
					// no break intensionally
				}
				case 3: {
					if ((pos = tempCmdLine.Find("/PIN:\"")) != -1) {
						pos += (int)  strlen("/PIN:\""); 
						pos2 = tempCmdLine.Find("\"", pos);
						PIN = tempCmdLine.Mid(pos, pos2 - pos);
					}
					else {
						cout << "Insert card PIN (decimal, no separation between characters): ";
						cin >> PIN;
						cout << endl << "PIN:  " << PIN << endl;
		                cout << endl << "INFO: Use /PIN:\"pin_value\" switch to provide PIN.\n";
					}
					if ((pos = tempCmdLine.Find("/CHALLENGE:\"")) != -1) {
						pos += (int)  strlen("/CHALLENGE:\""); 
						pos2 = tempCmdLine.Find("\"", pos);
						challenge = tempCmdLine.Mid(pos, pos2 - pos);
					}
					else {
						cout << "Insert challenge (decimal (EMV) or hexadecimal (SIM, 16B), no separation between characters): ";
						cin >> challenge;
						cout << endl << "Challenge:  " << challenge << endl;
		                cout << endl << "INFO: Use /CHALLENGE:\"challenge_value\" switch to provide challenge.\n";
					}
					break;
				}
				default: {
					cout << "Wrong operation index" << endl;
	    		}
			}
		}

        if (status == STAT_OK) {
			cout << endl << endl << endl;
			switch (targetOperation) {
				case 1: {
					cout << "Testing installed applets..." << endl;
					InstalledAppletTest(&pcscMngr);
					break;
				}
				case 2: {
					cout << "Obtaining EMV ACC response..." << endl;
					cout << "APPLET_AID: " << appletAID << endl;
					cout << "PIN: " << PIN << endl;
					cout << "CHALLENGE: " << challenge << endl;
					EMVTest(&pcscMngr, appletAIDBytes, (BYTE) appletAIDBytesLen, PIN, challenge, response);
					break;
				}
				case 3: {
					cout << "Obtaining GSM session key..." << endl;
					cout << "PIN: " << PIN << endl;
					cout << "CHALLENGE: " << challenge << endl;
					SIMTest(&pcscMngr, PIN.c_str(), challenge.c_str());
					break;
				}
				default: {
					cout << "Wrong operation index" << endl;
	    		}
			}
		}

	    pcscMngr.CloseSession();

	}

	system("pause");
	return nRetCode;
}


