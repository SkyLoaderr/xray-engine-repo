// UIPropertiesBox.cpp: 
//
// окшко с собщением для выбора действий, вызывается по нажатию правой 
// клавиши мыши
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIPropertiesBox.h"
#include "../hudmanager.h"
#include "../level.h"
#include "UIListBoxItem.h"
#include "xrXmlParser.h"
#include "UIXmlInit.h"

#define OFFSET_X (5)
#define OFFSET_Y (5)
#define FRAME_BORDER_WIDTH	20
#define FRAME_BORDER_HEIGHT	22

#define ITEM_HEIGHT (GetFont()->CurrentHeight()+2.0f)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIPropertiesBox::CUIPropertiesBox()
{
//	m_iClickedElement = -1;
	SetFont(HUD().Font().pFontArial14);
	m_UIListWnd.SetImmediateSelection(true);
}

CUIPropertiesBox::~CUIPropertiesBox()
{
}


void CUIPropertiesBox::Init(float x, float y, float width, float height)
{
	inherited::Init(x,y, width, height);

	AttachChild(&m_UIListWnd);

	CUIXml	xml_doc;
	xml_doc.Init(CONFIG_PATH, UI_PATH, "inventory_new.xml");

	LPCSTR t = xml_doc.Read("properties_box:texture", 0, "");
	R_ASSERT(t);
	InitTexture(t);

	CUIXmlInit::InitListBox(xml_doc, "properties_box:list", 0, &m_UIListWnd);

//	m_UIListWnd.SetItemHeight(ITEM_HEIGHT);
		
	m_UIListWnd.Init(OFFSET_X,
					 OFFSET_Y, 
					 width - OFFSET_X*2, 
					 height - OFFSET_Y*2);


}



void CUIPropertiesBox::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &m_UIListWnd)
	{
		if(msg == LIST_ITEM_CLICKED)
		{
//			m_iClickedElement = this->m_UIListWnd.GetSelectedItem()->getin;
			GetMessageTarget()->SendMessage(this, PROPERTY_CLICKED);
			
			Hide();
		}
	}

	inherited::SendMessage(pWnd, msg, pData);
}

bool CUIPropertiesBox::AddItem(const char*  str, void* pData, int value)
{
	//return m_UIListWnd.AddItem<CUIListItem>(str, 0, pData, value);
	CUIListBoxItem* itm		= m_UIListWnd.AddItem(str);
	itm->SetID				(u32(value));
	itm->SetData			(pData);

	return true;
}
void CUIPropertiesBox::RemoveItem(int index)
{
	m_UIListWnd.RemoveWindow(m_UIListWnd.GetItemByID(index));
}

void CUIPropertiesBox::RemoveAll()
{
	m_UIListWnd.Clear();
}

void CUIPropertiesBox::Show(float x, float y)
{
	float x_pos, y_pos;


	//выбрать позицию, чтобы окношко полностью влазило в экран
	if(x+GetWidth()<float(Device.dwWidth/UI()->GetScaleX()))
		x_pos = x;
	else
		x_pos = x - (x+GetWidth()-( Device.dwWidth/UI()->GetScaleX() )  );
	
	if(y+GetHeight()<float(Device.dwHeight/UI()->GetScaleY()))
		y_pos = y;
	else
		y_pos = y - (y+GetHeight()-( Device.dwHeight/UI()->GetScaleY() )  );
		
	SetWndPos(x_pos,y_pos);

	inherited::Show(true);
	inherited::Enable(true);

	ResetAll();

	//сразу захватываем ввод мыши
	GetParent()->SetCapture(this, true);
	//запретить дочерним окнам изментять захват мыши 
	//по отношению к нашему родительскому окну
	m_pOrignMouseCapturer = this;
	
//	m_iClickedElement = -1;

	m_UIListWnd.Reset();
}
void CUIPropertiesBox::Hide()
{
	CUIWindow::Show(false);
	CUIWindow::Enable(false);

	m_pMouseCapturer = NULL;
	
	if(GetParent()->GetMouseCapturer() == this)
		GetParent()->SetCapture(this, false);
}

bool CUIPropertiesBox::OnMouse(float x, float y, EUIMessages mouse_action)
{
	bool cursor_on_box;


	if(x>=0 && x<GetWidth() && y>=0 && y<GetHeight())
		cursor_on_box = true;
	else
		cursor_on_box = false;


	if(mouse_action == WINDOW_LBUTTON_DOWN && !cursor_on_box)
	{
		Hide();
		return true;
	}

	return inherited::OnMouse(x, y, mouse_action);
}

void CUIPropertiesBox::AutoUpdateSize()
{
	SetHeight(m_UIListWnd.GetItemHeight()*m_UIListWnd.GetSize()+ m_UIListWnd.GetVertIndent());
	m_UIListWnd.SetHeight(GetHeight());
	float f = float(m_UIListWnd.GetLongestLength()+m_UIListWnd.GetHorizIndent()) + 2; 
	SetWidth(_max(20.0f,f));
//		f = float(m_UIListWnd.GetLongestLength());
	m_UIListWnd.SetWidth(_max(20.0f,f));
	m_UIListWnd.UpdateChildrenLenght();
}

//int CUIPropertiesBox::GetClickedIndex() 
//{
////	return m_iClickedElement;
//}

CUIListBoxItem* CUIPropertiesBox::GetClickedItem()
{
	return m_UIListWnd.GetSelectedItem();
}
void CUIPropertiesBox::Update()
{
	inherited::Update();
}
void CUIPropertiesBox::Draw()
{
	inherited::Draw();
}

bool CUIPropertiesBox::OnKeyboard(int dik, EUIMessages keyboard_action){
	Hide();
	return true;
}