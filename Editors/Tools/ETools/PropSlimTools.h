#ifndef PropSlimToolsH
#define PropSlimToolsH

#ifdef ETOOLS_EXPORTS
#define ETOOLS_API __declspec( dllexport )
#else
#define ETOOLS_API __declspec( dllimport )
#endif

namespace ETOOLS{
	struct QSVert{
		Fvector3 pt;
		Fvector2 uv;
	};

	struct QSFace{
		u32 v[3];
	};

	struct QSMesh{
		QSVert*	verts;
		u32		verts_cnt;
		QSFace*	faces;
		u32		faces_cnt;
				QSMesh(u32 v_cnt, u32 f_cnt)
				{
					verts	= xr_alloc<QSVert>(v_cnt);	verts_cnt = v_cnt;
					faces	= xr_alloc<QSFace>(f_cnt);	faces_cnt = f_cnt;
				}
				~QSMesh()
				{
					xr_free	(verts);
					xr_free	(faces);
				}
	};

	struct QSContractionItem{
		u32		v_binned;
		u32		v_kept;
		float	error;
		QSFace*	f_binned;
		u32		f_binned_cnt;
		QSFace*	f_delta;
		u32		f_delta_cnt;
				QSContractionItem(u32 b_cnt, u32 d_cnt)
				{
					f_binned= xr_alloc<QSFace>(b_cnt);	f_binned_cnt = b_cnt;
					f_delta	= xr_alloc<QSFace>(d_cnt);	f_delta_cnt	 = d_cnt;
				}
				~QSContractionItem()
				{
					xr_free	(f_binned);
					xr_free	(f_delta);
				}
	};

	struct QSContraction{
		QSContractionItem**	items;
		u32					item_cnt;
	};
};

extern "C" {
	namespace ETOOLS{
		ETOOLS_API BOOL ContractionGenerate	(QSMesh* src_mesh, QSContraction* dst_conx, u32 min_faces, float max_error);
		ETOOLS_API void ContractionClear	(QSContraction* dst_conx);
	};
};

#endif // PropSlimToolsH