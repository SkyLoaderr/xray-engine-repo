///////////////////////////////////////////////////////////////
// PhraseScript.h
// классы для связи диалогов со скриптами
///////////////////////////////////////////////////////////////

#include "ui/xrXMLParser.h"


class CGameObject;

class CPhraseScript
{
public:
	CPhraseScript			();
	virtual ~CPhraseScript	();
	
	//загрузка из XML файла
	virtual void Load		(CUIXml& ui_xml, XML_NODE* phrase_node);

	virtual bool Precondition	(const CGameObject* pSpeaker1, const CGameObject* pSpeaker2) const;
	virtual void Action			(const CGameObject* pSpeaker1, const CGameObject* pSpeaker2) const;


	DEFINE_VECTOR(ref_str, PRECONDITION_VECTOR, PRECONDITION_VECTOR_IT);
	virtual const PRECONDITION_VECTOR& Preconditions() const {return m_Preconditions;}
	
	DEFINE_VECTOR(ref_str, ACTION_NAME_VECTOR, ACTION_NAME_VECTOR_IT);
	virtual const ACTION_NAME_VECTOR& Actions() const {return m_ScriptActions;}
protected:

	//скриптовые действия, которые активируется после того как 
	//говорится фраза
	DEFINE_VECTOR(ref_str, ACTION_NAME_VECTOR, ACTION_NAME_VECTOR_IT);
	ACTION_NAME_VECTOR m_ScriptActions;

	//список скриптовых предикатов, выполнение, которых необходимо
	//для того чтоб фраза стала доступной
	DEFINE_VECTOR(ref_str, PRECONDITION_VECTOR, PRECONDITION_VECTOR_IT);
	PRECONDITION_VECTOR m_Preconditions;
};