#ifndef _INCDEF_DetailModel_H_
#define _INCDEF_DetailModel_H_
#pragma once

// refs
class CEditableObject;

class CDetail{
	friend class EDetailManager;
	friend class CDetailManager;
    friend class TfrmDOShuffle;

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

    float				s_min;
    float				s_max;
    float 				m_fDensityFactor;

	// render
	fvfVertexIn			*vertices;
	DWORD				number_vertices;
	WORD				*indices;
	DWORD				number_indices;
	Shader*				shader;
	DWORD				flags;
	Fsphere				bv_sphere;
	Fbox				bv_bb;
    
    // references
    AnsiString			m_sRefs;
	CEditableObject*	m_pRefs;

	int 				_AddVert		(const Fvector& p, float u, float v);
public:
    bool				m_bMarkDel;
public:
						CDetail			();
	virtual             ~CDetail		();

	bool				Load            (CStream&);
	void				Save            (CFS_Base&);
    void				Export			(CFS_Base&);

	bool				Update			(LPCSTR name);
	void				Unload			();

    LPCSTR				GetName			();
	void				OnDeviceCreate	();
	void				OnDeviceDestroy	();
};
DEFINE_VECTOR(CDetail*,DOVec,DOIt);

#endif //_INCDEF_DetailModel_H_

