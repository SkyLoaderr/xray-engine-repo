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
//	CkSettings::disableStringCaching();
}

XRXMLPARSER_API void XML_CleanUpMemory()
{
//	CkSettings::cleanupMemory();
}


XRXMLPARSER_API CUIXml::CUIXml()
	:	m_root			(NULL),
		m_pLocalRoot	(NULL)
//		m_Doc			("x:\\Memory.xml")
{
}

XRXMLPARSER_API CUIXml::~CUIXml()
{
	m_Doc.Clear();
}



bool CUIXml::Init(LPCSTR path_alias, LPCSTR path, LPCSTR xml_filename)
{
	string_path str;
	sprintf(str,"%s\\%s", path, xml_filename);
	return Init(path_alias, str);
}

bool CUIXml::Init(LPCSTR path_alias, LPCSTR path1, LPCSTR path2, LPCSTR xml_filename)
{
	string_path str;
	sprintf(str,"%s\\%s\\%s", path1, path2, xml_filename);
	return Init(path_alias, str);
}

void ParseFile(LPCSTR path, CMemoryWriter& W, IReader *F )
{
	string4096	str;
	
	while( !F->eof() ){
		F->r_string		(str,sizeof(str));

		if (str[0] && (str[0]=='#') && strstr(str,"#include") ){
			string256	inc_name;	
			if (_GetItem	(str,1,inc_name,'"')){
				IReader* I 	= FS.r_open(path, inc_name);
				if(!I){
					string1024 str;
					sprintf(str,"XML file[%s] parsing failed. Can't find include file:[%s]",m_xml_file_name,inc_name);
					R_ASSERT2(false,str);"Can't find include file:",inc_name);
				}
				ParseFile(path, W, I);
				FS.r_close	(I);
			}
//			F->r_string		(str,sizeof(str));
		}else
			W.w_string		(str);

	}
}
//������������� � �������� XML �����
bool CUIXml::Init(LPCSTR path, LPCSTR  xml_filename)
{
	ZeroMemory(m_xml_file_name,sizeof(m_xml_file_name) );
	strcpy(m_xml_file_name, xml_filename);
	// Load and parse xml file

	IReader *F = FS.r_open(path, xml_filename);
	if(F==NULL) return false;

	CMemoryWriter W;
	ParseFile(path, W, F);
//	W.w(F->pointer(),F->length());
	W.w_stringZ("");
	FS.r_close(F);

	m_Doc.Parse((LPCSTR )W.pointer());
	if (m_Doc.Error()){
		string1024 str;
		sprintf(str, "XML file:%s value:%s errDescr:%s",m_xml_file_name,m_Doc.Value(), m_Doc.ErrorDesc());
		R_ASSERT2(false, str);
	} 

	m_root = m_Doc.FirstChildElement();

	return true;
}

