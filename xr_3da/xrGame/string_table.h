//////////////////////////////////////////////////////////////////////////
// string_table.h:		������� ����� ������������ � ����
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "string_table_defs.h"

DEFINE_MAP		(STRING_ID, STRING_INDEX, STRING_TABLE_MAP, STRING_TABLE_MAP_IT);
DEFINE_VECTOR	(STRING_VALUE, STRING_TABLE_VECTOR, STRING_TABLE_VECTOR_IT);


struct STRING_TABLE_DATA
{
	LPCSTR				m_sLanguage;
	//������������ ID <--> Index
	STRING_TABLE_MAP	m_StringTable;
	//������ - ��������� �����
	STRING_TABLE_VECTOR m_Strings;
};


class CStringTable 
{
public:
	CStringTable			();
	virtual ~CStringTable	();

	static void Destroy		();
	
	STRING_INDEX IndexById	(const STRING_ID& str_id)		const;
	STRING_VALUE operator() (const STRING_ID& str_id)		const;
	STRING_VALUE operator() (const STRING_INDEX str_index)	const;
private:
	virtual void Init		();
	virtual void Load		(LPCSTR xml_file);
	
	static STRING_TABLE_DATA* pData;
};