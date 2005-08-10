#pragma once

#include <vector>
#include "MyTexture.h"
#include "tinyxml.h"

class CTextureList
{
public:
	CTextureList(void);
	~CTextureList(void);
	void Add(const CString& str, int x, int y, int width, int height);
	void Add(const char*    str, int x, int y, int width, int height);
	bool Get(CMyTexture& texture, const char* id);
	bool Get(CMyTexture& texture, const CString& id);
	bool Del(const CString& id);
	void Reset();

	bool ParseFile(const CString& fileName);
	bool SaveFile(const CString& fileName);	

	std::vector<CMyTexture> m_list;
	CString					m_fileName;

protected:
	bool OpenFile(const CString& fileName);
	void ExportToXml();
	CString       GetTexFileName();
	CString       GetTextOfElement(TiXmlElement* element);
	void          SetTextFor(TiXmlElement& element, CString& text);	
	TiXmlElement* GetXmlFormTexture(CMyTexture& texture);
	TiXmlElement* GetFirstTexture();
	TiXmlElement* GetNextTexture(const TiXmlElement* texture);
	CMyTexture   GetTextureFromElement(const TiXmlElement* texture);

	TiXmlDocument			m_xmlDoc;
	bool					m_bFileAssociated;
};
