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
	IC bool		intersect(const _vector<T>& S, const _vector<T>& D)	
	{
		_vector<T> Q;	Q.sub(P,S);
	
		T c = Q.magnitude	();
		T v = Q.dotproduct	(D);
		T d = R*R - (c*c - v*v);
		return (d > 0);
	}
	IC bool		intersect(const _sphere<T>& S) const
	{	
		T SumR = R+S.R;
		return P.distance_to_sqr(S.P) < SumR*SumR;
	}
	IC bool		contains(const _vector<T>& PT) const 
	{
		return P.distance_to_sqr(PT) <= (R*R+EPS_S);
	}
	
	// returns true if this wholly contains the argument sphere
	IC bool		contains(const _sphere<T>& S) const	
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
	void		compute			(const _vector<T> *verts, int count);
	void		compute_fast	(const _vector<T> *verts, int count);
};

namespace
{
	template <class T> class Miniball;
	template <class T> class Basis;
	
	// Basis
	// -----
	template <class T>
	class Basis 
	{
		typedef _vector<T>	vec3D;
    private:
		enum { d = 3 }		eDimensions;
		
        // data members
        int					m, s;			// size and number of support vectors
        
		vec3D				q0;
		
        T					z[d+1];
        T					f[d+1];
        vec3D				v[d+1];
        vec3D				a[d+1];
        vec3D				c[d+1];
        T					sqr_r[d+1];
		
        vec3D*				current_c;      // vectors to some c[j]
        T					current_sqr_r;
		
        T					sqr (T r) const {return r*r;}
    public:
        Basis();
		
        // access
        const vec3D*		center			() const;
        T					squared_radius	() const;
        int                 size			() const;
        int                 support_size	() const;
        T					excess			(const vec3D& p) const;
		
        // modification
        void                reset			(); // generates empty sphere with m=s=0
        bool                push			(const vec3D& p);
        void                pop				();
	};
	
	// Miniball
	// --------
	template <class T>
	class Miniball
	{
		typedef _vector<T>	vec3D;
    public:
        // types
		// use my alloactor so that it doesn't leak forever !
		typedef std::list<vec3D>			VectorList;
		
        typedef VectorList::iterator        It;
        typedef VectorList::const_iterator  Cit;
		
    private:
        // data members
        VectorList	L;						// STL list keeping the gVectors
        Basis<T>    B;						// basis keeping the current ball
        It          support_end;			// past-the-end iterator of support set
		
        // private methods
        void		mtf_mb					(It k);
        void		pivot_mb				(It k);
        void		move_to_front			(It j);
        T			max_excess				(It t, It i, It& pivot) const;
        T			abs						(T r) const {return (r>0)? r: (-r);}
        T			sqr						(T r) const {return r*r;}
    public:
        // construction
        Miniball() {}
        void        check_in				(const vec3D& p);
        void        build					();
		
        // access
        vec3D      center					() const;
        T			squared_radius			() const;
        int         num_points				() const;
        Cit         points_begin			() const;
        Cit         points_end				() const;
        int         nr_support_gVectors		() const;
        Cit         support_points_begin	() const;
        Cit         support_points_end		() const;
	};
    
	// Miniball
	// --------
	
	template <class T>
	void Miniball<T>::check_in (const vec3D& p)
	{
		L.push_back(p);
	}
	
	template <class T>
	void Miniball<T>::build ()
	{
		B.reset();
		support_end = L.begin();
		
		// @@ pivotting or not ?
		if ( 1 )
			pivot_mb	(L.end());
		else
			mtf_mb		(L.end());
	}
	
	template <class T>
	void Miniball<T>::mtf_mb (It i)
	{
		support_end = L.begin();
		
		if ((B.size())== 4) 
			return;
		
		for (It k=L.begin(); k!=i;) 
		{
			It j=k++;
			if (B.excess(*j) > 0) 
			{
				if (B.push(*j)) 
				{
					mtf_mb (j);
					B.pop();
					move_to_front(j);
				}
			}
		}
	}
	
	template <class T>
	void Miniball<T>::move_to_front (It j)
	{
		if (support_end == j)
			support_end++;
		L.splice (L.begin(), L, j);
	}
	
	
	template <class T>
	void Miniball<T>::pivot_mb (It i)
	{
		It t = ++L.begin();
		mtf_mb (t);
		T max_e =0, old_sqr_r = 0;
		do 
		{
			It pivot = L.begin();
			max_e = max_excess (t, i, pivot);
			if (max_e > 0) 
			{
				t = support_end;
				if (t==pivot) ++t;
				old_sqr_r = B.squared_radius();
				B.push (*pivot);
				mtf_mb (support_end);
				B.pop();
				move_to_front (pivot);
			}
		} while ((max_e > 0) && (B.squared_radius() > old_sqr_r));
	}
	
