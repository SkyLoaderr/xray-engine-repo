////////////////////////////////////////////////////////////////////////////
//	Module 		: smart_container->cpp
//	Created 	: 22->10->2004
//  Modified 	: 22->10->2004
//	Author		: Dmitriy Iassenev
//	Description : Smart container class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"	

#pragma warning(push)
#pragma warning(disable:4995)
#include "xrCore.h"
#pragma warning(pop)
#include "smart_container.h"

// these header are needed for serialization only
//#include "in_stream.h"
//#include "out_stream.h"

void test_smart_container()
{
	smart_container		container0, container1;

	VertexA				*a = new VertexA();
	a->x				= 0;
	a->y				= 1;
	a->z				= 2;

	VertexB				*b = new VertexB();
	b->x				= 10;
	b->y				= 11;
	b->z				= 12;

	VertexB				*_b = new VertexB();
	_b->x				= 110;
	_b->y				= 111;
	_b->z				= 112;

	container0.add		(a);
	container0.add		(b);
	container0.add		(_b);

	{
		Msg			("\n");
		smart_container::iterator<VertexB>	I = container0.begin<VertexB>();
		smart_container::iterator<VertexB>	E = container0.end<VertexB>();
		for ( ; I != E; ++I) {
			(*I)->x	+= 1;
			Msg		("[%f][%f][%f]\n",(*I)->x,(*I)->y,(*I)->z);
		}
	}
	
	container0.remove	(_b);
	xr_delete			(_b);
	
	{
		Msg				("\n");
		smart_container::iterator<CPositionComponent>	I = container0.begin<CPositionComponent>();
		smart_container::iterator<CPositionComponent>	E = container0.end<CPositionComponent>();
		for ( ; I != E; ++I) {
			Msg			("[%f][%f][%f]\n",(*I)->x,(*I)->y,(*I)->z);
			
			VertexA		*a;
			if (I.try_convert(a))
				a->u	+= 1;
			
			VertexB		*b;
			if (I.try_convert(b))
				b->nx	+= 2;
			
			VertexC		*c;
			if (I.try_convert(c))
				c->ny	+= 3;
			
			VertexD		*d;
			if (I.try_convert(d))
				d->nz	+= 4;
			
			VertexE		*e;
			if (I.try_convert(e))
				e->u2	+= 5;
			
			VertexF		*f;
			if (I.try_convert(f))
				f->v2	+= 6;
		}
	}
//	{
//		IWriter			stream("smart_container.dat");
//		save_data		(container0,stream);
//	}
//	{
//		IReader			stream("smart_container.dat");
//		load_data		(container1,stream);
//	}
//
//	VERIFY				(equal(container0,container1));

	// speed test for RELEASE only
#ifndef _DEBUG
	{
		u32				start_time = timeGetTime();
		container0.clear();
		const u32 test_count = 1000000;

		for (u32 i=0; i<test_count; ++i) {
			container0.add(new VertexA(rand(),rand(),rand()));
			container0.add(new VertexB(rand(),rand(),rand()));
			container0.add(new VertexC(rand(),rand(),rand()));
			container0.add(new VertexD(rand(),rand(),rand()));
			container0.add(new VertexE(rand(),rand(),rand()));
			container0.add(new VertexF(rand(),rand(),rand()));
		}

		Msg				("%f\n",(float)(timeGetTime() - start_time)/1000.f);
		start_time		= timeGetTime();

		{
			smart_container::iterator<CPositionComponent>	I = container0.begin<CPositionComponent>();
			smart_container::iterator<CPositionComponent>	E = container0.end<CPositionComponent>();
			for ( ; I != E; ++I) {
				(*I)->x += rand();
				(*I)->y += rand();
				(*I)->z += rand();
			}
		}

		Msg				("%f\n",(float)(timeGetTime() - start_time)/1000.f);
		start_time		= timeGetTime();

		{
			DEFAULT_CONTAINER<DEFAULT_POINTER(VertexA)>::iterator	I = container0.container((vertex_container<VertexA>*)0)->objects().begin();
			DEFAULT_CONTAINER<DEFAULT_POINTER(VertexA)>::iterator	E = container0.container((vertex_container<VertexA>*)0)->objects().end();
			for ( ; I != E; ++I) {
				(*I)->x += rand();
				(*I)->y += rand();
				(*I)->z += rand();
			}
		}

		{
			DEFAULT_CONTAINER<DEFAULT_POINTER(VertexB)>::iterator	I = container0.container((vertex_container<VertexB>*)0)->objects().begin();
			DEFAULT_CONTAINER<DEFAULT_POINTER(VertexB)>::iterator	E = container0.container((vertex_container<VertexB>*)0)->objects().end();
			for ( ; I != E; ++I) {
				(*I)->x += rand();
				(*I)->y += rand();
				(*I)->z += rand();
			}
		}

		{
			DEFAULT_CONTAINER<DEFAULT_POINTER(VertexC)>::iterator	I = container0.container((vertex_container<VertexC>*)0)->objects().begin();
			DEFAULT_CONTAINER<DEFAULT_POINTER(VertexC)>::iterator	E = container0.container((vertex_container<VertexC>*)0)->objects().end();
			for ( ; I != E; ++I) {
				(*I)->x += rand();
				(*I)->y += rand();
				(*I)->z += rand();
			}
		}

		{
			DEFAULT_CONTAINER<DEFAULT_POINTER(VertexD)>::iterator	I = container0.container((vertex_container<VertexD>*)0)->objects().begin();
			DEFAULT_CONTAINER<DEFAULT_POINTER(VertexD)>::iterator	E = container0.container((vertex_container<VertexD>*)0)->objects().end();
			for ( ; I != E; ++I) {
				(*I)->x += rand();
				(*I)->y += rand();
				(*I)->z += rand();
			}
		}

		{
			DEFAULT_CONTAINER<DEFAULT_POINTER(VertexE)>::iterator	I = container0.container((vertex_container<VertexE>*)0)->objects().begin();
			DEFAULT_CONTAINER<DEFAULT_POINTER(VertexE)>::iterator	E = container0.container((vertex_container<VertexE>*)0)->objects().end();
			for ( ; I != E; ++I) {
				(*I)->x += rand();
				(*I)->y += rand();
				(*I)->z += rand();
			}
		}

		{
			DEFAULT_CONTAINER<DEFAULT_POINTER(VertexF)>::iterator	I = container0.container((vertex_container<VertexF>*)0)->objects().begin();
			DEFAULT_CONTAINER<DEFAULT_POINTER(VertexF)>::iterator	E = container0.container((vertex_container<VertexF>*)0)->objects().end();
			for ( ; I != E; ++I) {
				(*I)->x += rand();
				(*I)->y += rand();
				(*I)->z += rand();
			}
		}

		Msg				("%f\n",(float)(timeGetTime() - start_time)/1000.f);
	}
#endif
}
