#include <lwserver.h>
#include <lwgeneric.h>
#include <lwrender.h>
#include <lwenvel.h>
#include <lwchannel.h>
#include <lwhost.h>
#include <lwpanel.h>
#include <lwsurf.h>
#include <lwdisplay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <lwobjimp.h>
#include <sys/stat.h>
#include <lwmodeler.h>
#include <lwtxtr.h>
#include <lwimage.h>

/* some globals */

LWChannelInfo	*g_chinfo;
LWEnvelopeFuncs *g_envf;
LWItemInfo		*g_iteminfo;
LWSceneInfo		*g_lwsi;
LWInterfaceInfo *g_intinfo;
LWMessageFuncs	*g_msg;
LWBoneInfo		*g_boneinfo;
LWObjectFuncs	*g_objfunc;
LWObjectInfo	*g_objinfo;
LWSurfaceFuncs	*g_surff;
HostDisplayInfo *g_hdi;
LWStateQueryFuncs *g_query;
LWTextureFuncs	*g_txfunc;
LWImageList		*g_imglist;
   
static int get_globals( GlobalFunc *global )
{
	g_chinfo	= global( LWCHANNELINFO_GLOBAL,		GFUSE_TRANSIENT );
	g_envf		= global( LWENVELOPEFUNCS_GLOBAL,	GFUSE_TRANSIENT );
	g_iteminfo	= global( LWITEMINFO_GLOBAL,		GFUSE_TRANSIENT );
	g_lwsi		= global( LWSCENEINFO_GLOBAL,		GFUSE_TRANSIENT );
	g_intinfo	= global( LWINTERFACEINFO_GLOBAL,	GFUSE_TRANSIENT );
	g_msg		= global( LWMESSAGEFUNCS_GLOBAL,	GFUSE_TRANSIENT );
	g_boneinfo	= global( LWBONEINFO_GLOBAL,		GFUSE_TRANSIENT );	
	g_objfunc	= global( LWOBJECTFUNCS_GLOBAL,		GFUSE_TRANSIENT );
	g_objinfo	= global( LWOBJECTINFO_GLOBAL,		GFUSE_TRANSIENT );
	g_surff		= global( LWSURFACEFUNCS_GLOBAL,	GFUSE_TRANSIENT );
	g_hdi		= global( LWHOSTDISPLAYINFO_GLOBAL, GFUSE_TRANSIENT );
	g_query		= global( LWSTATEQUERYFUNCS_GLOBAL, GFUSE_TRANSIENT );	
	g_txfunc	= global( LWTEXTUREFUNCS_GLOBAL,	GFUSE_TRANSIENT );
	g_imglist	= global( LWIMAGELIST_GLOBAL,		GFUSE_TRANSIENT );
	
	return ( g_chinfo && g_envf && g_iteminfo && g_lwsi && g_intinfo && g_msg && g_boneinfo && g_objfunc && g_objinfo && g_surff && g_query && g_txfunc);
}


XCALL_( int ) LW_ObjectImport( long version, GlobalFunc *global, LWObjectImport *local, void *serverData );

XCALL_( int ) ObjectImport( long version, GlobalFunc *global, LWObjectImport *lwi, void *serverData )
{
	struct stat s;
	/* check the activation version */
	if ( version != LWOBJECTIMPORT_VERSION ) return AFUNC_BADVERSION;

	/* get the file size */
	if ( stat( lwi->filename, &s )) {
		lwi->result = LWOBJIM_BADFILE;
		return AFUNC_OK;
	}
	
	get_globals(global);

	return LW_ObjectImport(version, global, lwi, serverData);	
}

#include <lwcmdseq.h>
#include <lwobjimp.h>
#include <lwmodeler.h>

ServerRecord ServerDesc[] = {
	{ LWOBJECTIMPORT_CLASS, "Import_XRayObject", ObjectImport },
	{ NULL }
};
