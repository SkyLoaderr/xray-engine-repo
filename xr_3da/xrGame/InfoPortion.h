///////////////////////////////////////////////////////////////
// InfoPortion.h
// ��������, �������������� �������� ����������
///////////////////////////////////////////////////////////////


#pragma once

#include "shared_data.h"

#include "ui/xrXMLParser.h"
#include "PhraseScript.h"

#include "xml_str_id_loader.h"
#include "encyclopedia_article_defs.h"

#include "GameTaskDefs.h"
#include "PhraseDialogDefs.h"


//////////////////////////////////////////////////////////////////////////
// SInfoPortionData: ������ ��� InfoProtion
//////////////////////////////////////////////////////////////////////////
struct SInfoPortionData : CSharedResource
{
	SInfoPortionData ();
	virtual ~SInfoPortionData ();

	//������ � ������� ��������, ������� ����� ���� ������������
	//�� ����� InfoPortion
	DIALOG_ID_VECTOR m_DialogNames;

	//������ � ������� ��������, ������� ����� ���� ������������ �������
	//��� ��������� � ����������, ������� ����� ������ InfoPortion
	DIALOG_ID_VECTOR m_ActorDialogNames;

	//������ ������� �� �����
//	LOCATIONS_VECTOR	m_MapLocations;
	//������ ������ � ������������, ������� ���������� ���������� 
	ARTICLE_ID_VECTOR m_Articles;
	//������ ������ � ������������, ������� ���������� ������������ (�� ��� ������ ����
	//����� �������� ���� ������ ������)
	ARTICLE_ID_VECTOR m_ArticlesDisable;
    	
	//�������������� �������
	TASK_ID_VECTOR	m_GameTasks;

	//���������� ��������, ������� ������������ ����� ���� ��� 
	//���������� �������� ��������
	CPhraseScript m_PhraseScript;

	//������ � ��������� ��� ������ ����������, �������
	//��������, ����� ��������� ���� info_portion
	DEFINE_VECTOR(INFO_ID, INFO_ID_VECTOR, INFO_ID_VECTOR_IT);
	INFO_ID_VECTOR m_DisableInfo;

	//��������� ������������� ����������	
	shared_str		m_text;
	
	//���� true, �� info_portion ����� ��������� 
	//(������� CInventoryOwner::OnReceiveInfo()) ������ ����� ��������� ���������
	//� ������� ����� CInventoryOwner::TransferInfo �������� ��������� CInventoryOwner::OnReceiveInfo
	bool			m_bDeferInit;
};


class CInfoPortion;

//�����  - ������ ����������
class CInfoPortion : public CSharedClass<SInfoPortionData, INFO_ID, false>,
					 public CXML_IdToIndex<INFO_ID,	int, CInfoPortion>
{
private:
	typedef CSharedClass<SInfoPortionData, INFO_ID, false> inherited_shared;
	typedef CXML_IdToIndex<INFO_ID, int, CInfoPortion>		id_to_index;

	friend id_to_index;
public:
				CInfoPortion	(void);
	virtual		~CInfoPortion	(void);

	//������������� info �������
	//���� info � ����� id ������ �� �������������
	//�� ����� �������� �� �����
	virtual void Load	(INFO_ID info_str_id);
//	virtual void Load	(INFO_INDEX info_index);

//	const LOCATIONS_VECTOR&							MapLocations()	const {return info_data()->m_MapLocations;}
	const ARTICLE_ID_VECTOR&						Articles	()	const {return info_data()->m_Articles;}
	const ARTICLE_ID_VECTOR&						ArticlesDisable	()	const {return info_data()->m_ArticlesDisable;}
	const TASK_ID_VECTOR&							GameTasks	()	const {return info_data()->m_GameTasks;}
	const DIALOG_ID_VECTOR&							DialogNames	()	const {return info_data()->m_DialogNames;}
	const DIALOG_ID_VECTOR&							ActorDialogNames	()	const {return info_data()->m_ActorDialogNames;}
	const SInfoPortionData::INFO_ID_VECTOR&			DisableInfos()	const {return info_data()->m_DisableInfo;}
	
	//������ �������������� ���������� �������	
	virtual void RunScriptActions   (const CGameObject* pOwner)	{info_data()->m_PhraseScript.Action(pOwner, NULL, -1);}

	//��������� ������������� ����������
	virtual LPCSTR GetText () const ;
	virtual bool   DeferInit () const;


protected:
    INFO_ID		m_InfoId;

	void load_shared	(LPCSTR);
	SInfoPortionData* info_data() { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}
	const SInfoPortionData* info_data() const { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

	static void InitXmlIdToIndex();
};