///////////////////////////////////////////////////////////////
// AI_PhraseDialogManager.h
// Класс, от которого наследуются NPC персонажи, ведущие диалог
// с актером
//
///////////////////////////////////////////////////////////////

#pragma once

#include "PhraseDialogManager.h"


class CAI_PhraseDialogManager: public CPhraseDialogManager
{
private:
	typedef CPhraseDialogManager inherited;
public:
	CAI_PhraseDialogManager			(void);
	virtual ~CAI_PhraseDialogManager(void);

	virtual void ReceivePhrase			(DIALOG_SHARED_PTR& phrase_dialog);
	virtual void UpdateAvailableDialogs	(CPhraseDialogManager* partner);

	virtual void SetStartDialog		(LPCSTR phrase_dialog);
	virtual void ResetStartDialog	();
protected:
	//диалог, если не NULL, то его персонаж запустит
	//при встрече с актером
	ref_str m_sStartDialog;
};