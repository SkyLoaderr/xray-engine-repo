#pragma once
#include "gameObject.h"

class CTrigger :public CGameObject
{
	typedef CGameObject inherited ;
protected:
	u32		m_state;
public :

	CTrigger();

			u32				GetState			( ) const;
			void			SetState			( u32 state );
	virtual void			OnStateChanged		( u32 prev_state, u32 new_state );
			void			PlayAnim			( LPCSTR anim_name );
	// Methods
	virtual void			Load				( LPCSTR section );

	virtual BOOL			net_Spawn			( LPVOID DC );
	virtual void			net_Destroy			();
	virtual void			net_Export			( NET_Packet& P );
	virtual void			net_Import			( NET_Packet& P );

};