#ifndef TEXTUREMANAGER_RUNTIME_H
#define TEXTUREMANAGER_RUNTIME_H
#pragma once

#include "sh_texture.h"
#include "sh_matrix.h"
#include "sh_constant.h"
#include "sh_rt.h"
#include "sh_vs.h"

// API
IC void	CShaderManager::set_Code		(u32 dwCode)
{
	if (cache.pass.dwStateBlock!=dwCode)
	{
		cache.pass.dwStateBlock=dwCode;
		CHK_DX(HW.pDevice->ApplyStateBlock(dwCode));
		Device.Statistic.dwShader_Codes++;
	}
}

IC void CShaderManager::set_Textures	(STextureList* T)
{
	if (cache.pass.T != T)
	{
		cache.pass.T	= T;
		for (u32 it=0; it<T->size(); it++)
		{
			CTexture*	surf = (*T)[it];
			if (cache.surfaces[it]!=surf)	{
				cache.surfaces[it]=surf;
				surf->Apply	(it);
				Device.Statistic.dwShader_Textures++;
			}
		}
		u32 last				= T->size();
		if (cache.surfaces[last]) {
			cache.surfaces[last]	= 0;
			CHK_DX(HW.pDevice->SetTexture(last,NULL));
		}
	}
}

IC void CShaderManager::set_Matrices	(SMatrixList* M)
{
	if (cache.pass.M != M)
	{
		cache.pass.M = M;
		if (M)	{
			for (u32 it=0; it<M->size(); it++)
			{
				CMatrix*	mat = (*M)[it];
				if (mat && cache.matrices[it]!=mat)	{
					cache.matrices[it]=mat;
					mat->Calculate	();
					Device.set_xform	(D3DTS_TEXTURE0+it,mat->xform);
					Device.Statistic.dwShader_Matrices++;
				}
			}
		}
	}
}
IC void CShaderManager::set_Constants	(SConstantList* C, BOOL bPS)
{
	if (cache.pass.C != C)
	{
		cache.pass.C = C;
		if (C)	{
			if (bPS)
			{
				svector<Fcolor,8>	data;
				for (u32 it=0; it<C->size(); it++)	{
					CConstant* c	= (*C)[it];
					c->Calculate	();
					data.push_back	(c->const_float);
				}
				CHK_DX(HW.pDevice->SetPixelShaderConstant(0,data.begin(),data.size()));
			} else {
				CConstant* c	= (*C)[0];
				c->Calculate	();
				CHK_DX(HW.pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,c->const_dword));
			}
			Device.Statistic.dwShader_Constants++;
		}
	}
}
IC void CShaderManager::set_RT			(IDirect3DSurface8* RT, IDirect3DSurface8* ZB)
{
	if ((RT!=cache.pRT)||(ZB!=cache.pZB))	
	{
		Device.Statistic.RenderDUMP_RT.Begin();
		R_CHK								(HW.pDevice->SetRenderTarget(RT,ZB));
		cache.pRT		= RT;
		cache.pZB		= ZB;
		Device.Statistic.RenderDUMP_RT.End	();
	}
}
#endif