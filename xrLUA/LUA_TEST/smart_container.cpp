#include "stdafx.h"	

#pragma warning(push)
#pragma warning(disable:4995)
#pragma warning(disable:4530)
#include "xrCore.h"
#include <mmsystem.h>
#pragma warning(pop)

//#include <hash_set>
//#include <hash_map>

#undef STATIC_CHECK
#include "smart_container.h"

struct A {};
struct B : public A{};

void test_smart_container()
{
	container								m_container;

//	m_container.add							(VertexA());
//	m_container.add							(VertexB());
//	m_container.add							(VertexC());
//	m_container.add							(VertexD());
//	m_container.add							(VertexE());
//	m_container.add							(VertexF());
//	m_container.add							(VertexF());
//	m_container.add							(VertexF());
//	m_container.add							(VertexF());

	VertexA	a;
	a.x = 0;
	a.y = 1;
	a.z = 2;

	VertexB	b;
	b.x = 10;
	b.y = 11;
	b.z = 12;

	VertexB	_b;
	_b.x = 110;
	_b.y = 111;
	_b.z = 112;

	m_container.add	(&a);
	m_container.add	(&b);
	m_container.add	(&_b);

	{
		container::iterator<VertexB>	I = m_container.begin<VertexB>();
		container::iterator<VertexB>	E = m_container.end<VertexB>();
		for ( ; I != E; ++I) {
			(*I)->x		+= 1;
//			I->x		+= 1;
			printf		("[%f][%f][%f]\n",(*I)->x,(*I)->y,(*I)->z);
		}
	}
	{
		container::iterator<VertexB>	I = m_container.begin<VertexB>();
		container::iterator<VertexB>	E = m_container.end<VertexB>();
		for ( ; I != E; ++I) {
			printf		("[%f][%f][%f]\n",(*I)->x,(*I)->y,(*I)->z);
		}
	}

	CMemoryWriter							stream;
	save_data								(m_container,stream);
}
