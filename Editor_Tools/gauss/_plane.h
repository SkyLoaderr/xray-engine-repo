#ifndef _PLANE
#define _PLANE

class Fplane {
public:
	Fvector		n;
	float		d;
public:
	IC	void	set		(Fplane &P)
	{
		n.set(P.n);
		d = P.d;
	}
    IC bool 	similar (Fplane &P, float eps_n=EPS, float eps_d=EPS){
    	return (n.similar(P.n,eps_n)&&(fabsf(d-P.d)<eps_d));
    }
	IC	void	build	(const Fvector &v1, const Fvector &v2, const Fvector &v3) {
		Fvector t1,t2;
		t1.sub(v1,v2);
		t2.sub(v1,v3);
		n.crossproduct(t1,t2);
		n.normalize   ();
		d = -n.dotproduct(v1);
	}
	IC	void	build(const Fvector &_p, const Fvector &_n)
	{
		n.normalize(_n);
		d = - n.dotproduct(_p);
	}
	IC	void	build_unit_normal(const Fvector &_p, const Fvector &_n)
	{
		VERIFY(fsimilar(_n.magnitude(),1,EPS));
		n.set(_n);
		d = - n.dotproduct(_p);
	}
	IC	void	project(Fvector &pdest, Fvector &psrc)
	{
		float dist = classify(psrc);
		pdest.direct(psrc,n,-dist);
	}
	IC	float	classify(const Fvector &v) const	{
		return n.dotproduct(v)+d;
	}
	IC	void	normalize() {
		float denom = 1.f / n.magnitude();
		n.mul(denom);
		d*=denom;
	}
	IC	float	classify_inv(const Fvector &v) {
		return -classify(v);
	}

	IC	float	distance(const Fvector &v)	{
		return fabsf(classify(v));
	}

	IC bool intersectRayDist(const Fvector& P, const Fvector& D, float& dist)
	{
		float numer = classify(P);
		float denom = n.dotproduct(D);
	
		if (fabsf(denom)<EPS_S)  // normal is orthogonal to vector, cant intersect
			return false;
	
		dist = -(numer / denom);
		return true;
	}
	IC bool intersectRayPoint(const Fvector& P, const Fvector& D, Fvector& dest) {
		float numer = classify(P);
		float denom = n.dotproduct(D);
	
		if (fabsf(denom)<EPS_S) return false; // normal is orthogonal to vector, cant intersect
		else {
			dest.direct(P,D,-(numer / denom));
			return true;
		}
	}

	IC	BOOL	intersect (
		const Fvector& u, const Fvector& v,	// segment
	    Fvector&	isect)                  // intersection point
	{
		float		denom,dist;
		Fvector		t;

		t.sub(v,u);
		denom = n.dotproduct(t);
		if (fabsf(denom) < EPS) return false; // they are parallel

		dist = -(n.dotproduct(u) + d) / denom;
		if (dist < -EPS || dist > 1+EPS) return false;
		isect.direct(u,t,dist);
		return true;
	}

	IC	BOOL	intersect_2 (
		const Fvector& u, const Fvector& v,				// segment
	    Fvector& isect)						// intersection point
	{
		float		dist1, dist2;
		Fvector		t;

		dist1		= n.dotproduct(u)+d;
		dist2		= n.dotproduct(v)+d;

		if (dist1*dist2<0.0f)
			return false;

		t.sub		(v,u);
		isect.direct(u,t,dist1/fabsf(dist1-dist2));

		return true;
	}
	IC	void	transform(Fmatrix& M)
	{
		// rotate the normal
		M.transform_dir(n);
		// slide the offset
		d -= M.c.dotproduct(n);
	}
};

#endif