	template <class T>
	T Miniball<T>::max_excess (It t, It i, It& pivot) const
	{
		const	vec3D* pCenter = B.center();
		T				sqr_r	= B.squared_radius();
		
		T e, max_e = 0;
		
		for (It k=t; k!=i; ++k)
		{
			const vec3D & point = (*k);
			e = -sqr_r;
			
			e += point.distance_to_sqr(*pCenter);
			
			if (e > max_e)
			{
				max_e = e;
				pivot = k;
			}
		}
		
		return max_e;
	}
	
	template <class T>
	Miniball<T>::vec3D Miniball<T>::center () const
	{
		return *((vec3D *)B.center());
	}
	
	template <class T>
	T Miniball<T>::squared_radius () const
	{
		return B.squared_radius();
	}
	
	template <class T>
	int Miniball<T>::num_points () const
	{
		return L.size();
	}
	
	template <class T>
	Miniball<T>::Cit Miniball<T>::points_begin () const
	{
		return L.begin();
	}
	
	template <class T>
	Miniball<T>::Cit Miniball<T>::points_end () const
	{
		return L.end();
	}
	
	template <class T>
	int Miniball<T>::nr_support_gVectors () const
	{
		return B.support_size();
	}
	
	template <class T>
	Miniball<T>::Cit Miniball<T>::support_points_begin () const
	{
		return L.begin();
	}
	
	template <class T>
	Miniball<T>::Cit Miniball<T>::support_points_end () const
	{
		return support_end;
	}
	
	
	//----------------------------------------------------------------------
	// Basis
	//---------------------------------------------------------------------
	template <class T>
	const Basis<T>::vec3D* Basis<T>::center () const
	{
		return current_c;
	}
	
	template <class T>
	T Basis<T>::squared_radius() const
	{
		return current_sqr_r;
	}
	
	template <class T>
	int Basis<T>::size() const
	{
		return m;
	}
	
	template <class T>
	int Basis<T>::support_size() const
	{
		return s;
	}
	
	template <class T>
	T Basis<T>::excess (const vec3D& p) const
	{
		T e = -current_sqr_r;
		e += p.distance_to_sqr(*current_c);
		return e;
	}
	
	template <class T>
	void Basis<T>::reset ()
	{
		m = s = 0;
		// we misuse c[0] for the center of the empty sphere
		c[0].set(0,0,0);
		current_c = c;
		current_sqr_r = -1;
	}
	
	
	template <class T>
	Basis<T>::Basis ()
	{
		reset();
	}
	
	
	template <class T>
	void Basis<T>::pop ()
	{
		--m;
	}
	
	
	template <class T>
	bool Basis<T>::push (const vec3D& p)
	{
		if (m==0)
		{
			q0 = p;
			c[0] = q0;
			sqr_r[0] = 0;
		}
		else
		{
			int i;
			const T eps = 1e-16f;
			
			// set v_m to Q_m
			v[m].sub(p,q0);
			
			// compute the a_{m,i}, i< m
			for (i=1; i<m; ++i)
			{
				a[m][i] =  v[i].dotproduct(v[m]);
				a[m][i] *= (2.f/z[i]);
			}
			
			// update v_m to Q_m-\bar{Q}_m
			for (i=1; i<m; ++i)
			{
				v[m].direct(v[m],v[i],-a[m][i]);
			}
			
			// compute z_m
			z[m]=0;
			z[m] += v[m].square_magnitude();
			z[m]*=2;
			
			// reject push if z_m too small
			if (z[m]<eps*current_sqr_r)
			{
				return false;
			}
			
			// update c, sqr_r
			T e = -sqr_r[m-1];
			e += p.distance_to_sqr(c[m-1]);
			
			f[m]=e/z[m];
			
			c[m].direct(c[m-1],v[m],f[m]);
			
			sqr_r[m] = sqr_r[m-1] + e*f[m]/2;
		}
		
		current_c = c+m;
		current_sqr_r = sqr_r[m];
		s = ++m;
		return true;
	}
	
	
}; // nameless namespace

template <class T>
void _sphere<T>::compute(const _vector<T> *verts, int count)
{
		Miniball<T> mb;
		
		for(int i=0;i<count;i++)
			mb.check_in(verts[i]);
		
		mb.build	();
		
		P.set		(mb.center());
		R =			( sqrtf( mb.squared_radius() ));
}

typedef _sphere<float>	Fsphere;
typedef _sphere<double> Dsphere;


#endif