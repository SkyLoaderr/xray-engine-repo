#include "ExtendedGeom.h"
#include "MathUtils.h"
#include "Level.h"
#include "Geometry.h"
#include "tri-colliderknoopc/dtricollidermath.h"
ICF	void InitTriangle(CDB::TRI* XTri,Triangle& triangle)
{
	const Fvector* V_array=Level().ObjectSpace.GetStaticVerts();
	const float* VRT[3]={(dReal*)&V_array[XTri->verts[0]],(dReal*)&V_array[XTri->verts[1]],(dReal*)&V_array[XTri->verts[2]]};
	dVectorSub(triangle.side0,VRT[1],VRT[0])						;
	dVectorSub(triangle.side1,VRT[2],VRT[1])						;
	triangle.T=XTri													;
	dCROSS(triangle.norm,=,triangle.side0,triangle.side1)			;
	cast_fv(triangle.norm).normalize()								;
	triangle.pos=dDOT(VRT[0],triangle.norm)							;
}
ICF void CalculateTriangle(CDB::TRI* XTri,const float* pos,Triangle& triangle)
{
	InitTriangle(XTri,triangle);
	triangle.dist=dDOT(pos,triangle.norm)-triangle.pos;
}
ICF void CalculateTriangle(CDB::TRI* XTri,dGeomID g,Triangle& triangle)
{
	dVector3	v											;
	dMatrix3	m											;
	const float *p						=NULL				;
	const float *r						=NULL				;
	VERIFY								(g)					;
	CODEGeom::get_final_tx				(g,p,r,v,m)			;
	VERIFY								(p)					;
	CalculateTriangle					(XTri,p,triangle)	;
	
}

ICF bool TriContainPoint(Triangle* T,const float *pos)
{
	//TriContainPoint(const dReal* v0,const dReal* v1,const dReal* v2,const dReal* triAx,const dReal* triSideAx0,const dReal* triSideAx1, const dReal* pos)
	const Fvector* V_array=Level().ObjectSpace.GetStaticVerts();
	CDB::TRI	*XTri=T->T;
	const float* VRT[3]={(dReal*)&V_array[XTri->verts[0]],(dReal*)&V_array[XTri->verts[1]],(dReal*)&V_array[XTri->verts[2]]};
	return TriContainPoint(VRT[0],VRT[1],VRT[2],T->norm,T->side0,T->side1,pos);
}