//----------------------------------------------------
// file: Library.h
//----------------------------------------------------

#ifndef _INCDEF_Library_H_
#define _INCDEF_Library_H_

#include "FileSystem.h"
#include "SceneObject.h"
#include "SObject2.h"
#include "Texture.h"

//----------------------------------------------------

template <class _T>
class LF{
public:
	char m_Name[256];
	list< _T > m_Items;
	list< LF<_T>* > m_Subs;
public:
	LF( char *_Name ){ strcpy( m_Name, _Name ); }
	LF( ){ m_Name[0] = 0; }
	
	~LF(){
		list< LF<_T>* >::iterator _F = m_Subs.begin();
		for(;_F!=m_Subs.end();_F++){
			delete (*_F); }
		m_Subs.clear();
		m_Items.clear();
	};
};

typedef LF<ETexture*> TexFolder;
typedef LF<SceneObject*> ObjFolder;

//----------------------------------------------------

class ELibrary {
public:

	bool m_Valid;
	ObjectList m_Objects;
	SceneObject *m_Current;

	TextureList m_Lands;
	ETexture *m_LandCurrent;

	TexFolder m_TexFolders;
	ObjFolder m_ObjFolders;

protected:
	bool InitLandscapeFolder( TexFolder& folder, XScrBlock *block );
	bool InitObjectFolder( ObjFolder& folder, XScrBlock *block );
	bool AddObjectToFolder( ObjFolder& folder, XScrBlock *block );
	bool AddFileToObject( SObject2 *obj, XScrBlock *block );

public:
	__forceinline bool valid(){
		return m_Valid; }
	__forceinline ETexture *l() {
		return m_LandCurrent; }
	__forceinline void lset( ETexture *t ) {
		m_LandCurrent = t; }
	__forceinline SceneObject *o() {
		return m_Current; }
	__forceinline void oset( SceneObject * o ) {
		m_Current = o; }

	SceneObject *SearchObject( int classfilter, char *name );
	ETexture *SearchTexture( char *name );

public:
	
	void Init();
	void Clear();

	ELibrary();
	virtual ~ELibrary();
};

extern ELibrary Lib;

//----------------------------------------------------
#endif /*_INCDEF_Library_H_*/

