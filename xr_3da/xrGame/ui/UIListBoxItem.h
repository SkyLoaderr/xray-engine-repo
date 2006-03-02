#pragma once
#include "UILabel.h"

class CUIListBoxItem : public CUILabel, public CUISelectable{
public:
	using CUILabel::SetTextColor;

	CUIListBoxItem();

    virtual void	SetSelected(bool b);
	virtual void	Update();
	virtual void	OnMouseDown(bool left_button = true);
			void	SetTextColor(u32 color, u32 color_s);

			void	InitDefault();
protected:
	u32 txt_color;
	u32 txt_color_s;
};

