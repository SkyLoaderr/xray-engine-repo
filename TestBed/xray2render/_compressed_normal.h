#ifndef _COMPR_NORMAL_
#define _COMPR_NORMAL_

// A Unit Vector to 16-bit word conversion algorithm
// based on work of Rafael Baptista (rafael@oroboro.com)
// Accuracy improved by O.D. (punkfloyd@rocketmail.com)

// a compressed unit vector. reasonable fidelty for unit vectors in a 16 bit
// package. Good enough for surface normals we hope.

ENGINE_API WORD		pvCompress			( const Fvector& vec );
ENGINE_API void		pvDecompress		( Fvector& vec, WORD mVec );
ENGINE_API void		pvInitializeStatics	( void );

#endif