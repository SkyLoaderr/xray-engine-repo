#include "stdafx.h"
#include "vector.h"

ENGINE_API float pvUVAdjustment	[0x2000];

void pvInitializeStatics(void)
{
	for ( int idx = 0; idx < 0x2000; idx++ )
	{
		long xbits = idx >> 7;
		long ybits = idx & pvBOTTOM_MASK;
		
		// map the numbers back to the triangle (0,0)-(0,127)-(127,0)
		if (( xbits + ybits ) >= 127 ) 
		{ 
			xbits = 127 - xbits; 
			ybits = 127 - ybits; 
		}
		
		// convert to 3D vectors
		float x = float(xbits);
		float y = float(ybits);
		float z = float(126 - xbits - ybits );
		
		// calculate the amount of normalization required
		pvUVAdjustment[idx] = 1.0f / sqrtf( y*y + z*z + x*x );
	}
}
