#pragma once

class CUIMotionIcon : public CUIStatic
{
	typedef CUIWindow inherited;
public:
	enum	EState		{
						stNormal,
						stCrouch,
						stCreep,
						stClimb,
						stRun,
						stSprint,
						stLast
					};
private:
							EState				m_curren_state;
							CUIStatic			m_states[stLast];
							CUIProgressBar		m_power_progress;
							CUIProgressBar		m_luminosity_progress;
							CUIProgressBar		m_noise_progress;

		struct _npc_visibility{
			u16				id;
			s16				value;
			bool operator == (const u16& _id){
				return id == _id;
			}
			bool operator < (const _npc_visibility& m)
			{
				return (value < m.value);
			}
		};
		xr_vector<_npc_visibility>				m_npc_visibility;
		bool									m_bchanged;
public:
	virtual					~CUIMotionIcon		();
							CUIMotionIcon		();
	virtual	void			Update				();
			void			Init				();
			void			ShowState			(EState state);
			void			SetPower			(s16 Pos);
			void			SetNoise			(s16 Pos);
			void			SetLuminosity		(s16 Pos);
			void			SetActorVisibility	(u16 who_id, s16 value);
};
