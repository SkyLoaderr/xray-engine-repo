#include "stdafx.h"
#include "ai_pathnodes.h"
#include "custommonster.h"

using namespace AI;

//******* Distance 2 line segment
IC float	ClosestPointOnSegment(Fvector& Dest, const Fvector& P, const Fvector& A, const Fvector& B)
{
	// Determine t (the length of the vector from СaТ to СpТ)
	Fvector c; c.sub(P,A);
	Fvector V; V.sub(B,A); 
	
	float d = V.magnitude();
	
	V.div	(d); 
	float t = V.dotproduct(c);
	
	// Check to see if СtТ is beyond the extents of the line segment
	if (t <= 0.0f)	{ Dest.set(A); return P.distance_to_sqr(Dest); }
	if (t >= d)		{ Dest.set(B); return P.distance_to_sqr(Dest); }
	
	// Return the point between СaТ and СbТ
	// set length of V to t. V is normalized so this is easy
	Dest.mad(A,V,t);
	return P.distance_to_sqr(Dest);
}

//******* Vertical projection of point onto plane
IC void projectPoint(const Fplane& PL, Fvector& P) 
{	P.y -= PL.classify(P)/PL.n.y; }


//******* PContour unpacking
void	UnpackContour(CPathNodes::PContour& C, u32 ID)
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
	
	/**/
	C.v1.set(P0.x-st,P0.y,P0.z-st);	projectPoint(PL,C.v1);	// minX,minZ
	C.v2.set(P1.x+st,P0.y,P0.z-st);	projectPoint(PL,C.v2);	// maxX,minZ
	C.v3.set(P1.x+st,P1.y,P1.z+st);	projectPoint(PL,C.v3);	// maxX,maxZ
	C.v4.set(P0.x-st,P1.y,P1.z+st);	projectPoint(PL,C.v4);	// minX,maxZ
	/**
	C.v1.set(P0.x-st,ffGetY(*Node,P0.x-st,P0.z-st),P0.z-st);
	C.v2.set(P1.x+st,ffGetY(*Node,P1.x+st,P0.z-st),P0.z-st);
	C.v3.set(P1.x+st,ffGetY(*Node,P1.x+st,P1.z+st),P1.z+st);
	C.v4.set(P0.x-st,ffGetY(*Node,P0.x-st,P1.z+st),P1.z+st);
	/**/
}

/**
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
/**/

/**/
IC bool bfInsideContour(Fvector &tPoint, CPathNodes::PContour &tContour)
{
	return((tContour.v1.x - EPS_L <= tPoint.x) && (tContour.v1.z - EPS_L <= tPoint.z) && (tContour.v3.x + EPS_L >= tPoint.x) && (tContour.v3.z + EPS_L >= tPoint.z));
}

IC bool bfSimilar(Fvector &tPoint0, Fvector &tPoint1)
{
	return((_abs(tPoint0.x - tPoint1.x) < EPS_L) && (_abs(tPoint0.z - tPoint1.z) < EPS_L));
}

