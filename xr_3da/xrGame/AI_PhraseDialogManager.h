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
	virtual void AnswerPhrase			(DIALOG_SHARED_PTR& phrase_dialog);

	virtual bool NeedAnswerOnPending		();
	virtual void AnswerOnPending			();

	virtual void				SetStartDialog				(PHRASE_DIALOG_ID phrase_dialog);
	virtual void				SetDefaultStartDialog		(PHRASE_DIALOG_ID phrase_dialog);
	virtual PHRASE_DIALOG_ID	GetStartDialog				() {return m_sStartDialog;}
	virtual void				RestoreDefaultStartDialog	();
protected:
	//������, ���� �� NULL, �� ��� �������� ��������
	//��� ������� � �������
	PHRASE_DIALOG_ID m_sStartDialog;
	PHRASE_DIALOG_ID m_sDefaultStartDialog;

	DEFINE_VECTOR(DIALOG_SHARED_PTR, DIALOG_SHARED_VECTOR, DIALOG_SHARED_IT);
	//������ ��������, �� ������� ����� ��������
	DIALOG_SHARED_VECTOR m_PendingDialogs;
};