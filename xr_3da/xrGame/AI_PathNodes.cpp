#include "stdafx.h"
#include "ai_pathnodes.h"
#include "custommonster.h"

//******* Distance 2 line segment
IC float	ClosestPointOnSegment(Fvector& Dest, const Fvector& P, const Fvector& A, const Fvector& B)
{
	// Determine t (the length of the vector from ‘a’ to ‘p’)
	Fvector c; c.sub(P,A);
	Fvector V; V.sub(B,A); 
	
	float d = V.magnitude();
	
	V.div	(d); 
	float t = V.dotproduct(c);
	
	// Check to see if ‘t’ is beyond the extents of the line segment
	if (t <= 0.0f)	{ Dest.set(A); return P.distance_to_sqr(Dest); }
	if (t >= d)		{ Dest.set(B); return P.distance_to_sqr(Dest); }
	
	// Return the point between ‘a’ and ‘b’
	// set length of V to t. V is normalized so this is easy
	Dest.direct(A,V,t);
	return P.distance_to_sqr(Dest);
}

//******* Vertical projection of point onto plane
IC void projectPoint(const Fplane& PL, Fvector& P) 
{	P.y -= PL.classify(P)/PL.n.y; }


//******* PContour unpacking
void	UnpackContour(PContour& C, DWORD ID)
{
	CAI_Space&	AI			= Level().AI;
	NodeCompressed* Node	= AI.Node(ID);

	// decompress positions
	Fvector P0,P1;
	AI.UnpackPosition		(P0,Node->p0);
	AI.UnpackPosition		(P1,Node->p1);
	
	// decompress plane
	Fplane	PL;	
	pvDecompress			(PL.n,Node->plane);
	PL.d = - PL.n.dotproduct(P0);
	
	// create vertices
	float		st = AI.Header().size/2;
	C.v1.set(P0.x-st,P0.y,P0.z-st);	projectPoint(PL,C.v1);	// minX,minZ
	C.v2.set(P1.x+st,P0.y,P0.z-st);	projectPoint(PL,C.v2);	// maxX,minZ
	C.v3.set(P1.x+st,P1.y,P1.z+st);	projectPoint(PL,C.v3);	// maxX,maxZ
	C.v4.set(P0.x-st,P1.y,P1.z+st);	projectPoint(PL,C.v4);	// minX,maxZ
}

//******* Distance 2 node
void	ClosestPointOnContour(Fvector& Dest, const Fvector& P, PContour& C)
{
	// calculate minimal distance
	Fvector		T;
	float		best,dist;
	best		= ClosestPointOnSegment(Dest,P,C.v1,C.v2);
	dist		= ClosestPointOnSegment(T,P,C.v2,C.v3); if (dist<best) { best=dist; Dest.set(T); }
	dist		= ClosestPointOnSegment(T,P,C.v3,C.v4); if (dist<best) { best=dist; Dest.set(T); }
	dist		= ClosestPointOnSegment(T,P,C.v4,C.v1); if (dist<best) { best=dist; Dest.set(T); }
}

//******* PContour intersection
void	IntersectContours	(PSegment& Dest, PContour& C1, PContour& C2)
{
	Fvector T;
	ClosestPointOnContour	(Dest.v1,C2.v1,C1);
	ClosestPointOnContour	(T,C2.v2,C1);	
	if (!Dest.v1.similar(T)){Dest.v2.set(T);return;}
	ClosestPointOnContour	(T,C2.v3,C1);
	if (!Dest.v1.similar(T)){Dest.v2.set(T);return;}
	ClosestPointOnContour	(T,C2.v4,C1);
	if (!Dest.v1.similar(T)){Dest.v2.set(T);return;}

	Dest.v2.set(Dest.v1);
	Log("! AI_PathNodes: Can't find intersection segment");
}

//******* 2D segments intersection
#define	LI_NONE				0
#define LI_COLLINEAR        0
#define	LI_INTERSECT		1
#define LI_EQUAL	        2

