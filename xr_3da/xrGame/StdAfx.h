#pragma once

#pragma warning(disable:4995)
#include "../stdafx.h"
#pragma warning(default:4995)
#pragma warning( 4 : 4018 )
#pragma warning( 4 : 4244 )

#pragma warning(disable:4505)

#include "clsid_game.h"

// #pragma warning(disable:4201)
#include "../igame_persistent.h"
#include "../igame_level.h"
#include "../xr_object.h"
#include "../sound.h"
// #include "../xr_input.h"
#include "../x_ray.h"
#include "../std_classes.h"
#include "../GameFont.h"

#include "NET_Server.h"
#include "NET_Client.h"

//#include "../render.h"
//#include "../fmesh.h"
#include "../SkeletonAnimated.h"
#include "../SkeletonRigid.h"

// ???
#include <dinput.h>

// ???
#pragma todo("Oles to Kostya/Slipch: following header(ode_include) should'nt be in 'stdafx.h' !!!")
#include "ode_include.h"

#define BOOST_THROW_EXCEPTION_HPP_INCLUDED

#include "smart_cast.h"