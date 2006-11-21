///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifndef __GSUTILITY_H__
#define __GSUTILITY_H__


#include "gsPlatform.h"


#ifdef __cplusplus
extern "C" {
#endif


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Async DNS lookup
#if !defined(GSI_NO_THREADS)

// async way to resolve a hostname to an IP
typedef struct GSIResolveHostnameInfo * GSIResolveHostnameHandle;
#define GSI_STILL_RESOLVING_HOSTNAME   0
#define GSI_ERROR_RESOLVING_HOSTNAME   0xFFFFFFFF

// start resolving a hostname
// returns 0 on success, -1 on error
int  gsiStartResolvingHostname(const char * hostname, GSIResolveHostnameHandle * handle);
// cancel a resolve in progress
void gsiCancelResolvingHostname(GSIResolveHostnameHandle handle);
// returns GSI_STILL_RESOLVING if still resolving the hostname
// returns GSI_ERROR_RESOLVING if it was unable to resolve the hostname
// on success, returns the IP of the host in network byte order
unsigned int gsiGetResolvedIP(GSIResolveHostnameHandle handle);

#endif // (GSI_NO_THREADS)


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#if defined(GS_UNDER_CE)
int isdigit(int c);
int isxdigit(int c);
int isalnum(int c);
int isspace(int c);
int isgraph(int c);
#endif


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Get rid of compiler warnings when parameters are never used
// (Mainly used in sample apps and callback for platform switches)
#if (defined(__MWERKS__) && !defined(_NITRO)) || defined(WIN32)
	#define GSI_UNUSED(x) x
#elif defined(_PS2) || defined(_NITRO) || defined(_PS3) || defined(_MACOSX)
	#define GSI_UNUSED(x) {void* y=&x;y=NULL;}
#elif defined(_PSP)
#define GSI_UNUSED(x) (void)x;
	
#else
	#define GSI_UNUSED(x)
#endif


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Cross platform random number generator
void Util_RandSeed(unsigned long seed); // to seed it
int  Util_RandInt(int low, int high);   // retrieve a random int


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Base 64 encoding (printable characters)
void B64Encode(const char *input, char *output, int inlen, int encodingType);
void B64Decode(const char *input, char *output, int inlen, int * outlen, int encodingType);

// returns the length of the binary data represented by the base64 input string
int B64DecodeLen(const char *input, int encodingType);

typedef struct
{
	const char *input;
	int len;
	int encodingType;
} B64StreamData;

void B64InitEncodeStream(B64StreamData *data, const char *input, int len, int encodingType);

// returns gsi_false if the stream has ended
gsi_bool B64EncodeStream(B64StreamData *data, char output[4]);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#define XXTEA_KEY_SIZE 17
gsi_i8 * gsXxteaEncrypt(const gsi_i8 * iStr, gsi_i32 iLength, gsi_i8 key[XXTEA_KEY_SIZE], gsi_i32 *oLength);
gsi_i8 * gsXxteaDecrypt(const gsi_i8 * iStr, gsi_i32 iLength, gsi_i8 key[XXTEA_KEY_SIZE], gsi_i32 *oLength);

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#if defined(_DEBUG)
	void gsiCheckStack(void);
#else
	#define gsiCheckStack() 
#endif


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
gsi_time current_time();         // milliseconds
gsi_time current_time_hires();   // microseconds
void msleep(gsi_time msec);      // milliseconds


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Misc utilities



	
#if defined(GS_UNDER_CE) || defined(_NITRO)
	time_t time(time_t *timer);
#elif ! defined _PS3
	#include <time.h>
#endif

	#ifndef SOMAXCONN
	#define SOMAXCONN 5
#endif

typedef const char * (* GetUniqueIDFunction)();

extern GetUniqueIDFunction GOAGetUniqueID;

// Prototypes so the compiler won't warn
#ifdef _PS2
extern int wprintf(const wchar_t*,...);
#endif


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif

#endif //__GSUTILITY_H__

