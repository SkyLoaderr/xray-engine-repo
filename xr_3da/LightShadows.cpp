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
}
void CLightShadows::add_element	(NODE* N)
{
}
