#pragma once

#include "ai/script/ai_script_monster.h"

class CLAItem;

class CProjector: public CScriptMonster {
	typedef	CScriptMonster		inherited;

	friend void __stdcall BoneCallbackX(CBoneInstance *B);
	friend void __stdcall BoneCallbackY(CBoneInstance *B);

	float			fBrightness;
	CLAItem*		lanim;
	Fvector			m_pos;
	IRender_Light*	light_render;
	IRender_Glow*	glow_render;

	u16				guid_bone;

	struct SBoneRot {
		float	velocity;
		u16		id;
	} bone_x, bone_y;
	
	struct {
		float	yaw;
		float	pitch;
	} _start, _current, _target;

public:
					CProjector		();
	virtual			~CProjector		();

	virtual void	Load			( LPCSTR section);
	virtual BOOL	net_Spawn		( LPVOID DC);
	virtual void	shedule_Update	( u32 dt);							// Called by sheduler
	virtual void	UpdateCL		( );								// Called each frame, so no need for dt
	virtual void	renderable_Render( );

	virtual BOOL	UsedAI_Locations();

	virtual	bool	bfAssignWatch(CScriptEntityAction	*tpEntityAction);
	virtual	bool	bfAssignObject(CScriptEntityAction *tpEntityAction);
private:
			void	TurnOn			();
			void	TurnOff			();
	
	// Rotation routines
	static void __stdcall  BoneCallbackX(CBoneInstance *B);
	static void __stdcall  BoneCallbackY(CBoneInstance *B);

	void			SetTarget		(const Fvector &target_pos);
	
};


