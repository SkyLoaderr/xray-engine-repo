#include "stdafx.h"
#include "InventoryBox.h"
#include "level.h"

void CInventoryBox::OnEvent(NET_Packet& P, u16 type)
{
	inherited::OnEvent	(P, type);

	switch (type)
	{
	case GE_OWNERSHIP_TAKE:
		{
			u16 id;
            P.r_u16(id);
			CObject* itm = Level().Objects.net_Find(id);  VERIFY(itm);
			m_items.push_back	(id);
			itm->H_SetParent	(this);
			itm->setVisible		(FALSE);
			itm->setEnabled		(FALSE);
		}break;
	case GE_OWNERSHIP_REJECT:
		{
			u16 id;
            P.r_u16(id);
			CObject* itm = Level().Objects.net_Find(id);  VERIFY(itm);
			xr_vector<u16>::iterator it;
			it = std::find(m_items.begin(),m_items.end(),id); VERIFY(it!=m_items.end());
			m_items.erase		(it);
			itm->H_SetParent	(NULL);
		}break;
	};
}

BOOL CInventoryBox::net_Spawn(CSE_Abstract* DC)
{
	inherited::net_Spawn	(DC);
	setVisible				(TRUE);
	setEnabled				(TRUE);
	return					TRUE;
}

void CInventoryBox::net_Relcase(CObject* O)
{
	inherited::net_Relcase(O);
}
