#ifndef _AL_EAX_H_
#define _AL_EAX_H_

#define ALAPI __declspec(dllexport)
#define ALAPIENTRY __cdecl

#include "AL/altypes.h"

#ifdef __cplusplus
extern "C" {
#endif

ALboolean		CheckEAXSupport(ALubyte *szEAXName);
ALAPI ALenum    ALAPIENTRY EAXGet(const struct _GUID *propertySetID,ALuint property,ALuint source,ALvoid *value,ALuint size);
ALAPI ALenum    ALAPIENTRY EAXSet(const struct _GUID *propertySetID,ALuint property,ALuint source,ALvoid *value,ALuint size);

#ifdef __cplusplus
}
#endif

#endif