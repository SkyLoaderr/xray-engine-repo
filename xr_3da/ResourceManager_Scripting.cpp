#include "stdafx.h"
#pragma hdrstop

#include	"ResourceManager.h"
#include	"tss.h"
#include	"blenders\blender.h"
#include	"blenders\blender_recorder.h"
#include	"ai_script_space.h"
#include	"ai_script_lua_extension.h"
#include	"luabind/return_reference_to_policy.hpp"

using namespace				luabind;

#ifdef	DEBUG
#define MDB	Memory.dbg_check()
#else
#define MDB
#endif

// wrapper
class	adopt_sampler
{
	CBlender_Compile*		C;
	u32						stage;
public:
	adopt_sampler			(CBlender_Compile*	_C, u32 _stage)		: C(_C), stage(_stage)		{ 
		Msg	("S-create: %x",this);
		if (u32(-1)==stage) C=0;		
		MDB;
	}
	adopt_sampler			(const adopt_sampler&	_C)				: C(_C.C), stage(_C.stage)	{ 
		Msg	("S-copy:   %x (%x)",this, &_C);
		if (u32(-1)==stage) C=0;		
		MDB;	
	}
	~adopt_sampler			()										{ 
		Msg	("S-delete: %x",this);
		MDB;	
	}

	adopt_sampler&			_texture		(LPCSTR texture)		{ MDB;	if (C) C->i_Texture	(stage,texture);											MDB;return *this;	}
	adopt_sampler&			_projective		(bool _b)				{ MDB;	if (C) C->i_Projective(stage,_b);												MDB;return *this;	}
	adopt_sampler&			_clamp			()						{ MDB;	if (C) C->i_Address	(stage,D3DTADDRESS_CLAMP);									MDB;return *this;	}
	adopt_sampler&			_wrap			()						{ MDB;	if (C) C->i_Address	(stage,D3DTADDRESS_WRAP);									MDB;return *this;	}
	adopt_sampler&			_mirror			()						{ MDB;	if (C) C->i_Address	(stage,D3DTADDRESS_MIRROR);									MDB;return *this;	}
	adopt_sampler&			_f_anisotropic	()						{ MDB;	if (C) C->i_Filter	(stage,D3DTEXF_LINEAR,D3DTEXF_LINEAR,D3DTEXF_ANISOTROPIC);	MDB;return *this;	}
	adopt_sampler&			_f_trilinear	()						{ MDB;	if (C) C->i_Filter	(stage,D3DTEXF_LINEAR,D3DTEXF_LINEAR,D3DTEXF_LINEAR);		MDB;return *this;	}
	adopt_sampler&			_f_bilinear		()						{ MDB;	if (C) C->i_Filter	(stage,D3DTEXF_LINEAR,D3DTEXF_POINT, D3DTEXF_LINEAR);		MDB;return *this;	}
	adopt_sampler&			_f_linear		()						{ MDB;	if (C) C->i_Filter	(stage,D3DTEXF_LINEAR,D3DTEXF_NONE,  D3DTEXF_LINEAR);		MDB;return *this;	}
	adopt_sampler&			_f_none			()						{ MDB;	if (C) C->i_Filter	(stage,D3DTEXF_POINT, D3DTEXF_NONE,  D3DTEXF_POINT);		MDB;return *this;	}
	adopt_sampler&			_fmin_none		()						{ MDB;	if (C) C->i_Filter_Min(stage,D3DTEXF_NONE);										MDB;return *this;	}
	adopt_sampler&			_fmin_point		()						{ MDB;	if (C) C->i_Filter_Min(stage,D3DTEXF_POINT);									MDB;return *this;	}
	adopt_sampler&			_fmin_linear	()						{ MDB;	if (C) C->i_Filter_Min(stage,D3DTEXF_LINEAR);									MDB;return *this;	}
	adopt_sampler&			_fmin_aniso		()						{ MDB;	if (C) C->i_Filter_Min(stage,D3DTEXF_ANISOTROPIC);								MDB;return *this;	}
	adopt_sampler&			_fmip_none		()						{ MDB;	if (C) C->i_Filter_Mip(stage,D3DTEXF_NONE);										MDB;return *this;	}
	adopt_sampler&			_fmip_point		()						{ MDB;	if (C) C->i_Filter_Mip(stage,D3DTEXF_POINT);									MDB;return *this;	}
	adopt_sampler&			_fmip_linear	()						{ MDB;	if (C) C->i_Filter_Mip(stage,D3DTEXF_LINEAR);									MDB;return *this;	}
	adopt_sampler&			_fmag_none		()						{ MDB;	if (C) C->i_Filter_Mag(stage,D3DTEXF_NONE);										MDB;return *this;	}
	adopt_sampler&			_fmag_point		()						{ MDB;	if (C) C->i_Filter_Mag(stage,D3DTEXF_POINT);									MDB;return *this;	}
	adopt_sampler&			_fmag_linear	()						{ MDB;	if (C) C->i_Filter_Mag(stage,D3DTEXF_LINEAR);									MDB;return *this;	}
};																																							
																																							
