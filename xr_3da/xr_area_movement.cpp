#include "stdafx.h"
#include "xr_area.h"
#include "xr_collide_form.h"
#include "xr_object.h"
#include "collide\cl_intersect.h"
#include "physics\physicsobject.h"

#include "xr_creator.h"

using namespace	Collide;

int	ENGINE_API psCollideActDepth		= 4;
int	ENGINE_API psCollideActStuckDepth	= 8;

static float cl_epsilon;

// ----------------------------------------------------------------------
// Name  : classifyPoint()
// Input : point - point we wish to classify 
//         pO - Origin of plane
//         pN - Normal to plane 
// Notes : 
// Return: One of 3 classification codes
// -----------------------------------------------------------------------  
IC float classifyPoint(const Fvector& point, const Fvector& planeO, const Fvector& planeN)
{
	Fvector dir;
	dir.sub	(point,planeO);
	return dir.dotproduct(planeN);
}
// ----------------------------------------------------------------------
// Name  : intersectRayPlane()
// Input : rOrigin - origin of ray in world space
//         rVector - vector describing direction of ray in world space
//         pOrigin - Origin of plane 
//         pNormal - Normal to plane
// Notes : Normalized directional vectors expected
// Return: distance to plane in world units, -1 if no intersection.
// -----------------------------------------------------------------------  
IC float intersectRayPlane(const Fvector& rayOrigin,	const Fvector& rayDirection, 
						const Fvector& planeOrigin,	const Fvector& planeNormal)
{
	float numer = classifyPoint(rayOrigin,planeOrigin,planeNormal);
	float denom = planeNormal.dotproduct(rayDirection);
	
	if (denom == 0)  // normal is orthogonal to vector, cant intersect
		return (-1.0f);
	
	return -(numer / denom);	
}
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// Name  : closestPointOnLine()
// Input : a - first end of line segment
//         b - second end of line segment
//         p - point we wish to find closest point on line from 
// Notes : Helper function for closestPointOnTriangle()
// Return: closest point on line segment
// -----------------------------------------------------------------------  

IC void closestPointOnLine(Fvector& res, const Fvector& a, const Fvector& b, const Fvector& p) 
{
	
	// Determine t (the length of the vector from СaТ to СpТ)
	Fvector c; c.sub(p,a);
	Fvector V; V.sub(b,a); 
	
	float d = V.magnitude();
	
	V.div(d);  
	float t = V.dotproduct(c);
	
	// Check to see if СtТ is beyond the extents of the line segment
	if (t <= 0.0f)	{ res.set(a); return; }
	if (t >= d)		{ res.set(b); return; }
	
	// Return the point between СaТ and СbТ
	// set length of V to t. V is normalized so this is easy
	res.direct(a,V,t);
}
IC void closestPointOnEdge(Fvector& res,						// result
						   const Fvector& a, const Fvector& b,	// points
						   const Fvector& ED, float elen,		// edge direction (b-a) and length
						   const Fvector& P)					// query point
{
	// Determine t (the length of the vector from СaТ to СpТ)
	Fvector c; c.sub(P,a);
	float t = ED.dotproduct(c);
	
	// Check to see if СtТ is beyond the extents of the line segment
	if (t <= 0.0f)	{ res.set(a); return; }
	if (t >= elen)	{ res.set(b); return; }
	
	// Return the point between СaТ and СbТ
	res.direct(a,ED,t);
}

// ----------------------------------------------------------------------
// Name  : closestPointOnTriangle()
//         p - point we wish to find closest point on triangle from 
// Return: closest point on line triangle edge
// -----------------------------------------------------------------------  

