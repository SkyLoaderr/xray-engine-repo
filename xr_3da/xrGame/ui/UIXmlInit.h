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

	bool InitProgressBar(CUIXml& xml_doc, const char* path, 
						int index, CUIProgressBar* pWnd);

protected:
	
	
};

#endif // _UI_XML_INIT_H_
