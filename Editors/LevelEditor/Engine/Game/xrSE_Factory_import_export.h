#ifndef XRSE_FACTORY_IMPORT_EXPORTH
#define XRSE_FACTORY_IMPORT_EXPORTH

#ifdef XRSE_FACTORY_EXPORTS
#	define FACTORY_API __declspec(dllexport)
#else
#	define FACTORY_API __declspec(dllimport)
#endif

class CSE_Abstract;

extern "C" {
	FACTORY_API ISE_Abstract* __stdcall create_entity	(LPCSTR section);
	FACTORY_API void		  __stdcall destroy_entity	(CSE_Abstract *&);
};

#endif