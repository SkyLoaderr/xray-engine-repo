#ifndef xrXMLParserH
#define xrXMLParserH
#pragma once

#ifdef XRXMLPARSER_EXPORTS
	#define XRXMLPARSER_API __declspec(dllexport)
#else
	#define XRXMLPARSER_API __declspec(dllimport)
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


class XRXMLPARSER_API CUIXml  
{
public:
	CUIXml();
	virtual ~CUIXml();

	bool Init(LPCSTR path, const char* xml_filename);
	
	char* Read(const char *path, int index,  const char*  default_str_val = "" );
	int   ReadInt(const char *path, int index,  int default_int_val = 0);

	char* ReadAttrib(const char *path,  int index, 
						const char *attrib, const char*  default_str_val = "");

	int   ReadAttribInt(const char *path, int index,  
							const char *attrib, int default_int_val = 0);



	//возвращает количество узлов с заданым именем
	int GetNodesNum(const char *path, int index, const char* tag_name);
	
	

protected:
	
	//переместиться по XML дереву 
	//путь задается в форме PARENT:CHILD:CHIDLS_CHILD
	//node_index - номер, если узлов с одним именем несколько
	CkXml* NavigateToNode(const char* path, int node_index = 0);


	CkXml m_root;


};

#endif //xrXMLParserH