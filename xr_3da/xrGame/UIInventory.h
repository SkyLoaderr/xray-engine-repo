#pragma once

class CUIInventory
{
public:
	CUIInventory(void);
	~CUIInventory(void);

	void Show() { m_active = true; }
	void Hide() { m_active = false; }
	bool Visible() { return m_active; }

	virtual void OnFrame();
	virtual bool OnKeyboardPress(int dik);
	virtual bool OnKeyboardRelease(int dik);

	bool m_active, m_shift, m_deleteNext;
	f32 m_rowOffs, m_rowSkip, m_colOffs[3];
};
