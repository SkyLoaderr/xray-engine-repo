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
bool CUIXml::Init(LPCSTR path, LPCSTR  xml_filename)
{
	//if(!m_root.LoadXmlFile(xml_filename))
	//	return false;

	IReader *F = FS.r_open(path, xml_filename);
	if(F==NULL) return false;

	CMemoryWriter W;
	W.w(F->pointer(),F->length());
	W.w_stringZ("");
	if(!m_root.LoadXml((LPCSTR )W.pointer()))
		return false;
		
	F->close();

	m_pLocalRoot = NULL;

	return true;
}

XML_NODE* CUIXml::NavigateToNode(XML_NODE* start_node, 
								 LPCSTR  path, 
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

XML_NODE* CUIXml::NavigateToNode(LPCSTR  path, int node_index)
{
	return NavigateToNode(GetLocalRoot()?GetLocalRoot():GetRoot(), path, node_index);
}

XML_NODE* CUIXml::NavigateToNodeWithAttribute(LPCSTR tag_name, LPCSTR attrib_name, LPCSTR attrib_value)
{

	XML_NODE	*root		= GetLocalRoot() ? GetLocalRoot() : GetRoot();
	int			tabsCount	= GetNodesNum(root, tag_name);

	for (int i = 0; i < tabsCount; ++i)
	{
		if (xr_strcmp(ReadAttrib(root, tag_name, i, attrib_name, ""), attrib_value) == 0)
		{
			return NavigateToNode(root, tag_name, i);
		}
	}

	return NULL;
}


LPCSTR CUIXml::Read(LPCSTR path, int index, LPCSTR   default_str_val)
{
	XML_NODE* node = NavigateToNode(path, index);
	LPCSTR result = Read(node,  default_str_val);
	delete	(node);
	return	result;
}



LPCSTR CUIXml::Read(XML_NODE* start_node,  LPCSTR path, int index, LPCSTR   default_str_val)
{
	XML_NODE* node = NavigateToNode(start_node, path, index);
	LPCSTR result = Read(node,  default_str_val);
	delete	(node);
	return	result;
}


LPCSTR CUIXml::Read(XML_NODE* node,  LPCSTR   default_str_val)
{
	if(node == NULL)
		return default_str_val;
	else
		return node->get_Content();
}



int CUIXml::ReadInt(XML_NODE* node, int default_int_val)
{
	LPCSTR result_str = Read(node, NULL ); 

	if(result_str==NULL)
	{
		return default_int_val;
	}

	return atoi(result_str);
}
int CUIXml::ReadInt(LPCSTR path, int index, int default_int_val)
{
	LPCSTR result_str = Read(path, index, NULL ); 
	if(result_str==NULL)
	{
		return default_int_val;
	}
	return atoi(result_str);
}
int CUIXml::ReadInt(XML_NODE* start_node, LPCSTR path, int index, int default_int_val)
{
	LPCSTR result_str = Read(start_node, path, index, NULL ); 
	if(result_str==NULL)
	{
		return default_int_val;
	}
	return atoi(result_str);
}



LPCSTR CUIXml::ReadAttrib(XML_NODE* start_node, LPCSTR path,  int index, 
					LPCSTR attrib, LPCSTR   default_str_val)
{
	XML_NODE* node = NavigateToNode(start_node, path, index);
	LPCSTR result = ReadAttrib(node, attrib, default_str_val);
	delete	(node);

	return	result;
}


LPCSTR CUIXml::ReadAttrib(LPCSTR path,  int index, 
					LPCSTR attrib, LPCSTR   default_str_val)
{
	XML_NODE* node = NavigateToNode(path, index);
	LPCSTR result = ReadAttrib(node, attrib, default_str_val);
	delete	(node);
	return	result;
}
LPCSTR CUIXml::ReadAttrib(XML_NODE* node, LPCSTR attrib, LPCSTR   default_str_val)
{
	if(node == NULL)
		return default_str_val;
	else
	{
		CkString str;
		//обязательно делаем ref_str, а то 
		//не сможем запомнить строку и return вернет левый указатель
		ref_str result_str;

		bool result = node->GetAttrValue(attrib, str); 
		
		if(result)
		{
			result_str = str.getString();
			return *result_str;
		}
		else
		{
			return default_str_val;
		}
	}
}


int CUIXml::ReadAttribInt(XML_NODE* node, LPCSTR attrib, int default_int_val)
{
	LPCSTR result_str = ReadAttrib(node, attrib, NULL); 

	if(result_str==NULL)
	{
		return default_int_val;
	}
	return atoi(result_str);
}

int CUIXml::ReadAttribInt(LPCSTR path, int index,  
					LPCSTR attrib, int default_int_val)
{
	LPCSTR result_str = ReadAttrib(path, index, attrib, NULL); 

	if(result_str==NULL)
	{
		return default_int_val;
	}

	return atoi(result_str);
}


int CUIXml::ReadAttribInt(XML_NODE* start_node, LPCSTR path, int index,  
					LPCSTR attrib, int default_int_val)
{
	LPCSTR result_str = ReadAttrib(start_node, path, index, attrib, NULL); 

	if(result_str==NULL)
	{
		return default_int_val;
	}

	return atoi(result_str);
}

int CUIXml::GetNodesNum(LPCSTR path, int index, LPCSTR  tag_name)
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

int CUIXml::GetNodesNum(XML_NODE* node, LPCSTR  tag_name)
{
	if(node == NULL) return 0;
	return node->NumChildrenHavingTag(tag_name);
}

//нахождение элемнета по его атрибуту
XML_NODE* CUIXml::SearchForAttribute(LPCSTR path, int index, 
									 LPCSTR tag_name, 	
									 LPCSTR attrib, 
									 LPCSTR attrib_value_pattern)

{
	XML_NODE* start_node = NavigateToNode(path, index);
	XML_NODE* result =  SearchForAttribute(start_node, tag_name, attrib, attrib_value_pattern);
	delete	(start_node);
	return	result;
}

XML_NODE* CUIXml::SearchForAttribute(XML_NODE* start_node, 
									LPCSTR tag_name,
									LPCSTR attrib, 
									LPCSTR attrib_value_pattern)
{
	return m_root.SearchForAttribute(start_node, tag_name, 
									 attrib, attrib_value_pattern);
}


LPCSTR CUIXml::CheckUniqueAttrib (XML_NODE* start_node, 
									LPCSTR tag_name,
									LPCSTR attrib_name)
{
	m_AttribValues.clear();

	int tags_num = GetNodesNum(start_node, tag_name);
	for(int i=0; i<tags_num; i++)
	{
		LPCSTR attrib = ReadAttrib(start_node, tag_name, i, attrib_name, NULL);
		
		std::vector<ref_str>::iterator it = std::find(m_AttribValues.begin(), 
												 m_AttribValues.end(), ref_str(attrib));

		 if(m_AttribValues.end() != it) 
			 return attrib;
		 
		 m_AttribValues.push_back(attrib);
	}

	return NULL;
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

