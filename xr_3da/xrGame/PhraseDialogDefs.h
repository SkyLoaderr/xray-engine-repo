///////////////////////////////////////////////////////////////
// PhraseDialogDefs.h
// ����� ���������� ��� ������� ���������� � ��������
///////////////////////////////////////////////////////////////

#pragma once

#include "boost/shared_ptr.hpp"

//����� ��������� ��� �������, ��� ��� �� ����� 
//�������� �������������� ����������� ��������� ()
class CPhraseDialog;
typedef boost::shared_ptr<CPhraseDialog> DIALOG_SHARED_PTR;



//��������� ���� ��������
typedef enum {
	eDialogTypePDA,
	eDialogTypeScenery,
	eDialogTypeStandart,
	eDialogTypeMax
} EDialogType;

//��� id �����
typedef int PHRASE_ID;


#define  DIALOGS_XML			"dialogs.xml"
#define  NO_PHRASE				-1
#define  START_PHRASE			0
#define  START_PHRASE_STR		"0"