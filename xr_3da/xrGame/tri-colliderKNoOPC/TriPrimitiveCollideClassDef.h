#ifndef TRI_PRIMITIVE_COLIDE_CLASS_DEF
#define TRI_PRIMITIVE_COLIDE_CLASS_DEF

#define TRI_PRIMITIVE_COLIDE_CLASS_DECLARE(primitive)				\
class primitive##Tri\
{\
public:\
IC	static float Proj  (dxGeom* o,const dReal* normal){return d##primitive##Proj(o,normal);}\
static int	 Collide(\
					const dReal* v0,const dReal* v1,const dReal* v2,\
					CDB::TRI* T,\
					dxGeom *o1,dxGeom *o2,\
					int flags, dContactGeom *contact, int skip\
					);\
static int	CollidePlain(\
						const dReal* triSideAx0,const dReal* triSideAx1,\
						const dReal* triAx,\
						CDB::TRI* T,\
						dReal dist,\
						dxGeom *o1, dxGeom *o2,\
						int flags, dContactGeom *contact, int skip\
						);\
\
};


#define TRI_PRIMITIVE_COLIDE_CLASS_IMPLEMENT(primitive)			\
	int primitive##Tri:: Collide(\
	const dReal* v0,const dReal* v1,const dReal* v2,\
	CDB::TRI* T,\
	dxGeom *o1,dxGeom *o2,\
	int flags, dContactGeom *contact, int skip\
	){\
		return dTri##primitive(v0,v1,v2,T,o1,o2,flags,contact,skip);\
	}\
	int primitive##Tri::CollidePlain(\
	const dReal* triSideAx0,const dReal* triSideAx1,\
	const dReal* triAx,\
	CDB::TRI* T,\
	dReal dist,\
	dxGeom *o1, dxGeom *o2,\
	int flags, dContactGeom *contact, int skip\
	)\
	{\
	return dSortedTri##primitive(triSideAx0,triSideAx1,triAx,T,dist,o1,o2,flags,contact,skip);\
	}
#endif