XML_NODE* CUIXml::NavigateToNode(XML_NODE* start_node, 
								 LPCSTR  path, 
								 int node_index)
{
	R_ASSERT3	(start_node && path, "NavigateToNode failed in XML file ",m_xml_file_name);
	XML_NODE*	node			= NULL;
	XML_NODE*	node_parent		= NULL;

	char*		buf_str			= xr_strdup(path);

	char seps[]		= ":";
    char *token;
	int tmp			= 0;

    //������� ���� �� ��������� �������
	token = strtok( buf_str, seps );

	if( token != NULL )
	{
		node = start_node->FirstChild(token);

		while (tmp++ < node_index && node)
		{
			node = start_node->IterateChildren(token, node);
		}
	}
	
    while( token != NULL )
    {
		// Get next token: 
		token = strtok( NULL, seps );

		if( token != NULL)
			if(node != 0) 
			{
				node_parent = node;
				node = node_parent->FirstChild(token);
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
		LPCSTR result = ReadAttrib(root, tag_name, i, attrib_name, "");
		if (result && xr_strcmp(result, attrib_value) == 0)
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
	return	result;
}



LPCSTR CUIXml::Read(XML_NODE* start_node,  LPCSTR path, int index, LPCSTR   default_str_val)
{
	XML_NODE* node = NavigateToNode(start_node, path, index);
	LPCSTR result = Read(node,  default_str_val);
	return	result;
}


LPCSTR CUIXml::Read(XML_NODE* node,  LPCSTR   default_str_val)
{
	if(node == NULL)
		return default_str_val;
	else
	{
		node = node->FirstChild();
		if (!node) return default_str_val;

		TiXmlText *text = node->ToText();
		if (text) return text->Value();
		else return default_str_val;
	}
}



int CUIXml::ReadInt(XML_NODE* node, int default_int_val)
{
	LPCSTR result_str = Read(node, NULL ); 

	if(result_str==NULL)
		return default_int_val;

	return atoi(result_str);
}
int CUIXml::ReadInt(LPCSTR path, int index, int default_int_val)
{
	LPCSTR result_str = Read(path, index, NULL ); 
	if(result_str==NULL)
		return default_int_val;

	return atoi(result_str);
}
int CUIXml::ReadInt(XML_NODE* start_node, LPCSTR path, int index, int default_int_val)
{
	LPCSTR result_str = Read(start_node, path, index, NULL ); 
	if(result_str==NULL)
		return default_int_val;

	return atoi(result_str);
}

float   CUIXml::ReadFlt(LPCSTR path, int index,  float default_flt_val)
{
	LPCSTR result_str = Read(path, index, NULL ); 
	if(result_str==NULL)
		return default_flt_val;

	return (float)atof(result_str);
}
float   CUIXml::ReadFlt(XML_NODE* start_node,  LPCSTR path, int index,  float default_flt_val)
{
	LPCSTR result_str = Read(start_node, path, index, NULL ); 
	if(result_str==NULL)
		return default_flt_val;

	return (float)atof(result_str);
}
float   CUIXml::ReadFlt(XML_NODE* node,  float default_flt_val)
{
	LPCSTR result_str = Read(node, NULL ); 

	if(result_str==NULL)
		return default_flt_val;

	return (float)atof(result_str);
}



LPCSTR CUIXml::ReadAttrib(XML_NODE* start_node, LPCSTR path,  int index, 
					LPCSTR attrib, LPCSTR   default_str_val)
{
	XML_NODE* node = NavigateToNode(start_node, path, index);
	LPCSTR result = ReadAttrib(node, attrib, default_str_val);

	return	result;
}


LPCSTR CUIXml::ReadAttrib(LPCSTR path,  int index, 
					LPCSTR attrib, LPCSTR   default_str_val)
{
	XML_NODE* node = NavigateToNode(path, index);
	LPCSTR result = ReadAttrib(node, attrib, default_str_val);
	return	result;
}
LPCSTR CUIXml::ReadAttrib(XML_NODE* node, LPCSTR attrib, LPCSTR   default_str_val)
{
	if(node == NULL)
		return default_str_val;
	else
	{
		//����������� ������ ref_str, � �� 
		//�� ������ ��������� ������ � return ������ ����� ���������
		shared_str result_str;

		// ������� ���� �� ��������

		TiXmlElement *el = node->ToElement(); 
		
		if(el)
		{
			result_str = el->Attribute(attrib);
			if (result_str.size())
				return *result_str;
			else
				return default_str_val;
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
		return default_int_val;

	return atoi(result_str);
}

int CUIXml::ReadAttribInt(LPCSTR path, int index,  
					LPCSTR attrib, int default_int_val)
{
	LPCSTR result_str = ReadAttrib(path, index, attrib, NULL); 

	if(result_str==NULL)
		return default_int_val;

	return atoi(result_str);
}


int CUIXml::ReadAttribInt(XML_NODE* start_node, LPCSTR path, int index,  
					LPCSTR attrib, int default_int_val)
{
	LPCSTR result_str = ReadAttrib(start_node, path, index, attrib, NULL); 

	if(result_str==NULL)
		return default_int_val;
	return atoi(result_str);
}

float   CUIXml::ReadAttribFlt(LPCSTR path,	int index,  LPCSTR attrib, float default_flt_val)
{
	LPCSTR result_str = ReadAttrib(path, index, attrib, NULL); 

	if(result_str==NULL)
		return default_flt_val;

	return (float)atof(result_str);
}

float   CUIXml::ReadAttribFlt(XML_NODE* start_node, LPCSTR path, int index,  LPCSTR attrib, float default_flt_val)
{
	LPCSTR result_str = ReadAttrib(start_node, path, index, attrib, NULL); 

	if(result_str==NULL)
		return default_flt_val;

	return (float)atof(result_str);
}
float   CUIXml::ReadAttribFlt(XML_NODE* node,	LPCSTR attrib, float default_flt_val)
{
	LPCSTR result_str = ReadAttrib(node, attrib, NULL); 

	if(result_str==NULL)
		return default_flt_val;

	return (float)atof(result_str);
}

int CUIXml::GetNodesNum(LPCSTR path, int index, LPCSTR  tag_name)
{
	XML_NODE* node = NULL;
	
	XML_NODE *root = GetLocalRoot()?GetLocalRoot():GetRoot();
	if(path!=NULL)
	{
		node = NavigateToNode(path, index);

		if(node==NULL) node = root;
	}
	else
		node = root;
	
	if(node == NULL) return 0;

	return GetNodesNum(node, tag_name);
}

int CUIXml::GetNodesNum(XML_NODE* node, LPCSTR  tag_name)
{
	if(node == NULL) return 0;

	XML_NODE *el = NULL;

	if (!tag_name)
		el = node->FirstChild();
	else
		el = node->FirstChild(tag_name);

	int result = 0;

	while (el)
	{
		++result;
		if (!tag_name)
			el = el->NextSibling();
		else
			el = el->NextSibling(tag_name);
	}
	
	return result;
}

//���������� �������� �� ��� ��������
XML_NODE* CUIXml::SearchForAttribute(LPCSTR path, int index, 
									 LPCSTR tag_name, 	
									 LPCSTR attrib, 
									 LPCSTR attrib_value_pattern)

{
	XML_NODE* start_node = NavigateToNode(path, index);
	XML_NODE* result =  SearchForAttribute(start_node, tag_name, attrib, attrib_value_pattern);
	return	result;
}

XML_NODE* CUIXml::SearchForAttribute(XML_NODE* start_node, 
									LPCSTR tag_name,
									LPCSTR attrib, 
									LPCSTR attrib_value_pattern)
{
	while (start_node)
	{
		TiXmlElement *el = start_node->ToElement();
		if (el)
		{
			LPCSTR attribStr	= el->Attribute(attrib);
			LPCSTR valueStr		= el->Value();
			if (attribStr &&  0 == xr_strcmp(attribStr, attrib_value_pattern) &&
				valueStr && 0 == xr_strcmp(valueStr, tag_name))
			{
				return el;
			}
		}

		XML_NODE *newEl = start_node->FirstChild(tag_name);
		newEl = SearchForAttribute(newEl, tag_name, attrib, attrib_value_pattern);
		if (newEl) return newEl;
		start_node = start_node->NextSibling(tag_name);
	}

	return NULL;
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
		
		std::vector<shared_str>::iterator it = std::find(m_AttribValues.begin(), 
												 m_AttribValues.end(), attrib);

		 if(m_AttribValues.end() != it) 
			 return attrib;
		 
		 m_AttribValues.push_back(attrib);
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

XML_ATTRIBUTE * CUIXml::QueryForAttrib(XML_NODE *node, int attribIdx)
{
	R_ASSERT3(node,"QueryForAttrib failed in XML file ",m_xml_file_name);

	XML_ELEM * elem = node->ToElement();
	R_ASSERT3(elem,"ToElement failed in XML file ",m_xml_file_name);

	XML_ATTRIBUTE * attrib = elem->FirstAttribute();
	int idx = 0;

	while (!attrib && idx < attribIdx)
	{
		attrib = attrib->Next();
	}

	return attrib;
}

//////////////////////////////////////////////////////////////////////////

CUIXml::AttribPair CUIXml::QueryAttribData(XML_ATTRIBUTE *attrib)
{
	R_ASSERT3	(attrib, "QueryAttribData failed in XML file ",m_xml_file_name);
	return std::make_pair(attrib->Name(), attrib->Value());
}

//////////////////////////////////////////////////////////////////////////

BOOL APIENTRY DllMain( HANDLE hModule, 
                       u32  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
//		CkSettings::disableStringCaching();
		break;
	}
	case DLL_PROCESS_DETACH:
//		CkSettings::cleanupMemory();
		break;
	}
    return TRUE;
}

