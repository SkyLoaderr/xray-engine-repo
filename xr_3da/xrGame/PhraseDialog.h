///////////////////////////////////////////////////////////////
// PhraseDialog.h
// ����� ������� - ������� ��� ������ ���� 2� ���������� � ����
///////////////////////////////////////////////////////////////

#pragma once


#include "phrase.h"
#include "graph_manager.h"


typedef CGraphAbstract<CPhrase*, float, u32, u32> CPhraseGraph;
typedef CGraphManagerAbstract<CPhrase*, float, u32, u32> CPhraseGraphManagerAbstract;

class CPhraseDialog	:public CPhraseGraphManagerAbstract
{
private:
	typedef CPhraseGraphManagerAbstract inherited;
public:
	CPhraseDialog(void);
	virtual ~CPhraseDialog(void);

	//�������� ������� �� XML �����
	virtual void Load(LPCSTR xml_file);

protected:
	//��������������� ���� ����
	//��������� ��� ��������� �������� �������� �������
	CPhraseGraph m_PhraseGraph;
};