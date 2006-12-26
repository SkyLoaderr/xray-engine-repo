#include "stdafx.h"
#include "Level.h"
#include "xrMessages.h"
#include "../x_ray.h"
#include "GameSpy/GameSpy_GCD_Client.h"


void						CLevel::OnGameSpyChallenge			(NET_Packet* P)
{
	Msg("GameSpy::CDKey::Level : Responding on Challenge");

	string64 ChallengeStr;
	P->r_stringZ(ChallengeStr);
	
	//--------------------------------------------------------------------
	string128 ResponseStr="";
	CGameSpy_GCD_Client GCD;
	GCD.CreateRespond(ResponseStr, ChallengeStr);
	//--------- Send Respond ---------------------------------------------
	NET_Packet newP;

	newP.w_begin	(M_GAMESPY_CDKEY_VALIDATION_CHALLENGE_RESPOND);
	newP.w_stringZ(ResponseStr);
	Send(newP);

	pApp->LoadTitle				("Validating CD Key...");
};

