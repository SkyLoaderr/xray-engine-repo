#include "stdafx.h"
#pragma hdrstop

#include	"ResourceManager.h"
#include	"tss.h"
#include	"blenders\blender.h"
#include	"blenders\blender_recorder.h"
#include	"ai_script_space.h"

using namespace				luabind;

// wrapper
class	adopt_sampler
{
	CBlender_Compile*		C;
	u32						stage;
public:
	adopt_sampler			(CBlender_Compile*	_C, u32 _stage)		: C(_C), stage(_stage)		{ }
	adopt_sampler			(adopt_sampler&	_C)						: C(_C.C), stage(_C.Stage)	{ }

	adopt_sampler&			_texture		(LPCSTR texture)						{ C->i_Texture	(stage,texture);			return *this;	}
	adopt_sampler&			_projective		(bool _b)								{ C->i_Projective(stage,_b);				return *this;	}
	adopt_sampler&			_clamp			()										{ C->i_Address	(stage,D3DTADDRESS_CLAMP);	return *this;	}
	adopt_sampler&			_wrap			()										{ C->i_Address	(stage,D3DTADDRESS_WRAP);	return *this;	}
	adopt_sampler&			_mirror			()										{ C->i_Address	(stage,D3DTADDRESS_MIRROR);	return *this;	}
	adopt_sampler&			_f_anisotropic	()										{ C->i_Filter	(stage,D3DTEXF_LINEAR,D3DTEXF_LINEAR,D3DTEXF_ANISOTROPIC);	return *this;	}
	adopt_sampler&			_f_trilinear	()										{ C->i_Filter	(stage,D3DTEXF_LINEAR,D3DTEXF_LINEAR,D3DTEXF_LINEAR);		return *this;	}
	adopt_sampler&			_f_bilinear		()										{ C->i_Filter	(stage,D3DTEXF_LINEAR,D3DTEXF_POINT, D3DTEXF_LINEAR);		return *this;	}
	adopt_sampler&			_f_none			()										{ C->i_Filter	(stage,D3DTEXF_POINT, D3DTEXF_NONE,  D3DTEXF_POINT);		return *this;	}
	adopt_sampler&			_fmin_none		()										{ C->i_Filter_Min(stage,D3DTEXF_NONE);		return *this;	}
	adopt_sampler&			_fmin_point		()										{ C->i_Filter_Min(stage,D3DTEXF_POINT);		return *this;	}
	adopt_sampler&			_fmin_linear	()										{ C->i_Filter_Min(stage,D3DTEXF_LINEAR);	return *this;	}
	adopt_sampler&			_fmin_aniso		()										{ C->i_Filter_Min(stage,D3DTEXF_ANISOTROPIC);	return *this;	}
	adopt_sampler&			_fmip_none		()										{ C->i_Filter_Mip(stage,D3DTEXF_NONE);		return *this;	}
	adopt_sampler&			_fmip_point		()										{ C->i_Filter_Mip(stage,D3DTEXF_POINT);		return *this;	}
	adopt_sampler&			_fmip_linear	()										{ C->i_Filter_Mip(stage,D3DTEXF_LINEAR);	return *this;	}
	adopt_sampler&			_fmag_none		()										{ C->i_Filter_Mag(stage,D3DTEXF_NONE);		return *this;	}
	adopt_sampler&			_fmag_point		()										{ C->i_Filter_Mag(stage,D3DTEXF_POINT);		return *this;	}
	adopt_sampler&			_fmag_linear	()										{ C->i_Filter_Mag(stage,D3DTEXF_LINEAR);	return *this;	}
};

// wrapper
class	adopt_compiler
{
	CBlender_Compile*		C;
public:
	adopt_compiler			(CBlender_Compile*	_C)	: C(_C)		{ }
	adopt_compiler			(adopt_compiler&	_C)	: C(_C.C)	{ }

	adopt_compiler&			_pass			(LPCSTR	vs,		LPCSTR ps)				{	C->r_Pass			(vs,ps,true);	return	*this;			}
	adopt_compiler&			_fog			(BOOL	_fog)							{	C->PassSET_LightFog	(FALSE,_fog);	return	*this;			}
	adopt_compiler&			_ZB				(BOOL	_test,	BOOL _write)			{	C->PassSET_ZB		(_test,_write);	return	*this;			}
	adopt_compiler&			_blend			(BOOL	_blend, u32 abSRC, u32 abDST)	{	C->PassSET_ablend_mode(_blend,abSRC,abDST);	return 	*this;	}
	adopt_compiler&			_aref			(BOOL	_aref,  u32 aref)				{	C->PassSET_ablend_aref(_aref,aref);	return 	*this;			}
	adopt_sampler&			_sampler		(LPCSTR _name)							{	u32 s = C->r_Sampler(_name,0); return adopt_sampler(C,s);	}
};

// export
void	export	(lua_State*	LS)
{
	open	(LS);
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
}
