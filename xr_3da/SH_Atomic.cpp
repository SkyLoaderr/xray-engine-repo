#include "stdafx.h"
#include "sh_atomic.h"
#pragma hdrstop

// Atomic
SVS::~SVS								()			{	_RELEASE(vs);		Device.Shader._DeleteVS			(this);	}
SPS::~SPS								()			{	_RELEASE(ps);		Device.Shader._DeletePS			(this);	}
SState::~SState							()			{	_RELEASE(state);	Device.Shader._DeleteState		(this);	}
SDeclaration::~SDeclaration				()			{	_RELEASE(dcl);		Device.Shader._DeleteDecl		(this);	}
