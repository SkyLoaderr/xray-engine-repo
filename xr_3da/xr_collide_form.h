#ifndef __XR_COLLIDE_FORM_H__
#define __XR_COLLIDE_FORM_H__

#include "collide\cl_rapid.h"
#include "collide\cl_defs.h"
#include "xr_list.h"

// refs
class ENGINE_API	CObject;
class ENGINE_API	CInifile;

// t-defs
const DWORD	clGET_TRIS			= (1<<0);
const DWORD	clGET_BOXES			= (1<<1);
const DWORD	clGET_SPHERES		= (1<<2);
const DWORD	clQUERY_ONLYFIRST	= (1<<3);	// stop if was any collision
const DWORD	clQUERY_TOPLEVEL	= (1<<4);	// get only top level of model box/sphere
const DWORD	clQUERY_STATIC		= (1<<5);	// static
const DWORD	clQUERY_DYNAMIC		= (1<<6);	// dynamic
const DWORD	clCOARSE			= (1<<7);	// coarse test (triangles vs obb)

struct clQueryTri
{
	Fvector				p[3];
	const RAPID::tri	*T;
};

struct clQueryCollision
{
	CList<CObject*>		objects;		// affected objects
	CList<clQueryTri>	tris;			// triangles		(if queried)
	CList<Fobb>			boxes;			// boxes/ellipsoids	(if queried)
	CList<Fvector4>		spheres;		// spheres			(if queried)
	
	IC void				Clear	()
	{
		objects.clear	();
		tris.clear		();
		boxes.clear		();
		spheres.clear	();
	}
	IC void				AddTri( const Fmatrix& m, const RAPID::tri* one ) 
	{
		clQueryTri	T;
		m.transform_tiny	(T.p[0],*one->verts[0]);
		m.transform_tiny	(T.p[1],*one->verts[1]);
		m.transform_tiny	(T.p[2],*one->verts[2]);
		T.T = one;
		tris.push_back		(T);
	}
	IC void				AddTri(const RAPID::tri* one ) 
	{
		clQueryTri	T;
		T.p[0]	= *one->verts[0];
		T.p[1]	= *one->verts[1];
		T.p[2]	= *one->verts[2];
		T.T		= one;
		tris.push_back		(T);
	}
	IC void				AddBox(const Fmatrix& M, const Fbox& B)
	{
		Fobb			box;
		Fvector			c;
		B.getcenter		(c);
		B.getradius		(box.m_halfsize);
		
		Fmatrix			T,R;
		T.translate		(c);
		R.mul_43		(M,T);

		box.xform_set	(R);
		boxes.push_back	(box);
	}
	IC void				AddBox(const Fobb& B)
	{
		boxes.push_back	(B);
	}
};

class ENGINE_API	CCFModel
{
	friend class	CObjectSpace;
protected:
	struct			RayQuery
	{
		Fvector		start;
		Fvector		dir;
		float		range;
		int			element;
	};

	Fsphere			Sphere;

	BOOL			enabled, enabled_prev;

	CObject*		owner;			// владелец
	Irect			last_rect;		// последние занимаемые слоты
	Fbox			s_box;			// BBox объекта
	DWORD			dwQueryID;
public:

					CCFModel		( CObject* _owner );
	virtual			~CCFModel		( );

	virtual BOOL	_clRayTest		( RayQuery& Q) = 0;
	virtual BOOL	_svRayTest		( RayQuery& Q) = 0;
	virtual void	_BoxQuery		( const Fbox& B, const Fmatrix& M, DWORD flags) = 0;

	IC CObject*		Owner			( )						{ return owner;}
	IC BOOL			GetEnable		( )						{ return enabled;}
	void			Enable 			( BOOL _enable )		{ enabled_prev = enabled; enabled = _enable; }
	void			EnableRollback	( )						{ enabled = enabled_prev; }

	const Fbox&		GetBBox			( )						{ return s_box;}
	float			GetRadius		( )						{ return Sphere.R;}
	Fsphere&		GetSphere		( )						{ return Sphere; }

	void			OnMove			( );
};

class ENGINE_API	CCF_Polygonal : public CCFModel
{
private:
	RAPID::Model	model;
public:
					CCF_Polygonal	( CObject* _owner );

	virtual BOOL	_clRayTest		( RayQuery& Q);
	virtual BOOL	_svRayTest		( RayQuery& Q);
	virtual void	_BoxQuery		( const Fbox& B, const Fmatrix& M, DWORD flags);

	BOOL			LoadModel		( CInifile* ini, const char *section );
	RAPID::tri*		GetTris			() { return model.GetTris();  }
	RAPID::Model*	GetModel		() { return &model; }
};

class ENGINE_API	CCF_Skeleton : public CCFModel
{
public:
	struct xOBB
	{
		// Ray test
		Fmatrix		IM;
		Fbox		B;
		// Ready to use OBB
		Fobb		OBB;
	};
private:
	Fbox			base_box;
	vector<xOBB>	model;

	DWORD			dwFrame;		// The model itself
	DWORD			dwFrameTL;		// Top level

	void			BuildState		();
	void			BuildTopLevel	();
public:
					CCF_Skeleton	( CObject* _owner );

	virtual BOOL	_clRayTest		( RayQuery& Q );
	virtual BOOL	_svRayTest		( RayQuery& Q );
	virtual void	_BoxQuery		( const Fbox& B, const Fmatrix& M, DWORD flags);
};

class ENGINE_API	CCF_EventBox : public CCFModel
{
private:
	Fplane			Planes[6];
public:
					CCF_EventBox	( CObject* _owner );

	virtual BOOL	_clRayTest		( RayQuery& Q);
	virtual BOOL	_svRayTest		( RayQuery& Q);
	virtual void	_BoxQuery		( const Fbox& B, const Fmatrix& M, DWORD flags);

	BOOL			Contact			( CObject* O);
};

#endif //__XR_COLLIDE_FORM_H__
