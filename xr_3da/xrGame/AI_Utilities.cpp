#include "stdafx.h"
#include "ai_space.h"

//******* Distance 2 line segment
IC float	SqrDistance2Segment(const Fvector& P, const Fvector& A, const Fvector& B)
{
	// Determine t (the length of the vector from ‘a’ to ‘p’)
	Fvector c; c.sub(P,A);
	Fvector V; V.sub(B,A); 
	
	float d = V.magnitude();
	
	V.div	(d); 
	float t = V.dotproduct(c);
	
	// Check to see if ‘t’ is beyond the extents of the line segment
	if (t <= 0.0f)	return P.distance_to_sqr(A);
	if (t >= d)		return P.distance_to_sqr(B);
	
	// Return the point between ‘a’ and ‘b’
	// set length of V to t. V is normalized so this is easy
	Fvector	R;	R.direct(A,V,t);
	return P.distance_to_sqr(R);
}

//******* Vertical projection of point onto plane
IC void projectPoint(const Fplane& PL, Fvector& P) 
{	P.y -= PL.classify(P)/PL.n.y; }

//******* Distance 2 node
float	CAI_Space::u_SqrDistance2Node(const Fvector& P, const NodeCompressed* Node)
{
	// decompress positions
	Fvector P0,P1;
	UnpackPosition			(P0,Node->p0);
	UnpackPosition			(P1,Node->p1);
	
	// decompress plane
	Fplane	PL;	
	pvDecompress			(PL.n,Node->plane);
	PL.d = - PL.n.dotproduct(P0);
	
	// create vertices
	Fvector		v1,v2,v3,v4;
	float		st = Header().size/2;
	v1.set(P0.x-st,P0.y,P0.z-st);	projectPoint(PL,v1);	// minX,minZ
	v2.set(P1.x+st,P0.y,P0.z-st);	projectPoint(PL,v2);	// maxX,minZ
	v3.set(P1.x+st,P1.y,P1.z+st);	projectPoint(PL,v3);	// maxX,maxZ
	v4.set(P0.x-st,P1.y,P1.z+st);	projectPoint(PL,v4);	// minX,maxZ
	
	// calculate minimal distance
	float		best,dist;
	best		= SqrDistance2Segment(P,v1,v2);
	dist		= SqrDistance2Segment(P,v2,v3); if (dist<best) best=dist;
	dist		= SqrDistance2Segment(P,v3,v4); if (dist<best) best=dist;
	dist		= SqrDistance2Segment(P,v4,v1); if (dist<best) best=dist;
	
	return	best;
}

