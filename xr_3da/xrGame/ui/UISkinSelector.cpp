//-----------------------------------------------------------------------------/
//  Окно выбора скина в сетевой игре
//-----------------------------------------------------------------------------/

// PCH
#include "StdAfx.h"

// Another includes
#include "UISkinSelector.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include"../hudmanager.h"

// Constants
const char * const	SKIN_SELECTOR_XML		= "skin_selector.xml";
const u32			clActive				= 0xffffffff;
const u32			clInactive				= 0xaa888888;

//-----------------------------------------------------------------------------/
// Constructors and Destructor
//-----------------------------------------------------------------------------/

CUISkinSelectorWnd::CUISkinSelectorWnd(const char* strSectionName)
	: m_uActiveIndex(0)
{
	Init(strSectionName);
}

////////////////////////////////////////////////////////////////////////////////

CUISkinSelectorWnd::CUISkinSelectorWnd()
{
	Init("a");
}

////////////////////////////////////////////////////////////////////////////////

CUISkinSelectorWnd::~CUISkinSelectorWnd()
{
}

////////////////////////////////////////////////////////////////////////////////

void CUISkinSelectorWnd::Init(const char *strSectionName)
{
	CUIXml xml_doc;
	bool xml_result = xml_doc.Init("$game_data$", SKIN_SELECTOR_XML);
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit xml_init;

	CUIWindow::Init(0,0, Device.dwWidth, Device.dwHeight);

	RECT rect, r;

	// Читаем из xml файла параметры окна и контролов

	// Код в цикле почти повторяет код функции InitFrameWindow. Все из-за невозможности повлиять
	// на размер FrameWindow с помощью простого изменения кординат. Другими словами SetWndRect делает
	// только половину работы - положение меняет, а размеры нет.
	int					xC		= 0;
	int					yC;
	int					width	= CalculateSkinWindowWidth();

	const char * const	path	= "frame_window";
	const int			index	= 0;
	CUIFrameWindow		*pWnd	= NULL;

	for (int i = 0; i < SKINS_COUNT; ++i)
	{
		AttachChild(&m_vSkinWindows[i].UIBackground);
		pWnd = &m_vSkinWindows[i].UIBackground;

		R_ASSERT2(xml_doc.NavigateToNode(path,index), "XML node not found");

		string256 buf;

		int height = xml_doc.ReadAttribInt(path, index, "height");

		ref_str base_name = xml_doc.Read(strconcat(buf,path,":base_texture"), index, NULL);

		if(!base_name) return;

		yC	 = Device.dwHeight / 2 - height / 2;

		// Единственное место, где надо установить не считанные данные, а вычесленные
		pWnd->Init(*base_name, xC, yC, width, height);

		strconcat(buf,path,":left_top_texture");
		ref_str tex_name = xml_doc.Read(buf, index, NULL);

		int x = xml_doc.ReadAttribInt(buf, index, "x");
		int y = xml_doc.ReadAttribInt(buf, index, "y");

		if(*tex_name) pWnd->InitLeftTop(*tex_name, x,y);


		strconcat(buf,path,":left_bottom_texture");
		tex_name = xml_doc.Read(buf, index, NULL);

		x = xml_doc.ReadAttribInt(buf, index, "x");
		y = xml_doc.ReadAttribInt(buf, index, "y");

		if(*tex_name) pWnd->InitLeftBottom(*tex_name, x,y);

		//инициализировать заголовок окна
		strconcat(buf,path,":title");
		if(xml_doc.NavigateToNode(buf,index)) xml_init.InitStatic(xml_doc, buf, index, &pWnd->UITitleText);

		xC	+= width;
	}

	// Тут происходит инициализация изображений скинов 
	InitializeSkins();

	// Ok and Cancel buttons
	r		= m_vSkinWindows[0].UIBackground.GetWndRect();

	AttachChild(&UIOkBtn);
	xml_init.InitButton(xml_doc, "button", 0, &UIOkBtn);
	rect	= UIOkBtn.GetWndRect();

	UIOkBtn.MoveWindow(Device.dwWidth / 2 - static_cast<int>((rect.right - rect.left) * 1.5),
		r.bottom + r.top / 2 - UIOkBtn.GetHeight());

	AttachChild(&UICancelBtn);
	xml_init.InitButton(xml_doc, "button", 1, &UICancelBtn);
	rect	= UICancelBtn.GetWndRect();
	UICancelBtn.MoveWindow(Device.dwWidth / 2 + static_cast<int>((rect.right - rect.left) * 0.5),
		r.bottom + r.top / 2 - UICancelBtn.GetHeight());
}

////////////////////////////////////////////////////////////////////////////////

void CUISkinSelectorWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if (&UIOkBtn == pWnd && CUIButton::BUTTON_CLICKED == msg)
	{
		// Нажали ОК.
		HUD().GetUI()->UIGame()->StartStopMenu(this);
//		HUD().GetUI()->UIGame()->OnSkinMenu_Ok();
	}
	else if (&UICancelBtn == pWnd && CUIButton::BUTTON_CLICKED == msg)
	{
		// Нажали Cancel.
		HUD().GetUI()->UIGame()->StartStopMenu(this);
//		HUD().GetUI()->UIGame()->OnSkinMenu_Cancel();
	}

	inherited::SendMessage(pWnd, msg, pData);
}

////////////////////////////////////////////////////////////////////////////////

u8 CUISkinSelectorWnd::SwitchSkin(const u8 idx)
{
	if (idx >= SKINS_COUNT) return m_uActiveIndex;

	u8 prevIdx = m_uActiveIndex;
	m_vSkinWindows[m_uActiveIndex].UIHighlight.SetColor(clInactive);
	m_vSkinWindows[idx].UIHighlight.SetColor(clActive);
	m_uActiveIndex = static_cast<u8>(idx);
	return prevIdx;
}

