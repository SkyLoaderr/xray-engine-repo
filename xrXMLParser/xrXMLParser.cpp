// UIXml.cpp: implementation of the CUIXml class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "xrXMLParser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XRXMLPARSER_API void XML_DisableStringCaching()
{
	CkSettings::disableStringCaching();
}

XRXMLPARSER_API void XML_CleanUpMemory()
{
	CkSettings::cleanupMemory();
}


XRXMLPARSER_API CUIXml::CUIXml()
{
	// The Chilkat library can keep internal caches of string or other objects
	// and these may seem like memory leaks.  Call this to disable string
	// object caching, and call CkSettings::cleanupMemory to delete other
	// internal caches before checking for memory leaks.
}

XRXMLPARSER_API CUIXml::~CUIXml()
{
	//and call CkSettings::cleanupMemory to delete other
	// internal caches before checking for memory leaks.
}


//инициализация и загрузка XML файла
bool CUIXml::Init(LPCSTR path, const char* xml_filename)
{
	//if(!m_root.LoadXmlFile(xml_filename))
	//	return false;

	IReader *F = FS.r_open(path, xml_filename);
	if(F==NULL) return false;

	CMemoryWriter W;
	W.w(F->pointer(),F->length());
	W.w_stringZ("");
	if(!m_root.LoadXml((const char*)W.pointer()))
		return false;
		
	F->close();

	m_pLocalRoot = NULL;

	return true;
}

XML_NODE* CUIXml::NavigateToNode(XML_NODE* start_node, 
								 const char* path, 
								 int node_index)
{
	XML_NODE*	node			= NULL;
	XML_NODE*	node_parent		= NULL;

	char*		buf_str			= xr_strdup(path);

	char seps[]   = ":";
    char *token;

    //разбить путь на отдельные подпути
	token = strtok( buf_str, seps );

	if( token != NULL )
			node = start_node->GetNthChildWithTag(token, node_index);
	
    while( token != NULL )
    {
		// Get next token: 
		token = strtok( NULL, seps );

		if( token != NULL)
			if(node != 0) 
			{
				node_parent = node;
				node = node_parent->GetChildWithTag(token);
				delete(node_parent);
			}

    }

	xr_free	(buf_str);
	return node;
}

XML_NODE* CUIXml::NavigateToNode(const char* path, int node_index)
{
	return NavigateToNode(GetLocalRoot()?GetLocalRoot():GetRoot(), path, node_index);
}

XML_NODE* CUIXml::NavigateToNodeWithAttribute(const char *tag_name, const char *attrib_name, const char *attrib_value)
{

	XML_NODE	*root		= GetLocalRoot() ? GetLocalRoot() : GetRoot();
	int			tabsCount	= GetNodesNum(root, tag_name);

	for (int i = 0; i < tabsCount; ++i)
	{
		if (xr_strcmp(ReadAttrib(root, attrib_name, ""), attrib_value) == 0)
		{
			return NavigateToNode(root, tag_name, i);
		}
	}

	return NULL;
}


char* CUIXml::Read(const char *path, int index, const char*  default_str_val)
{
	XML_NODE* node = NavigateToNode(path, index);
	char* result = Read(node,  default_str_val);
	delete	(node);
	return	result;
}



char* CUIXml::Read(XML_NODE* start_node,  const char *path, int index, const char*  default_str_val)
{
	XML_NODE* node = NavigateToNode(start_node, path, index);
	char* result = Read(node,  default_str_val);
	delete	(node);
	return	result;
}


char* CUIXml::Read(XML_NODE* node,  const char*  default_str_val)
{
	if(node == NULL)
		return (char*)default_str_val;
	else
		return (char*)node->get_Content();
}



