//////////////////////////////////////////////////////////////////////////
// string_table.h:		таблица строк используемых в игре
//////////////////////////////////////////////////////////////////////////

#pragma once

DEFINE_MAP(ref_str, ref_str, STRING_TABLE_MAP, STRING_TABLE_IT);


struct STRING_TABLE_DATA
{
	STRING_TABLE_MAP	m_StringTable;
};



class CStringTable
{
public:
	CStringTable			();
	virtual ~CStringTable	();

	static void Destroy		();
	ref_str operator() (const ref_str& str_name) const;
private:
	virtual void Init		();
	virtual void Load		(LPCSTR xml_file);
	
	static STRING_TABLE_DATA* pData;
};