// wrapper																																					
class	adopt_compiler																																		
{
	CBlender_Compile*		C;
public:
	adopt_compiler			(CBlender_Compile*	_C)	: C(_C)							{ 
		Msg	("C-create: %x",this);MDB;	
	}
	adopt_compiler			(const adopt_compiler&	_C)	: C(_C.C)					{ 
		Msg	("C-copy:   %x (%x)",this, &_C);MDB;	
	}
	~adopt_compiler			()														{ 
		Msg	("C-delete: %x",this);MDB;	
	}

	adopt_compiler&			_pass			(LPCSTR	vs,		LPCSTR ps)				{	MDB;	C->r_Pass			(vs,ps,true);			MDB;return	*this;		}
	adopt_compiler&			_fog			(bool	_fog)							{	MDB;	C->PassSET_LightFog	(FALSE,_fog);			MDB;return	*this;		}
	adopt_compiler&			_ZB				(bool	_test,	bool _write)			{	MDB;	C->PassSET_ZB		(_test,_write);			MDB;return	*this;		}
	adopt_compiler&			_blend			(bool	_blend, u32 abSRC, u32 abDST)	{	MDB;	C->PassSET_ablend_mode(_blend,abSRC,abDST);	MDB;return 	*this;		}
	adopt_compiler&			_aref			(bool	_aref,  u32 aref)				{	MDB;	C->PassSET_ablend_aref(_aref,aref);			MDB;return 	*this;		}
	adopt_sampler			_sampler		(LPCSTR _name)							{	MDB;	u32 s = C->r_Sampler(_name,0);				MDB;return	adopt_sampler(C,s);	}
};

class	adopt_blend
{
public:
};

void LuaLog(LPCSTR caMessage)
{
	MDB;	
	Lua::LuaOut	(Lua::eLuaMessageTypeMessage,"%s",caMessage);
}
void LuaError(lua_State* L)
{
	Debug.fatal("LUA error: %s",lua_tostring(L,-1));
}

