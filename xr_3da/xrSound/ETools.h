#ifndef EToolsH
#define EToolsH

#ifdef ETOOLS_EXPORTS
#define ETOOLS_API __declspec( dllexport )
#else
#define ETOOLS_API __declspec( dllimport )
#endif

#include "xrCDB.h"

extern "C" {
	// fast functions
	namespace ETOOLS{
		ETOOLS_API bool 			TestRayTriA		(const Fvector& C, const Fvector& D, Fvector** p, float& u, float& v, float& range, bool bCull);
		ETOOLS_API bool 			TestRayTriB		(const Fvector& C, const Fvector& D, Fvector* p, float& u, float& v, float& range, bool bCull);
		ETOOLS_API bool 			TestRayTri2		(const Fvector& C, const Fvector& D, Fvector* p, float& range);
		
		typedef  void __stdcall		pb_callback		(void* user_data, float& val);
		ETOOLS_API void				SimplifyCubeMap	(u32* src_data, u32 src_width, u32 src_height, u32* dst_data, u32 dst_width, u32 dst_height, float sample_factor=1.f, pb_callback cb=0, void* pb_data=0);

		ETOOLS_API CDB::COLLIDER*	get_collider	();
		ETOOLS_API CDB::MODEL*		create_model	(Fvector* V, int Vcnt, CDB::TRI* T, int Tcnt);
		ETOOLS_API void				destroy_model	(CDB::MODEL*&);
		ETOOLS_API CDB::RESULT*		r_begin			();
		ETOOLS_API CDB::RESULT*		r_end			(); 
		ETOOLS_API int				r_count			();
		ETOOLS_API void 			ray_options		(u32 flags);
		ETOOLS_API void				ray_query		(const CDB::MODEL *m_def, const Fvector& r_start,  const Fvector& r_dir, float r_range);
		ETOOLS_API void				ray_query_m		(const Fmatrix& inv_parent, const CDB::MODEL *m_def, const Fvector& r_start,  const Fvector& r_dir, float r_range);
		ETOOLS_API void 			box_options		(u32 flags);
		ETOOLS_API void				box_query		(const CDB::MODEL *m_def, const Fvector& b_center, const Fvector& b_dim);
		ETOOLS_API void				box_query_m		(const Fmatrix& inv_parent, const CDB::MODEL *m_def, const Fbox& src);

		ETOOLS_API int				ogg_enc			(const char* in_fn, const char* out_fn, float quality, void* comment, int comment_size);
	};

	#include <vorbis/vorbisfile.h>
/*
	ETOOLS_API		int				ov_clear		(OggVorbis_File *vf);
	ETOOLS_API		vorbis_info*	ov_info			(OggVorbis_File *vf,int link);
	ETOOLS_API		vorbis_comment*	ov_comment		(OggVorbis_File *vf,int link);
	ETOOLS_API		int				ov_clear		(OggVorbis_File *vf);
	ETOOLS_API		vorbis_info*	ov_info			(OggVorbis_File *vf,int link);
	ETOOLS_API		vorbis_comment*	ov_comment		(OggVorbis_File *vf,int link);
	ETOOLS_API		long			ov_read_float	(OggVorbis_File *vf,float ***pcm_channels,int samples, int *bitstream);
	ETOOLS_API		ogg_int64_t		ov_pcm_tell		(OggVorbis_File *vf);
	ETOOLS_API		int				ov_pcm_seek		(OggVorbis_File *vf,ogg_int64_t pos);
	ETOOLS_API		int				ov_open_callbacks(void *datasource, OggVorbis_File *vf, char *initial, long ibytes, ov_callbacks callbacks);
	ETOOLS_API		ogg_int64_t		ov_pcm_total	(OggVorbis_File *vf,int i);
*/
};

#endif // EToolsH