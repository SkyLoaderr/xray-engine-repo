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

	virtual void InitDialog			(CPhraseDialogManager* dialog_partner,
									 DIALOG_SHARED_PTR& phrase_dialog);
	virtual void AddDialog			(DIALOG_SHARED_PTR& phrase_dialog);

	//��������� �����, ����������� �������, 
	//������ ���� �������������� ��� ��������� � ������
	virtual void ReceivePhrase		(DIALOG_SHARED_PTR& phrase_dialog);
	//�������� �� ��������� ����� � �������
	virtual void SayPhrase			(DIALOG_SHARED_PTR& phrase_dialog,
									 PHRASE_ID phrase_id);

	//����������� �������, ��������� ������, ��� ���������, �������
	//�������� ����� ������������ � ������ ������
	virtual void UpdateAvailableDialogs(CPhraseDialogManager* partner);

	DEFINE_VECTOR(DIALOG_SHARED_PTR, DIALOG_VECTOR, DIALOG_VECTOR_IT);
	virtual const DIALOG_VECTOR& AvailableDialogs() {return m_AvailableDialogs;}

protected:
	virtual bool AddAvailableDialog (PHRASE_DIALOG_ID dialog_id, CPhraseDialogManager* partner);
	
	//��������� ������ ��������, ������� ���� ���������
	//�� ����� UpdateAvailableDialogs
	xr_vector<PHRASE_DIALOG_ID> m_CheckedDialogs;

	//������ �������� ��������
	DIALOG_VECTOR m_ActiveDialogs;
	//������ ��������� ��������
	DIALOG_VECTOR m_AvailableDialogs;
};