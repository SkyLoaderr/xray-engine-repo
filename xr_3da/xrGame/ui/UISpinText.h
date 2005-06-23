// file:		UISpinNum.h
// description:	Spin Button with text data (unlike numerical data)
// created:		15.06.2005
// author:		Serge Vynnychenko
//

#include "UICustomSpin.h"

class CUISpinText : public CUICustomSpin{
public:
	CUISpinText();
	~CUISpinText();
	// CUIOptionsItem
	virtual void	SetCurrentValue();
	virtual void	SaveValue();

	// own
	virtual void	OnBtnUpClick();
	virtual void	OnBtnDownClick();


    void	AddItem(const char* item);

protected:
	void	SetItem();
	typedef xr_vector<xr_string>			Items;
	typedef xr_vector<xr_string>::iterator	Items_it;

    Items	m_list;
	u32		m_curItem;
};