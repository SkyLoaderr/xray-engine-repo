#pragma once

#include "ai/script/ai_script_monster.h"

class CLAItem;

class CSearchlight: public CScriptMonster {
	typedef	CScriptMonster		inherited;

	float			fBrightness;
	CLAItem*		lanim;
	Fvector			m_pos;
	IRender_Light*	light_render;
	IRender_Glow*	glow_render;
	Fvector			m_focus;

public:
					CSearchlight	();
	virtual			~CSearchlight	();

	virtual void	Load			( LPCSTR section);
	virtual BOOL	net_Spawn		( LPVOID DC);
	virtual void	shedule_Update	( u32 dt);							// Called by sheduler
	virtual void	UpdateCL		( );								// Called each frame, so no need for dt
	virtual void	renderable_Render( );

	virtual BOOL	UsedAI_Locations();
};


