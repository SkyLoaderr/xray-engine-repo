#include "stdafx.h"
#include "dxerr8.h"

// *****************************************************************************************
// Error handling

void CRenderDevice::Error(HRESULT hr, const char *file, int line)
{
	char errmsg_buf[1024];

	const char *errStr = DXGetErrorString8A(hr);
	if (errStr==0) {
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,hr,0,errmsg_buf,1024,0);
		errStr = errmsg_buf;
	}
	_verify(errStr,(char *)file,line);
}

void __cdecl CRenderDevice::Fatal(const char* F,...)
{
	char errmsg_buf[1024];

	va_list		p;
	va_start	(p,F);
	vsprintf	(errmsg_buf,F,p);
	va_end		(p);
	_verify		(errmsg_buf,"<unknown>",0);
}

//----------------------------- FLAGS
static struct _DF {
	char *	name;
	DWORD	mask;
} DF[] = {
	{"rsFullscreen",	rsFullscreen	},
	{"rsTriplebuffer",	rsTriplebuffer  },
	{"rsClearBB",		rsClearBB 		},
	{"rsNoVSync",		rsNoVSync 		},
	{"rsWireframe",		rsWireframe		},
	{"rsAntialias",		rsAntialias		},
	{"rsNormalize",		rsNormalize		},
	{"rsOverdrawView",	rsOverdrawView	},
    {NULL,0}
};

void CRenderDevice::DumpFlags()
{
	Log("- Dumping device flags");
	_DF *p = DF;
	while (p->name) {
		Msg("* %20s %s",p->name,(psDeviceFlags&p->mask)?"on":"off");
		p++;
	}
}
