#ifndef _F_SPHERE_H_
#define _F_SPHERE_H_

template <class T>
struct _sphere {
	_vector<T>	P;
	T			R;
public:
	IC void		set(const _vector<T> &_P, T _R)		{ P.set(_P); R = _R; }
	IC void		set(const _sphere<T> &S)			{ P.set(S.P); R=S.R; }
	IC void		identity()							{ P.set(0,0,0); R=1; }

	// Ray-sphere intersection
	IC BOOL		intersect(const _vector<T>& S, const _vector<T>& D, T& range)	
    {
		_vector<T> Q;	Q.sub(P,S);
	
		T c = Q.magnitude	();
		T v = Q.dotproduct	(D);
		T d = R*R - (c*c - v*v);
		if ((d>0) && (d<range)){ range = d; return TRUE;}else return FALSE;
    }
	IC BOOL		intersect(const _vector<T>& S, const _vector<T>& D)	
	{
		_vector<T> Q;	Q.sub(P,S);
	
		T c = Q.magnitude	();
		T v = Q.dotproduct	(D);
		T d = R*R - (c*c - v*v);
		return (d > 0);
	}
	IC BOOL		intersect(const _sphere<T>& S) const
	{	
		T SumR = R+S.R;
		return P.distance_to_sqr(S.P) < SumR*SumR;
	}
	IC BOOL		contains(const _vector<T>& PT) const 
	{
		return P.distance_to_sqr(PT) <= (R*R+EPS_S);
	}
	
	// returns true if this wholly contains the argument sphere
	IC BOOL		contains(const _sphere<T>& S) const	
	{
		// can't contain a sphere that's bigger than me !
		const T RDiff		= R - S.R;
		if ( RDiff < 0 )	return false;

		return ( P.distance_to_sqr(S.P) <= RDiff*RDiff );
	}

	// return's volume of sphere
	IC T		volume	() const
	{
		return T( PI_MUL_4 / 3 ) * (R*R*R);
	}
	void		compute_fast	(const _vector<T> *verts, int count);
};

typedef _sphere<float>	Fsphere;
typedef _sphere<double> Dsphere;

void	XRCORE_API		Fsphere_compute		(Fsphere& dest, const Fvector *verts, int count);

#endif