///////////////////////////////////////////////////////////////
// PhraseDialogDefs.h
// ����� ���������� ��� ������� ���������� � ��������
///////////////////////////////////////////////////////////////

#pragma once

#include "boost/shared_ptr.hpp"

//����� ��������� ��� �������, ��� ��� �� ����� 
//�������� �������������� ����������� ��������� ()
class CPhraseDialog;
typedef boost::shared_ptr<CPhraseDialog> DIALOG_SHARED_PRT;



//��������� ���� ��������
typedef enum {
	eDialogTypePDA,
	eDialogTypeScenery,
	eDialogTypeStandart,
	eDialogTypeMax
} EDialogType;

//��� id �����
typedef int PHRASE_ID;

