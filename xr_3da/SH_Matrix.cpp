#include "stdafx.h"

void CMatrix::Load(	CStream* fs )
{
	dwMode	= fs->Rdword	();
	tcm		= fs->Rdword	();
}