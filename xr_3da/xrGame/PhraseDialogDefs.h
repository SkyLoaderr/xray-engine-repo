///////////////////////////////////////////////////////////////
// PhraseDialogDefs.h
// общие объ€влени€ дл€ классов работающих с диалогом
///////////////////////////////////////////////////////////////

#pragma once

#include "boost/shared_ptr.hpp"

//умный указатель дл€ диалога, так как он может 
//совмесно использоватьс€ несколькими объектами ()
class CPhraseDialog;
typedef boost::shared_ptr<CPhraseDialog> DIALOG_SHARED_PRT;



//возможные типы диалогов
typedef enum {
	eDialogTypePDA,
	eDialogTypeScenery,
	eDialogTypeStandart,
	eDialogTypeMax
} EDialogType;

//тип id фразы
typedef int PHRASE_ID;

