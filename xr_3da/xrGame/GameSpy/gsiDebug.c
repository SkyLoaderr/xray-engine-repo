///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "nonport.h"
#include "gsiDebug.h"
//#include <stdio.h>
//#include <stdarg.h>


// THIS FILE ONLY INCLUDED WHEN USING GAMESPY DEBUG FUNCTIONS
//    (don't put this above the header includes or VC will whine
#ifdef GSI_COMMON_DEBUG


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Static debug data
static struct GSIDebugInstance gGSIDebugInstance; // simple singleton "class"


// Line prefixes, e.g. "[ cat][type][ lev] text"
static char* gGSIDebugCatStrings[GSIDebugCat_Count] =
{
	" GP ", "PEER", " QR2", "  SB", "  V2", " CMN", " APP"
};
static char* gGSIDebugTypeStrings[GSIDebugType_Count] =
{
	" NET", "FILE", " MEM", "MISC"
};
static char* gGSIDebugLevelStrings[GSIDebugLevel_Count] =
{
	"*ERR", "****", "----", "    ", "    ", "    ", "  ->"
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// utility to convert bit flag back to base  (e.g. 1<<2 returns 2)
static gsi_u32 gsiDebugLog2(gsi_u32 theInt)
{
	int total = 0;
	while (theInt > 1)
	{
		theInt = theInt >> 1;
		total++;
	}
	return total;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// process debug output
void gsDebugVaList(GSIDebugCategory theCat, GSIDebugType theType, 
					  GSIDebugLevel theLevel, const char* theTokenStr, 
					  va_list theParamList)
{
	// Retrieve the current debug level
	GSIDebugLevel aCurLevel;

	// Verify Parameters
	assert(theCat   <= GSIDebugCat_Count);
	assert(theType  <= GSIDebugType_Count);
	assert(theLevel <= (1<<GSIDebugLevel_Count));
	assert(theTokenStr);

	// Are we currently logging this type and level?
	aCurLevel = gGSIDebugInstance.mGSIDebugLevel[theCat][theType];
	if (aCurLevel & theLevel) // check the flag
	{
		// Output line prefix
		if (gGSIDebugInstance.mGSIDebugFile)
		{
			fprintf(gGSIDebugInstance.mGSIDebugFile, "[%s][%s][%s] ", 
				gGSIDebugCatStrings[theCat], 
				gGSIDebugTypeStrings[theType],
				gGSIDebugLevelStrings[gsiDebugLog2(theLevel)]);
			
			// Output to file
			vfprintf(gGSIDebugInstance.mGSIDebugFile, theTokenStr, 
				theParamList);
		}

		// Output to developer function if provided
		if (gGSIDebugInstance.mDebugCallback != NULL)
		{
			(*gGSIDebugInstance.mDebugCallback)(theCat, theType, theLevel,
			                                     theTokenStr, theParamList);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// process debug output
void gsDebugFormat(GSIDebugCategory theCat, GSIDebugType theType, 
					  GSIDebugLevel theLevel, const char* theTokenStr, 
					  ...)
{
	va_list aParameterList;

	// Verify Parameters
	assert(theCat   <= GSIDebugCat_Count);
	assert(theType  <= GSIDebugType_Count);
	assert(theLevel <= (1<<GSIDebugLevel_Count));
	assert(theTokenStr);

	// Find start of var arg list
	va_start(aParameterList, theTokenStr);
	
	// Pass to VA version
	gsDebugVaList(theCat, theType, theLevel, theTokenStr, aParameterList);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Converts binary buffer to memory view form:
//    0000 0000 0000 0000 0000 0000 0000 0000  ................
static void HexEncode16(const char* theInStream, char* theOutStream, 
				 unsigned int theInLen)
{
	const int  aRowWidth     = 64;     // width of the output
	const char aReplaceChar  = '.';    // Replace non print characters
	const int  aTextOffSet   = 41;     // text comes after hex bytes
	char* aTextOutStream = (theOutStream+aTextOffSet); // set the write ptr
	const unsigned int aWriteBit = theInLen & 1; // write on odd or even bytes?

	assert(theInLen <= 16);

	// Set buffer to ' '
	memset(theOutStream, ' ', aRowWidth);

	// Convert characters one at a time
	while(theInLen--)
	{
		// Read the next byte
		unsigned char aChar = (unsigned char)(*theInStream++);

		// Write one byte in hex form
		sprintf(theOutStream, "%02X", aChar);

		// Write the printable character
		if (isgraph(aChar))
			*(aTextOutStream++) = aChar;
		else
			*(aTextOutStream++) = aReplaceChar;

		// Move to next hex byte
		theOutStream += 2;

		// Insert a space every other byte
		if ((theInLen & 1) == aWriteBit)
			*theOutStream++ = ' ';
	}

	// Remove NULL terminator from last sprintf
	*theOutStream = ' ';

	// NULL terminate the full string
	*(aTextOutStream) = '\0';
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Write binary data as B64 bytes (40 bytes per line)
void gsDebugBinary(GSIDebugCategory theCat, GSIDebugType theType,
             GSIDebugLevel theLevel, const char* theBuffer, gsi_i32 theLength)
{
	int aBytesLeft = theLength;
	const char* aReadPos = theBuffer;
	char aHexStr[80];

	// convert and display in 40 byte segments
	while(aBytesLeft > 0)
	{
		gsi_i32 aBytesToRead = min(aBytesLeft, 16);

		HexEncode16(aReadPos, aHexStr, aBytesToRead);
		gsDebugFormat(theCat, theType, theLevel, "  %s\r\n", aHexStr);

		aReadPos   += aBytesToRead;
		aBytesLeft -= aBytesToRead;
	};
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void gsSetDebugLevel(GSIDebugCategory theCat, GSIDebugType theType, 
					  GSIDebugLevel theLevel)
{
	// Verify Parameters
	assert(theCat   <= GSIDebugCat_Count);
	assert(theType  <= GSIDebugType_Count);

	// Set for all categories?
	if (theCat == GSIDebugCat_Count)
	{
		int i=0;
		for (; i<GSIDebugCat_Count; i++)
			gsSetDebugLevel((GSIDebugCategory)i, theType, theLevel);
		return;
	}
	
	// Set for all types?
	if (theType == GSIDebugType_Count)
	{
		int i=0;
		for (; i<GSIDebugType_Count; i++)
			gsSetDebugLevel(theCat, (GSIDebugType)i, theLevel);
		return;
	}

	// Is the new level different from the old?
	if (gGSIDebugInstance.mGSIDebugLevel[theCat][theType] != theLevel)
	{
		// Notify of the change
		gsDebugFormat(GSIDebugCat_Common, GSIDebugType_Misc, 
			GSIDebugLevel_Comment,
			"Changing debug level: [%s][%s][%02X]\r\n",
			gGSIDebugCatStrings[theCat], 
			gGSIDebugTypeStrings[theType], 
			theLevel );
		gGSIDebugInstance.mGSIDebugLevel[theCat][theType] = theLevel;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Set the debug output file to an already open file
// Set to "stdout" for console output
void gsSetDebugFile(FILE* theFile)
{
	if (theFile != gGSIDebugInstance.mGSIDebugFile)
	{
		// If the old file is valid, notify it of the closing
		if (gGSIDebugInstance.mGSIDebugFile != NULL)
		{
			gsDebugFormat(GSIDebugCat_Common, GSIDebugType_Misc,
				GSIDebugLevel_Comment, "Debug disabled in this file\r\n");
		}

		// If the new file is valid, notify it of the opening
		{
			gsDebugFormat(GSIDebugCat_Common, GSIDebugType_Misc,
				GSIDebugLevel_Comment, "Debug enabled in this file\r\n");
		}

		gGSIDebugInstance.mGSIDebugFile = theFile;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Opens and sets the debug output file
FILE* gsOpenDebugFile(const char* theFileName)
{
	// The new file
	FILE* aFile = NULL;

	// Verify parameters
	assert(theFileName != NULL);

	// Open the new file (clear contents)
	aFile = fopen(theFileName, "w+");
	if (aFile != NULL)
		gsSetDebugFile(aFile);

	return aFile;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Retrieve the current debug file (if any)
FILE* gsGetDebugFile()
{
	return gGSIDebugInstance.mGSIDebugFile;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Set the developer callback
void gsSetDebugCallback(GSIDebugCallback theCallback)
{
	gGSIDebugInstance.mDebugCallback = theCallback;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // GSI_COMMON_DEBUG