#include "stdafx.h"
#pragma hdrstop

#include	"ResourceManager.h"
#include	"tss.h"
#include	"blenders\blender.h"
#include	"blenders\blender_recorder.h"
#include	"ai_script_space.h"
#include	"ai_script_lua_extension.h"

using namespace				luabind;

// wrapper
class	adopt_sampler
{
	CBlender_Compile*		C;
	u32						stage;
public:
	adopt_sampler			(CBlender_Compile*	_C, u32 _stage)		: C(_C), stage(_stage)		{ }
	adopt_sampler			(const adopt_sampler&	_C)				: C(_C.C), stage(_C.stage)	{ }

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
	adopt_compiler			(CBlender_Compile*	_C)	: C(_C)							{ }
	adopt_compiler			(const adopt_compiler&	_C)	: C(_C.C)					{ }

	adopt_compiler&			_pass			(LPCSTR	vs,		LPCSTR ps)				{	C->r_Pass			(vs,ps,true);	return	*this;			}
	adopt_compiler&			_fog			(bool	_fog)							{	C->PassSET_LightFog	(FALSE,_fog);	return	*this;			}
	adopt_compiler&			_ZB				(bool	_test,	bool _write)			{	C->PassSET_ZB		(_test,_write);	return	*this;			}
	adopt_compiler&			_blend			(bool	_blend, u32 abSRC, u32 abDST)	{	C->PassSET_ablend_mode(_blend,abSRC,abDST);	return 	*this;	}
	adopt_compiler&			_aref			(bool	_aref,  u32 aref)				{	C->PassSET_ablend_aref(_aref,aref);	return 	*this;			}
	adopt_sampler			_sampler		(LPCSTR _name)							{	u32 s = C->r_Sampler(_name,0); return adopt_sampler(C,s);	}
};


void LuaLog(LPCSTR caMessage)
{
	Lua::LuaOut	(Lua::eLuaMessageTypeMessage,"%s",caMessage);
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

	luabind::open	(LSVM);

	function		(LSVM, "log",	LuaLog);

	class_<adopt_sampler>("_sampler")
		.def(								constructor<const adopt_sampler&>())
		.def("texture",						&adopt_sampler::_texture		)
		.def("project",						&adopt_sampler::_projective		)
		.def("clamp",						&adopt_sampler::_clamp			)
		.def("wrap",						&adopt_sampler::_wrap			)
		.def("mirror",						&adopt_sampler::_mirror			)
		.def("f_anisotropic",				&adopt_sampler::_f_anisotropic	)
		.def("f_trilinear",					&adopt_sampler::_f_trilinear	)
		.def("f_bilinear",					&adopt_sampler::_f_bilinear		)
		.def("f_none",						&adopt_sampler::_f_none			)
		.def("fmin_none",					&adopt_sampler::_fmin_none		)
		.def("fmin_point",					&adopt_sampler::_fmin_point		)
		.def("fmin_linear",					&adopt_sampler::_fmin_linear	)
		.def("fmin_aniso",					&adopt_sampler::_fmin_aniso		)
		.def("fmip_none",					&adopt_sampler::_fmip_none		)
		.def("fmip_point",					&adopt_sampler::_fmip_point		)
		.def("fmip_linear",					&adopt_sampler::_fmip_linear	)
		.def("fmag_none",					&adopt_sampler::_fmag_none		)
		.def("fmag_point",					&adopt_sampler::_fmag_point		)
		.def("fmag_linear",					&adopt_sampler::_fmag_linear	)
		;

	class_<adopt_compiler>("_compiler")
		.def(								constructor<const adopt_compiler&>())
		.def("pass",						&adopt_compiler::_pass			)
		.def("fog",							&adopt_compiler::_fog			)
		.def("zb",							&adopt_compiler::_ZB			)
		.def("blend",						&adopt_compiler::_blend			)
		.def("aref",						&adopt_compiler::_aref			)
		.def("sampler",						&adopt_compiler::_sampler		)
		;

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

	// Access to template
	C.BT				= NULL;
	C.bEditor			= FALSE;
	C.bDetail			= FALSE;

	// Prepare
	_ParseList			(C.L_textures,	s_textures	);
	string256			fname;

	// Compile element	(LOD0 - HQ)
	if (Script::bfIsObjectPresent(LSVM,s_shader,"normal_hq",LUA_TFUNCTION))
	{
		// Analyze possibility to detail this shader
		C.iElement			= 0;
		C.detail_texture	= NULL;
		C.detail_scaler		= NULL;
		C.bDetail			= Device.Resources->_GetDetailTexture(C.L_textures[0],C.detail_texture,C.detail_scaler);
		if (C.bDetail)		S.E[0]	= C._lua_Compile(strconcat(fname,s_shader,".normal_hq"));
		else				S.E[0]	= C._lua_Compile(strconcat(fname,s_shader,".normal"));
	} else {
		C.iElement			= 0;
		C.bDetail			= FALSE;
		S.E[0]				= C._lua_Compile		(strconcat(fname,s_shader,".normal"));
	}

	// Compile element	(LOD1)
	if (Script::bfIsObjectPresent(LSVM,s_shader,"normal",LUA_TFUNCTION))
	{
		C.iElement			= 1;
		C.bDetail			= FALSE;
		S.E[1]				= C._lua_Compile(strconcat(fname,s_shader,".normal"));
	}

	// Compile element
	if (Script::bfIsObjectPresent(LSVM,s_shader,"l_point",LUA_TFUNCTION))
	{
		C.iElement			= 2;
		C.bDetail			= FALSE;
		S.E[2]				= C._lua_Compile(strconcat(fname,s_shader,".l_point"));;
	}

	// Compile element
	if (Script::bfIsObjectPresent(LSVM,s_shader,"l_spot",LUA_TFUNCTION))
	{
		C.iElement			= 3;
		C.bDetail			= FALSE;
		S.E[3]				= C._lua_Compile(strconcat(fname,s_shader,".l_spot"));;
	}

	// Compile element
	if (Script::bfIsObjectPresent(LSVM,s_shader,"l_special",LUA_TFUNCTION))
	{
		C.iElement			= 4;
		C.bDetail			= FALSE;
		S.E[4]				= C._lua_Compile(strconcat(fname,s_shader,".l_special"));
	}

	// Search equal in shaders array
	for (u32 it=0; it<v_shaders.size(); it++)
		if (S.equal(v_shaders[it]))	return v_shaders[it];

	// Create _new_ entry
	Shader*		N			=	xr_new<Shader>(S);
	N->dwFlags				|=	xr_resource::RF_REGISTERED;
	v_shaders.push_back		(N);
	return N;
}

ShaderElement*		CBlender_Compile::_lua_Compile	(LPCSTR name)
{
	ShaderElement		E;
	SH =				&E;
	RS.Invalidate		();

	// Compile
	adopt_compiler		ac			(this);
	LPCSTR				t_0			= L_textures[0];
	LPCSTR				t_1			= (L_textures.size() > 1) ? L_textures[1] : "null";
	call_function<void>				(Device.Resources->LSVM,name/*,ac,t_0,t_1*/);
	r_End							();

	return				Device.Resources->_CreateElement(E);
}
