///////////////////////////////////////////////////////////////
// PhraseScript.h
// ������ ��� ����� �������� �� ���������
///////////////////////////////////////////////////////////////

#include "ui/xrXMLParser.h"


class CGameObject;

class CPhraseScript
{
public:
	CPhraseScript			();
	virtual ~CPhraseScript	();
	
	//�������� �� XML �����
	virtual void Load		(CUIXml& ui_xml, XML_NODE* phrase_node);

	virtual bool Precondition	(const CGameObject* pSpeaker1, const CGameObject* pSpeaker2) const;
	virtual void Action			(const CGameObject* pSpeaker1, const CGameObject* pSpeaker2) const;


	DEFINE_VECTOR(ref_str, PRECONDITION_VECTOR, PRECONDITION_VECTOR_IT);
	virtual const PRECONDITION_VECTOR& Preconditions() const {return m_Preconditions;}
	
	DEFINE_VECTOR(ref_str, ACTION_NAME_VECTOR, ACTION_NAME_VECTOR_IT);
	virtual const ACTION_NAME_VECTOR& Actions() const {return m_ScriptActions;}
protected:

	//���������� ��������, ������� ������������ ����� ���� ��� 
	//��������� �����
	DEFINE_VECTOR(ref_str, ACTION_NAME_VECTOR, ACTION_NAME_VECTOR_IT);
	ACTION_NAME_VECTOR m_ScriptActions;

	//������ ���������� ����������, ����������, ������� ����������
	//��� ���� ���� ����� ����� ���������
	DEFINE_VECTOR(ref_str, PRECONDITION_VECTOR, PRECONDITION_VECTOR_IT);
	PRECONDITION_VECTOR m_Preconditions;
};