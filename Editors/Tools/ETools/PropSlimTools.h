#ifndef PropSlimToolsH
#define PropSlimToolsH

#ifdef ETOOLS_EXPORTS
#define ETOOLS_API __declspec( dllexport )
#else
#define ETOOLS_API __declspec( dllimport )
#endif

namespace ETOOLS{
	template <class T>
	class qs_vector 
	{
	public:
		typedef	size_t				size_type;
		typedef T					value_type;
		typedef value_type*			iterator;
		typedef const value_type*	const_iterator;
		typedef value_type&			reference;
		typedef const value_type&	const_reference;

	private:
		value_type*	array;
		u32			count;
		u32			dim;
	public:
							qs_vector	(): count(0), dim(0), array(0){reserve(8);}
							qs_vector	(u32 sz) : count(0), dim(0), array(0){resize(sz);}
		IC iterator			begin		()				{ return array;							}
		IC iterator			end			()				{ return array+count;					}
		IC const_iterator	begin		()	const		{ return array;							}
		IC const_iterator	end			()	const		{ return array+count;					}
		IC u32				size		()	const		{ return count;							}
		IC void				clear		()				{ count = 0;							}
		IC void				resize		(u32 c)			{ if (c>=dim) reserve(c); count=c;		}
		IC void				reserve		(u32 c)			{ dim=c; array=(value_type*)xr_realloc(array,dim*sizeof(value_type)); }

		IC void				push_back	(value_type e)	{ if (count>=dim) reserve(dim*2); array[count++]=e;	}

		IC reference		operator[]	(u32 id)		{ VERIFY(id<count); return array[id];	}
		IC const_reference	operator[]	(u32 id) const	{ VERIFY(id<count); return array[id];	}

		IC bool				empty		() const		{ return 0==count;	}

		IC void				erase		(u32 id)		{
			VERIFY(id<count);
			count--;
			for (u32 i=id; i<count; i++)
				array[i] = array[i+1];
		}
		IC void				erase		(iterator it)	{ erase(u32(it-begin()));	}
	};

	struct QSVert{
		Fvector3 pt;
		Fvector2 uv;
		u32		old_index;
		void	set				(const Fvector3& p, const Fvector2& t){pt.set(p);uv.set(t);}
	};

	struct QSFace{
		u32		v[3];
		u32		tag;
		void	set				(u32 v0, u32 v1, u32 v2){v[0]=v0;v[1]=v1;v[2]=v2;tag=0;}
	};

	struct QSMesh{
		ArbitraryList<QSVert>				verts;
		ArbitraryList<QSFace>				faces;
		ArbitraryList<SlidingWindowRecord>	swrRecords;			// The records of the collapses.
		QSMesh				(u32 v_cnt, u32 f_cnt):verts(v_cnt),faces(f_cnt) {}
		~QSMesh				() {}
	};
};

extern "C" {
	namespace ETOOLS{
		ETOOLS_API BOOL ContractionGenerate	(QSMesh* src_mesh, QSMesh*& new_mesh, float error_tolerance);
		ETOOLS_API void ContractionClear	(QSContraction*& dst_conx);
	};
};

#endif // PropSlimToolsH