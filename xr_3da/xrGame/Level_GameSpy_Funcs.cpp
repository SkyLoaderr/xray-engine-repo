#include "stdafx.h"
#include "Level.h"
#include "xrMessages.h"
#include "GameSpy/CDKey/gcdkeyc.h"

#define	REGISTRY_CDKEY_STR	"Software\\S.T.A.L.K.E.R\\CDKey"

void						CLevel::OnGameSpyChallenge			(NET_Packet* P)
{
	Msg("GameSpy::CDKey::Level : Responding on Challenge");

	string64 ChallengeStr;
	P->r_stringZ(ChallengeStr);
	//--------------- Get CDKey ------------------------------------------
	string128 CDKeyStr = "aaaa-ssss-dddd-ffff";

	HKEY KeyCDKey = 0;
	long res = RegOpenKeyEx(HKEY_CURRENT_USER, 
		REGISTRY_CDKEY_STR, 0, KEY_READ, &KeyCDKey);
	if (res == ERROR_SUCCESS && KeyCDKey != 0)
	{
		DWORD KeyValueSize = 128;
		DWORD KeyValueType = REG_SZ;
		res = RegQueryValueEx(KeyCDKey, "value", NULL, &KeyValueType, (LPBYTE)CDKeyStr, &KeyValueSize);
	};
	switch (res)
	{
	case ERROR_PATH_NOT_FOUND:
	case ERROR_FILE_NOT_FOUND:
		{
			Msg("No CD-key found");
//			return;
		}break;
	};
	//--------------- Create Respond -------------------------------------
	string128 ResponseStr;	
	gcd_compute_response(CDKeyStr, ChallengeStr, ResponseStr);
	//--------- Send Respond ---------------------------------------------
	NET_Packet newP;

	newP.w_begin	(M_GAMESPY_CDKEY_VALIDATION_CHALLENGE_RESPOND);
	newP.w_stringZ(ResponseStr);
	Send(newP);

	pApp->LoadTitle				("Validating CD Key...");
};

