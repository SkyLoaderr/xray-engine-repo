#include "stdafx.h"
#pragma hdrstop

#include "ESceneSoundEnvTools.h"
#include "SoundManager_LE.h"

void ESceneSoundEnvTools::CreateControls()
{
	inherited::CreateControls();
}
//----------------------------------------------------
 
void ESceneSoundEnvTools::RemoveControls()
{
	inherited::RemoveControls();
}
//----------------------------------------------------

void ESceneSoundEnvTools::Clear(bool bSpecific)
{
	inherited::Clear	(bSpecific);
    LSndLib->RefreshEnvGeometry	();
}
//----------------------------------------------------

