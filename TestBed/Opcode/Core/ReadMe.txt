
 OPCODE distribution 1.1
 -----------------------

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
	If someone out there wants to donate some art / level for the cause, I'd be glad to release a demo. (current
	demo uses copyrighted art I'm not allowed to spread)
	- Sorry for the lack of real docs and/or solid examples. I just don't have enough time.

 ===============================================================================

 WHAT ?

    OPCODE means OPtimized COllision DEtection.
    So this is a collision detection package similar to RAPID. Here's a
    quick list of features:

    - C++ interface, developed for Windows systems using VC++ 6.0
    - Works on arbitrary meshes (convex or non-convex), even polygon soups
    - Current implementation uses AABB-trees
    - Supports CD queries for 2 bodies only. The sweep-and-prune and N-body code is part
      of a bigger unreleased package (Z-COLLIDE)
    - Introduces Primitive-BV overlap tests during recursive collision queries (whereas
      standard libraries only rely on Primitive-Primitive and BV-BV tests)
    - Introduces no-leaf trees, i.e. collision trees whose leaf nodes have been removed
    - Supports collision queries on quantized trees (decompressed on-the-fly)
    - Supports "first contact" or "all contacts" modes (à la RAPID)
    - Uses temporal coherence for "first contact" mode (~10 to 20 times faster, useful
      in rigid body simulation during bisection)
    - Memory footprint is 7.2 times smaller than RAPID's one, which is ideal for console
      games with limited ram (actually, if you use the unmodified RAPID code using double
      precision, it's more like 13 times smaller...)
    - And yet it often runs faster than RAPID (according to RDTSC, sometimes more than 5
      times faster when objects are deeply overlapping)
    - Performance is usually close to RAPID's one in close-proximity situations
	- Stabbing & sphere queries
    - Does not work on deformable meshes. Yet.

 WHY ?

    - Because RAPID uses too many bytes.
    - Because the idea was nice...

 WHEN ?

    It's been coded in march 2001 following a thread on the GD-Algorithms list.

      GDAlgorithms-list mailing list
      GDAlgorithms-list@lists.sourceforge.net
      http://lists.sourceforge.net/lists/listinfo/gdalgorithms-list

 WHO ?

    Pierre Terdiman
    October 9, 2001

    p.terdiman@wanadoo.fr
    p.terdiman@codercorner.com
 
    http://www.codercorner.com
    http://www.codercorner.com/Opcode.htm
