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

typedef ref_str	PHRASE_DIALOG_ID;
typedef int		PHRASE_DIALOG_INDEX;

DEFINE_VECTOR(PHRASE_DIALOG_ID,		DIALOG_ID_VECTOR,		DIALOG_ID_IT);
DEFINE_VECTOR(PHRASE_DIALOG_INDEX,	DIALOG_INDEX_VECTOR,	DIALOG_INDEX_IT);

#define  NO_PHRASE_DIALOG	PHRASE_DIALOG_INDEX(-1)

#define  NO_PHRASE				-1
#define  START_PHRASE			0
#define  START_PHRASE_STR		"0"