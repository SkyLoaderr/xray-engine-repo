//----------------------------------------------------
// file: ObjectOptionPack.h
//----------------------------------------------------

#ifndef _INCDEF_ObjectOptionPack_H_
#define _INCDEF_ObjectOptionPack_H_

#include "FileSystem.h"
//----------------------------------------------------
class ObjectOptionPack{
public:
	void Copy( const ObjectOptionPack& source ){
	};

	void Reset(){
	};

    void Init();
    void Result();

//	bool ReadScript( XScrBlock *block );
	bool Load( FSChunkDef *chunk );
	bool Save( int handle );

	bool RunEditor( );

	ObjectOptionPack(){
		Reset();
	};

	ObjectOptionPack( const ObjectOptionPack& source ){
		Copy( source );
	};
};


//----------------------------------------------------
#endif /*_INCDEF_ObjectOptionPack_H_*/

