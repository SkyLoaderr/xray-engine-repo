// UIXmlInit.h: класс инициализации элементов окошек при помощи XML
//
//////////////////////////////////////////////////////////////////////

#ifndef _UI_XML_INIT_H_
#define _UI_XML_INIT_H_

#pragma once

#include "xrXMLParser.h"


#define RGB_ALPHA(a, r, g ,b)  ((u32) (((u8) (b) | ((u16) (g) << 8)) | (((u32) (u8) (r)) << 16)) | (((u32) (u8) (a)) << 24)) 

#include "UIFrameWindow.h"
#include "UIStatic.h"
#include "UIButton.h"
#include "UIDragDropList.h"
#include "UIProgressBar.h"
#include "UIListWnd.h"
#include "UITabControl.h"

class CUIXmlInit
{
public:
	CUIXmlInit();
	virtual ~CUIXmlInit();

	
	bool InitWindow(CUIXml& xml_doc, const char* path, 	int index, CUIWindow* pWnd);

	bool InitFrameWindow(CUIXml& xml_doc, const char* path, 
							int index, CUIFrameWindow* pWnd);

	bool InitStatic(CUIXml& xml_doc, const char* path, 
							int index, CUIStatic* pWnd);

	
	bool InitButton(CUIXml& xml_doc, const char* path, 
							int index, CUIButton* pWnd);

	bool InitDragDropList(CUIXml& xml_doc, const char* path, 
						int index, CUIDragDropList* pWnd);

	bool InitListWnd(CUIXml& xml_doc, const char* path, 
								   int index, CUIListWnd* pWnd);

	bool InitProgressBar(CUIXml& xml_doc, const char* path, 
						int index, CUIProgressBar* pWnd);

	bool InitFont(CUIXml &xml_doc, const char *path, 
				int index, u32 &color, CGameFont *&pFnt);

	bool InitTabControl(CUIXml &xml_doc, const char *path,
						int index, CUITabControl *pWnd);

	//автоматическая инициализация статических элеменитов
	bool InitAutoStatic(CUIXml& xml_doc, const char* tag_name, CUIWindow* pParentWnd);


protected:
	
	// Вводим пересчет координат для соответствия нужному разрешению
	// Params:	originalCoordinate - оригинальная базовая координата для разрешения 1024ч768
	// Return:	Пересчитанная координата для текущего разоешения	
	int RecalcXForResolution(int origninalCoordinate);
	int RecalcYForResolution(int origninalCoordinate);
};

#endif // _UI_XML_INIT_H_
