///////////////////////////////////////////////////////////////
// AI_PhraseDialogManager.h
// �����, �� �������� ����������� NPC ���������, ������� ������
// � �������
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

	virtual void SetStartDialog		(PHRASE_DIALOG_ID phrase_dialog);
	virtual void ResetStartDialog	();
protected:
	//������, ���� �� NULL, �� ��� �������� ��������
	//��� ������� � �������
	PHRASE_DIALOG_ID m_sStartDialog;
};