IC void vfIntersectContours(CPathNodes::PSegment &tSegment, CPathNodes::PContour &tContour0, CPathNodes::PContour &tContour1)
{
	bool bFound = false;
	
	if (bfInsideContour(tContour0.v1,tContour1)) {
		tSegment.v1 = tContour0.v1;
		bFound = true;
	}

	if (bfInsideContour(tContour0.v2,tContour1)) {
		if (!bFound) {
			tSegment.v1 = tContour0.v2;
			bFound = true;
		}
		else {
			tSegment.v2 = tContour0.v2;
			return;
		}
	}
	if (bfInsideContour(tContour0.v3,tContour1)) {
		if (!bFound) {
			tSegment.v1 = tContour0.v3;
			bFound = true;
		}
		else {
			tSegment.v2 = tContour0.v3;
			return;
		}
	}
	if (bfInsideContour(tContour0.v4,tContour1)) {
		if (!bFound) {
			tSegment.v1 = tContour0.v4;
			bFound = true;
		}
		else {
			tSegment.v2 = tContour0.v4;
			return;
		}
	}
	if (bFound) {
		if (bfInsideContour(tContour1.v1,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v1)))) {
			tSegment.v2 = tContour1.v1;
			return;
		}
		if (bfInsideContour(tContour1.v2,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v2)))) {
			tSegment.v2 = tContour1.v2;
			return;
		}
		if (bfInsideContour(tContour1.v3,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v3)))) {
			tSegment.v2 = tContour1.v3;
			return;
		}
		if (bfInsideContour(tContour1.v4,tContour0) && (!(bfSimilar(tSegment.v1,tContour1.v4)))) {
			tSegment.v2 = tContour1.v4;
			return;
		}
	}
	else {
		if (bfInsideContour(tContour1.v1,tContour0)) {
			tSegment.v1 = tContour1.v1;
			bFound = true;
		}
		if (bfInsideContour(tContour1.v2,tContour0)) {
			if (!bFound) {
				tSegment.v1 = tContour1.v2;
				bFound = true;
			}
			else {
				tSegment.v2 = tContour1.v2;
				return;
			}
		}
		if (bfInsideContour(tContour1.v3,tContour0)) {
			if (!bFound) {
				tSegment.v1 = tContour1.v3;
				bFound = true;
			}
			else {
				tSegment.v2 = tContour1.v3;
				return;
			}
		}
		if (bfInsideContour(tContour1.v4,tContour0)) {
			if (!bFound) {
				tSegment.v1 = tContour1.v4;
				bFound = true;
			}
			else {
				tSegment.v2 = tContour1.v4;
				return;
			}
		}
	}

	if (bFound) {
		tSegment.v2 = tSegment.v1;
		Msg("! AI_PathNodes: segment has null length ([%6.4f,%6.4f],[%6.4f,%6.4f] -> [%6.4f,%6.4f],[%6.4f,%6.4f])",tContour0.v1.x,tContour0.v1.z,tContour0.v3.x,tContour0.v3.z,tContour1.v1.x,tContour1.v1.z,tContour1.v3.x,tContour1.v3.z);
	}
	else
		Msg("! AI_PathNodes: Can't find intersection segment ([%6.4f,%6.4f],[%6.4f,%6.4f] -> [%6.4f,%6.4f],[%6.4f,%6.4f])",tContour0.v1.x,tContour0.v1.z,tContour0.v3.x,tContour0.v3.z,tContour1.v1.x,tContour1.v1.z,tContour1.v3.x,tContour1.v3.z);
}
/**/

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
	if ( _abs(r1*r2)<EPS_S && _abs(r3*r4)<EPS_S ) return LI_NONE;

    /* Line segments intersect: compute intersection point. 
	*/
	
    denom = a1 * b2 - a2 * b1;
    if ( _abs(denom) < EPS ) return ( LI_COLLINEAR );
	
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
	if (fis_zero(lx,EPS_L))	bary1=bary2;
	if (fis_zero(lz,EPS_L))	bary2=bary1;
	//VERIFY(fsimilar(bary1,bary2,EPS_L));

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
	CTravelNode				current,next;
	bNeedRebuild			= FALSE;

	// start point
	TravelPath.clear		();
	current.P.set			(current_pos);
	current.floating		= FALSE;
	TravelPath.push_back	(current);

	// end point
	CAI_Space&	AI			= Level().AI;
	Fvector		Last		= AI.tfGetNodeCenter(Nodes.back());

	// segmentation
	PContour				Ccur,Cnext;
	UnpackContour			(Ccur,Nodes[0]);
	Segments.clear			();
	for (u32 I=1; I<Nodes.size(); I++)
	{
		PSegment				S;
		UnpackContour		(Cnext,Nodes[I]);
		vfIntersectContours	(S,Ccur,Cnext);
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

		// try to cast a line from 'current.P' to 'fp'
		Fvector	ip;
		if (SegmentsIntersect(ip,S.v1,S.v2,current.P,fp)) {
			next.P.set(ip);
		} else {
			// find nearest point to segment 'current.P' to 'fp'
			float d1 = S.v1.distance_to(fp)+S.v1.distance_to(current.P);
			float d2 = S.v2.distance_to(fp)+S.v2.distance_to(current.P);
			if (d1<d2)	next.P.set(S.v1);
			else		next.P.set(S.v2);
		}

		// record _new point
		if (!next.P.similar(TravelPath.back().P))	TravelPath.push_back(next);
		current				= next;
	}
	next.P.set			(Last);
	if (!next.P.similar(TravelPath.back().P))	TravelPath.push_back(next);

	// setup variables
	TravelStart				= 0;
	/**
	// checking for Y-s
	for (int i=1; i<TravelPath.size(); i++)
		if (TravelPath[i].P.y - TravelPath[i - 1].P.y > .5f) {
			Msg("AI_BuildTravelLine : suspicious Y-point found");
		}
	/**/
	//Engine.Sheduler.Slice	();
}

