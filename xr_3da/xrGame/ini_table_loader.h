///////////////////////////////////////////////////////////////
// ini_table_loader.h
// темплейтовый класс, который загружает из ini файла 
// квадратную таблицу для элементов
///////////////////////////////////////////////////////////////

#pragma once


//T_ITEM		-	тип элемента таблицы
//					
//T_INI_LOADER	-	тип класса CIni_IdToIndex, 
//					откуда будет браться информация размерах таблицы


#define TEMPLATE_SPECIALIZATION	template<typename T_ITEM, typename T_INI_LOADER>
#define CSIni_Table	CIni_Table<T_ITEM, T_INI_LOADER>


TEMPLATE_SPECIALIZATION
class CIni_Table
{
public:
			CIni_Table	();
	virtual ~CIni_Table	();

	typedef						xr_vector<T_ITEM>						ITEM_VECTOR;
	typedef						xr_vector<ITEM_VECTOR>					ITEM_TABLE;


	static ITEM_TABLE&			table			();
	static void					clear			();
	static void					set_table_sect	(LPCSTR sect) {table_sect = sect;}

private:
	static ITEM_TABLE*			m_pTable;
	static LPCSTR				table_sect;
};

TEMPLATE_SPECIALIZATION
typename CSIni_Table::ITEM_TABLE* CSIni_Table::m_pTable = NULL;

//имя секции таблицы
TEMPLATE_SPECIALIZATION
LPCSTR CSIni_Table::table_sect = NULL;


TEMPLATE_SPECIALIZATION
typename CSIni_Table::ITEM_TABLE& CSIni_Table::table	()
{
	T_INI_LOADER::InitIdToIndex ();

	if(m_pTable)
		return *m_pTable;

		m_pTable = xr_new<ITEM_TABLE>();

	VERIFY(table_sect);
	std::size_t table_size = T_INI_LOADER::GetMaxIndex()+1;

	m_pTable->resize(table_size);

	string64 buffer;
	CInifile::Sect&	table_ini = pSettings->r_section(table_sect);

	R_ASSERT3(table_ini.size() == table_size, "wrong size for table in section", table_sect);

	for (CInifile::SectIt i = table_ini.begin(); table_ini.end() != i; ++i)
	{
		T_INI_LOADER::index_type cur_index = T_INI_LOADER::IdToIndex((*i).first, type_max(T_INI_LOADER::index_type));

		if(type_max(T_INI_LOADER::index_type) == cur_index)
			Debug.fatal("wrong community %s in section [%s]", (*i).first, table_sect);

		(*m_pTable)[cur_index].resize(table_size);
		for(std::size_t j=0; j<table_size; j++)
		{
			(*m_pTable)[cur_index][j] = (T_ITEM)atoi(_GetItem(*(*i).second,(int)j,buffer));
		}
	}

	return *m_pTable;
}

TEMPLATE_SPECIALIZATION
void CSIni_Table::clear()
{
	xr_delete(m_pTable);
}

#undef TEMPLATE_SPECIALIZATION