#ifndef R_BACKEND_RUNTIMEH
#define R_BACKEND_RUNTIMEH
#pragma once

#include "sh_texture.h"
#include "sh_matrix.h"
#include "sh_constant.h"
#include "sh_rt.h"

IC void		R_xforms::set_c_w			(R_constant* C)		{	c_w		= C;	RCache.set_c(C,m_w);	};
IC void		R_xforms::set_c_v			(R_constant* C)		{	c_v		= C;	RCache.set_c(C,m_v);	};
IC void		R_xforms::set_c_p			(R_constant* C)		{	c_p		= C;	RCache.set_c(C,m_p);	};
IC void		R_xforms::set_c_wv			(R_constant* C)		{	c_wv	= C;	RCache.set_c(C,m_wv);	};
IC void		R_xforms::set_c_vp			(R_constant* C)		{	c_vp	= C;	RCache.set_c(C,m_vp);	};
IC void		R_xforms::set_c_wvp			(R_constant* C)		{	c_wvp	= C;	RCache.set_c(C,m_wvp);	};

IC void		CBackend::set_xform			(u32 ID, const Fmatrix& M)
{
	stat.xforms			++;
	CHK_DX				(HW.pDevice->SetTransform((D3DTRANSFORMSTATETYPE)ID,(D3DMATRIX*)&M));
}
IC	void	CBackend::set_xform_world	(const Fmatrix& M)
{ 
	xforms.set_W(M);	
}
IC	void	CBackend::set_xform_view	(const Fmatrix& M)					
{ 
	xforms.set_V(M);	
}
IC	void	CBackend::set_xform_project	(const Fmatrix& M)
{ 
	xforms.set_P(M);	
}
IC void CBackend::set_RT				(IDirect3DSurface9* RT, u32 ID)
{
	if (RT!=pRT[ID])
	{
		stat.target_rt	++;
		pRT[ID]			= RT;
		CHK_DX			(HW.pDevice->SetRenderTarget(ID,RT));
	}
}

IC void	CBackend::set_ZB				(IDirect3DSurface9* ZB)
{
	if (ZB!=pZB)
	{
		stat.target_zb	++;
		pZB				= ZB;
		CHK_DX			(HW.pDevice->SetDepthStencilSurface(ZB));
	}
}

IC void	CBackend::set_States			(IDirect3DStateBlock9* _state)
{
	if (state!=_state)
	{
		stat.states		++;
		state			= _state;
		state->Apply	();
	}
}

IC void CBackend::set_Textures			(STextureList* _T)
{
	if (T != _T)
	{
		T	= _T;
		for (u32 it=0; it<T->size(); it++)
		{
			CTexture*	surf = (*T)[it];
			if (textures[it]!=surf)	{
				stat.textures	++;
				textures[it]	=surf;
				surf->Apply		(it);
			}
		}
		u32 last				= T->size();
		if (textures[last]) {
			textures[last]	= 0;
			CHK_DX				(HW.pDevice->SetTexture(last,NULL));
		}
	}
}

IC void CBackend::set_Matrices			(SMatrixList*	_M)
{
	if (M != _M)
	{
		M = _M;
		if (M)	{
			for (u32 it=0; it<M->size(); it++)
			{
				CMatrix*	mat = (*M)[it];
				if (mat && matrices[it]!=mat)
				{
					matrices	[it]	= mat;
					mat->Calculate		();
					set_xform			(D3DTS_TEXTURE0+it,mat->xform);
					stat.matrices		++;
				}
			}
		}
	}
}

IC void CBackend::set_Constants			(R_constant_table* C)
{
	// caching
	if (ctable==C)	return;
	ctable			= C;
	if (0==C)		return;

	// process constant-loaders
	R_constant**	it	= C->table.begin();
	R_constant**	end	= C->table.end	();
	for ()
}

IC void CBackend::set_Element			(ShaderElement* S, u32	pass)
{
	SPass&	P		= *(S->Passes[pass]);
	set_States		(P.state);
	set_PS			(P.ps);
	set_VS			(P.vs);
	set_Constants	(P.constants);
	set_Textures	(P.T);
	set_Matrices	(P.M);
}

IC void CBackend::set_Format			(IDirect3DVertexDeclaration9* _decl)
{
	if (decl!=_decl)
	{
		stat.decl		++;
		decl			= _decl;
		CHK_DX			(HW.pDevice->SetVertexDeclaration(decl));
	}
}

IC void CBackend::set_PS				(IDirect3DPixelShader9* _ps)
{
	if (ps!=_ps)
	{
		stat.ps			++;
		ps				= _ps;
		CHK_DX			(HW.pDevice->SetPixelShader(ps));
	}
}

IC void CBackend::set_VS				(IDirect3DVertexShader9* _vs)
{
	if (vs!=_vs)
	{
		stat.vs			++;
		vs				= _vs;
		CHK_DX			(HW.pDevice->SetVertexShader(vs));
	}
}

IC void CBackend::set_Vertices			(IDirect3DVertexBuffer9* _vb, u32 _vb_stride)
{
	if ((vb!=_vb) || (vb_stride!=_vb_stride))
	{
		stat.vb			++;
		vb				= _vb;
		vb_stride		= _vb_stride;
		CHK_DX			(HW.pDevice->SetStreamSource(0,vb,0,vb_stride));
	}
}

IC void CBackend::set_Indices			(IDirect3DIndexBuffer9* _ib)
{
	if (ib!=_ib)
	{
		stat.ib			++;
		ib				= _ib;
		CHK_DX			(HW.pDevice->SetIndices(ib));
	}
}

IC void CBackend::Render				(D3DPRIMITIVETYPE T, u32 baseV, u32 startV, u32 countV, u32 startI, u32 PC)
{
	stat.calls			++;
	stat.verts			+= countV;
	stat.polys			+= PC;
	xforms.flush		();
	constants.flush		();
	CHK_DX				(HW.pDevice->DrawIndexedPrimitive(T,baseV, startV, countV,startI,PC));
}

IC void CBackend::Render				(D3DPRIMITIVETYPE T, u32 startV, u32 PC)
{
	stat.calls			++;
	stat.verts			+= 3*PC;
	stat.polys			+= PC;
	xforms.flush		();
	constants.flush		();
	CHK_DX				(HW.pDevice->DrawPrimitive(T, startV, PC));
}

IC void CBackend::set_Shader			(Shader* S, u32 pass)
{
	set_Element			(S->lod0,pass);
}
IC void CBackend::set_Geometry			(SGeometry* _geom)
{
	set_Format			(_geom->dcl);
	set_Vertices		(_geom->vb, _geom->vb_stride);
	set_Indices			(_geom->ib);
}

#endif