IC int lines_intersect( 
			float x1, float y1,		/* First line segment */
		    float x2, float y2,

		    float x3, float y3,		/* Second line segment */
		    float x4, float y4,

		    float *x, float *y      /* Output value: point of intersection */
               )
{
    float a1, a2, b1, b2, c1, c2;	/* Coefficients of line eqns. */
    float r1, r2, r3, r4;			/* 'Sign' values */
    float denom, num;				/* Intermediate values */
	
									/* Compute a1, b1, c1, where line joining points 1 and 2
									* is "a1 x  +  b1 y  +  c1  =  0".
	*/
	
    a1 = y2 - y1;
    b1 = x1 - x2;
    c1 = x2 * y1 - x1 * y2;
	
    /* Compute r3 and r4.
	*/
	
    r3 = a1 * x3 + b1 * y3 + c1;
    r4 = a1 * x4 + b1 * y4 + c1;
	
    /* Check signs of r3 and r4.  If both point 3 and point 4 lie on
	* same side of line 1, the line segments do not intersect.
	*/
	
    if ( r3*r4 > 0 )      return ( LI_NONE );
	
    /* Compute a2, b2, c2 */
	
    a2 = y4 - y3;
    b2 = x3 - x4;
    c2 = x4 * y3 - x3 * y4;
	
    /* Compute r1 and r2 */
	
    r1 = a2 * x1 + b2 * y1 + c2;
    r2 = a2 * x2 + b2 * y2 + c2;
	
    /* Check signs of r1 and r2.  If both point 1 and point 2 lie
	* on same side of second line segment, the line segments do
	* not intersect.
	*/
	
    if ( r1*r2 > 0 )		return ( LI_NONE );
	
	// Check for equality
	if ( fabsf(r1*r2)<EPS_S && fabsf(r3*r4)<EPS_S ) return LI_NONE;

    /* Line segments intersect: compute intersection point. 
	*/
	
    denom = a1 * b2 - a2 * b1;
    if ( fabsf(denom) < EPS ) return ( LI_COLLINEAR );
	
    num = b1 * c2 - b2 * c1;
    *x = num / denom;
	
    num = a2 * c1 - a1 * c2;
    *y = num / denom;
	
    return ( LI_INTERSECT );
} /* lines_intersect */

//******* pseudo 3D segments intersection
const float corner_r = 0.05f;
BOOL SegmentsIntersect(Fvector& dst, Fvector& v1, Fvector& v2, Fvector& v3, Fvector& v4)
{
	// corner check (v4 - end, v1-v2 - segm)
	if (v4.similar(v1,corner_r))	{ dst.set(v1); return TRUE; }
	if (v4.similar(v2,corner_r))	{ dst.set(v2); return TRUE; }
	if (v3.similar(v1,corner_r))	{ dst.set(v1); return TRUE; }
	if (v3.similar(v2,corner_r))	{ dst.set(v2); return TRUE; }

	// projected intersection
	Fvector T;
	if (LI_INTERSECT!=lines_intersect(v1.x,v1.z,v2.x,v2.z,v3.x,v3.z,v4.x,v4.z,&T.x,&T.z))	return FALSE;
	
	// unproject into 3D
	float lx	= (v2.x-v1.x);
	float lz	= (v2.z-v1.z);
	float bary1 = (T.x-v1.x)/lx;
	float bary2 = (T.z-v1.z)/lz;
	if (fis_zero(lx))	bary1=bary2;
	if (fis_zero(lz))	bary2=bary1;
	VERIFY(fsimilar(bary1,bary2));

	float bary	= (bary1+bary2)/2;
	dst.x = T.x;
	dst.y = v1.y + bary*(v2.y-v1.y);
	dst.z = T.z;
	return TRUE;

	/*
	T.y=0;
	ClosestPointOnSegment(dst,T,v1,v2);
	return FALSE;
	*/
}

