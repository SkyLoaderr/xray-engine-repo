#include "stdafx.h"
#include "ai_bloodsucker.h"
#include "ai_bloodsucker_misc.h"


CBloodsuckerVisibility::CBloodsuckerVisibility()
{
	timeStartBlinking		= 0;
	timeLastBlink			= 0; 
	bCurVisibility			= flagVisibility	= true;
}

void CBloodsuckerVisibility::Load(LPCTSTR section)
{
	timeBlinking		= pSettings->r_u32(section,"BlinkTime");
	timeBlinkInterval	= pSettings->r_u32(section,"BlinkMicroInterval");
}

void CBloodsuckerVisibility::Switch (bool bVis) 
{
	timeStartBlinking			= Level().timeServer();
	bCurVisibility				= flagVisibility;
	flagVisibility				= bVis;
	timeLastBlink				= 0; 
	timeBlinkIntervalCurrent	= timeBlinkInterval;
}

bool CBloodsuckerVisibility::Update () 
{
	TTime cur_time = Level().timeServer();

	if (timeStartBlinking + timeBlinking > cur_time) {
		if (timeLastBlink + timeBlinkInterval < cur_time) {
			// blink
			timeLastBlink = cur_time; 
			bCurVisibility = !bCurVisibility;

			timeBlinkIntervalCurrent = (TTime) (timeBlinkInterval - (float)(((timeBlinkInterval - 1) * (float)(cur_time - timeStartBlinking)) / timeBlinking));
		}
		return bCurVisibility;
	} else bCurVisibility = flagVisibility;

	return flagVisibility;
}

void CAI_Bloodsucker::MotionToAnim(EMotionAnim motion, int &index1, int &index2, int &index3)
{
	index1 = index2 = 0;		// bug protection ;) todo: find out the reason
	index3 = -1;

	switch(motion) {
		case eMotionStandIdle:		index1 = 0; index2 = 0;	 break;
		case eMotionLieIdle:		index1 = 1; index2 = 0;	 break;
		case eMotionStandTurnLeft:	index1 = 0; index2 = 0;	 break;
		case eMotionWalkFwd:		index1 = 0; index2 = 2;	 break;
		case eMotionWalkBkwd:		index1 = 0; index2 = 3;  break;
		case eMotionWalkTurnLeft:	index1 = 0; index2 = 0;  break;
		case eMotionWalkTurnRight:	index1 = 0; index2 = 0;  break;
		case eMotionRun:			index1 = 0; index2 = 6;  break;
		case eMotionRunTurnLeft:	index1 = 0; index2 = 6;  break;
		case eMotionRunTurnRight:	index1 = 0; index2 = 6;  break;
		case eMotionAttack:			index1 = 0; index2 = 9;  break;
		case eMotionAttackRat:		index1 = 0; index2 = 9;	 break;
		case eMotionFastTurnLeft:	index1 = 0; index2 = 0;  break;
		case eMotionEat:			index1 = 1; index2 = 12; break;
		case eMotionStandDamaged:	index1 = 0; index2 = 0;  break;
		case eMotionScared:			index1 = 0; index2 = 0;  break;
		case eMotionDie:			index1 = 0; index2 = 0; break;
		case eMotionLieDown:		index1 = 0; index2 = 16; break;
		case eMotionStandUp:		index1 = 1; index2 = 17; break;
		case eMotionCheckCorpse:	index1 = 0; index2 = 9;	 index3 = 0;	break;
		case eMotionLieDownEat:		index1 = 0; index2 = 16; break;
		case eMotionAttackJump:		index1 = 0; index2 = 0;  break;
			///default:					NODEFAULT;
	} 

	if (index3 == -1) index3 = ::Random.randI((int)m_tAnimations.A[index1].A[index2].A.size());

}