void CPathNodes::Calculate(CCustomMonster* Me, Fvector& p_dest, Fvector& p_src, float speed, float dt)
{
	if ((TravelPath.empty()) || (TravelPath.size() - 1 <= TravelStart))	{
		fSpeed = 0;
		return;
	}
	//Msg("TP %d",TravelPath.size());
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
	Fvector				motion;
	motion.mul			(mdir,dist/mdist);

	// resolve stucking
	Device.Statistic.Physics.Begin	();
	Me->UpdateTransform	();
//	if (m_bCollision) {
//		Fvector final;
//		Me->Movement.Move	(final,motion,FALSE);
//		motion.sub			(final,p_dest);
//		p_dest.set			(final);
//	}
//	else
		p_dest.add			(motion);

	float	real_motion	= motion.magnitude() + dist_save-dist;
	float	real_speed	= real_motion/dt;
	fSpeed				= 0.5f*fSpeed + 0.5f*real_speed;
	Device.Statistic.Physics.End	();
}

void CPathNodes::Direction(Fvector& dest)
{
	if (TravelPath.size() < 2)	
	{
		dest.set	(0,0,1);
		return;
	}
	dest.sub		(TravelPath[TravelStart+1].P, TravelPath[TravelStart].P);
	if (dest.magnitude() < EPS_L)
		dest.set(0,0,1);
	else
		dest.normalize();
}
// MovementControl.cpp: implementation of the CMovementControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\cl_intersect.h"

#include "MovementControl.h"
#include "entity.h"

#define GROUND_FRICTION	10.0f
#define AIR_FRICTION	0.01f
#define WALL_FRICTION	3.0f
//#define AIR_RESIST		0.001f

#define def_X_SIZE_2	0.35f
#define def_Y_SIZE_2	0.8f
#define def_Z_SIZE_2	0.35f

