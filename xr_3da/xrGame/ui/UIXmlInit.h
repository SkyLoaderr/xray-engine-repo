// UIXmlInit.h: класс инициализации элементов окошек при помощи XML
//
//////////////////////////////////////////////////////////////////////

#ifndef _UI_XML_INIT_H_
#define _UI_XML_INIT_H_

#pragma once

#include "xrXMLParser.h"

#define RGB_ALPHA(a, r, g ,b)  ((u32) (((u8) (b) | ((u16) (g) << 8)) | (((u32) (u8) (r)) << 16)) | (((u32) (u8) (a)) << 24)) 

//////////////////////////////////////////////////////////////////////////

class CUIWindow;
class CUIFrameWindow;
class CUIStatic;
class CUIButton;
class CUIDragDropList;
class CUIProgressBar;
class CUIListWnd;
class CUITabControl;
class CUIFrameLineWnd;
class CUITextBanner;
class CUIMultiTextStatic;
class CUIAnimatedStatic;

//////////////////////////////////////////////////////////////////////////

class CUIXmlInit
{
public:
	CUIXmlInit();
	virtual ~CUIXmlInit();

	
	bool InitWindow				(CUIXml& xml_doc, const char* path,
								int index, CUIWindow* pWnd);

	bool InitFrameWindow		(CUIXml& xml_doc, const char* path, 
								int index, CUIFrameWindow* pWnd);

	bool InitFrameLine			(CUIXml& xml_doc, const char* path, 
								int index, CUIFrameLineWnd* pWnd);

	bool InitStatic				(CUIXml& xml_doc, const char* path, 
								int index, CUIStatic* pWnd);
	
	bool InitButton				(CUIXml& xml_doc, const char* path, 
								int index, CUIButton* pWnd);

	bool InitDragDropList		(CUIXml& xml_doc, const char* path, 
								int index, CUIDragDropList* pWnd);

	bool InitListWnd			(CUIXml& xml_doc, const char* path, 
								int index, CUIListWnd* pWnd);

	bool InitProgressBar		(CUIXml& xml_doc, const char* path, 
								int index, CUIProgressBar* pWnd);

	bool InitFont				(CUIXml &xml_doc, const char *path, 
								int index, u32 &color, CGameFont *&pFnt);

	bool InitColor				(CUIXml &xml_doc, XML_NODE* node, u32 &color);

	bool InitTabControl			(CUIXml &xml_doc, const char *path,
								int index, CUITabControl *pWnd);

	bool InitTextBanner			(CUIXml &xml_doc, const char *path,
								int index, CUITextBanner *pBnr);

	bool InitMultiTextStatic	(CUIXml &xml_doc, const char *path,
								int index, CUIMultiTextStatic *pWnd);

	bool InitAnimatedStatic		(CUIXml &xml_doc, const char *path,
								int index, CUIAnimatedStatic *pWnd);

	bool InitTexture			(CUIXml &xml_doc, const char *path,	int index, CUIStatic *pWnd);

	// ‘ункци€ чтени€ алайна из xml файла и применени€ его к координатам.
	// Return true если дл€ данного окна есть выравнивание
	static bool					InitAlignment(CUIXml &xml_doc, const char *path,
											  int index, int &x, int &y);

	// јвтоматическа€ инициализаци€ статических элеменитов
	// „тобы вернуть указатели на созданые статики (нам бывает необходимо пр€тать их, например)
	// создадим тип - вектор указателей на статики
	typedef		xr_vector<CUIStatic*>	StaticsVec;
	typedef		StaticsVec::iterator	StaticsVec_it;

	StaticsVec InitAutoStatic	(CUIXml& xml_doc, const char* tag_name, CUIWindow* pParentWnd);

	// ‘ункции дл€ пересчета координат дл€ применени€ выравнивани€
	// Params:
	// 1. align - выравнивание (см. EUIItemAlign)
	// 2. coord - координата к которй будет примененно выравнивание
	// Return: измененна€ координата
	static int					ApplyAlignX(int coord, u32 align);
	static int					ApplyAlignY(int coord, u32 align);
	static void					ApplyAlign(int &x, int &y, u32 align);

	// Initialize and store predefined colors
	typedef std::pair<shared_str, int> ColorMap;
	DEF_VECTOR(ColorDefs, ColorMap);

	static const ColorDefs		*GetColorDefs()		{ R_ASSERT(m_pColorDefs); return m_pColorDefs; }

	void						InitColorDefs();
	static void					DeleteColorDefs()	{ xr_delete(m_pColorDefs); }
private:
	static	ColorDefs			*m_pColorDefs;
};

#endif // _UI_XML_INIT_H_
