//-----------------------------------------------------------------------------/
//  Окно выбора скина в сетевой игре
//-----------------------------------------------------------------------------/

#include "StdAfx.h"
#include "UISkinSelector.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include"../hudmanager.h"
#include"../level.h"

#include "../game_cl_deathmatch.h"

// Constants
const char * const	SKIN_SELECTOR_XML		= "skin_selector.xml";
const u32			clActive				= 0xffffffff;
const u32			clInactive				= 0xaa888888;
const u16			SKIN_TEX_HEIGHT			= 341;
const u16			SKIN_TEX_WIDTH			= 128;

//-----------------------------------------------------------------------------/
// Constructors and Destructor
//-----------------------------------------------------------------------------/

CUISkinSelectorWnd::CUISkinSelectorWnd(const char* strSectionName)
	: m_uActiveIndex(0)
{
	Init(strSectionName);

	Hide();
}

////////////////////////////////////////////////////////////////////////////////

CUISkinSelectorWnd::CUISkinSelectorWnd()
{
	// For tests
	Init("deathmatch_team0");
}

////////////////////////////////////////////////////////////////////////////////

CUISkinSelectorWnd::~CUISkinSelectorWnd()
{
}

////////////////////////////////////////////////////////////////////////////////

void CUISkinSelectorWnd::Init(const char *strSectionName)
{
	R_ASSERT(xr_strcmp(strSectionName, ""));
	std::strcpy(m_strSection, strSectionName);

	CUIXml xml_doc;
	bool xml_result = xml_doc.Init("$game_data$", SKIN_SELECTOR_XML);
	R_ASSERT2(xml_result, "xml file not found");

	CUIXmlInit xml_init;

	CUIWindow::Init(CUIXmlInit::ApplyAlignX(0, alCenter),
					CUIXmlInit::ApplyAlignY(0, alCenter),
					UI_BASE_WIDTH, UI_BASE_HEIGHT);

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

		yC	 = UI_BASE_HEIGHT / 2 - height / 2;

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

	UIOkBtn.MoveWindow(UI_BASE_WIDTH / 2 - static_cast<int>((rect.right - rect.left) * 1.5),
		r.bottom + r.top / 2 - UIOkBtn.GetHeight());

	AttachChild(&UICancelBtn);
	xml_init.InitButton(xml_doc, "button", 1, &UICancelBtn);
	rect	= UICancelBtn.GetWndRect();
	UICancelBtn.MoveWindow(UI_BASE_WIDTH / 2 + static_cast<int>((rect.right - rect.left) * 0.5),
		r.bottom + r.top / 2 - UICancelBtn.GetHeight());

	SetFont(HUD().pFontHeaderRussian);
}

////////////////////////////////////////////////////////////////////////////////

void CUISkinSelectorWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if (&UIOkBtn == pWnd && BUTTON_CLICKED == msg)
	{
		// Нажали ОК.
		Game().StartStopMenu(this);
//		HUD().GetUI()->UIGame()->OnSkinMenu_Ok();
		game_cl_Deathmatch * dm = dynamic_cast<game_cl_Deathmatch *>(&(Game()));
		dm->OnSkinMenu_Ok();

	}
	else if (&UICancelBtn == pWnd && BUTTON_CLICKED == msg)
	{
		// Нажали Cancel.
		Game().StartStopMenu(this);
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
	return static_cast<u32>(UI_BASE_WIDTH / SKINS_COUNT);
}

////////////////////////////////////////////////////////////////////////////////

void CUISkinSelectorWnd::OnMouse(int x, int y, EUIMessages mouse_action)
{
	POINT activePoint;

	activePoint.x = x;
	activePoint.y = y;

	RECT rect;

	// При нажатии на окно со скином, мы делаем его активным
	if (mouse_action == WINDOW_LBUTTON_DOWN)
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

	xr_vector<ref_str>		skin_names;
	ref_str					cfgRecord;
	string256				singleSkinName;
	
	// Читаем данные этого поля
	cfgRecord = pSettings->r_string(m_strSection, "skins");
	u32 count = _GetItemCount(*cfgRecord);
	// теперь для каждое имя скина, разделенные запятыми, заносим в массив
	for (u32 i = 0; i < count; ++i)
	{
		_GetItem(*cfgRecord, i, singleSkinName);
		skin_names.push_back(singleSkinName);
	}

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
			texWidth			= SKIN_TEX_WIDTH,
			texHeight			= SKIN_TEX_HEIGHT;

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

		m_vSkinWindows[i].UIHighlight.SetShader(GetMPCharIconsShader());
		m_vSkinWindows[i].UIHighlight.GetUIStaticItem().SetColor(clInactive);

		int m_iSkinX = 0, m_iSkinY = 0;
		sscanf(pSettings->r_string("multiplayer_skins", *skin_names[i]), "%i,%i", &m_iSkinX, &m_iSkinY);
	
		// Размеры иконки персонажа : 128х341
		m_vSkinWindows[i].UIHighlight.GetUIStaticItem().SetOriginalRect(
			m_iSkinX,
			m_iSkinY,
			CHAR_ICON_FULL_WIDTH*ICON_GRID_HEIGHT,
			static_cast<int>(5.07f*ICON_GRID_WIDTH));

		m_vSkinWindows[i].UIHighlight.ClipperOn();
		m_vSkinWindows[i].UIHighlight.SetTextureScale(stretchKoeff);
	}

	m_vSkinWindows[0].UIHighlight.GetUIStaticItem().SetColor(clActive);

}

////////////////////////////////////////////////////////////////////////////////

bool CUISkinSelectorWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	if (dik > DIK_ESCAPE && dik < SKINS_COUNT + 2 && WINDOW_KEY_PRESSED == keyboard_action)
	{
		SwitchSkin(static_cast<u8>(dik - 2));
		return true;
	}
	if (DIK_RETURN == dik && WINDOW_KEY_PRESSED == keyboard_action)
	{
		SendMessage(&UIOkBtn, BUTTON_CLICKED, NULL);
		return true;
	}
	if (DIK_ESCAPE == dik && WINDOW_KEY_PRESSED == keyboard_action)
	{
		SendMessage(&UICancelBtn, BUTTON_CLICKED, NULL);
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

		Irect r;
		r.set(0, 0, UI_BASE_WIDTH, UI_BASE_WIDTH);

		HUD().OutText(GetFont(), r,
			(rect.right - rect.left) / 2.0f + rect.left, 
			float(rect.bottom - HUD().pFontHeaderRussian->CurrentHeight()- 40),
			"%d", i + 1);
		GetFont()->OnRender();
	}
}

////////////////////////////////////////////////////////////////////////////////

void CUISkinSelectorWnd::Draw()
{
	inherited::Draw();
	DrawKBAccelerators();
}
