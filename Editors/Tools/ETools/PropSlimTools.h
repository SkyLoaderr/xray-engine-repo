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
		void	set				(const Fvector3& p, const Fvector2& t){pt.set(p);uv.set(t);}
	};

	struct QSFace{
		u32		v[3];
		u32		tag;
		void	set				(u32 v0, u32 v1, u32 v2){v[0]=v0;v[1]=v1;v[2]=v2;tag=0;}
		int		remap_vertex	(u32 from, u32 to)
		{
			int nmapped = 0;
			for(int i=0; i<3; i++){
				if( v[i]==from ){
					v[i]=to;
					nmapped++;
				}
			}
			return nmapped;
		}
	};

	struct QSMesh{
		qs_vector<QSVert>	verts;
		qs_vector<QSFace>	faces;
		QSMesh				(u32 v_cnt, u32 f_cnt):verts(v_cnt),faces(f_cnt) {}
		~QSMesh				() {}
	};

	struct QSContractionItem{
		u32					v_dead;
		u32					v_kept;
		float				error;
		qs_vector<u32>		f_dead;
		qs_vector<u32>		f_delta;
		QSContractionItem	(u32 dead_cnt, u32 delta_cnt):f_dead(dead_cnt),f_delta(delta_cnt)	{}
		~QSContractionItem	(){}
	};

	struct QSContraction{
		qs_vector<QSContractionItem*> items;
		QSContraction		(u32 init_reserved){items.reserve(init_reserved);}
		~QSContraction		()
		{
			for (qs_vector<QSContractionItem*>::iterator it=items.begin(); it!=items.end(); it++)
				xr_delete	(*it);
		}
		void	AppendItem	(QSContractionItem* item)
		{
			items.push_back	(item);
		}
	};
};

extern "C" {
	namespace ETOOLS{
		ETOOLS_API BOOL ContractionGenerate	(QSMesh* src_mesh, QSContraction*& dst_conx, u32 min_faces, float max_error);
		ETOOLS_API void ContractionClear	(QSContraction*& dst_conx);
	};
};

#endif // PropSlimToolsH