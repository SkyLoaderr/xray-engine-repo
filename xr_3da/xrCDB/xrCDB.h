
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

// This class is exported from the xrCDB.dll
class XRCDB_API CDB 
{
public:
	CDB		();
	~CDB	();

	void	oRAY

};

extern XRCDB_API int nXrCDB;

XRCDB_API int fnXrCDB(void);

