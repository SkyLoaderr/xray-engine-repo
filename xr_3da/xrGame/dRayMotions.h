#ifndef dRayMotions_h
#define dRayMotions_h

struct dxRayMotions;
extern int dRayMotionsClassUser;


dxGeom *dCreateRayMotions (dSpaceID space, dReal r, dReal lz);
void dGeomRayMotionsSetParams (dGeomID g, dReal x, dReal y, dReal z);
void dGeomRayMotionsGetParams (dGeomID g,  dReal x, dReal y, dReal z);
#endif //dRayMotions_h