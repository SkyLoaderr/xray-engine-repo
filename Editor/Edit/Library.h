//----------------------------------------------------
// file: Library.h
//----------------------------------------------------

#ifndef _INCDEF_Library_H_
#define _INCDEF_Library_H_

#include "SceneObject.h"
//----------------------------------------------------
class ETexture;
class SceneObject;
struct FSChunkDef;

//----------------------------------------------------
class ELibrary {
public:
	bool m_Valid;
	ObjectList  m_Objects;
	SceneObject *m_Current;
protected:
    bool ReadObject( FSChunkDef *chunk );
    bool Load();
    void Save();
public:
	__inline bool valid(){ return m_Valid; }
	__inline SceneObject *o() { return m_Current; }
	__inline void oset( SceneObject * o ) {	m_Current = o; }

	SceneObject *SearchObject( int classfilter, char *name );
	ETexture    *SearchTexture( char *name );
public:
	void Init();
	void Clear();

    void SaveLibrary(){Save();}
    void ReloadLibrary();

	ELibrary();
	virtual ~ELibrary();
};

extern ELibrary Lib;

//----------------------------------------------------
#endif /*_INCDEF_Library_H_*/

