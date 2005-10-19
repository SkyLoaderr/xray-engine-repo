#include "ExtendedGeom.h"
#include "MathUtils.h"
#include "Level.h"
#include "Geometry.h"
ICF	void InitTriangle(CDB::TRI* XTri,Triangle& triangle)
{
	const Fvector* V_array=Level().ObjectSpace.GetStaticVerts();
	const float* VRT[3]={(dReal*)&V_array[XTri->verts[0]],(dReal*)&V_array[XTri->verts[1]],(dReal*)&V_array[XTri->verts[2]]};
	dVectorSub(triangle.side0,VRT[1],VRT[0])						;
	dVectorSub(triangle.side1,VRT[2],VRT[1])						;
	triangle.T=XTri													;
	dCROSS(triangle.norm,=,triangle.side0,triangle.side1)			;
	accurate_normalize(triangle.norm)								;
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
