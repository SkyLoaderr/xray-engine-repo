#include "stdafx.h"
#include "clsid.h"

ENGINE_API void CLSID2TEXT(CLASS_ID id, char *text) {
	text[8]=0;
	for (int i=7; i>=0; i--) { text[i]=char(id&0xff); id>>=8; }
}
ENGINE_API CLASS_ID TEXT2CLSID(const char *text) {
	char buf[10];
	strncpy(buf,text,9);
	int need = 8-strlen(buf);
	while (need) { buf[8-need]=' '; need--; }
	return MK_CLSID(buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
}

