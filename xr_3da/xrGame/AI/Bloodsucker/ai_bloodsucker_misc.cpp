#include "stdafx.h"
#include "ai_bloodsucker.h"
#include "ai_bloodsucker_misc.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// Реализация невидимости (мерцания)
///////////////////////////////////////////////////////////////////////////////////////////////

CBloodsuckerVisibility::CBloodsuckerVisibility()
{
	timeStartBlinking		= 0;
	timeLastBlink			= 0; 
	bCurVisibility			= flagVisibility	= true;

	timeStartRestore		= 0;
	timeStartInvisible		= 0;

	bBlinking				= false;
}

void CBloodsuckerVisibility::Load(LPCTSTR section)
{
	timeBlinking		= pSettings->r_u32(section,"BlinkTime");
	timeBlinkInterval	= pSettings->r_u32(section,"BlinkMicroInterval");
	timeRestoreInterval	= pSettings->r_u32(section,"RestoreInterval");
	timeInvisibilityMin	= pSettings->r_u32(section,"InvisibilityTimeMin");;
	timeInvisibilityMax	= pSettings->r_u32(section,"InvisibilityTimeMax");;
}

bool CBloodsuckerVisibility::Switch (bool bVis) 
{
	if ((bVis == false) && !IsInvisibilityReady()) return false;		// если ещё не восстановился
	if (bBlinking) return false;										// если в переходном состоянии
	if (bVis == flagVisibility) return false;							// если уже находимся в этом состоянии

	timeStartBlinking			= timeCurrent;
	bCurVisibility				= flagVisibility;
	flagVisibility				= bVis;
	timeLastBlink				= 0; 
	timeBlinkIntervalCurrent	= timeBlinkInterval;

	bBlinking					= true;
	
	if (flagVisibility) timeStartRestore = timeCurrent; // если монстр стал видим
	else {
		timeStartInvisible = timeCurrent;
		timeInvisibleInterval = ::Random.randI(timeInvisibilityMin,timeInvisibilityMax);
	}
	
	return true;
}	

bool CBloodsuckerVisibility::Update () 
{
	timeCurrent = Level().timeServer();

	if (timeStartBlinking + timeBlinking > timeCurrent) {
		if (timeLastBlink + timeBlinkInterval < timeCurrent) {
			// blink
			timeLastBlink = timeCurrent; 
			bCurVisibility = !bCurVisibility;

			timeBlinkIntervalCurrent = (TTime) (timeBlinkInterval - (float)(((timeBlinkInterval - 1) * (float)(timeCurrent - timeStartBlinking)) / timeBlinking));
		}
		return bCurVisibility;
	} else {
		bCurVisibility = flagVisibility;
		bBlinking = false;
	}

	// если время невидимости вышло
	if (!flagVisibility && (timeStartInvisible + timeInvisibleInterval < timeCurrent)) Switch(true);

	return flagVisibility;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Другие функции
///////////////////////////////////////////////////////////////////////////////////////////////

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

