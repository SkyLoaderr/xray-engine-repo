#include "stdafx.h"	
#pragma warning(push)
#pragma warning(disable:4995)
#pragma warning(disable:4530)
#include "xrCore.h"
#include <mmsystem.h>
#pragma warning(pop)

#undef STATIC_CHECK
#include "smart_container.h"

void test_smart_container()
{
	container								m_container;

	m_container.add							(new VertexA());
	m_container.add							(new VertexB());
	m_container.add							(new VertexC());
	m_container.add							(new VertexD());
	m_container.add							(new VertexE());
	m_container.add							(new VertexF());
	m_container.add							(new VertexF());
	m_container.add							(new VertexF());
	m_container.add							(new VertexF());

	container::iterator<CSecondTCComponent>	I = m_container.begin<CSecondTCComponent>();
	container::iterator<CSecondTCComponent>	E = m_container.end<CSecondTCComponent>();
	for ( ; I != E; ++I) {
		(*I)->u2							+= 1;
	}
}