IC void closestPointOnTriangle(Fvector& result, const Fvector* V, const Fvector& p) 
{
	Fvector Rab; closestPointOnLine(Rab, V[0], V[1], p); float dAB = p.distance_to_sqr(Rab);
	Fvector Rbc; closestPointOnLine(Rbc, V[1], V[2], p); float dBC = p.distance_to_sqr(Rbc);
	Fvector Rca; closestPointOnLine(Rca, V[2], V[0], p); float dCA = p.distance_to_sqr(Rca);
	
	float min = dAB;
	result.set(Rab);
	if (dBC < min) {
		min = dBC;
		result.set(Rbc);
    }
	if (dCA < min)
		result.set(Rca);
}
IC void closestPointOnTriangle(Fvector& result, const tri& T, const Fvector& P) 
{
	Fvector Rab; closestPointOnEdge(Rab, T.p[0], T.p[1], T.e10, T.e10s, P); float dAB = P.distance_to_sqr(Rab);
	Fvector Rbc; closestPointOnEdge(Rbc, T.p[1], T.p[2], T.e21, T.e21s, P); float dBC = P.distance_to_sqr(Rbc);
	Fvector Rca; closestPointOnEdge(Rca, T.p[2], T.p[0], T.e02, T.e02s, P); float dCA = P.distance_to_sqr(Rca);
	
	float min;
	if (dBC < dAB)	{ min = dBC; result.set(Rbc); } 
	else			{ min = dAB; result.set(Rab); }
	if (dCA < min)	result.set(Rca);
}

// ----------------------------------------------------------------------
// Name  : intersectRaySphere()
// Input : rO - origin of ray in world space
//         rV - vector describing direction of ray in world space
//         sO - Origin of sphere 
//         sR - radius of sphere
// Notes : Normalized directional vectors expected
// Return: distance to sphere in world units, -1 if no intersection.
// -----------------------------------------------------------------------  

IC float intersectRaySphere(const Fvector& rO, const Fvector& rV, const Fvector& sO, float sR) 
{
	Fvector Q;
	Q.sub(sO,rO);
	
	float c = Q.magnitude();
	float v = Q.dotproduct(rV);
	float d = sR*sR - (c*c - v*v);
	
	// If there was no intersection, return -1
	if (d < 0.0) return (-1.0f);
	
	// Return the distance to the [first] intersecting point
	return (v - sqrtf(d));
}
IC float intersectRayIdentitySphere(const Fvector& rO, const Fvector& rV) 
{
	Fvector Q;
	Q.invert(rO);
	
	float c = Q.magnitude();
	float v = Q.dotproduct(rV);
	float d = 1 - (c*c - v*v);
	
	// If there was no intersection, return -1
	if (d < 0.0) return (-1.0f);
	
	// Return the distance to the [first] intersecting point
	return (v - sqrtf(d));
}
// ----------------------------------------------------------------------
// Name  : CheckPointInSphere()
// Input : point - point we wish to check for inclusion
//         sO - Origin of sphere
//         sR - radius of sphere 
// Notes : 
// Return: TRUE if point is in sphere, FALSE if not.
// -----------------------------------------------------------------------  
IC BOOL CheckPointInSphere(const Fvector& point, const Fvector& sO, float sR) 
{
	return (sO.distance_to_sqr(point)< sR*sR);
}
IC BOOL CheckPointInIdentitySphere(const Fvector& point) 
{ 
	return (point.square_magnitude() <= 1);
}

