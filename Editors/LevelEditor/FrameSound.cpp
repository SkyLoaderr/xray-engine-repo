#include "stdafx.h"
#pragma hdrstop

#include "FrameSound.h"
#include "ui_tools.h"
#include "scene.h"
#include "ESound.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraSound::TfraSound(TComponent* Owner)
        : TFrame(Owner)
{
    DEFINE_INI(fsStorage);
}
//---------------------------------------------------------------------------
void __fastcall TfraSound::PaneMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraSound::ExpandClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

ESound::ESoundType TfraSound::GetCurrentSoundType()
{
	if (ebTypeStaticSource->Down) 		return ESound::stStaticSource;
    else if (ebTypeEnvironment->Down) 	return ESound::stEnvironment;
    else return ESound::stUndefined;
}




