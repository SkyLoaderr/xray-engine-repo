#pragma once

#include "UIStatic.h"

class CUIColorAnimatorWrapper;

class CUIStatix : public CUIStatic {
public:
	CUIStatix();
	~CUIStatix();

	virtual void Update();
	virtual void OnFocusReceive();
	virtual void OnFocusLost();
	virtual void OnMouseDown(bool left_button = true);
			void SetSelectedState(bool state);
			bool GetSelectedState();

private:
	bool		m_bSelected;
	CUIColorAnimatorWrapper*	m_anim;
};
