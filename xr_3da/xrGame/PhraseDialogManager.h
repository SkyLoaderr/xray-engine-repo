///////////////////////////////////////////////////////////////
// PhraseDialogManager.h
// �����, �� �������� ����������� ���������, ������� ������
// ����� �����.
//
///////////////////////////////////////////////////////////////

#pragma once

#include "PhraseDialogDefs.h"

class CPhraseDialogManager
{
public:
	CPhraseDialogManager			(void);
	virtual ~CPhraseDialogManager	(void);

	virtual DIALOG_SHARED_PRT InitDialog	(CPhraseDialogManager* dialog_partner,
											ref_str dialog_id,
											EDialogType dialog_type);
	//��������� �����, ����������� �������, 
	//������ ���� �������������� ��� ��������� � ������
	virtual void ReceivePhrase		(DIALOG_SHARED_PRT phrase_dialog);
	//�������� �� ��������� ����� � �������
	virtual void SayPhrase			(DIALOG_SHARED_PRT phrase_dialog,
									 PHRASE_ID phrase_id);
protected:
	DEFINE_VECTOR(DIALOG_SHARED_PRT, DIALOG_VECTOR, DIALOG_VECTOR_IT);
	//������ �������� ��������
	DIALOG_VECTOR m_Dialogs;
};