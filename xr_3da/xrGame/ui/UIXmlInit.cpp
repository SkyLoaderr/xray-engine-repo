// UIXmlInit.cpp: класс инициализации элементов окошек при помощи XML
//
//////////////////////////////////////////////////////////////////////


#include"stdafx.h"

#include"UIXmlInit.h"


CUIXmlInit::CUIXmlInit()
{
}
CUIXmlInit::~CUIXmlInit()
{
}


bool CUIXmlInit::InitFrameWindow(CUIXml& xml_doc, const char* path, 
									int index, CUIFrameWindow* pWnd)
{
	string256 buf;

	int x = xml_doc.ReadAttribInt(path, index, "x");
	int y = xml_doc.ReadAttribInt(path, index, "y");
	int width = xml_doc.ReadAttribInt(path, index, "width");
	int height = xml_doc.ReadAttribInt(path, index, "height");
	
	char* base_name = xml_doc.Read(strconcat(buf,path,":base_texture"), index, NULL);

	if(!base_name) return false;
		
	pWnd->Init(base_name, x, y, width, height);

	strconcat(buf,path,":left_top_texture");
	char* tex_name = xml_doc.Read(buf, index, NULL);

	x = xml_doc.ReadAttribInt(buf, index, "x");
	y = xml_doc.ReadAttribInt(buf, index, "y");

	if(tex_name) pWnd->InitLeftTop(tex_name, x,y);


	strconcat(buf,path,":left_bottom_texture");
	tex_name = xml_doc.Read(buf, index, NULL);

	x = xml_doc.ReadAttribInt(buf, index, "x");
	y = xml_doc.ReadAttribInt(buf, index, "y");

	if(tex_name) pWnd->InitLeftBottom(tex_name, x,y);

	pWnd->InitLeftBottom(tex_name, x,y);

	return true;
}


bool CUIXmlInit::InitStatic(CUIXml& xml_doc, const char* path, 
									int index, CUIStatic* pWnd)
{
	string256 buf;

	int x = xml_doc.ReadAttribInt(path, index, "x");
	int y = xml_doc.ReadAttribInt(path, index, "y");
	int width = xml_doc.ReadAttribInt(path, index, "width");
	int height = xml_doc.ReadAttribInt(path, index, "height");
	
	char* texture = xml_doc.Read(strconcat(buf,path,":texture"), index, NULL);

	if(texture)
	{
		pWnd->Init(texture, x, y, width, height);
	}

	char* text = xml_doc.Read(strconcat(buf,path,":text"), index, NULL);
	pWnd->SetText(text);

	return true;
}

bool CUIXmlInit::InitButton(CUIXml& xml_doc, const char* path, 
						int index, CUIButton* pWnd)
{
	string256 buf;

	int x = xml_doc.ReadAttribInt(path, index, "x");
	int y = xml_doc.ReadAttribInt(path, index, "y");
	int width = xml_doc.ReadAttribInt(path, index, "width");
	int height = xml_doc.ReadAttribInt(path, index, "height");
	
	char* texture = xml_doc.Read(strconcat(buf,path,":texture"), index, NULL);

	if(!texture) return false;
	
	pWnd->Init(texture, x, y, width, height);
	
	char* text = xml_doc.Read(strconcat(buf,path,":text"), index, NULL);
	pWnd->SetText(text);

	return true;
}

bool CUIXmlInit::InitDragDropList(CUIXml& xml_doc, const char* path, 
						int index, CUIDragDropList* pWnd)
{
	int x = xml_doc.ReadAttribInt(path, index, "x");
	int y = xml_doc.ReadAttribInt(path, index, "y");
	int width = xml_doc.ReadAttribInt(path, index, "width");
	int height = xml_doc.ReadAttribInt(path, index, "height");


	pWnd->Init(x,y, width,height);

	int cell_width = xml_doc.ReadAttribInt(path, index, "cell_width");
	int cell_height = xml_doc.ReadAttribInt(path, index, "cell_height");
	int rows_num = xml_doc.ReadAttribInt(path, index, "rows_num");
	int cols_num = xml_doc.ReadAttribInt(path, index, "cols_num");

	pWnd->SetCellWidth(cell_width);
	pWnd->SetCellHeight(cell_height);
	pWnd->InitGrid(rows_num,cols_num);


	return true;
}

bool CUIXmlInit::InitProgressBar(CUIXml& xml_doc, const char* path, 
						int index, CUIProgressBar* pWnd)
{
	string256 buf;
	

	int x = xml_doc.ReadAttribInt(path, index, "x");
	int y = xml_doc.ReadAttribInt(path, index, "y");
	int width = xml_doc.ReadAttribInt(path, index, "length");
	int height = xml_doc.ReadAttribInt(path, index, "broad");
	bool is_horizontal = (xml_doc.ReadAttribInt(path, index, "horz")==1);

	pWnd->Init(x, y, width, height, is_horizontal);

	int min = xml_doc.ReadAttribInt(path, index, "min");
	int max = xml_doc.ReadAttribInt(path, index, "max");
	int pos = xml_doc.ReadAttribInt(path, index, "pos");
	
	pWnd->SetRange((s16)min,(s16)max);
	pWnd->SetProgressPos((s16)pos);
	
	strconcat(buf,path,":texture");

	char* texture = xml_doc.Read(buf, index, NULL);
	if(!texture) return false;

	int progress_length = xml_doc.ReadAttribInt(buf, index, "length");
	bool tile = (xml_doc.ReadAttribInt(buf, index, "tile")==1);
	int r = xml_doc.ReadAttribInt(buf, index, "r");
	int g = xml_doc.ReadAttribInt(buf, index, "g");
	int b = xml_doc.ReadAttribInt(buf, index, "b");

	u32 color = RGB_ALPHA(0xFF, r,g,b);

	pWnd->SetProgressTexture(texture,progress_length,tile,color);


	texture = xml_doc.Read(strconcat(buf,path,":background"), index, NULL);
	x = xml_doc.ReadAttribInt(buf, index, "x");
	y = xml_doc.ReadAttribInt(buf, index, "y");

	pWnd->SetBackgroundTexture(texture,x,y);
	

	return true;
}