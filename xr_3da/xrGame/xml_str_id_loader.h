///////////////////////////////////////////////////////////////
// xml_str_id_loader.h
// темплейтовый класс, который вначале проходится по файлу
// и считывает все строковые ID тегов, запоминает местонахождение
// элемента с ID и присваивает ему уникальный индекс
///////////////////////////////////////////////////////////////

#pragma once

#include "ui/xrxmlparser.h"

//T_ID    - уникальный текстовый идентификатор (аттрибут id в XML файле)
//T_INDEX - уникальный числовой индекс 
//T_INIT -  класс где определена статическая InitXmlIdToIndex
//          функция инициализации file_str и tag_name

#define TEMPLATE_SPECIALIZATION template<typename T_ID, typename T_INDEX,  typename T_INIT>
#define CSXML_IdToIndex CXML_IdToIndex<T_ID, T_INDEX, T_INIT>

TEMPLATE_SPECIALIZATION
class CXML_IdToIndex
{
public:
	//структура хранит строковый id элемента 
	//файл и позицию, где этот элемент находится
	struct ITEM_DATA
	{
		T_ID		id;
		T_INDEX		index;
		int			pos_in_file;
		shared_str	file_name;
	};

private:
	typedef xr_vector<ITEM_DATA>	T_VECTOR;
	static	T_VECTOR*		m_pItemDataVector;
	static	T_VECTOR&		ItemDataVector ();

protected:
	//имена xml файлов (разделенных запятой) из которых 
	//производить загрузку элементов
	static LPCSTR file_str;
	//имена тегов
	static LPCSTR tag_name;
public:
	CXML_IdToIndex							();
	virtual	~CXML_IdToIndex					();
	
	static const ITEM_DATA&		GetById		(const T_ID& str_id);
	static const ITEM_DATA&		GetByIndex	(T_INDEX index);

	static const T_INDEX&		IdToIndex	(const T_ID& str_id) {return GetById(str_id).index;}
	static const T_ID&			IndexToId	(T_INDEX index)		 {return GetByIndex(index).id;}

	static const T_INDEX		GetMaxIndex	()					 {return ItemDataVector().size()-1;}

	//удаление статичекого массива
	static void					DeleteIdToIndexData		();
};


TEMPLATE_SPECIALIZATION
typename CSXML_IdToIndex::T_VECTOR* CSXML_IdToIndex::m_pItemDataVector = NULL;

TEMPLATE_SPECIALIZATION
LPCSTR CSXML_IdToIndex::file_str = NULL;
TEMPLATE_SPECIALIZATION
LPCSTR CSXML_IdToIndex::tag_name = NULL;


TEMPLATE_SPECIALIZATION
CSXML_IdToIndex::CSXML_IdToIndex()
{
}


TEMPLATE_SPECIALIZATION
CSXML_IdToIndex::~CSXML_IdToIndex()
{
}


TEMPLATE_SPECIALIZATION
const typename CSXML_IdToIndex::ITEM_DATA& CSXML_IdToIndex::GetById (const T_ID& str_id)
{
	T_INIT::InitXmlIdToIndex();
		
	for(T_VECTOR::iterator it = ItemDataVector().begin();
		ItemDataVector().end() != it; it++)
	{
#pragma todo("Oles to Yura: Really dumb and slow code. 'ref_str' designed for sharing only!!!")
		if((*it).id == str_id)
			break;
	}

	R_ASSERT3(ItemDataVector().end() != it, "item not found, id", *shared_str(str_id));
	return *it;
}

TEMPLATE_SPECIALIZATION
const typename CSXML_IdToIndex::ITEM_DATA& CSXML_IdToIndex::GetByIndex(T_INDEX index)
{
	T_INIT::InitXmlIdToIndex();
	return ItemDataVector()[index];
}

TEMPLATE_SPECIALIZATION
void CSXML_IdToIndex::DeleteIdToIndexData	()
{
	T_INIT::InitXmlIdToIndex();
	xr_delete(m_pItemDataVector);
}

TEMPLATE_SPECIALIZATION
typename CSXML_IdToIndex::T_VECTOR&	CSXML_IdToIndex::ItemDataVector ()
{
	T_INIT::InitXmlIdToIndex();
	if(!m_pItemDataVector)
	{
		m_pItemDataVector = xr_new<T_VECTOR>();

		VERIFY(file_str);
		VERIFY(tag_name);

		string_path	xml_file;
		int			count = _GetItemCount	(file_str);
		T_INDEX		index = 0;
		for (int it=0; it<count; ++it)	
		{
			_GetItem	(file_str, it, xml_file);

			CUIXml uiXml;
			std::string xml_file_full;
			xml_file_full = xml_file;
			xml_file_full += ".xml";
			//strconcat(xml_file_full, *ref_str(xml_file), ".xml");
			bool xml_result = uiXml.Init("$game_data$", xml_file_full.c_str());
			R_ASSERT3(xml_result, "xml file not found", xml_file_full.c_str());

			//общий список
			int items_num = uiXml.GetNodesNum(uiXml.GetRoot(), tag_name);
			for(int i=0; i<items_num; ++i)
			{
				LPCSTR item_name = uiXml.ReadAttrib(uiXml.GetRoot(), tag_name, i, "id", NULL);

				string256 buf;
				sprintf(buf, "id for item don't set, number %d in %s", i, xml_file);
				R_ASSERT2(item_name, buf);


				//проверетить ID на уникальность
				T_VECTOR::iterator t_it = m_pItemDataVector->begin();
				for(;m_pItemDataVector->end() != t_it; t_it++)
				{
					if(shared_str((*t_it).id) == shared_str(item_name))
						break;
				}

				R_ASSERT3(m_pItemDataVector->end() == t_it, "duplicate item id", item_name);

				ITEM_DATA data;
				data.id = item_name;
				data.index = index;
				data.pos_in_file = i;
				data.file_name = xml_file;
				m_pItemDataVector->push_back(data);

				index++; 
			}
		}
	}

	return *m_pItemDataVector;
}

#undef TEMPLATE_SPECIALIZATION