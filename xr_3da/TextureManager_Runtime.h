#pragma once

#include "sh_texture.h"
#include "sh_matrix.h"
#include "sh_constant.h"


// API
IC void	CShaderManager::set_Code		(DWORD dwCode)
{
	if (cache.pass.dwStateBlock!=dwCode)
	{
		cache.pass.dwStateBlock=dwCode;
		CHK_DX(HW.pDevice->ApplyStateBlock(dwCode));
	}
}
IC void CShaderManager::set_Textures	(STextureList* T)
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
IC void CShaderManager::set_Matrices	(SMatrixList* M)
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
IC void CShaderManager::set_Constants	(SConstantList* C, BOOL bPS)
{
	if (cache.pass.C != C)
	{
		cache.pass.C = C;
		if (C)	{
			if (bPS)
			{
				svector<Fcolor,8>	data;
				for (DWORD it=0; it<C->size(); it++)	data.push_back((*C)[it]->const_float);
				CHK_DX(HW.pDevice->SetPixelShaderConstant(0,data.begin(),data.size()));
			} else {
				CHK_DX(HW.pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,(*C)[0]->const_dword))
			}
		}
	}
}