void CPathNodes::BuildTravelLine(const Fvector& current_pos)
{
	R_ASSERT				(!Nodes.empty());
	CTrevelNode				current,next;
	bNeedRebuild			= FALSE;

	// start point
	TravelPath.clear		();
	current.P.set			(current_pos);
	current.floating		= FALSE;
	TravelPath.push_back	(current);

	// end point
	CAI_Space&	AI			= Level().AI;
	NodeCompressed* Node	= AI.Node(Nodes.back());
	Fvector P0,P1;
	AI.UnpackPosition		(P0,Node->p0);
	AI.UnpackPosition		(P1,Node->p1);
	Fvector Last; Last.lerp	(P0,P1,.5f);

	// segmentation
	PContour					Ccur,Cnext;
	UnpackContour			(Ccur,Nodes[0]);
	Segments.clear			();
	for (DWORD I=1; I<Nodes.size(); I++)
	{
		PSegment				S;
		UnpackContour		(Cnext,Nodes[I]);
		IntersectContours	(S,Ccur,Cnext);
		Segments.push_back	(S);
		Ccur				= Cnext;
	}

	// path building
	for (I=0; I<Segments.size(); I++)
	{
		// build probe point
		PSegment& S = Segments[I];
		ClosestPointOnSegment(next.P,current.P,S.v1,S.v2);

		// select far point
		Fvector fp	= Last;
		if ((I+1)<Segments.size())	{
			PSegment& F = Segments[I+1];
			ClosestPointOnSegment(fp,current.P,F.v1,F.v2);
		}

		// try to cast an line from 'current.P' to 'fp'
		Fvector	ip;
		if (SegmentsIntersect(ip,S.v1,S.v2,current.P,fp))
		{
			next.P.set(ip);
		} else {
			// find nearest point to segment 'current.P' to 'fp'
			float d1 = S.v1.distance_to(fp)+S.v1.distance_to(current.P);
			float d2 = S.v2.distance_to(fp)+S.v2.distance_to(current.P);
			if (d1<d2)	next.P.set(S.v1);
			else		next.P.set(S.v2);
		}

		// record new point
		if (!next.P.similar(TravelPath.back().P))	TravelPath.push_back(next);
		current				= next;
	}
	next.P.set			(Last);
	if (!next.P.similar(TravelPath.back().P))	TravelPath.push_back(next);

	// setup variables
	TravelStart				= 0;
}

void CPathNodes::Calculate(CCustomMonster* Me, Fvector& p_dest, Fvector& p_src, float speed, float dt)
{
	if (TravelPath.empty())	{
		fSpeed	= 0;
		return	;
	}
	if (dt<EPS)			return;
	float	dist		=	speed*dt;
	float	dist_save	=	dist;
	p_dest				=	p_src;

	// move full steps
	Fvector	mdir,target;
	target.set		(TravelPath[TravelStart+1].P);
	mdir.sub		(target, p_dest);
	float	mdist	=	mdir.magnitude();
	while (dist>mdist) 
	{
		p_dest.set	(target);

		if ((TravelStart+2) >= TravelPath.size()) 
		{
			fSpeed			= 0;
			return;
		} else {
			dist			-= mdist;
			TravelStart		++;
			target.set		(TravelPath[TravelStart+1].P);
			mdir.sub		(target, p_dest);
			mdist			= mdir.magnitude();
		}
	}

	// move last quantity
	Fvector				motion,final;
	motion.mul			(mdir,dist/mdist);

	// resolve stucking
	Device.Statistic.Physics.Begin	();
	Me->UpdateTransform	();
	Me->Movement.Move	(final,motion,TRUE);
	motion.sub			(final,p_dest);
	p_dest.set			(final);

	float	real_motion	= motion.magnitude() + dist_save-dist;
	float	real_speed	= real_motion/dt;
	fSpeed				= 0.5f*fSpeed + 0.5f*real_speed;
	Device.Statistic.Physics.End	();
}

void CPathNodes::Direction(Fvector& dest)
{
	if (TravelPath.empty())	
	{
		dest.set	(0,0,1);
		return;
	}
	dest.sub		(TravelPath[TravelStart+1].P, TravelPath[TravelStart].P);
	dest.normalize	();
}
