///////////////////////////////////////////////////////////////
// PhraseDialogManager.h
// Класс, от которого наследуются персонажи, ведущие диалог
// между собой.
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

	//получение фразы, виртуальная функция, 
	//должна быть переопределена для сталкеров и актера
	virtual void ReceivePhrase		(DIALOG_SHARED_PTR& phrase_dialog);
	//ответить на сказанную фразу в диалоге
	virtual void SayPhrase			(DIALOG_SHARED_PTR& phrase_dialog,
									 PHRASE_ID phrase_id);

	//виртуальная функция, заполняет массив, тем диалогами, которые
	//персонаж может инициировать в данный момент
	virtual void UpdateAvailableDialogs(CPhraseDialogManager* partner);

	DEFINE_VECTOR(DIALOG_SHARED_PTR, DIALOG_VECTOR, DIALOG_VECTOR_IT);
	virtual const DIALOG_VECTOR& AvailableDialogs() {return m_AvailableDialogs;}

protected:
	virtual bool AddAvailableDialog (PHRASE_DIALOG_ID dialog_id, CPhraseDialogManager* partner);
	
	//буфферный список диалогов, которые были проверены
	//во время UpdateAvailableDialogs
	xr_vector<PHRASE_DIALOG_INDEX> m_CheckedDialogs;

	//список активных диалогов
	DIALOG_VECTOR m_ActiveDialogs;
	//список доступных диалогов
	DIALOG_VECTOR m_AvailableDialogs;
};