//----------------------------------------------------
// file: MSC_AddSndPlane.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MSC_List.h"
#include "MSC_AddSound.h"
#include "Scene.h"
#include "Sound.h"
#include "SceneClassList.h"

//----------------------------------------------------

MouseCallback_AddSound::MouseCallback_AddSound()
	:MouseCallback( TARGET_SOUND, ACTION_ADD )
{
}

MouseCallback_AddSound::~MouseCallback_AddSound(){
}

//----------------------------------------------------

bool MouseCallback_AddSound::Start(){
	Scene.UndoPrepare();
	AddHere();
	return false;
}

bool MouseCallback_AddSound::End(){
	return true;
}

void MouseCallback_AddSound::Move(){
}

//----------------------------------------------------

bool MouseCallback_AddSound::AddHere(){
	IVector p;
	if( UI.PickGround(&p,&m_CurrentRStart,&m_CurrentRNorm) ){
		char folder				[_MAX_PATH];
		BROWSEINFO F;
		F.hwndOwner             = NULL;
		F.pidlRoot              = NULL; //".\\";
		F.pszDisplayName		= folder;
		F.lpszTitle             = "Select sound file";
		F.ulFlags               = BIF_BROWSEINCLUDEFILES |BIF_EDITBOX |BIF_VALIDATE;
		F.lpfn                  = NULL;
		F.lParam                = NULL;
		F.iImage                = 0;
		LPITEMIDLIST Q			= SHBrowseForFolder(&F);
		if (Q) {
			char namebuffer[MAX_OBJ_NAME];
			Scene.GenObjectName( namebuffer );
			
			Scene.UndoPrepare();
			CSound *sound = new CSound( namebuffer );
			
			sound->SetFName(folder);
			sound->Move( p );
			Scene.AddObject( sound );
		}
	}

	return false;
}

//----------------------------------------------------