int CUIXml::ReadInt(XML_NODE* node, int default_int_val)
{
	char* result_str = Read(node, NULL ); 

	if(result_str==NULL)
	{
		return default_int_val;
	}

	return atoi(result_str);
}
int CUIXml::ReadInt(const char *path, int index, int default_int_val)
{
	char* result_str = Read(path, index, NULL ); 
	if(result_str==NULL)
	{
		return default_int_val;
	}
	return atoi(result_str);
}
int CUIXml::ReadInt(XML_NODE* start_node, const char *path, int index, int default_int_val)
{
	char* result_str = Read(start_node, path, index, NULL ); 
	if(result_str==NULL)
	{
		return default_int_val;
	}
	return atoi(result_str);
}



char* CUIXml::ReadAttrib(XML_NODE* start_node, const char *path,  int index, 
					const char *attrib, const char*  default_str_val)
{
	XML_NODE* node = NavigateToNode(start_node, path, index);
	char* result = ReadAttrib(node, attrib, default_str_val);
	delete	(node);

	return	result;
}


char* CUIXml::ReadAttrib(const char *path,  int index, 
					const char *attrib, const char*  default_str_val)
{
	XML_NODE* node = NavigateToNode(path, index);
	char* result = ReadAttrib(node, attrib, default_str_val);
	delete	(node);
	return	result;
}
char* CUIXml::ReadAttrib(XML_NODE* node, const char *attrib, const char*  default_str_val)
{
	if(node == NULL)
		return (char*)default_str_val;
	else
	{
		CkString str;
		char* result_str;

		bool result = node->GetAttrValue(attrib, str); 
		
		if(result)
		{
			result_str = str.extractString(NULL);
			return result_str;
		}
		else
		{
			return (char*)default_str_val;
		}
	}
}


int CUIXml::ReadAttribInt(XML_NODE* node, const char *attrib, int default_int_val)
{
	char* result_str = ReadAttrib(node, attrib, NULL); 

	if(result_str==NULL)
	{
		return default_int_val;
	}
	return atoi(result_str);
}

int CUIXml::ReadAttribInt(const char *path, int index,  
					const char *attrib, int default_int_val)
{
	char* result_str = ReadAttrib(path, index, attrib, NULL); 

	if(result_str==NULL)
	{
		return default_int_val;
	}

	return atoi(result_str);
}


int CUIXml::ReadAttribInt(XML_NODE* start_node, const char *path, int index,  
					const char *attrib, int default_int_val)
{
	char* result_str = ReadAttrib(start_node, path, index, attrib, NULL); 

	if(result_str==NULL)
	{
		return default_int_val;
	}

	return atoi(result_str);
}

int CUIXml::GetNodesNum(const char *path, int index, const char* tag_name)
{
	XML_NODE* node = NULL;
	
	if(path!=NULL)
	{
		node = NavigateToNode(path, index);

		if(node==NULL) node = &m_root;
	}
	else
		node = &m_root;
	
	if(node == NULL) return 0;
	int result =  node->NumChildrenHavingTag(tag_name);
	if(GetRoot()!=node) delete		(node);
	return result;
}

int CUIXml::GetNodesNum(XML_NODE* node, const char* tag_name)
{
	if(node == NULL) return 0;
	return node->NumChildrenHavingTag(tag_name);
}

//нахождение элемнета по его атрибуту
XML_NODE* CUIXml::SearchForAttribute(const char *path, int index, 
									 const char *tag_name, 	
									 const char *attrib, 
									 const char *attrib_value_pattern)

{
	XML_NODE* start_node = NavigateToNode(path, index);
	XML_NODE* result =  SearchForAttribute(start_node, tag_name, attrib, attrib_value_pattern);
	delete	(start_node);
	return	result;
}

XML_NODE* CUIXml::SearchForAttribute(XML_NODE* start_node, 
									const char *tag_name,
									const char *attrib, 
									const char *attrib_value_pattern)
{
	return m_root.SearchForAttribute(start_node, tag_name, 
									 attrib, attrib_value_pattern);
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       u32  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		CkSettings::disableStringCaching();
		break;
	}
	case DLL_PROCESS_DETACH:
		CkSettings::cleanupMemory();
		break;
	}
    return TRUE;
}