void CMovementControl::dbg_Draw()
{
	if (!bDebug)		return;
	/*
	Fvector P2; P2.add(vPosition,vVelocity);
	Device.Primitive.dbg_DrawLINE(Fidentity,vPosition,P2,D3DCOLOR_RGBA(255,255,255,255));
	P2.add(vPosition,vLastMotion);
	Device.Primitive.dbg_DrawLINE(Fidentity,vPosition,P2,D3DCOLOR_RGBA(0,255,0,255));
	*/

	Fvector sz,C; aabb.getsize(sz); sz.div(2); aabb.getcenter(C);
	Fmatrix	M = pObject->svXFORM();
	M.c.add (C);
	Device.Primitive.dbg_DrawOBB (M,sz,D3DCOLOR_RGBA(0,0,255,255));

	Fvector fct;
	pObject->svXFORM().transform_tiny(fct,vFootCenter);
	Fmatrix fm;
	fm.translate(fct);
	Device.Primitive.dbg_DrawOBB (fm,vFootExt,D3DCOLOR_RGBA(255,0,255,255));

	Fmatrix	A;	A.set(pObject->Rotation());
	Fmatrix		xform	=	pObject->svXFORM();
	Fvector		foot_ext, foot_center;
	aabb.getsize(foot_ext);
	foot_ext.set(foot_ext.x*.5f,vFootExt.y,foot_ext.z*.5f);
	xform.transform_tiny(foot_center,vFootCenter);
	A.c.set		(foot_center);
	Device.Primitive.dbg_DrawOBB (A,foot_ext,D3DCOLOR_RGBA(255,255,255,255));
/*	
	Fvector		foot_ext;
	foot_ext.set	(2.f*vFootExt.x,vFootExt.y,2.f*vFootExt.z);
	Device.Primitive.dbg_DrawAABB (fct,foot_ext.x,foot_ext.y,foot_ext.z,D3DCOLOR_RGBA(255,255,255,255));
*/
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMovementControl::CMovementControl()
{
	pObject	=			NULL;
	eOldEnvironment =	peInAir;
	eEnvironment =		peInAir;
	aabb.set			(-def_X_SIZE_2,0,-def_Z_SIZE_2, def_X_SIZE_2, def_Y_SIZE_2*2, def_Z_SIZE_2);
	vFootCenter.set		(0,0,0);
	vFootExt.set		(0,0,0);
	fMass				= 100;
	fMinCrashSpeed		= 12.0f;
	fMaxCrashSpeed		= 25.0f;
	vVelocity.set		(0,0,0);
	vPosition.set		(0,0,0);
	vExternalImpulse.set(0,0,0);
	fLastMotionMag		= 1.f;
	fAirFriction		= AIR_FRICTION;
	fWallFriction		= WALL_FRICTION;
	fGroundFriction		= GROUND_FRICTION;
	fFriction			= fAirFriction;
	bIsAffectedByGravity= TRUE;
	fActualVelocity		= 0;
	m_fGroundDelayFactor= 1.f;
	gcontact_HealthLost = 0;
	fContactSpeed		= 0.f;
}

CMovementControl::~CMovementControl()
{

}

float	psSqueezeVelocity;
int		psPhysicsFPS=100;

// Velocity+Friction(dumping), Movement
void Integrate(float& v, float &s, float& a, float dt, float f)
{
	float	Q		= 1.f/float(psPhysicsFPS);
	int		steps	= iFloor(dt/Q);	// note: round-towards-zero
	float	v0		= v;
	float	QmulF	= Q*f;
	float	Qdiv2	= Q*0.5f;
	for (int i=0; i<steps; i++) 
	{
		// velocity
		v0 = v;
		v += a*Q - (v0 + a*Q)*QmulF;
		// motion
		s += (v0 + v)*Qdiv2;
	}
	Q = dt-float(steps)*Q;
	QmulF	= Q*f;
	Qdiv2	= Q*0.5f;

	// velocity
	v0 = v;
	v += a*Q - (v0 + a*Q)*QmulF;

	// motion
	s += (v0 + v)*Qdiv2;
}

float Integrate1D_to	(float v, float &s, float& a,  float f, float s_desired)
{
	float	Q		= 1.f/float(psPhysicsFPS*10.f);
	float	v0		= v;
	float	QmulF	= Q*f;
	float	Qdiv2	= Q*0.5f;
	while (_abs(s)<_abs(s_desired)) 
	{
		// velocity
		v0 = v;
		v += a*Q - (v0 + a*Q)*QmulF;

		// motion
		s += (v0 + v)*Qdiv2;
	}
	return v0;
}

void Integrate(Fvector& v, Fvector &s, Fvector& a, float dt, float f, float yf=1.f)
{
	float	Q		= 1.f/float(psPhysicsFPS);
	int		steps	= iFloor(dt/Q);	// note: round-towards-zero
	Fvector	v0		= v;
	float	QmulF	= Q*f;
	float	Qdiv2	= Q*0.5f;
	for (int i=0; i<steps; i++) 
	{
		// velocity
		v0.set(v);
		v.x += a.x*Q - (v0.x + a.x*Q)*QmulF;
//		v.y += a.y*Q - (v0.y + a.y*Q)*QmulF;
		v.y += a.y*Q - (v0.y + a.y*Q)*(QmulF*yf);
		v.z += a.z*Q - (v0.z + a.z*Q)*QmulF;

		// motion
		s.x += (v0.x + v.x)*Qdiv2;
		s.y += (v0.y + v.y)*Qdiv2;
		s.z += (v0.z + v.z)*Qdiv2;
	}
	Q = dt-float(steps)*Q;
	QmulF	= Q*f;
	Qdiv2	= Q*0.5f;

	// velocity
	v0.set	(v);
	v.x += a.x*Q - (v0.x + a.x*Q)*QmulF;
//	v.y += a.y*Q - (v0.y + a.y*Q)*QmulF;
	v.y += a.y*Q - (v0.y + a.y*Q)*(QmulF*yf);
	v.z += a.z*Q - (v0.z + a.z*Q)*QmulF;

	// motion
	s.x += (v0.x + v.x)*Qdiv2;
	s.y += (v0.y + v.y)*Qdiv2;
	s.z += (v0.z + v.z)*Qdiv2;
}

void CMovementControl::CalcMaximumVelocity	(float& v, float a, float friction){
	// first approximation
	float Q		= 5;
	float QmulF = Q*friction;
	v = a*Q - a*Q*QmulF;

	// sample iterations
	float		S; S=0;
	Integrate	(v,S,a,100,friction);
}

void CMovementControl::CalcMaximumVelocity(Fvector& v, Fvector& a, float friction)
{
	// first approximation
	float Q		= 5;
	float QmulF = Q*friction;
	v.x = a.x*Q - a.x*Q*QmulF;
	v.y = a.y*Q - a.y*Q*QmulF;
	v.z = a.z*Q - a.z*Q*QmulF;

	// sample iterations
	Fvector		S; S.set(0,0,0);
	Integrate	(v,S,a,100,friction,1.f);
}

static const float s_fGroundDelaySpeed		= 10.f;
static const float s_fMaxGroundDelayFactor	= 30.f;

void CMovementControl::Calculate(Fvector &_Accel, float ang_speed, float jump, float dt, bool bLight)
{
	Fvector motion,vAccel;
	float	fOldFriction	= fFriction;
	float	fVelocityBefore	= vVelocity.magnitude	();
	float	fVelocityY		= vVelocity.y;

	vExternalImpulse.div(dt);
	vAccel.add			(vExternalImpulse,_Accel);

	// Analyze jump
	bool bJump = (_abs(jump)>EPS) && (eEnvironment==peOnGround);

	// Check for Sleep-State
	bSleep = FALSE;
	float a_speed = _abs(ang_speed);
	float accel_mag = vAccel.magnitude();
	switch (eEnvironment)
	{
	case peOnGround:
		bSleep = (accel_mag<EPS) && (fLastMotionMag<EPS_L) && (!bJump) && (a_speed<EPS_S);
		break;
	default:
		bSleep = (accel_mag<EPS) && (!bIsAffectedByGravity) && (fLastMotionMag<EPS_L) && (!bJump) && (a_speed<EPS_S);
		break;
	}
	if (bSleep)	{
		fLastMotionMag		= 0;
		fActualVelocity		= 0;
		vVelocity.set		(0,0,0);
		return;
	}

	Device.Statistic.Physics.Begin();

	// Calculate gravity
	float gravity;
	gravity = psGravity;
	if (eEnvironment==peOnGround && !bJump) gravity *= 0.75f;
	vAccel.y -= bIsAffectedByGravity?gravity:0;

	// Physics integration
	vExternalImpulse.set(0,0,0);
	motion.set			(0,0,0);
	if (bJump){
//		vPosition.y	+= vFootExt.y;
		vVelocity.x	*= 0.9f;
		vVelocity.z	*= 0.9f;
		vVelocity.y = 0; 
		vAccel.y	+= psGravity+jump/dt;
		fFriction	= fAirFriction;
	}

	// stay ground friction
	if ((eEnvironment==peOnGround)&&(_Accel.magnitude()<EPS_L)){
		m_fGroundDelayFactor += s_fGroundDelaySpeed*dt;
		clamp(m_fGroundDelayFactor,1.f,s_fMaxGroundDelayFactor);
	}else m_fGroundDelayFactor = 1.f;
 
	Integrate	(vVelocity,motion,vAccel,dt,fFriction*m_fGroundDelayFactor);

	// Calculate collisions
	Fvector	final_pos,final_vel;
	
	Fbox bbf;
	bbf.set(-vFootExt.x, -vFootExt.y, -vFootExt.z, +vFootExt.x, +vFootExt.y, +vFootExt.z );
	bbf.add(vFootCenter);
	pCreator->ObjectSpace.cl_Move( 
		pObject->CFORM(), pObject->svXFORM(), 
		motion, aabb, bbf, final_pos, final_vel, EPS);
	
	// Velocity stuff
	float s_calc	= motion.magnitude();	// length of motion - dS - requested
	if (bJump)		final_pos.y	+= vFootExt.y;
	motion.sub		(final_pos,vPosition);	// motion - resulting

	if (peAtWall==eEnvironment)
	{
		float s_dummy	= 0;
		float s_desired	= motion.y;
		vVelocity.y		= Integrate1D_to	(fVelocityY,s_dummy,vAccel.y,fOldFriction,s_desired);
/*
#ifdef _DEBUG		
		Msg				("o:%f / n:%f",fVelocityY,vVelocity.y);
#endif
*/
	}

	//	Don't allow _new velocity to go against original velocity unless told otherwise
	Fvector vel_dir;
	final_vel.normalize_safe	();
	vel_dir.normalize_safe		(vVelocity);
	float s_res					; // length of motion - dS - resulting
	if (s_calc>EPS_S) 
	{
		s_res					= motion.magnitude();	
		// не ставить final_vel.dotproduct(vel_dir)< (=) 0.f равным нулю!!!
		if ((final_vel.dotproduct(vel_dir)<0.f) || (s_res/s_calc)<0.001f){ 
			vVelocity.set	(0,0,0);
			final_pos.set	(vPosition);
			s_res			= 0;
		} else {
			vPosition.set	(final_pos);
		}
	} else {
		vPosition.set	(final_pos);
		s_res			= 0;
	}
	float src				= 20*dt;	clamp(src,0.f,1.f);
	float dst				= 1-src;
	fLastMotionMag			= dst*fLastMotionMag + src*s_res;
	float	fAvgVelocity	= fLastMotionMag/dt;
	fActualVelocity			= dst*fActualVelocity+ src*fAvgVelocity;
	
	// Environment
	if (fLastMotionMag>EPS_S)
		CheckEnvironment	(vPosition);
 
	// Set movement friction
	switch(eEnvironment)
	{
		case peInAir:		fFriction = fAirFriction;		break;
		case peAtWall:		fFriction = fWallFriction;		break;
		case peOnGround:	fFriction = fGroundFriction;	break;
		default: NODEFAULT;
	}

	// если определили приземление или резко уменьшилась скорость (пор€док fMinCrashSpeed)
	// определим возможное повреждение
	gcontact_Was		= FALSE;
	if (
		((eOldEnvironment==peInAir || eOldEnvironment==peAtWall) && (eEnvironment==peOnGround)) // ||
//		((_abs(jump)>EPS) && (eOldEnvironment==peOnGround) && (eEnvironment==peOnGround))
		)
	{
		gcontact_Was		= TRUE;
		gcontact_Power		= 0;
		gcontact_HealthLost = 0;
		if (s_res<=s_calc)	
		{
			float		dummy_s	= 0;
			float		a		= vAccel.magnitude();
			fContactSpeed		= Integrate1D_to	(fVelocityBefore,dummy_s,a,fOldFriction,s_res);
			// s_res, dummy_s ???
/*
#ifdef _DEBUG		
			Msg	("dummy_s: %2.3f, sres: %2.3f, scalc: %2.1f, old_aspeed: %2.3f, cspeed: %2.3f (min: %2.1f)",
				dummy_s,s_res,s_calc,fVelocityBefore,fContactSpeed,fMinCrashSpeed);
#endif
*/
			// contact with ground
			gcontact_Power				= fContactSpeed/fMaxCrashSpeed;

			gcontact_HealthLost			= 0;
			if (fContactSpeed>fMinCrashSpeed) 
			{
				//float k=10000.f/(B-A);
				//float dh=_sqrt((dv-A)*k);
				gcontact_HealthLost = 
					(100*(fContactSpeed-fMinCrashSpeed))/(fMaxCrashSpeed-fMinCrashSpeed);
			}
		}
	}

	Device.Statistic.Physics.End();

	//
	Engine.Sheduler.Slice();
}

void CMovementControl::Move(Fvector& Dest, Fvector& Motion, BOOL bDynamic)
{
	Device.Statistic.Physics.Begin();
	Fbox	bbf;
	bbf.set(-vFootExt.x, -vFootExt.y, -vFootExt.z, +vFootExt.x, +vFootExt.y, +vFootExt.z );
	bbf.add(vFootCenter);
	Fvector final_vel;
	pCreator->ObjectSpace.cl_Move( 
		pObject->CFORM(), pObject->svXFORM(), 
		Motion, aabb, bbf, Dest, final_vel, EPS, bDynamic);
	Device.Statistic.Physics.End();

	//
	Engine.Sheduler.Slice();
}

void CMovementControl::CheckEnvironment(const Fvector& newpos)
{
	// verify surface and update Player State
	int cp_cnt=pCreator->ObjectSpace.q_result.tris.size();
	eOldEnvironment	= eEnvironment;
	eEnvironment	= peInAir;

	if (cp_cnt)
	{
		Fmatrix33	A; A.set(pObject->Rotation());
		Fmatrix		xform	=	pObject->svXFORM();
		xform.c				=	newpos;
		Fvector		foot_ext, foot_center;
		aabb.getsize(foot_ext);
		foot_ext.set(foot_ext.x*.5f,vFootExt.y,foot_ext.z*.5f);
		xform.transform_tiny(foot_center,vFootCenter);
		for(int i=0; i<cp_cnt; i++)
		{
			clQueryTri& T=pCreator->ObjectSpace.q_result.tris[i];
			// тестируем положение ног
			if (CDB::TestBBoxTri(A,foot_center,foot_ext,&T.p[0],false))
			{
				Fvector N; 
				N.mknormal(T.p[0],T.p[1],T.p[2]);
				if ((N.y>.7f))
				{
					eEnvironment=peOnGround;
					return;
				} else  {
					if((N.y>=0)&&(N.y<=0.7f))
					{
//						eEnvironment=peOnGround;
						eEnvironment=peInAir;//AtWall;
					}
				}
			}
		}
	}
}

void	CMovementControl::SetEnvironment( int enviroment,int old_enviroment){
	switch(enviroment){
	case 0: eEnvironment=peOnGround;
	break;
	case 1: eEnvironment=peAtWall;
	break;
	case 2: eEnvironment=peInAir;
	}
	switch(old_enviroment){
	case 0: eOldEnvironment=peOnGround;
	break;
	case 1: eOldEnvironment=peAtWall;
	break;
	case 2: eOldEnvironment=peInAir;
	}
}