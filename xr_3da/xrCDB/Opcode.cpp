///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Main file for Opcode.dll.
 *	\file		Opcode.cpp
 *	\author		Pierre Terdiman
 *	\date		March, 20, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
	New in Opcode 1.1:
	- stabbing queries
	- sphere queries
	- abtract base class for colliders
	- settings validation methods
	- compilation flags now grouped in OPC_Settings.h
	- smaller files, new VC++ virtual dirs (cleaner)

	Notes:
	- "override(baseclass)" is a personal cosmetic thing. It's the same as "virtual", but provides more info.
	- I code in 1600*1200, so some lines may look a bit long..
	- This version is not as polished as the previous one due to lack of time. The stabbing & sphere queries
	can still be optimized: for example by trying other atomic overlap tests. I'm using my first ray-AABB
	code, but the newer one seems better. Tim Schröder's one is good as well. See: www.codercorner.com/RayAABB.cpp
	- The trees can easily be compressed even more, I save this for later (lack of time, lack of time!)
	- I removed various tests before releasing this one:
		- a separation line, a.k.a. "front" in QuickCD, because gains were unclear
		- distance queries in a PQP style, because it was way too slow
		- support for deformable models, too slow as well
	- You can easily use Opcode to do your player-vs-world collision detection, in a Nettle/Telemachos way.
	If someone out there want to donate some art / level for the cause, I'd be glad to release a demo. (current
	demo uses copyrighted art I'm not allowed to spread)
	- Sorry for the lack of real docs and/or solid examples. I just don't have enough time.

	-------------------------------------------

	Finding a good name is difficult!
	Here's the draft for this lib.... Spooky, uh?

		VOID?			Very Optimized Interference Detection
		ZOID?			Zappy's Optimized Interference Detection
		CID?			Custom/Clever Interference Detection
		AID / ACID!		Accurate Interference Detection
		QUID?			Quick Interference Detection
		RIDE?			Realtime Interference DEtection
		WIDE?			Wicked Interference DEtection (....)
		GUID!
		KID !			k-dop interference detection :)
		OPCODE!			OPtimized COllision DEtection
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Precompiled Header
#include "Stdafx.h"

#ifdef ICE_MAIN

void ModuleAttach(udword hmod)
{
	Log("// Opening OPCODE\n\n");
}

void ModuleDetach()
{
	Log("// Closing OPCODE\n\n");
}

#endif