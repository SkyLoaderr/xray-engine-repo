///////////////////////////////////////////////////////////////
// PhraseDialog.cpp
// Класс диалога - общения при помощи фраз 2х персонажей в игре
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "phrasedialog.h"


CPhraseDialog::CPhraseDialog(void)
{
}
CPhraseDialog::~CPhraseDialog(void)
{
}
/*
	структура XML-файла диалога
	---------------------------
	<dialog id="unique_name_id">
		<character_list>
			список персонажей, которые, могут учавствовать
			в диалоге
		</character_list>
		<precondition>
			предикат, который описывает, условия которые 
			необходимы, чтоб диалог мог быть активирован
		</precondition>


	</dialog>

*/
void CPhraseDialog::Load(LPCSTR xml_file)
{
}