#include "stdafx.h"
#include "xrMU_Model.h"

void xrMU_Model::Load(CStream& F)
{
	F.Read			(&m_name,128);
	u32 v_count		= F.Rdword();
	while (v_count)
	{
		Fvector		P;

		v_count--;
	}
}
