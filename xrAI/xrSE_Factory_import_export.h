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

#ifdef AI_COMPILER
CSE_Abstract *F_entity_Create(LPCSTR section)
{
	ISE_Abstract	*i = create_entity(section);
	CSE_Abstract	*j = dynamic_cast<CSE_Abstract*>(i);
	return			(j);
}
#endif

#endif