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

	virtual DIALOG_SHARED_PRT InitDialog	(CPhraseDialogManager* dialog_partner,
											ref_str dialog_id,
											EDialogType dialog_type);
	//получение фразы, виртуальная функция, 
	//должна быть переопределена для сталкеров и актера
	virtual void ReceivePhrase		(DIALOG_SHARED_PRT phrase_dialog);
	//ответить на сказанную фразу в диалоге
	virtual void SayPhrase			(DIALOG_SHARED_PRT phrase_dialog,
									 PHRASE_ID phrase_id);
protected:
	DEFINE_VECTOR(DIALOG_SHARED_PRT, DIALOG_VECTOR, DIALOG_VECTOR_IT);
	//список активных диалогов
	DIALOG_VECTOR m_Dialogs;
};