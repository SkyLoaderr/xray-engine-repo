#include "UIScrollView.h"

class CUIListBoxItem;

class CUIListBox : 
	public CUIScrollView,
	public IUIFontControl{
public:
	CUIListBox();
	void DeselectAll();
	void AddItem(LPCSTR text);

    LPCSTR	GetFirstText();
	LPCSTR	GetNextText();
	LPCSTR	GetSelectedText();
	LPCSTR	GetNextSelectedText();
	void	MoveSelectedUp();
	void	MoveSelectedDown();

virtual	void	SetSelected(CUIWindow*);

virtual bool OnMouse(float x, float y, EUIMessages mouse_action);

	// IUIFontControl
	virtual void			SetTextColor(u32 color);
			void			SetTextColorS(u32 color);
	virtual u32				GetTextColor();
	virtual void			SetFont(CGameFont* pFont);
	virtual CGameFont*		GetFont();
	virtual void			SetTextAlignment(ETextAlignment alignment);
	virtual ETextAlignment	GetTextAlignment();

protected:
	float			m_def_item_height;
	int				m_last_selection;
	u32				m_text_color;
	u32				m_text_color_s;
	ETextAlignment	m_text_al;
	WINDOW_LIST_it	m_cur_wnd_it;
	CUIWindow*		m_last_wnd;
};