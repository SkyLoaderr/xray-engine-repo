#include "stdafx.h"
#pragma hdrstop

#include	"ResourceManager.h"
#include	"tss.h"
#include	"blenders\blender.h"
#include	"blenders\blender_recorder.h"
#include	"ai_script_space.h"

using namespace			luabind;

// wrapper
class	adopt_compiler
{
	CBlender_Compile*		C;
public:
	adopt_compiler			(CBlender_Compile*	_C)	: C(_C)		{ }
	adopt_compiler			(adopt_compiler&	_C)	: C(_C.C)	{ }

	void	r_Pass			(LPCSTR vs,		LPCSTR ps,		BOOL bFog,	BOOL	bZtest=TRUE,	BOOL	bZwrite=TRUE,			BOOL	bABlend=FALSE,			u32		abSRC=D3DBLEND_ONE,		u32 abDST=D3DBLEND_ZERO,	BOOL aTest=FALSE,	u32 aRef=0);
	void	r_Sampler		(LPCSTR name,	LPCSTR texture, bool b_ps1x_ProjectiveDivide=false, u32	address=D3DTADDRESS_WRAP,	u32		fmin=D3DTEXF_LINEAR,	u32		fmip=D3DTEXF_LINEAR,	u32 fmag=D3DTEXF_LINEAR,	u32 element=0);
	void	r_Sampler_rtf	(LPCSTR name,	LPCSTR texture,	bool b_ps1x_ProjectiveDivide=false, u32	element=0);
	void	r_Sampler_clf	(LPCSTR name,	LPCSTR texture,	bool b_ps1x_ProjectiveDivide=false, u32	element=0);
	void	r_End			();
};
class	adopt_pass
{
	CBlender_Compile*		C;
};

// export
void	export	(lua_State* LS)
{
	module(LS)
	[
		class_<adopt_compiler>("shader")
			.def(								constructor<const CMovementAction::EInputKeys>())
			.def(								constructor<const CMovementAction::EInputKeys, float>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,MonsterSpace::EPathType,CLuaGameObject*>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,MonsterSpace::EPathType,CLuaGameObject*,float>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,MonsterSpace::EPathType,const CPatrolPathParams &>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,MonsterSpace::EPathType,const CPatrolPathParams &,float>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,MonsterSpace::EPathType,const Fvector &>())
			.def(								constructor<MonsterSpace::EBodyState,MonsterSpace::EMovementType,MonsterSpace::EPathType,const Fvector &,float>())
			.def(								constructor<MonsterSpace::EActState,const Fvector &>())
			.def(								constructor<MonsterSpace::EActState,const CPatrolPathParams &>())
			.def(								constructor<MonsterSpace::EActState,CLuaGameObject*>())
			.def(								constructor<const Fvector &,float>())
			.def("body",						&CMovementAction::SetBodyState)
			.def("move",						&CMovementAction::SetMovementType)
			.def("path",						&CMovementAction::SetPathType)
			.def("object",						&CMovementAction::SetObjectToGo)
			.def("patrol",						&CMovementAction::SetPatrolPath)
			.def("position",					&CMovementAction::SetPosition)
			.def("input",						&CMovementAction::SetInputKeys)
			.def("act",							&CMovementAction::SetAct),
	];
}