// -----------------------------------------------------------------------  
void CObjectSpace::clCheckCollision(SCollisionData& cl) 
{
	// from package
	Fvector			source;
	Fvector			velocity;
	source.set		(cl.vSourcePoint);
	velocity.set	(cl.vVelocity);
	
	// keep a copy of this as it's needed a few times
	Fvector			normalizedVelocity;
	normalizedVelocity.normalize_safe(cl.vVelocity);
	
	// intersection data
	Fvector			sIPoint;    // sphere intersection point
	Fvector			pIPoint;    // plane intersection point 	
	Fvector			polyIPoint; // polygon intersection point
	
	// how long is our velocity
	float			distanceToTravel = velocity.magnitude();	
	
	float			distToPlaneIntersection;
	float			distToEllipsoidIntersection;
	
	for (DWORD i_t=0; i_t!=clContactedT.size(); i_t++)
	{
		tri& T=clContactedT[i_t];
		
		//ignore backfaces. What we cannot see we cannot collide with ;)
		if (T.N.dotproduct(normalizedVelocity) < 0.0f)
		{
			// calculate sphere intersection point (in future :)
			// OLES: 'cause our radius has unit length, this point lies exactly on sphere
			sIPoint.sub(source, T.N);
			
			// find the plane intersection point
			// classify point to determine if ellipsoid span the plane
			BOOL bInsideTri;
			if ((sIPoint.dotproduct(T.N)+T.d) < -EPS_S) 
			{ 
				// plane is embedded in ellipsoid / sphere
				// find plane intersection point by shooting a ray from the 
				// sphere intersection point along the planes normal.
				bInsideTri = RAPID::TestRayTri2(sIPoint,T.N,T.p,distToPlaneIntersection);
				
				// calculate plane intersection point
				pIPoint.direct(sIPoint,T.N,distToPlaneIntersection);
			}
			else
			{ 
				// shoot ray along the velocity vector
				bInsideTri = RAPID::TestRayTri2(sIPoint,normalizedVelocity,T.p,distToPlaneIntersection);
				
				// calculate plane intersection point
				pIPoint.direct(sIPoint,normalizedVelocity,distToPlaneIntersection);
			}
			
			
			// find polygon intersection point. By default we assume its equal to the 
			// plane intersection point
			
			polyIPoint.set(pIPoint);
			distToEllipsoidIntersection = distToPlaneIntersection;
			
			if (!bInsideTri) 
			{ 
				// if not in triangle
				closestPointOnTriangle(polyIPoint, T, pIPoint);	
				
				Fvector _normalizedVelocity;
				_normalizedVelocity.invert(normalizedVelocity);
				distToEllipsoidIntersection = intersectRaySphere(polyIPoint, _normalizedVelocity, source, 1.0f);  
				
				if (distToEllipsoidIntersection >= 0){
					// calculate true sphere intersection point
					sIPoint.direct(polyIPoint, normalizedVelocity, -distToEllipsoidIntersection);
				}
			} 
			
			// Here we do the error checking to see if we got ourself stuck last frame
			if (CheckPointInSphere(polyIPoint, source, 1.0f)) 
			{
				cl.bStuck = TRUE;
			}
			
			// Ok, now we might update the collision data if we hit something
			if ((distToEllipsoidIntersection >= 0) && (distToEllipsoidIntersection <= distanceToTravel)) 
			{ 
				if ((cl.bFoundCollision == FALSE) || (distToEllipsoidIntersection < cl.fNearestDistance))  
				{
					// if we are hit we have a closest hit so far. We save the information
					cl.fNearestDistance = distToEllipsoidIntersection;
					cl.vNearestIntersectionPoint.set(sIPoint);
					cl.vNearestPolygonIntersectionPoint.set(polyIPoint);
					cl.bFoundCollision = TRUE;
				}
			} 
		} // if not backface
	} // for all faces	
	
	// for all elipsoids
	for (DWORD i_e=0; i_e!=clContactedE.size(); i_e++)
	{
		elipsoid& E = clContactedE[i_e];
		Fvector	Vxform,Pxform;
		E.mW2L.transform_tiny		(Pxform,source);
		E.mW2L.transform_dir		(Vxform,normalizedVelocity); Vxform.normalize	();
		float d = intersectRayIdentitySphere(Pxform,Vxform);
		if (d>0) {
			// Was intersection
			// Get point in Sphere2 local coords
			Fvector IPointS2,IPointS1;
			IPointS2.direct(Pxform,Vxform,d);

			// Calc surface normal and position in S1 space
			Fvector NS2,NS1;
			NS2.set(IPointS2);
			E.mL2W.transform_dir	(NS1,NS2);
			E.mL2W.transform_tiny	(IPointS1,IPointS2);

			// Correct IPointS1 to S1 space
			NS1.normalize();
			IPointS1.sub(NS1);

			// Sphere i-point
			sIPoint.sub(source,NS1);

			// Select nearest i-point
			float dist = sIPoint.distance_to(IPointS1)-1;

			if (dist<distanceToTravel) {
				if ((!cl.bFoundCollision) || (dist < cl.fNearestDistance))  
				{
					// if we are hit we have a closest hit so far. We save the information
					cl.fNearestDistance = dist;
					cl.vNearestIntersectionPoint.add		(sIPoint);
					cl.vNearestPolygonIntersectionPoint.set	(IPointS1);
					cl.bFoundCollision = TRUE;
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------
void CObjectSpace::clResolveStuck(SCollisionData& cl, Fvector& position)
{
	// intersection data
	Fvector			polyIPoint;		// polygon intersection point
	Fvector			stuckDir;
	int				stuckCount;
	
	float			dist;
	float			safe_R = 1.f + EPS_L*2;//psSqueezeVelocity*Device.fTimeDelta;
	
	for (int passes=0; passes<psCollideActStuckDepth; passes++)
	{
		// initialize
		stuckDir.set	(0,0,0);
		stuckCount		= 0;
		
		// for all faces
		for (DWORD i_t=0; i_t!=clContactedT.size(); i_t++)
		{
			tri& T=clContactedT[i_t];
			Fvector N_inv;
			N_inv.invert(T.N);
			
			// find plane intersection point by shooting a ray from the 
			// sphere intersection point along the planes normal.
			if (RAPID::TestRayTri2(position,N_inv,T.p,dist)){
				// calculate plane intersection point
				polyIPoint.direct(position,N_inv,dist);
			}else{
				// calculate plane intersection point
				Fvector tmp;
				tmp.direct(position,N_inv,dist);
				closestPointOnTriangle(polyIPoint, T, tmp);	
			}
			if (CheckPointInSphere(polyIPoint, position, safe_R)) 
			{
				Fvector dir;
				dir.sub(position,polyIPoint);
				float len = dir.magnitude();
				dir.mul( 1.1f*(safe_R-len)/len );
				stuckDir.add(dir);
				stuckCount++;
			}
		}

		// for all elipsoids
		for (DWORD i_e=0; i_e!=clContactedE.size(); i_e++)
		{
			elipsoid& E = clContactedE[i_e];
			Fvector	SRE; 
			E.mW2L.transform_tiny	(SRE,position);
			if (CheckPointInIdentitySphere(SRE)) {
				Fvector dir;
				E.mL2W.transform_dir(dir,SRE);
				float len = dir.magnitude();
				dir.mul( -(safe_R-len)/len );
				stuckDir.add(dir);
				stuckCount++;
			}
		}

		if (stuckCount){
			stuckDir.div(float(stuckCount));
			position.add(stuckDir);
			if (stuckDir.magnitude()<EPS) break;
		} else break;
	}
}

//-----------------------------------------------------------------------------
// Name: collideWithWorld()
// Desc: Recursive part of the collision response. This function is the
//       one who actually calls the collision check on the meshes
//-----------------------------------------------------------------------------
Fvector CObjectSpace::CollideWithWorld(SCollisionData& cl, Fvector position, Fvector velocity,WORD cnt) 
{
	// 
	clResolveStuck(cl,position);

	// do we need to worry ?
	if (velocity.magnitude() <= cl_epsilon){
		cl.vVelocity.set(0,0,0);
		return position;
	}
	if (cnt>psCollideActDepth)				return cl.vLastSafePosition;
	
	Fvector ret_pos;
	Fvector destinationPoint;
	destinationPoint.add(position,velocity);
	
	// reset the collision package we send to the mesh 
	cl.vVelocity.set	(velocity);
	cl.vSourcePoint.set	(position);
	cl.bFoundCollision	= FALSE;
	cl.bStuck			= FALSE;
	cl.fNearestDistance	= -1;	
	
	// Check collision
	clCheckCollision(cl);	
	
	// check return value here, and possibly call recursively 	
	if (cl.bFoundCollision == FALSE  && !cl.bStuck){ 
		// if no collision move very close to the desired destination. 
		float l = velocity.magnitude();
		Fvector V;
		V.mul(velocity,(l-cl_epsilon)/l); 
		
		// return the final position
		ret_pos.add(position,V);
		
		// update the last safe position for future error recovery	
		cl.vLastSafePosition.set(ret_pos);
		return ret_pos;
	}else{ // There was a collision
		// OK, first task is to move close to where we hit something :
		Fvector newSourcePoint;
		
		// If we are stuck, we just back up to last safe position
		if (cl.bStuck){
			return cl.vLastSafePosition;
		}
		// only update if we are not already very close
		if (cl.fNearestDistance >= cl_epsilon) {
			Fvector V;
			V.set(velocity);
			V.set_length(cl.fNearestDistance-cl_epsilon);
			newSourcePoint.add(cl.vSourcePoint,V);
		}else {
			newSourcePoint.set(cl.vSourcePoint);
		}
		
		// Now we must calculate the sliding plane
		Fvector slidePlaneOrigin; slidePlaneOrigin.set(cl.vNearestPolygonIntersectionPoint);
		Fvector slidePlaneNormal; slidePlaneNormal.sub(newSourcePoint,cl.vNearestPolygonIntersectionPoint);
		
		// We now project the destination point onto the sliding plane
		float l = intersectRayPlane(destinationPoint, slidePlaneNormal, 
			slidePlaneOrigin, slidePlaneNormal); 

		// We can now calculate a new destination point on the sliding plane
		Fvector newDestinationPoint;
		newDestinationPoint.direct(destinationPoint,slidePlaneNormal,l);
		
		// Generate the slide vector, which will become our new velocity vector
		// for the next iteration
		Fvector newVelocityVector;
		newVelocityVector.sub(newDestinationPoint, cl.vNearestPolygonIntersectionPoint);
		
		// now we recursively call the function with the new position and velocity 
		cl.vLastSafePosition.set(position);
		return CollideWithWorld(cl, newSourcePoint, newVelocityVector,cnt+1); 
	}
}

void CObjectSpace::cl_Move( const CCFModel *object, const Fmatrix& X, const Fvector& velocity,  const Fbox& aabb, const Fbox& aabb_foots, Fvector& final_pos, Fvector& final_vel, float sq_vel, BOOL bDynamic )
{
	SCollisionData cl_data;

	aabb.getradius(cl_data.vRadius);

	// !!!!!!!!!!! Incorrect!!!!!!!!!!
	Fmatrix		Xinv;
	Xinv.invert	(X);
	Fvector		Vlocal;
	Xinv.transform_dir(Vlocal,velocity);

	Fbox		bb; 
	bb.set		(aabb);
	bb.add		(velocity);
	bb.merge	(aabb);
	bb.merge	(aabb_foots);
	bb.grow		(0.05f);	// гарантировано тестируем на 5 см больше
	// !!!!!!!!!!! Incorrect!!!!!!!!!!

	// Rotate+Translate of ellipsoid (without scale)
	Fmatrix			R,T;
	Fvector			c;
	aabb.getcenter	(c);
	T.translate		(c);
	R.mul_43		(X,T);

	if (bDebug) 
	{
		Fobb			box; 
		box.xform_set	(R);
		box.m_halfsize	= cl_data.vRadius;
		q_debug.AddBox	(box);
	}

	// Collision query
	{
		CCFModel *O			= (CCFModel *)object;
		O->Enable			(false);
		if (bDynamic)		{
			BoxQuery		(bb,X,clGET_TRIS|clGET_BOXES|clQUERY_TOPLEVEL|clQUERY_DYNAMIC);
			if (q_result.boxes.size() || q_result.tris.size())
				BoxQuery	(bb,X,clGET_TRIS|clGET_BOXES|clQUERY_TOPLEVEL|clQUERY_STATIC|clQUERY_DYNAMIC);
		} else {
			BoxQuery		(bb,X,clGET_TRIS|clGET_BOXES|clQUERY_TOPLEVEL|clQUERY_STATIC|clQUERY_DYNAMIC);
		}
		O->EnableRollback	();
	}
	
	// Calc epsilon
	cl_epsilon = sq_vel*Device.fTimeDelta;
	
	// XForm everything to ellipsoid space
	Fmatrix				mXForm,mInvXForm,TS;
	TS.scale			(cl_data.vRadius);
	mXForm.mul_43		(R,TS);		// Create Sphere(0,1) to world ellipsoid xform
	mInvXForm.invert	(mXForm);	// World ellipse to Sphere(0,1)

	Fvector							LPosition, LVelocity, vel_dir;
	mInvXForm.transform_dir			(LVelocity,velocity);
	LPosition.set					(0,0,0);
	cl_data.vLastSafePosition.set	(0,0,0);

	// Get the data for the triangles in question and scale to ellipsoid space
	int tri_count = q_result.tris.size();
	clContactedT.resize		(tri_count);
	if (tri_count) {
		vel_dir.normalize_safe	(LVelocity);
		int	wpos				= 0;
		for (int i_t=0; i_t<tri_count; i_t++){
			tri& T				= clContactedT[wpos];
			clQueryTri&	O		= q_result.tris[i_t];
			mInvXForm.transform_tiny(T.p[0],O.p[0]);
			mInvXForm.transform_tiny(T.p[1],O.p[1]);
			mInvXForm.transform_tiny(T.p[2],O.p[2]);
			T.N.mknormal			(T.p[0],T.p[1],T.p[2]);
			
			if (T.N.dotproduct(vel_dir) < 0.35f)
			{
				T.d = -T.N.dotproduct(T.p[0]);
				T.e10.sub(T.p[1],T.p[0]); T.e10s = T.e10.magnitude(); T.e10.div(T.e10s);
				T.e21.sub(T.p[2],T.p[1]); T.e21s = T.e21.magnitude(); T.e21.div(T.e21s);
				T.e02.sub(T.p[0],T.p[2]); T.e02s = T.e02.magnitude(); T.e02.div(T.e02s);
				wpos++;
			}
		}
		if (wpos != tri_count) 
		{
			clContactedT.resize	(wpos);
		}
	}

	// Convert boxes to elipsoids
	int box_count = q_result.boxes.size();
	clContactedE.resize(box_count);
	dbg_E.clear();
	if (box_count) 
	{
		for (int i=0; i<box_count; i++) {
			elipsoid& E			= clContactedE[i];
			Fobb& B				= q_result.boxes[i];

			Fmatrix				mEllipse,mR,mS;
			B.xform_get			(mR);
			mS.scale			(B.m_halfsize);
			mEllipse.mul		(mR,mS);

			Fmatrix				mEllipseInSSpace;
			mEllipseInSSpace.mul(mInvXForm,mEllipse);
			
			Fmatrix				mScale2;
			mScale2.scale		(1.1f,1.1f,1.1f);
			mEllipseInSSpace.mul(mScale2);


			// 3rd - set up structure
			E.mL2W.set			(mEllipseInSSpace);
			E.mW2L.invert		(mEllipseInSSpace);

			if (bDebug)
			{
				Fmatrix	NE;
				NE.mul			(mXForm,mEllipseInSSpace);
				dbg_E.push_back	(NE);
			}
		}
	}

	// call the recursive collision response function	
	Fvector cxform;
	X.transform_dir			(cxform,c);		// -- offset (oriented)
	Fvector POS = CollideWithWorld(cl_data, LPosition, LVelocity);
	mXForm.transform_tiny	(final_pos,POS);// -- center of the (oriented) box in world space
	final_pos.sub			(cxform);
	mXForm.transform_dir	(final_vel, cl_data.vVelocity);
}

BOOL CObjectSpace::EllipsoidCollide( CCFModel *object, const Fmatrix& T, const Fvector& center_pos, const Fbox& bounding_box ){
	Fvector vRadius;
	bounding_box.getradius(vRadius);

	object->Enable			(false);
	BoxQuery				(bounding_box,T,
		clGET_TRIS|clGET_BOXES|clQUERY_TOPLEVEL|clQUERY_STATIC|clQUERY_DYNAMIC);
	object->EnableRollback	();

	Fvector scaledPosition;	 
	Fvector inv_radius;
	inv_radius.set(1,1,1);
	inv_radius.div(vRadius);

	// the first thing we do is scale the player and his velocity to
	// ellipsoid space
	scaledPosition.mul(center_pos,inv_radius);
	
	Fvector p[3], e0,e1;

	// Get the data for the triangles in question and scale to ellipsoid space
	int tri_count = q_result.tris.size();
	for (int i_t=0; i_t<tri_count; i_t++){
		clQueryTri&	O = q_result.tris[i_t];
		
		p[0].mul(O.p[0],inv_radius);
		p[1].mul(O.p[1],inv_radius);
		p[2].mul(O.p[2],inv_radius);
		e0.sub(p[1],p[0]);
		e1.sub(p[2],p[0]);
		if (RAPID::TestSphereTri(scaledPosition, 1.f-EPS_L, p[0], e0, e1)) return TRUE;
	}
	
	return FALSE;
}

//-----------------------------------------------------------------------------
/*
#ifdef _DEBUG
	Fvector box_center, box_radius;
	bb.getcenter	(box_center);
	bb.getradius	(box_radius);
	object->mTransform.transform(box_center);
	debug_last_bb_transform.set	(object->mTransform);
	debug_last_bb_transform.c.set	(box_center);
	debug_last_bb_size.set		(box_radius);
#endif

	__int64 qwStartTime,qwEndTime;
	QueryPerformanceCounter((LARGE_INTEGER *)&qwStartTime);

	QueryPerformanceCounter((LARGE_INTEGER *)&qwEndTime);
	double	diff	= double(qwEndTime-qwStartTime)/double(tri_count);
	Log("CyclesM: ",float(diff));
*/