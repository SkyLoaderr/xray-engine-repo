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
						stClimb,
						stLast
					};
private:
					EState				m_curren_state;
					CUIStatic			m_states[states_number]							;
					CUIProgressBar		m_PowerBar[states_number];
public:
	virtual			~CUIMotionIcon				()										;
					CUIMotionIcon				()										;
			void			Init				(int x, int y, int width, int height)	;
			void			ShowState			(EState state)							;
			void			SetProgressPos		(s16 Pos);
};
