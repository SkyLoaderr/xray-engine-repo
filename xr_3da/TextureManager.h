// TextureManager.h: interface for the CTextureManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "shader.h"
#include "tss_def.h"

class ENGINE_API CShaderManager
{
private:
	struct str_pred : public binary_function<char*, char*, bool> 
	{	
		IC bool operator()(LPCSTR x, LPCSTR y) const
		{	return strcmp(x,y)<0;	}
	};
	
	// data
	map<LPSTR,CBlender*,str_pred>	blenders;
	map<LPSTR,CTexture*,str_pred>	textures;
	map<LPSTR,CMatrix*,str_pred>	matrices;
	map<LPSTR,CConstant*,str_pred>	constants;

	// shader code array
	struct sh_Code {
		DWORD				SB;
		DWORD				Reference;
		SimulatorStates		Code;
	};
	vector<sh_Code>					codes;

	// lists
	vector<STextureList*>			lst_textures;
	vector<SMatrixList*>			lst_matrices;
	vector<SConstantList*>			lst_constants;

	// cache
	struct
	{
		CPass						pass;
		CTexture*					surfaces	[8];
		CMatrix*					matrices	[8];

		void						Invalidate	()
		{	ZeroMemory(this,sizeof(*this));		}
	}								cache;

	// misc
	BOOL							bDeferredLoad;
public:
	// Miscelaneous
	void							_ParseList			(sh_list& dest, LPCSTR names);
	CBlender*						_GetBlender			(LPCSTR Name);
	DWORD							_GetMemoryUsage		();

	// Low level resource creation
	CTexture*						_CreateTexture		(LPCSTR Name);
	void							_DeleteTexture		(CTexture* &T);
	CMatrix*						_CreateMatrix		(LPCSTR Name);
	void							_DeleteMatrix		(CMatrix*  &M);
	CConstant*						_CreateConstant		(LPCSTR Name);
	void							_DeleteConstant		(CConstant* &C);

	// Shader compiling / optimizing
	DWORD							_CreateCode			(SimulatorStates& Code);
	void							_DeleteCode			(DWORD& SB);
	STextureList*					_CreateTextureList	(STextureList& L);
	void							_DeleteTextureList	(STextureList* &L);
	SMatrixList*					_CreateMatrixList	(SMatrixList& L);
	void							_DeleteMatrixList	(SMatrixList* &L);
	SConstantList*					_CreateConstantList	(SConstantList& L);
	void							_DeleteConstantList	(SConstantList* &L);

	CShaderManager			()
	{
		bDeferredLoad		= FALSE;
		cache.Invalidate	();
	}

	void	xrStartUp		();
	void	xrShutDown		();

	void	OnDeviceDestroy	(void);
	void	OnDeviceCreate	(void);
	IC void	OnFrameEnd		()
	{
		cache.Invalidate	();
	}

	// Creation/Destroying
	Shader	Create			(LPCSTR s_shader="null", LPCSTR s_textures = "$null", LPCSTR s_constants = "", LPCSTR s_matrices = "");
	void	Delete			(Shader	&S);
	void	DeferredLoad	(BOOL E)	{ bDeferredLoad=E;	}

	void	TexturesLoad	();
	void	TexturesUnload	();

	// API
	IC void	set_Code		(DWORD dwCode)
	{
		if (cache.pass.dwStateBlock!=dwCode)
		{
			cache.pass.dwStateBlock=dwCode;
			CHK_DX(HW.pDevice->ApplyStateBlock(dwCode));
		}
	}
	IC void set_Textures	(STextureList* T)
	{
		if (cache.pass.T != T)
		{
			cache.pass.T	= T;
			for (DWORD it=0; it<T->size(); it++)
			{
				CTexture*	surf = (*T)[it];
				if (cache.surfaces[it]!=surf)	{
					cache.surfaces[it]=surf;
					surf->Apply	(it);
				}
			}
		}
	}
	IC void set_Matrices	(SMatrixList* M)
	{
		if (cache.pass.M != M)
		{
			cache.pass.M = M;
			if (M)	{
				for (DWORD it=0; it<M->size(); it++)
				{
					CMatrix*	mat = (*M)[it];
					if (mat && cache.matrices[it]!=mat)	{
						cache.matrices[it]=mat;
						CHK_DX(HW.pDevice->SetTransform(D3DTRANSFORMSTATETYPE(D3DTS_TEXTURE0+it),mat->xform.d3d()));
					}
				}
			}
		}
	}
	IC void set_Constants	(SConstantList* C, BOOL bPS)
	{
		if (cache.pass.C != C)
		{
			cache.pass.C = C;
			if (C)	{
				if (bPS)
				{
					svector<Fcolor,8>	data;
					for (DWORD it=0; it<C.size(); it++)	data.push_back((*C)[it]->const_float);
					CHK_DX(HW.pDevice->SetPixelShaderConstant(0,data.begin(),data.size()));
				} else {
					CHK_DX(HW.pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,(*C)[0]->const_dword))
				}
			}
		}
	}
	IC void set_Shader		(Shader& S, DWORD pass=0)
	{
		CPass&	P = S.Passes[pass];
		set_Code		(P.dwStateBlock);
		set_Textures	(P.T);
		set_Matrices	(P.M);
		set_Constants	(P.C,S.Flags.bPixelShader);
	}
};

#endif // !defined(AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
