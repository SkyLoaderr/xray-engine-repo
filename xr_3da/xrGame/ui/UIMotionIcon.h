#pragma once

class CUIMotionIcon : public CUIWindow
{
	typedef CUIWindow inherited													  ;
	static const	int					states_number							=4;
public:
	enum	EState		{
						stNormal,
						stCrouch,
						stCreep,
						stLast
					};
private:
					EState				m_curren_state;
//					CUIStatic			m_states[states_number]							;
					CUIProgressBar		m_PowerBar[states_number];
public:
	virtual			~CUIMotionIcon				()										;
					CUIMotionIcon				()										;
			void			Init				(float x, float y, float width, float height)	;
			void			ShowState			(EState state)							;
			void			SetProgressPos		(s16 Pos);
};
