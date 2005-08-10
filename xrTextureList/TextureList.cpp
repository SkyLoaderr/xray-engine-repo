#include "StdAfx.h"
#include <string>
#include ".\texturelist.h"

typedef unsigned int u32;

CTextureList::CTextureList(void)
{
}

CTextureList::~CTextureList(void)
{
}

void CTextureList::Add(const CString& str, int x, int y, int width, int height){
    CMyTexture tex;

	tex.m_name = str;
	tex.x = x;
	tex.y = y;
	tex.width = width;
	tex.height = height;

	m_list.push_back(tex);
}

void CTextureList::Add(const char*    str, int x, int y, int width, int height){
    CMyTexture tex;

	tex.m_name = str;
	tex.x = x;
	tex.y = y;
	tex.width = width;
	tex.height = height;

	m_list.push_back(tex);
}

bool CTextureList::Get(CMyTexture& texture, const char* id){
	for (u32 i = 0; i<m_list.size(); i++){
		if (m_list[i].m_name == id)
		{
			texture = m_list[i];
			return true;
		}
	}

	return false;
}

bool CTextureList::Get(CMyTexture& texture, const CString& id){
	for (u32 i = 0; i<m_list.size(); i++){
		if (m_list[i].m_name == id)
		{
			texture = m_list[i];
			return true;
		}
	}

	return false;
}

CString CTextureList::GetTexFileName(){
   	TiXmlHandle docHandle(&m_xmlDoc);
	TiXmlElement* fn = docHandle.FirstChild("ui_texture").FirstChild("file_name").Element();

    return GetTextOfElement(fn);	
}

CString CTextureList::GetTextOfElement(TiXmlElement* element){
	CString txt;
	for (TiXmlNode* child = element->FirstChild(); child; child = child->NextSibling())
	{
		if (child->Type() == TiXmlElement::TEXT)
		{
			TiXmlText* xmlText = (TiXmlText*)child;
			txt = xmlText->Value();

			txt.Replace(_T("&amp;"),  _T("&"));
			txt.Replace(_T("&lt;"),   _T("<"));
			txt.Replace(_T("&gt;"),   _T(">"));
			txt.Replace(_T("&quot;"), _T("\""));
			txt.Replace(_T("&apos;"), _T("'"));

			return txt;
		}
	}

	return txt;
}

bool CTextureList::ParseFile(const CString& fileName){
	OpenFile(fileName);
	m_fileName = GetTexFileName();

	CMyTexture item;
	m_list.clear();
	for(TiXmlElement* element = GetFirstTexture(); element; element = GetNextTexture(element))
	{
		item = GetTextureFromElement(element);
		m_list.push_back(item);
	}    
	return true;
}

CMyTexture CTextureList::GetTextureFromElement(const TiXmlElement* texture){
	CMyTexture tex;
	const char* name;
	int ival;

	texture->Attribute("x", &ival);
	tex.x = ival;
	texture->Attribute("y", &ival);
	tex.y = ival;
	texture->Attribute("width", &ival);
	tex.width = ival;
	texture->Attribute("height", &ival);
	tex.height = ival;
	name = texture->Attribute("id");
	tex.m_name = name;

	return tex;
}

bool CTextureList::Del(const CString& id){
	for (u32 i = 0; i<m_list.size(); i++){
		if (m_list[i].m_name == id)
		{			
			m_list.erase(m_list.begin()+i);
			return true;
		}
	}

	return false;
}

bool CTextureList::OpenFile(const CString& fileName){
	m_xmlDoc.LoadFile(fileName);

	TiXmlHandle docHandle(&m_xmlDoc);
	TiXmlElement* game_dialogs = docHandle.FirstChild("ui_texture").Element();

	if (!game_dialogs)
	{
		MessageBox(NULL,"Wrong Texture XML file","Error",MB_OK);
		m_xmlDoc.Clear();
		m_bFileAssociated = false;
		return false;
	}

	m_bFileAssociated = true;

	return true;
}

void CTextureList::Reset(){
//	TiXmlDeclaration declaration("1.0","windows-1251","yes");
	TiXmlElement element("ui_texture");
	TiXmlElement file_name("file_name");
	element.InsertEndChild(file_name);

//	m_bFileAssociated = false;

	m_xmlDoc.Clear();
//	m_xmlDoc.InsertEndChild(declaration);
	m_xmlDoc.InsertEndChild(element);	
}

void CTextureList::ExportToXml(){
	Reset();

	TiXmlHandle docHandle(&m_xmlDoc);
	TiXmlElement* ui_texture = docHandle.FirstChild("ui_texture").Element();
	TiXmlElement* file_name = docHandle.FirstChild("ui_texture").FirstChild("file_name").Element();
    
	SetTextFor(*file_name, m_fileName);

	for (unsigned int i = 0; i < m_list.size(); i++)
	{
		TiXmlElement* el = GetXmlFormTexture(m_list[i]);
		ui_texture->InsertEndChild(*el);
		delete el;
	}
}

TiXmlElement* CTextureList::GetXmlFormTexture(CMyTexture& texture){
    TiXmlElement* element = new TiXmlElement("texture");

	element->SetAttribute("id", texture.m_name.GetBuffer());
	texture.m_name.ReleaseBuffer();
	element->SetAttribute("x", texture.x);
	element->SetAttribute("y", texture.y);
	element->SetAttribute("width", texture.width);
	element->SetAttribute("height", texture.height);

	return element;
}

bool CTextureList::SaveFile(const CString& fileName){
	ExportToXml();

	if (!m_bFileAssociated && fileName.IsEmpty())
		return false;

	if (!fileName.IsEmpty())
	{
		m_xmlDoc.SaveFile(fileName);
		m_bFileAssociated = true;
	}
	else
	{
		m_xmlDoc.SaveFile();
		if (m_xmlDoc.Error())
		{
			AfxMessageBox(m_xmlDoc.ErrorDesc());
			return false;
		}
	}

	
    
	return true;
}

void CTextureList::SetTextFor(TiXmlElement& element, CString& text){
	TiXmlText xmlText(text);
	element.InsertEndChild(xmlText);
}


TiXmlElement* CTextureList::GetFirstTexture(){
	TiXmlHandle docHandle(&m_xmlDoc);
	TiXmlElement* texture = docHandle.FirstChild("ui_texture").FirstChild("texture").Element();

	return texture;
}

TiXmlElement* CTextureList::GetNextTexture(const TiXmlElement* texture){
	CString value("texture");
	CString gotValue;
	TiXmlElement* nextTexture = (TiXmlElement*)texture;
	while (nextTexture = nextTexture->NextSiblingElement())
	{
		gotValue = nextTexture->Value();
		if (gotValue == value)
			return nextTexture;
	}
    return NULL;
}