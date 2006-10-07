#include "stdafx.h"
#include "uber_deffer.h"
void fix_texture_name(LPSTR fn);

void	uber_deffer	(CBlender_Compile& C, bool hq, LPCSTR _vspec, LPCSTR _pspec, BOOL _aref, LPCSTR _detail_replace, bool DO_NOT_FINISH)
{
	// Uber-parse
	string256		fname,fnameA,fnameB;
	strcpy			(fname,*C.L_textures[0]);	//. andy if (strext(fname)) *strext(fname)=0;
	fix_texture_name(fname);
	ref_texture		_t;		_t.create			(fname);
	bool			bump	= _t.bump_exist		();

	// detect lmap
	bool			lmap	= true;
	if	(C.L_textures.size()<3)	lmap = false;
	else {
		pcstr		tex		= C.L_textures[2].c_str();
		if (tex[0]=='l' && tex[1]=='m' && tex[2]=='a' && tex[3]=='p')	lmap = true	;
		else															lmap = false;
	}


	string256		ps,vs,dt;
	strconcat		(vs,"deffer_", _vspec, lmap?"_lmh":""	);
	strconcat		(ps,"deffer_", _pspec, lmap?"_lmh":""	);
	strcpy			(dt,_detail_replace?_detail_replace:( C.detail_texture?C.detail_texture:"" ) );

	if	(_aref)		{ strcat(ps,"_aref");	}

	if	(!bump)		{
		fnameA[0] = fnameB[0] = 0;
		strcat			(vs,"_flat");
		strcat			(ps,"_flat");
		if (hq && (C.bDetail_Diffuse || C.bDetail_Bump) )	{
			strcat		(vs,"_d");
			strcat		(ps,"_d");
		}
	} else {
		strcpy			(fnameA,_t.bump_get().c_str());
		strconcat		(fnameB,fnameA,"#");
		strcat			(vs,"_bump");
		strcat			(ps,"_bump");
		if (hq && (C.bDetail_Diffuse || C.bDetail_Bump) )	{
			strcat		(vs,"_d"	);
			strcat		(ps,"_d"	);
//			strcat		(dt,"_bump"	);
		}
	}

	// HQ
	if (bump && hq)		{
		strcat			(vs,"-hq");
		strcat			(ps,"-hq");
	}

	// Uber-construct
	C.r_Pass		(vs,ps,	FALSE);
	C.r_Sampler		("s_base",		C.L_textures[0],	false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);
	C.r_Sampler		("s_bumpX",		fnameB,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);	// should be before base bump
	C.r_Sampler		("s_bump",		fnameA,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);
	C.r_Sampler		("s_bumpD",		dt,					false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);
	C.r_Sampler		("s_detail",	dt,					false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_LINEAR);
	if (lmap)C.r_Sampler("s_hemi",	C.L_textures[2],	false,	D3DTADDRESS_CLAMP,	D3DTEXF_LINEAR,		D3DTEXF_NONE,	D3DTEXF_LINEAR);
	if (!DO_NOT_FINISH)		C.r_End	();
}
