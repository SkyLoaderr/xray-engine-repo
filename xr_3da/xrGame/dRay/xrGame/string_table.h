//////////////////////////////////////////////////////////////////////////
// string_table.h:		таблица строк используемых в игре
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "string_table_defs.h"

DEFINE_MAP		(STRING_ID, STRING_INDEX, STRING_TABLE_MAP, STRING_TABLE_MAP_IT);
DEFINE_VECTOR	(STRING_VALUE, STRING_TABLE_VECTOR, STRING_TABLE_VECTOR_IT);


struct STRING_TABLE_DATA
{
	shared_str				m_sLanguage;
	
	//соответствие ID <--> Index
	STRING_TABLE_MAP		m_StringTable;
	
	//вектор - хранилище строк
	STRING_TABLE_VECTOR		m_Strings;
};


class CStringTable 
{
public:
								CStringTable			();
	virtual						~CStringTable			();

	static void					Destroy					();
	
	STRING_INDEX				IndexById				(const STRING_ID& str_id)		const;
//	STRING_VALUE				operator()				(const STRING_ID& str_id)		const;
//	STRING_VALUE				operator()				(const STRING_INDEX str_index)	const;
	STRING_VALUE				translate				(const STRING_ID& str_id)		const;
	STRING_VALUE				translate				(const STRING_INDEX str_index)	const;

	static BOOL					m_bWriteErrorsToLog;
private:
	virtual void				Init					();
	virtual void				Load					(LPCSTR xml_file);
	STRING_VALUE				ParseLine				(LPCSTR str);
	bool						GetKeyboardItem			(LPCSTR src, char* dst);
	static STRING_TABLE_DATA*	pData;
};