// UIXml.cpp: implementation of the CUIXml class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "xrXMLParser.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XRXMLPARSER_API CUIXml::CUIXml()
{
	// The Chilkat library can keep internal caches of string or other objects
	// and these may seem like memory leaks.  Call this to disable string
	// object caching, and call CkSettings::cleanupMemory to delete other
	// internal caches before checking for memory leaks.
	
	//CkSettings::disableStringCaching();

}

XRXMLPARSER_API CUIXml::~CUIXml()
{
	//and call CkSettings::cleanupMemory to delete other
	// internal caches before checking for memory leaks.

	//CkSettings::cleanupMemory();

}


//инициализация и загрузка XML файла
bool XRXMLPARSER_API CUIXml::Init(LPCSTR path, const char* xml_filename)
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
	


	return true;


	
}


CkXml* CUIXml::NavigateToNode(const char* path, int node_index)
{
	CkXml* node = NULL;

	char* buf_str;	
	
	buf_str = (char*)xr_malloc(strlen(path)*sizeof(char));
	strcpy(buf_str, path);


	char seps[]   = ":";
    char *token;

    //разбить путь на отдельные подпути
	token = strtok( buf_str, seps );

	if( token != NULL )
			node = m_root.GetNthChildWithTag( token, node_index);

	//path don't exists
//	ASSERT(node!=NULL);
	
    while( token != NULL )
    {
		// Get next token: 
		token = strtok( NULL, seps );

		if( token != NULL)
			if(node != 0) 
				node = node->GetChildWithTag(token);

    }

	
	xr_free(buf_str);


	return node;
}


char* CUIXml::Read(const char *path, int index, const char*  default_str_val)
{
	CkXml* node = NavigateToNode(path, index);

	
	if(node == NULL)
		return (char*)default_str_val;
	else
		return (char*)node->get_Content();
}


int XRXMLPARSER_API CUIXml::ReadInt(const char *path, int index, int default_int_val)
{
	char* result_str = Read(path, index, NULL ); 

	if(result_str==NULL)
	{
		return default_int_val;
	}

	return atoi(result_str);
}


char* CUIXml::ReadAttrib(const char *path,  int index, 
					const char *attrib, const char*  default_str_val)
{
	CkXml* node = NavigateToNode(path, index);
	
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

int XRXMLPARSER_API CUIXml::ReadAttribInt(const char *path, int index,  
					const char *attrib, int default_int_val)
{
	char* result_str = ReadAttrib(path, index, attrib, NULL); 

	if(result_str==NULL)
	{
		return default_int_val;
	}

	return atoi(result_str);

}

int XRXMLPARSER_API CUIXml::GetNodesNum(const char *path, int index, const char* tag_name)
{
	CkXml* node = NULL;
	
	if(path!=NULL)
	{
		node = NavigateToNode(path, index);

		if(node==NULL) node = &m_root;
	}
	else
		node = &m_root;

	

	
	if(node == NULL) return 0;

	return node->NumChildrenHavingTag(tag_name);
}