// export
void	CResourceManager::LS_Load			()
{
	LSVM			= lua_open();
	if (!LSVM)		{
		Msg			("! ERROR : Cannot initialize LUA VM!");
		return;
	}

	// initialize lua standard library functions 
	luaopen_base	(LSVM); 
	luaopen_table	(LSVM);
	luaopen_string	(LSVM);
	luaopen_math	(LSVM);

	luabind::open					(LSVM);
	luabind::set_error_callback		(LuaError);

	function		(LSVM, "log",	LuaLog);

	module			(LSVM)
	[
		class_<adopt_sampler>("_sampler")
			.def(								constructor<const adopt_sampler&>())
			.def("texture",						&adopt_sampler::_texture		,return_reference_to(self))
			.def("project",						&adopt_sampler::_projective		,return_reference_to(self))
			.def("clamp",						&adopt_sampler::_clamp			,return_reference_to(self))
			.def("wrap",						&adopt_sampler::_wrap			,return_reference_to(self))
			.def("mirror",						&adopt_sampler::_mirror			,return_reference_to(self))
			.def("f_anisotropic",				&adopt_sampler::_f_anisotropic	,return_reference_to(self))
			.def("f_trilinear",					&adopt_sampler::_f_trilinear	,return_reference_to(self))
			.def("f_bilinear",					&adopt_sampler::_f_bilinear		,return_reference_to(self))
			.def("f_linear",					&adopt_sampler::_f_linear		,return_reference_to(self))
			.def("f_none",						&adopt_sampler::_f_none			,return_reference_to(self))
			.def("fmin_none",					&adopt_sampler::_fmin_none		,return_reference_to(self))
			.def("fmin_point",					&adopt_sampler::_fmin_point		,return_reference_to(self))
			.def("fmin_linear",					&adopt_sampler::_fmin_linear	,return_reference_to(self))
			.def("fmin_aniso",					&adopt_sampler::_fmin_aniso		,return_reference_to(self))
			.def("fmip_none",					&adopt_sampler::_fmip_none		,return_reference_to(self))
			.def("fmip_point",					&adopt_sampler::_fmip_point		,return_reference_to(self))
			.def("fmip_linear",					&adopt_sampler::_fmip_linear	,return_reference_to(self))
			.def("fmag_none",					&adopt_sampler::_fmag_none		,return_reference_to(self))
			.def("fmag_point",					&adopt_sampler::_fmag_point		,return_reference_to(self))
			.def("fmag_linear",					&adopt_sampler::_fmag_linear	,return_reference_to(self)),

		class_<adopt_compiler>("_compiler")
			.def(								constructor<const adopt_compiler&>())
			.def("begin",						&adopt_compiler::_pass			,return_reference_to(self))
			.def("fog",							&adopt_compiler::_fog			,return_reference_to(self))
			.def("zb",							&adopt_compiler::_ZB			,return_reference_to(self))
			.def("blend",						&adopt_compiler::_blend			,return_reference_to(self))
			.def("aref",						&adopt_compiler::_aref			,return_reference_to(self))
			.def("sampler",						&adopt_compiler::_sampler		),	// returns sampler-object

		class_<adopt_blend>("blend")
			.enum_("blend")
			[
				value("zero",					int(D3DBLEND_ZERO)),
				value("one",					int(D3DBLEND_ONE)),
				value("srccolor",				int(D3DBLEND_SRCCOLOR)),
				value("invsrccolor",			int(D3DBLEND_INVSRCCOLOR)),
				value("srcalpha",				int(D3DBLEND_SRCALPHA)),
				value("invsrcalpha",			int(D3DBLEND_INVSRCALPHA)),
				value("destalpha",				int(D3DBLEND_DESTALPHA)),
				value("invdestalpha",			int(D3DBLEND_INVDESTALPHA)),
				value("destcolor",				int(D3DBLEND_DESTCOLOR)),
				value("invdestcolor",			int(D3DBLEND_INVDESTCOLOR)),
				value("srcalphasat",			int(D3DBLEND_SRCALPHASAT))
			]
	];

	// load shaders
	xr_vector<char*>*	folder	= FS.file_list_open	("$game_shaders$","",FS_ListFiles|FS_RootOnly);
	for (u32 it=0; it<folder->size(); it++)
	{
		string256						namesp,fn;
		strcpy							(namesp,(*folder)[it]);
		if		(strext(namesp))		*strext	(namesp)=0;
		if		(0==namesp[0])			strcpy	(namesp,"_G");
		FS.update_path					(fn,"$game_shaders$",(*folder)[it]);
		Script::bfLoadFileIntoNamespace	(LSVM,fn,namesp,true);
	}
	FS.file_list_close			(folder);
}

void	CResourceManager::LS_Unload			()
{
	lua_close	(LSVM);
	LSVM		= NULL;
}

BOOL	CResourceManager::_lua_HasShader	(LPCSTR s_shader)
{
#ifdef _EDITOR
	return Script::bfIsObjectPresent(LSVM,s_shader,"editor",LUA_TFUNCTION);
#else
	return Script::bfIsObjectPresent(LSVM,s_shader,"normal",LUA_TFUNCTION);
#endif
}

