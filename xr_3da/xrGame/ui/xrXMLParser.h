#ifndef xrXMLParserH
#define xrXMLParserH
#pragma once

#ifdef XRXMLPARSER_EXPORTS
	#define XRXMLPARSER_API __declspec(dllexport)
#else
	#define XRXMLPARSER_API __declspec(dllimport)
	#pragma comment(lib,"x:\\xrXMLParser.lib")
#endif


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

#include "CkXml.h"
#include "CkString.h"
#include "CkSettings.h"

#pragma once


typedef CkXml XML_NODE;


class XRXMLPARSER_API CUIXml  
{

public:
	CUIXml();
	virtual ~CUIXml();

	bool Init(LPCSTR path, const char* xml_filename);
	
	
	//чтение элементов
	char* Read(const char *path, int index,  const char*  default_str_val = "" );
	char* Read(XML_NODE* start_node, const char *path, int index,  const char*  default_str_val = "" );
	char* Read(XML_NODE* node,  const char*  default_str_val);
	
	int   ReadInt(const char *path, int index,  int default_int_val = 0);
	int   ReadInt(XML_NODE* start_node,  const char *path, int index,  int default_int_val = 0);
	int   ReadInt(XML_NODE* node,  int default_int_val);


	char* ReadAttrib(const char *path,  int index, 
						const char *attrib, const char*  default_str_val = "");
	char* ReadAttrib(XML_NODE* start_node, const char *path,  int index, 
						const char *attrib, const char*  default_str_val = "");
	char* ReadAttrib(XML_NODE* node,
						const char *attrib, const char*  default_str_val);

	int   ReadAttribInt(const char *path, int index,  
							const char *attrib, int default_int_val = 0);
	int   ReadAttribInt(XML_NODE* start_node, const char *path, int index,  
							const char *attrib, int default_int_val = 0);
	int   ReadAttribInt(XML_NODE* node,
							const char *attrib, int default_int_val);

	//нахождение элемента по содержимому 
	//по параметру
	XML_NODE* SearchForAttribute(const char *path, int index, 
								 const char *tag_name,
								 const char *attrib, 
								 const char *attrib_value_pattern);

	XML_NODE* SearchForAttribute(XML_NODE* start_node, 
								 const char *tag_name,
								 const char *attrib, 
								 const char *attrib_value_pattern);

	//возвращает количество узлов с заданым именем
	int GetNodesNum(const char *path, int index, const char* tag_name);
	int GetNodesNum(XML_NODE* node, const char* tag_name);
	
	
	//переместиться по XML дереву 
	//путь задается в форме PARENT:CHILD:CHIDLS_CHILD
	//node_index - номер, если узлов с одним именем несколько
	XML_NODE* NavigateToNode(const char* path, int node_index = 0);
	XML_NODE* NavigateToNode(XML_NODE* start_node, 
							 const char* path, int node_index = 0);

	XML_NODE* GetRoot() {return &m_root;}

protected:
	XML_NODE m_root;
};

#endif //xrXMLParserH