#pragma once

#include "ai/script/ai_script_monster.h"
#include "ai/ai_monster_bones.h"

class CLAItem;

class CSearchlight: public CScriptMonster {
	typedef	CScriptMonster		inherited;

	float			fBrightness;
	CLAItem*		lanim;
	Fvector			m_pos;
	IRender_Light*	light_render;
	IRender_Glow*	glow_render;
	Fvector			m_focus;

	u16				rot_bone;
	u16				guid_bone;
	u16				cone_bone;

	struct SRot {
		float yaw;
		float pitch;
	} _start, _cur, _target;

public:
					CSearchlight	();
	virtual			~CSearchlight	();

	virtual void	Load			( LPCSTR section);
	virtual BOOL	net_Spawn		( LPVOID DC);
	virtual void	shedule_Update	( u32 dt);							// Called by sheduler
	virtual void	UpdateCL		( );								// Called each frame, so no need for dt
	virtual void	renderable_Render( );

	virtual BOOL	UsedAI_Locations();

	virtual	bool	bfAssignWatch(CEntityAction	*tpEntityAction);
	virtual	bool	bfAssignObject(CEntityAction *tpEntityAction);
	
private:
			void	TurnOn			();
			void	TurnOff			();
	
	// Rotation routines
	static void __stdcall  BoneCallback(CBoneInstance *B);
	void			UpdateBones		();
	void			SetTarget		(const Fvector &target_pos);
	
	float			bone_vel_x;
	float			bone_vel_y;
};


