#ifndef XRSE_FACTORY_IMPORT_EXPORTH
#define XRSE_FACTORY_IMPORT_EXPORTH

#ifdef XRSE_FACTORY_EXPORTS
#	define FACTORY_API __declspec(dllexport)
#else
#	define FACTORY_API __declspec(dllimport)
#endif

class CSE_Abstract;

extern "C" {
	FACTORY_API CSE_Abstract	*create_entity	(LPCSTR section);
	FACTORY_API void			destroy_entity	(CSE_Abstract *&);
};

#endif