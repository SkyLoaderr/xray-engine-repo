#ifndef _PLANE
#define _PLANE

class Fplane {
public:
	union {
		struct{
			float	a,b,c;
		};
		_vector		n;
	};
	float	d;
public:
	__forceinline	void	build(_vector &v1, _vector &v2, _vector &v3) {
		_vector t1,t2;
		t1.sub(v1,v2);
		t2.sub(v1,v3);
		n.crossproduct(t1,t2);
		n.normalize   ();
		d = - n.dotproduct(v1);
	}
	__forceinline	void	build(_vector &_p, _vector &_n)
	{
		n.normalize(_n);
		d = - n.dotproduct(_p);
	}
	__forceinline	void	project(_vector &pdest, _vector &psrc)
	{
		float dist = classify(psrc);
		pdest.direct(psrc,n,-dist);
	}
	__forceinline	float	classify(_vector &v)	{
		return n.dotproduct(v)+d;
	}

	__forceinline	float	distance(_vector &v)	{
		return fabsf(classify(v));
	}

	__forceinline	BOOL	intersect(
		_vector& u, _vector& v,				// segment
	    _vector&	isect)                  // intersection point
	{
		float		denom,dist;
		_vector		t;

		t.sub(v,u);
		denom = n.dotproduct(t);
		if (fabsf(denom) < EPS) return false; // they are parallel

		dist = -(n.dotproduct(u) + d) / denom;
		if (dist < -EPS || dist > 1+EPS) return false;
		isect.direct(u,t,dist);
		return true;
	}

	__forceinline	BOOL	intersect_2 (
		_vector& u, _vector& v,				// segment
	    _vector& isect)						// intersection point
	{
		float		dist1, dist2;
		_vector		t;

		dist1		= n.dotproduct(u)+d;
		dist2		= n.dotproduct(v)+d;

		if (dist1*dist2<0.0f)
			return false;

		t.sub		(v,u);
		isect.direct(u,t,dist1/fabsf(dist1-dist2));

		return true;
	}
};

#endif
