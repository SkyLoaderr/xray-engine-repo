#include "stdafx.h"
#include "PHDestroyableNotificate.h"
#include "PHDestroyable.h"
#include "Level.h"
#include "xrServer_Object_Base.h"
#include "../../xr_object.h"
void CPHDestroyableNotificate::spawn_notificate(CSE_Abstract* so)
{
	CPHDestroyable* D= smart_cast<CPHDestroyable*>(Level().Objects.net_Find(so->ID_Parent));
	D->NotificateDestroy(cast_phdestroyable_notificate			());
}