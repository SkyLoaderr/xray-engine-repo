// LightShadows.cpp: implementation of the CLightShadows class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightShadows.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightShadows::CLightShadows()
{
	current	= 0;
}

CLightShadows::~CLightShadows()
{

}

void CLightShadows::set_object	(CObject* O)
{
	current	= O;
	if (current)
	{
		casters.push_back	(caster());
		casters.back().O	= current;
	}
}
void CLightShadows::add_element	(NODE* N)
{
	if (0==current)	return;
	casters.back().nodes.push_back	(N);
}
