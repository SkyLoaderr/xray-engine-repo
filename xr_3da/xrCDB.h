
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
		DWORD	ray_mode;
		DWORD	box_mode;
		DWORD	frustum_mode;
	public:
		CDB		();
		~CDB	();
		
		void	ray_mode		(DWORD f)	{	ray_mode = f;		}
		void	ray_query		(const Fmatrix* parent, const Model *o, const Fvector& start,  const Fvector& dir, float max_range = 10000.f);
		
		void	box_mode		(DWORD f)	{	box_mode = f;		}
		
		
		void	frustum_mode	(DWORD f)	{	frustum_mode = f;	}
	};
};

