////////////////////////////////////////////////////////////////////////////
//	Module 		: object_factory_space.h
//	Created 	: 30.06.2004
//  Modified 	: 30.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Object factory space
////////////////////////////////////////////////////////////////////////////

#ifndef object_factory_spaceH
#define object_factory_spaceH

#pragma once

#ifndef XRGAME_EXPORTS
#	define NO_XR_GAME
#endif

#ifdef NO_XR_GAME
#	ifndef XRSE_FACTORY_EXPORTS
#		define NO_SCRIPTS
#	endif
#endif

#ifndef NO_XR_GAME
	class CAttachableItem;
#endif

class CSE_Abstract;

namespace ObjectFactory {

#ifndef NO_XR_GAME
	typedef DLL_Pure			CLIENT_BASE_CLASS;
#endif
	typedef CSE_Abstract		SERVER_BASE_CLASS;

#ifndef NO_SCRIPTS
#	ifndef NO_XR_GAME
		typedef CAttachableItem	CLIENT_SCRIPT_BASE_CLASS;
#	endif
	typedef CSE_Abstract		SERVER_SCRIPT_BASE_CLASS;
#endif

};

#endif