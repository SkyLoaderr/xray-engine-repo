#include "stdafx.h"
#include "xrmu_model.h"

// vertex utilities
void	xrMU_Model::_vertex::prep_add				(_face* F)
{
	for (v_faces_it I=adjacent.begin(); I!=adjacent.end(); I++)
		if (F==(*I)) return;
	adjacent.push_back(F);
}
void	xrMU_Model::_vertex::prep_remove			(_face* F)
{
	v_faces_it	I = find(adjacent.begin(),adjacent.end(),F);
	if (I!=adjacent.end())	adjacent.erase(I);
}
void	xrMU_Model::_vertex::calc_normal_adjacent	()
{
	N.set(0,0,0);
	for (v_faces_it ad = adjacent.begin(); ad!=adjacent.end(); ad++)
		N.add( (*ad)->N );
	N.normalize_safe();
}


// face utilities
Shader_xrLC&	xrMU_Model::_face::Shader		()
{
	u32 shader_id = pBuild->materials[dwMaterial].reserved;
	return *(pBuild->shaders.Get(shader_id));
}
void			xrMU_Model::_face::CalcNormal	()
{
	Fvector t1,t2;

	Fvector*	v0 = &(v[0]->P);
	Fvector*	v1 = &(v[1]->P);
	Fvector*	v2 = &(v[2]->P);
	t1.sub			(*v1,*v0);
	t2.sub			(*v2,*v1);
	N.crossproduct	(t1,t2);
	float mag		= N.magnitude();
	if (mag<EPS_S)
	{
		N.set(0,1,0);
	} else {
		N.div(mag);
	}
	N.normalize		();
};

// Does the face contains this vertex?
bool			xrMU_Model::_face::VContains	(_vertex* pV)
{
	return VIndex(pV)>=0;
}

// Replace ONE vertex by ANOTHER
void			xrMU_Model::_face::VReplace		(_vertex* what, _vertex* to)
{
	if (v[0]==what) { v[0]=to; what->prep_remove(this); to->prep_add(this); }
	if (v[1]==what) { v[1]=to; what->prep_remove(this); to->prep_add(this); }
	if (v[2]==what) { v[2]=to; what->prep_remove(this); to->prep_add(this); }
}
int				xrMU_Model::_face::VIndex		(_vertex* pV)
{
	if (v[0]==pV) return 0;
	if (v[1]==pV) return 1;
	if (v[2]==pV) return 2;
	return -1;
}
void			xrMU_Model::_face::VSet			(int idx, _vertex* V)
{
	v[idx]=V; V->prep_add(this);
}
void			xrMU_Model::_face::VSet			(_vertex *V1, _vertex *V2, _vertex *V3)
{
	VSet(0,V1);	VSet(1,V2);	VSet(2,V3);
}
BOOL			xrMU_Model::_face::isDegenerated()
{
	return (v[0]==v[1] || v[0]==v[2] || v[1]==v[2]);
};
float			xrMU_Model::_face::EdgeLen		(int edge)
{
	_vertex*	V1 = v[edge2idx[edge][0]];
	_vertex*	V2 = v[edge2idx[edge][1]];
	return		V1->P.distance_to(V2->P);
};
void			xrMU_Model::_face::EdgeVerts	(int e, _vertex** A, _vertex** B)
{
	*A			= v[edge2idx[e][0]];
	*B			= v[edge2idx[e][1]];
}
void			xrMU_Model::_face::CalcCenter	(Fvector &C)
{
	C.set(v[0]->P);
	C.add(v[1]->P);
	C.add(v[2]->P);
	C.div(3);
};

float			xrMU_Model::_face::CalcArea		()
{
	float	e1 = v[0]->P.distance_to(v[1]->P);
	float	e2 = v[0]->P.distance_to(v[2]->P);
	float	e3 = v[1]->P.distance_to(v[2]->P);

	float	p  = (e1+e2+e3)/2.f;
	return	_sqrt( p*(p-e1)*(p-e2)*(p-e3) );
}
float			xrMU_Model::_face::CalcMaxEdge	()
{
	float	e1 = v[0]->P.distance_to(v[1]->P);
	float	e2 = v[0]->P.distance_to(v[2]->P);
	float	e3 = v[1]->P.distance_to(v[2]->P);
	
	if (e1>e2 && e1>e3) return e1;
	if (e2>e1 && e2>e3) return e2;
	return e3;
}

BOOL			xrMU_Model::_face::RenderEqualTo	(Face *F)
{
	if (F->dwMaterial	!= dwMaterial		)	return FALSE;
	if (F->tc.size()	!= F->tc.size()		)	return FALSE;	// redundant???
	return TRUE;
}

void			xrMU_Model::_face::CacheOpacity()
{
	bOpaque	= false;
	
	b_material& M		= pBuild->materials		[dwMaterial];
	b_texture&	T		= pBuild->textures		[M.surfidx];
	if (T.bHasAlpha)	bOpaque = FALSE;
	else				bOpaque = TRUE;
}