Shader*	CResourceManager::_lua_Create		(LPCSTR s_shader, LPCSTR s_textures)
{
	CBlender_Compile	C;
	Shader				S;

	MDB;

	// Access to template
	C.BT				= NULL;
	C.bEditor			= FALSE;
	C.bDetail			= FALSE;

	// Prepare
	_ParseList			(C.L_textures,	s_textures	);

	// Compile element	(LOD0 - HQ)
	MDB;
	if (Script::bfIsObjectPresent(LSVM,s_shader,"normal_hq",LUA_TFUNCTION))
	{
		// Analyze possibility to detail this shader
		C.iElement			= 0;
		C.detail_texture	= NULL;
		C.detail_scaler		= NULL;
		C.bDetail			= Device.Resources->_GetDetailTexture(C.L_textures[0],C.detail_texture,C.detail_scaler);
		if (C.bDetail)		S.E[0]	= C._lua_Compile(s_shader,"normal_hq");
		else				S.E[0]	= C._lua_Compile(s_shader,"normal");
	} else {
		C.iElement			= 0;
		C.bDetail			= FALSE;
		S.E[0]				= C._lua_Compile		(s_shader,"normal");
	}

	// Compile element	(LOD1)
	MDB;
	if (Script::bfIsObjectPresent(LSVM,s_shader,"normal",LUA_TFUNCTION))
	{
		C.iElement			= 1;
		C.bDetail			= FALSE;
		S.E[1]				= C._lua_Compile(s_shader,"normal");
	}

	// Compile element
	MDB;
	if (Script::bfIsObjectPresent(LSVM,s_shader,"l_point",LUA_TFUNCTION))
	{
		C.iElement			= 2;
		C.bDetail			= FALSE;
		S.E[2]				= C._lua_Compile(s_shader,"l_point");;
	}

	// Compile element
	MDB;
	if (Script::bfIsObjectPresent(LSVM,s_shader,"l_spot",LUA_TFUNCTION))
	{
		C.iElement			= 3;
		C.bDetail			= FALSE;
		S.E[3]				= C._lua_Compile(s_shader,"l_spot");;
	}

	// Compile element
	MDB;
	if (Script::bfIsObjectPresent(LSVM,s_shader,"l_special",LUA_TFUNCTION))
	{
		C.iElement			= 4;
		C.bDetail			= FALSE;
		S.E[4]				= C._lua_Compile(s_shader,"l_special");
	}

	// Search equal in shaders array
	MDB;
	for (u32 it=0; it<v_shaders.size(); it++)
		if (S.equal(v_shaders[it]))	return v_shaders[it];

	// Create _new_ entry
	MDB;
	Shader*		N			=	xr_new<Shader>(S);
	N->dwFlags				|=	xr_resource::RF_REGISTERED;
	v_shaders.push_back		(N);
	return N;
}

ShaderElement*		CBlender_Compile::_lua_Compile	(LPCSTR namesp, LPCSTR name)
{
	ShaderElement		E;
	SH =				&E;
	RS.Invalidate		();

	// Compile
	Memory.dbg_check	();
	LPCSTR				t_0		= L_textures[0]				? L_textures[0] : "null";
	LPCSTR				t_1		= (L_textures.size() > 1)	? L_textures[1] : "null";
	LPCSTR				t_d		= detail_texture			? detail_texture: "null" ;
	lua_State*			LSVM	= Device.Resources->LSVM;
	object				shader	= get_globals(LSVM)[namesp];
	functor<void>		element	= object_cast<functor<void> >(shader[name]);
	adopt_compiler		ac		= adopt_compiler(this);
	Memory.dbg_check	();
	element						(ac,t_0,t_1,t_d);
	Memory.dbg_check	();
	r_End				();
	lua_setgcthreshold	(LSVM,0);
	Memory.dbg_check	();
	ShaderElement*	_r	= Device.Resources->_CreateElement(E);
	Memory.dbg_check	();
	return			_r;
}
