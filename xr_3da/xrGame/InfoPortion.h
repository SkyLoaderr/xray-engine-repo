///////////////////////////////////////////////////////////////
// InfoPortion.h
// ��������, �������������� �������� ����������
///////////////////////////////////////////////////////////////


#pragma once

#include "ui/xrXMLParser.h"
#include "map_location.h"

#include "shared_data.h"


//////////////////////////////////////////////////////////////////////////
// SInfoPortionData: ������ ��� InfoProtion
//////////////////////////////////////////////////////////////////////////
struct SInfoPortionData : CSharedResource
{
	SInfoPortionData ();
	virtual ~SInfoPortionData ();

	//���������� ������ ������ ����������
	INFO_ID		m_InfoId;

	//������ � ������� ��������, ������� ����� ���� ������������
	//�� ����� InfoPortion
	DEFINE_VECTOR(ref_str, DIALOG_NAME_VECTOR, DIALOG_NAME_VECTOR_IT);
	DIALOG_NAME_VECTOR m_DialogNames;

	//������ ������� �� �����
	LOCATIONS_VECTOR	m_MapLocations;

	//���������� ��������, ������� ������������ ����� ���� ��� 
	//���������� �������� ��������
	DEFINE_VECTOR(ref_str, ACTION_NAME_VECTOR, ACTION_NAME_VECTOR_IT);
	ACTION_NAME_VECTOR	m_ScriptActions;

	//������ � ��������� ��� ������ ����������, �������
	//��������, ����� ��������� ���� info_portion
	DEFINE_VECTOR(INFO_ID, INFO_INDEX_VECTOR, INFO_INDEX_VECTOR_IT);
	INFO_INDEX_VECTOR m_DisableInfo;

	//��������� ������������� ����������	
	ref_str		m_text;
};




//�����  - ������ ����������
class CInfoPortion : public CSharedClass<SInfoPortionData, INFO_ID>
{
private:
	typedef CSharedClass<SInfoPortionData, INFO_ID> inherited_shared;
public:
				CInfoPortion(void);
	virtual		~CInfoPortion(void);

	//������������� info �������
	//���� info � ����� id ������ �� �������������
	//�� ����� �������� �� �����
	virtual void Load	(INFO_ID info_id);

	const LOCATIONS_VECTOR&							MapLocations()	{return info_data()->m_MapLocations;}
	const SInfoPortionData::DIALOG_NAME_VECTOR&		DialogNames()	{return info_data()->m_DialogNames;}
	const SInfoPortionData::INFO_INDEX_VECTOR&		DisableInfos()	{return info_data()->m_DisableInfo;}
	const SInfoPortionData::ACTION_NAME_VECTOR&		ScriptActions()	{return	info_data()->m_ScriptActions;}

	//��������� ������������� ����������
	virtual LPCSTR GetText ();
protected:
    INFO_ID	m_InfoId;

	void load_shared	(LPCSTR xml_file);
	SInfoPortionData* info_data() { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}
};