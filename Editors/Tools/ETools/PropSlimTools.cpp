#include	"stdafx.h"
#include	"PropSlimTools.h"

using namespace ETOOLS;

BOOL ContractionGenerate	(QSMesh* src_mesh, QSContraction* dst_conx, u32 min_faces, float max_error)
{
	dst_conx->items			= (QSContractionItem**)xr_realloc(dst_conx->items,sizeof(QSContractionItem*)*10);
	return TRUE;
}

void ContractionClear		(QSContraction* dst_conx)
{
	VERIFY					(dst_conx&&dst_conx->items);
	for (u32 k=0; k<dst_conx->item_cnt; k++)
		xr_delete			(dst_conx->items[k]);
	xr_free					(dst_conx->items);
}
