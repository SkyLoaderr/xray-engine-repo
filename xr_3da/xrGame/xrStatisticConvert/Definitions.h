typedef	signed		int		s32;
typedef	unsigned	int		u32;
typedef	signed		short	s16;
typedef	unsigned	short	u16;

#define IC __forceinline

extern void DbgOut( const char *format, ... );
extern void ConvOut(FILE* pFile, const char *format, ... );
extern	void	ReadName(char* Name, FILE* pFile);

#define WUS_IDENT	(('S'<<24)+('U'<<16)+('W'<<8)+' ')
#define WUS_VERSION	2


typedef	char	string1024	[1024];
typedef float	Fvector		[3];

IC float	Magnitude (Fvector v0, Fvector v1)
{
	Fvector vr;
	
	vr[0] = v1[0] - v0[0];
	vr[1] = v1[1] - v0[1];
	vr[2] = v1[2] - v0[2];

	return sqrt(vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2]);
}


//---------------------------------------------------------------------------
#ifdef DEBUG
#define	NODEFAULT Debug.fatal("nodefault: reached")
#define VERIFY(expr) if (!(expr)) ::Debug.fail(#expr,__FILE__, __LINE__)
#define VERIFY2(expr, e2) if (!(expr)) ::Debug.fail(#expr,e2,__FILE__, __LINE__)
#define VERIFY3(expr, e2, e3) if (!(expr)) ::Debug.fail(#expr,e2,e3,__FILE__, __LINE__)
#define CHK_DX(expr) { HRESULT hr = expr; if (FAILED(hr)) ::Debug.error(hr,#expr,__FILE__, __LINE__); }
#else
#ifdef __BORLANDC__
#define NODEFAULT
#else
#define NODEFAULT __assume(0)
#endif
#define VERIFY(expr)
#define VERIFY2(expr, e2)
#define VERIFY3(expr, e2, e3)
#define CHK_DX(a) a
#endif
//---------------------------------------------------------------------------
template <class T>
class xr_vector	: public std::vector<T> {
public: 
	typedef	size_t		size_type;
	typedef T&			reference;
	typedef const T&	const_reference;
public: 
	xr_vector			()								: std::vector<T>	()				{}
	xr_vector			(size_t _count, const T& _value): std::vector<T>	(_count,_value)	{}
	explicit xr_vector			(size_t _count)					: std::vector<T> 	(_count)		{}
	void	clear				()								{ erase(begin(),end());				} 
	void	clear_and_free		()								{ std::vector<T>::clear();			}
	void	clear_not_free()									{ erase(begin(),end());	}
	const_reference operator[]	(size_type _Pos) const			{ {VERIFY(_Pos<size());} return (*(begin() + _Pos)); }
	reference operator[]		(size_type _Pos)				{ {VERIFY(_Pos<size());} return (*(begin() + _Pos)); }
};

#define DEF_VECTOR(N,T)				typedef xr_vector< T > N;		typedef N::iterator N##_it;