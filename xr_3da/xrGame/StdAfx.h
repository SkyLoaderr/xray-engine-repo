#pragma once

#pragma warning(disable:4995)
#include "../stdafx.h"
#pragma warning(default:4995)
#pragma warning( 4 : 4018 )
#pragma warning( 4 : 4244 )

#pragma warning(disable:4505)

// Dima to All : We can remove these includes, but, I don't know why, 
// without them xrGame cannot be linked properly
#include "net_utils.h"
#include "../xrLevel.h"

// this include MUST be here, since smart_cast is used >1800 times in the project
#include "smart_cast.h"

#define BOOST_THROW_EXCEPTION_HPP_INCLUDED