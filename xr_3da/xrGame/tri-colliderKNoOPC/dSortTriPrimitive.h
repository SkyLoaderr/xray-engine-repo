#ifndef D_SORT_TRI_PRIMITIVE_H
#define D_SORT_TRI_PRIMITIVE_H
typedef int dTriPrimitiveType(const dReal*,const dReal*,const dReal*,CDB::TRI*,dxGeom *,dxGeom *,int, dContactGeom *,int);
typedef int dSortedTriPrimitiveType(const dReal*,const dReal*,const dReal*,CDB::TRI*,dReal,dxGeom *,dxGeom *,int, dContactGeom *,int);
int dSortTriPrimitiveCollide (	dTriPrimitiveType dTriPrimitive,
							  dSortedTriPrimitiveType	dSortedTriPrimitive,
							  dxGeom *o1,		dxGeom *o2,
							  int flags,		dContactGeom *contact,	int skip,
							  CDB::RESULT*		R_begin,
							  CDB::RESULT*		R_end ,
							  CDB::TRI*		T_array,
							  const Fvector*	V_array,
							  Fvector			AABB
							  );
#endif