#ifndef EToolsH
#define EToolsH

#ifdef ETOOLS_EXPORTS
#define ETOOLS_API __declspec( dllexport )
#else
#define ETOOLS_API __declspec( dllimport )
#endif

#include "xrXRC.h"

extern "C" {
	// fast functions
	namespace ETOOLS{
		ETOOLS_API bool TestRayTriA		(const Fvector& C, const Fvector& D, Fvector** p, float& u, float& v, float& range, bool bCull);
		ETOOLS_API bool TestRayTriB		(const Fvector& C, const Fvector& D, Fvector* p, float& u, float& v, float& range, bool bCull);
		ETOOLS_API bool TestRayTri2		(const Fvector& C, const Fvector& D, Fvector* p, float& range);
		
		typedef  void __stdcall			pb_callback(void* user_data, float& val);
		ETOOLS_API void SimplifyCubeMap	(u32* src_data, u32 src_width, u32 src_height, u32* dst_data, u32 dst_width, u32 dst_height, float sample_factor=1.f, pb_callback cb=0, void* pb_data=0);

		ETOOLS_API CDB::RESULT*	r_begin	();
		ETOOLS_API CDB::RESULT*	r_end	(); 
		ETOOLS_API int	r_count			();
		ETOOLS_API void ray_options		(u32 flags);
		ETOOLS_API void	ray_query		(const Fmatrix& inv_parent, const CDB::MODEL *m_def, const Fvector& r_start,  const Fvector& r_dir, float r_range);
	};
};

#endif // EToolsH