////////////////////////////////////////////////////////////////////////////////

u32 CUISkinSelectorWnd::CalculateSkinWindowWidth() const
{
	return static_cast<u32>(Device.dwWidth / SKINS_COUNT);
}

////////////////////////////////////////////////////////////////////////////////

void CUISkinSelectorWnd::OnMouse(int x, int y, E_MOUSEACTION mouse_action)
{
	POINT activePoint;

	activePoint.x = x;
	activePoint.y = y;

	RECT rect;

	// При нажатии на окно со скином, мы делаем его активным
	if (mouse_action == LBUTTON_DOWN)
	{
		for (int i = 0; i < SKINS_COUNT; ++i)
		{
			rect = m_vSkinWindows[i].UIHighlight.GetAbsoluteRect();
			if (PtInRect(&rect, activePoint) && i != m_uActiveIndex)
			{
				SwitchSkin(static_cast<u8>(i));
			}
		}
	}

	inherited::OnMouse(x, y, mouse_action);
}

////////////////////////////////////////////////////////////////////////////////

void CUISkinSelectorWnd::InitializeSkins()
{
	// Бордюр - 30 пикселов
	const int border = 30;

	std::string skin_names[4];
	
	skin_names[0] = "without_outfit";
	skin_names[1] = "exo_outfit";
	skin_names[2] = "stalker_outfit";
	skin_names[3] = "military_outfit";

	for (int i = 0; i < SKINS_COUNT; ++i)
	{
		
		AttachChild(&m_vSkinWindows[i].UIHighlight);
		RECT r	= m_vSkinWindows[i].UIBackground.GetAbsoluteRect();

		// Координаты бекграунда
		r.bottom	-= border;
		r.left		+= border;
		r.top		+= border;
		r.right		-= border;

		int bkWidth				= r.right	- r.left, 
			bkHeight			= r.bottom	- r.top,
			// Координаты текстуры
			texWidth			= 128,
			texHeight			= 341;

		// Коеффициент сжатия
		float stretchKoeff = 1.0f;

		// Проверяем какое из отношений меньше
		float dW = static_cast<float>(bkWidth)	/ texWidth;
		float dH = static_cast<float>(bkHeight)	/ texHeight;

		(dW < dH) ? stretchKoeff = dW : stretchKoeff = dH;

		texWidth	= static_cast<int>(texWidth		* stretchKoeff);
		texHeight	= static_cast<int>(texHeight	* stretchKoeff);

		if (dW < dH)
		{
			float f		 = (bkHeight - texHeight) / 2.0f;
			r.top		+= static_cast<long>(f);
			r.bottom	-= static_cast<long>(f);
		}
		else
		{
			float f		 = (bkWidth - texWidth) / 2.0f;
			r.left		+= static_cast<long>(f);
			r.right		-= static_cast<long>(f);
		}

		m_vSkinWindows[i].UIHighlight.Init(r.left,
										   r.top,
										   r.right - r.left,
										   r.bottom - r.top);
		m_vSkinWindows[i].UIHighlight.SetShader(GetCharIconsShader());
		m_vSkinWindows[i].UIHighlight.GetUIStaticItem().SetColor(clInactive);

		int m_iSkinX = pSettings->r_u32(skin_names[i].c_str(), "full_scale_icon_x");
		int m_iSkinY = pSettings->r_u32(skin_names[i].c_str(), "full_scale_icon_y");

		// Размеры иконки персонажа : 128х341
		m_vSkinWindows[i].UIHighlight.GetUIStaticItem().SetOriginalRect(
			m_iSkinX*ICON_GRID_WIDTH,
			m_iSkinY*ICON_GRID_HEIGHT,
			m_iSkinY+CHAR_ICON_FULL_WIDTH*ICON_GRID_HEIGHT,
			m_iSkinX+static_cast<int>(5.328f*ICON_GRID_WIDTH));
		m_vSkinWindows[i].UIHighlight.ClipperOn();
		m_vSkinWindows[i].UIHighlight.SetTextureScale(stretchKoeff);
	}

	m_vSkinWindows[0].UIHighlight.GetUIStaticItem().SetColor(clActive);

}

////////////////////////////////////////////////////////////////////////////////

bool CUISkinSelectorWnd::OnKeyboard(int dik, E_KEYBOARDACTION keyboard_action)
{
	if (dik > DIK_ESCAPE && dik < SKINS_COUNT + 2 && KEY_PRESSED == keyboard_action)
	{
		SwitchSkin(static_cast<u8>(dik - 2));
		return true;
	}
	if (DIK_RETURN == dik && KEY_PRESSED == keyboard_action)
	{
		SendMessage(&UIOkBtn, CUIButton::BUTTON_CLICKED, NULL);
		return true;
	}
	if (DIK_ESCAPE == dik && KEY_PRESSED == keyboard_action)
	{
		SendMessage(&UICancelBtn, CUIButton::BUTTON_CLICKED, NULL);
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////

void CUISkinSelectorWnd::DrawKBAccelerators()
{
	for (u8 i = 0; i < SKINS_COUNT; ++i)
	{
		RECT rect = m_vSkinWindows[i].UIBackground.GetAbsoluteRect();

		HUD().pFontHeaderRussian->Out((rect.right - rect.left) / 2.0f + rect.left, 
			float(rect.bottom - HUD().pFontHeaderRussian->CurrentHeight()- 40),
			"%d", i + 1);
	}
}

////////////////////////////////////////////////////////////////////////////////

void CUISkinSelectorWnd::Draw()
{
	inherited::Draw();
	DrawKBAccelerators();
}
