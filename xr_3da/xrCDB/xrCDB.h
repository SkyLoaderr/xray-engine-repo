
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XRCDB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XRCDB_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef XRCDB_EXPORTS
#define XRCDB_API __declspec(dllexport)
#else
#define XRCDB_API __declspec(dllimport)
#endif

// forward declarations
class CFrustum;

namespace CDB
{
	enum {
		RAY_CULL		= (1<<0),
		RAY_ONLYFIRST	= (1<<1),
		RAY_ONLYNEAREST	= (1<<2)
	};
	
	class XRCDB_API MODEL;

	class XRCDB_API COLLIDER 
	{
		DWORD			ray_mode;
		DWORD			box_mode;
		DWORD			frustum_mode;

		// Result management
		int*			rd_ptr;
		int				rd_count;
		int				rd_size;
	public:
		COLLIDER		();
		~COLLIDER		();
		
		void			ray_options		(DWORD f)	{	ray_mode = f;		}
		void			ray_query		(const MODEL *m_def, const Fvector& r_start,  const Fvector& r_dir, float r_range = 10000.f);
		
		void			box_options		(DWORD f)	{	box_mode = f;		}
		void			box_query		(const MODEL *m_def, const Fvector& b_center, const Fvector& b_dim);
		
		void			frustum_options	(DWORD f)	{	frustum_mode = f;	}
		void			frustum_query	(const MODEL *m_def, const CFrustum& F);
		
		void			r_add			(int id);
		int*			r_begin			()	{	return rd_ptr;				};
		int*			r_end			()	{	return rd_ptr + rd_count;	};
		int				r_count			()	{	return rd_count;			};
		void			r_clear			()	{	rd_count = 0;				};
		void			r_free			();
};
