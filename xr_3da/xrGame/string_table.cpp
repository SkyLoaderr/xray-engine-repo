//////////////////////////////////////////////////////////////////////////
// string_table.cpp:		������� ����� ������������ � ����
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "string_table.h"

#include "ui/xrxmlparser.h"

#define STRING_TABLE_SECT "string_table"

STRING_TABLE_DATA* CStringTable::pData = NULL;
BOOL CStringTable::m_bWriteErrorsToLog = FALSE;

CStringTable::CStringTable	()
{
	Init();
}


CStringTable::~CStringTable	()
{
}

void CStringTable::Destroy	()
{
	xr_delete(pData);
}


void CStringTable::Init		()
{
	if(NULL != pData) return;
    
	pData = xr_new<STRING_TABLE_DATA>();
	
	//��� �����, ���� �� ������ (NULL), �� ������ <text> � <string> � XML
	pData->m_sLanguage = pSettings->r_string(STRING_TABLE_SECT, "language");

	LPCSTR S = pSettings->r_string(STRING_TABLE_SECT,"files");
	if (S && S[0]) 
	{
		string128	xml_file;
		int			count = _GetItemCount	(S);
		for (int it=0; it<count; ++it)	
		{
			_GetItem	(S,it, xml_file);
			Load		(xml_file);
		}
	}
}

void CStringTable::Load	(LPCSTR xml_file)
{
	CUIXml uiXml;
	string128 xml_file_full;
	strconcat(xml_file_full, xml_file, ".xml");
	bool xml_result = uiXml.Init("$game_data$", xml_file_full);
	R_ASSERT3(xml_result, "xml file not found", xml_file_full);


	//����� ������ ���� ������� ������� � �����
	int string_num = uiXml.GetNodesNum(uiXml.GetRoot(), "string");
	for(int i=0; i<string_num; ++i)
	{
		LPCSTR string_name = uiXml.ReadAttrib(uiXml.GetRoot(), "string", i, "id", NULL);

		string128 buf;
		sprintf(buf, "%d", i);
		R_ASSERT3(string_name, "id for string table string don't set :", buf);

		//����������� ID �� ������������
		R_ASSERT3(pData->m_StringTable.find(string_name) == pData->m_StringTable.end(),
					"duplicate string table id", string_name);

		XML_NODE* string_node = uiXml.NavigateToNode(uiXml.GetRoot(), "string", i);
		R_ASSERT(string_node);
		uiXml.SetLocalRoot(string_node);

		LPCSTR string_text = NULL;
		XML_NODE* text_node = NULL;
		if(pData->m_sLanguage)
			text_node = uiXml.NavigateToNodeWithAttribute("text", "ln", pData->m_sLanguage);
		
		if(text_node)
		{
			string_text = uiXml.Read(text_node, NULL);
		}
		//���� ������ ��� ���������������� ����� �� �������, �� ���� ������ �� ���� ��� ����
		else
		{
			string_text = uiXml.Read(uiXml.GetRoot(), "string:text", i,  NULL);
			if(m_bWriteErrorsToLog && pData->m_sLanguage && string_text)
				Msg("[string table] '%s' no translation in '%s'", string_name, pData->m_sLanguage);
		}

		R_ASSERT3(string_text, "string table entry does not has a text", string_name);

		pData->m_Strings.push_back(STRING_VALUE(string_text));
		pData->m_StringTable.insert(mk_pair(STRING_ID(string_name), STRING_INDEX(pData->m_Strings.size()-1)));
	}
}

STRING_VALUE CStringTable::operator() (const STRING_ID& str_id) const
{
	VERIFY(pData);
	STRING_INDEX index = IndexById(str_id);
	//���� ������ � ��������������� ID ���, �� ���������� ��� ��������� ID
	if(NO_STRING == index)
	{
		if(m_bWriteErrorsToLog && *str_id != NULL && xr_strlen(*str_id)>0)
			Msg("[string table] '%s' has no entry", *str_id);
		return str_id;
	}
	return pData->m_Strings[index];
}

STRING_VALUE CStringTable::operator() (const STRING_INDEX str_index) const
{
	VERIFY(pData);
	return pData->m_Strings[str_index];
}


STRING_INDEX CStringTable::IndexById	(const STRING_ID& str_id)		const
{
	VERIFY(pData);
	STRING_TABLE_MAP_IT it =  pData->m_StringTable.find(str_id);
	if(pData->m_StringTable.end() == it) return NO_STRING;
	return (*it).second;
}