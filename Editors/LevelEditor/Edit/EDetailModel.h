#ifndef EDetailModelH
#define EDetailModelH
#pragma once

#include "DetailModel.h"

// refs
class CEditableObject;

class EDetail: public CDetail{
	friend class EDetailManager;
	friend class CDetailManager;
    friend class TfrmDOShuffle;

    struct EVertexIn: public fvfVertexIn
    {
        				EVertexIn	(const Fvector& _P, float _u, float _v){P.set(_P); u=_u; v=_v;};
        IC void			set			(EVertexIn& src){P.set(src.P); u=src.u; v=src.v;};
        IC void			set			(const Fvector& _P, float _u, float _v){P.set(_P); u=_u; v=_v;};
        IC BOOL			similar		(EVertexIn& V)
        {
            if (!fsimilar	(u,V.u,EPS_L))	return FALSE;
            if (!fsimilar	(v,V.v,EPS_L))	return FALSE;
            if (!P.similar	(V.P,EPS_L))	return FALSE;
            return TRUE;
        }
        IC void			remapUV		(const fvfVertexIn& src, const Fvector2& offs, const Fvector2& scale, bool bRotate)
        {
        	P.set		(src.P);
        	if (bRotate){
	        	u		= scale.x*src.v+offs.x;
	        	v		= scale.y*src.u+offs.y;
            }else{
	        	u		= scale.x*src.u+offs.x;
	        	v		= scale.y*src.v+offs.y;
            }
        }
    };
/*
	struct fvfVertexIn{
		Fvector 		P;
		float			u,v;
        				fvfVertexIn	(const Fvector& _P, float _u, float _v){P.set(_P); u=_u; v=_v;};
        void			set			(fvfVertexIn& src){P.set(src.P); u=src.u; v=src.v;};
        void			set			(const Fvector& _P, float _u, float _v){P.set(_P); u=_u; v=_v;};
        BOOL			similar		(fvfVertexIn& V)
        {
            if (!fsimilar	(u,V.u,EPS_L))	return FALSE;
            if (!fsimilar	(v,V.v,EPS_L))	return FALSE;
            if (!P.similar	(V.P,EPS_L))	return FALSE;
            return TRUE;
        }
	};
	struct fvfVertexOut
	{
		Fvector 		P;
		DWORD			C;
		float			u,v;
	};

    float				m_fMinScale;
    float				m_fMaxScale;

	// render
	fvfVertexIn			*vertices;
	DWORD				number_vertices;
	WORD				*indices;
	DWORD				number_indices;
	ref_shader			shader;
	Flags32				m_Flags;
	Fsphere				bv_sphere;
	Fbox				bv_bb;
*/
    float 				m_fDensityFactor;

    // references
    AnsiString			m_sRefs;
	CEditableObject*	m_pRefs;

	int 				_AddVert		(const Fvector& p, float u, float v);
public:
//    bool				m_bMarkDel;
public:
						EDetail			();
	virtual             ~EDetail		();

	bool				Load            (IReader&);
	void				Save            (IWriter&);
    void				Export			(IWriter&, LPCSTR tex_name, const Fvector2& offs, const Fvector2& scale, bool rot);

	bool				Update			(LPCSTR name);
	virtual void		Unload			();

    LPCSTR				GetName			();
    LPCSTR				GetTextureName	();
	void				OnDeviceCreate	();
	void				OnDeviceDestroy	();
    void				DefferedLoad	();
};
DEFINE_VECTOR(EDetail*,DOVec,DOIt);

#endif //_INCDEF_DetailModel_H_

