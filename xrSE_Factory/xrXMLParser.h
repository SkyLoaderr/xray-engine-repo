#ifndef xrXMLParserH
#define xrXMLParserH
#pragma once

#ifdef XRXMLPARSER_EXPORTS
#define XRXMLPARSER_API __declspec(dllexport)
#else
#define XRXMLPARSER_API __declspec(dllimport)
#pragma comment(lib,"x:\\xrXMLParser.lib")
#endif

XRXMLPARSER_API void XML_DisableStringCaching();
XRXMLPARSER_API void XML_CleanUpMemory();

struct XRXMLPARSER_API StructSampleExport
{
};

XRXMLPARSER_API void FunctionSampleExport();


// UIXml.h: interface for the CUIXml class.
//
//
// для чтения параметров из XML файла, используя Chilkat XML
//
//////////////////////////////////////////////////////////////////////

#include "tinyxml.h"

#pragma once

typedef TiXmlNode		XML_NODE;
typedef TiXmlAttribute	XML_ATTRIBUTE;

class XRXMLPARSER_API CUIXml  
{

public:
	CUIXml();
	virtual ~CUIXml();

	bool Init(LPCSTR path, LPCSTR  xml_filename);


	//чтение элементов
	LPCSTR Read(LPCSTR path, int index,  LPCSTR   default_str_val = "" );
	LPCSTR Read(XML_NODE* start_node, LPCSTR path, int index,  LPCSTR   default_str_val = "" );
	LPCSTR Read(XML_NODE* node,  LPCSTR   default_str_val);

	int   ReadInt(LPCSTR path, int index,  int default_int_val = 0);
	int   ReadInt(XML_NODE* start_node,  LPCSTR path, int index,  int default_int_val = 0);
	int   ReadInt(XML_NODE* node,  int default_int_val);


	LPCSTR ReadAttrib(LPCSTR path,  int index, 
		LPCSTR attrib, LPCSTR default_str_val = "");
	LPCSTR ReadAttrib(XML_NODE* start_node, LPCSTR path,  int index, 
		LPCSTR attrib, LPCSTR default_str_val = "");
	LPCSTR ReadAttrib(XML_NODE* node,
		LPCSTR attrib, LPCSTR default_str_val);

	int   ReadAttribInt(LPCSTR path, int index,  
		LPCSTR attrib, int default_int_val = 0);
	int   ReadAttribInt(XML_NODE* start_node, LPCSTR path, int index,  
		LPCSTR attrib, int default_int_val = 0);
	int   ReadAttribInt(XML_NODE* node,
		LPCSTR attrib, int default_int_val);

	//нахождение элемента по содержимому 
	//по параметру
	XML_NODE* SearchForAttribute(LPCSTR path, int index, 
		LPCSTR tag_name,
		LPCSTR attrib, 
		LPCSTR attrib_value_pattern);

	XML_NODE* SearchForAttribute(XML_NODE* start_node, 
		LPCSTR tag_name,
		LPCSTR attrib, 
		LPCSTR attrib_value_pattern);

	//возвращает количество узлов с заданым именем
	int GetNodesNum(LPCSTR path, int index, LPCSTR tag_name);
	int GetNodesNum(XML_NODE* node, LPCSTR  tag_name);

	//проверка того, что аттрибуты у тегов уникальны
	//(если не NULL, то уникальность нарушена и возврашается имя 
	//повторяющегося атрибута)
	LPCSTR CheckUniqueAttrib (XML_NODE* start_node, 
		LPCSTR tag_name,
		LPCSTR attrib_name);

	//переместиться по XML дереву 
	//путь задается в форме PARENT:CHILD:CHIDLS_CHILD
	//node_index - номер, если узлов с одним именем несколько
	XML_NODE* NavigateToNode(LPCSTR  path, int node_index = 0);
	XML_NODE* NavigateToNode(XML_NODE* start_node, 
		LPCSTR  path, int node_index = 0);
	XML_NODE* NavigateToNodeWithAttribute(LPCSTR tag_name,
		LPCSTR attrib_name, LPCSTR attrib_value);

	void		SetLocalRoot(XML_NODE* pLocalRoot)		 	{m_pLocalRoot = pLocalRoot;}
	XML_NODE*	GetLocalRoot()							 	{return m_pLocalRoot;}

	XML_NODE*	GetRoot() {return m_root;}

	// Iterate through attributte
	typedef std::pair<shared_str, shared_str> AttribPair;
	// Params:
	// 1. prevAttrib - pointer to prev attrib. Place NULL to get first
	XML_ATTRIBUTE * QueryForAttrib(XML_NODE *node, int attribIdx);
	AttribPair		QueryAttribData(XML_ATTRIBUTE *attrib);

protected:
	XML_NODE* m_root;
	XML_NODE* m_pLocalRoot;

	//буфферный вектор для проверки уникальность аттрибутов
	std::vector<shared_str> m_AttribValues;
private:
	typedef TiXmlElement	XML_ELEM;
	TiXmlDocument	m_Doc;
};

#endif //xrXMLParserH