// file: XScript.h

#include "stdafx.h"

#ifndef _INCDEF_XSCRIPT_H_
#define _INCDEF_XSCRIPT_H_


typedef enum {
	TOK_SYMBOL = 0,
	TOK_INT = 1,
	TOK_FLOAT = 2,
	TOK_DELIMITER = 3,
	TOK_FORCE_DWORD = 0xffffffff
} TOKENTYPES;

typedef enum {
	CTE_FLOAT = 0,
	CTE_INT = 2,
	CTE_CHARPTR = 4,
	CTE_FORCE_DWORD = 0xffffffff
} CTYPECONV;



#pragma pack(push,1)

struct XBinScrHeader {
	DWORD magic;
	DWORD dicSize;
	DWORD dicOffset;
	DWORD tokenCount;
	DWORD tokenOffset;
};

class XToken{
public:
	int tokFrom;
	int tokChars;
	TOKENTYPES tokType;
	union {
		int		tokInt;
		float	tokFloat;
		char	tokDelim;
	};
};

#pragma pack(pop)


class XScr{
protected:
	char scrName[MAX_PATH];
	char *scrBuffer;
	long scrBufferSize;
	vector<XToken> scrTokens;
	bool scrCompiled;
protected:
	bool LoadFile();
	bool LoadBinary();
	bool LoadText();
	bool Compile();
public:
	bool Merge		( XScr *_S );
	bool AddToken	( XToken *_T, char *_Text );
	bool AddText	( char *_Text, int _TextSize );
	bool SaveBinary	( char *_fileName );
	bool SaveText	( char *_fileName );
public:
	XScr( char *_fileName, bool _bLoad = true );
	XScr( char *_Buffer, int _bufferSize, bool _Text = true );
	~XScr();
	inline int getTokenCount(){
		return scrTokens.size(); };
	inline TOKENTYPES getTokenType( int _at ){
		_ASSERTE( _at >= 0 && _at < getTokenCount() );
		return scrTokens[_at].tokType; };
	inline char getDelim( int _at ){
		_ASSERTE( _at >= 0 && _at < getTokenCount() );
		_ASSERTE( getTokenType( _at ) == TOK_DELIMITER );
		return scrTokens[_at].tokDelim; };
	inline int getInt( int _at ){
		_ASSERTE( _at >= 0 && _at < getTokenCount() );
		_ASSERTE( getTokenType( _at ) == TOK_INT );
		return scrTokens[_at].tokInt; };
	inline float getFloat( int _at ){
		_ASSERTE( _at >= 0 && _at < getTokenCount() );
		_ASSERTE( getTokenType( _at ) == TOK_FLOAT );
		return scrTokens[_at].tokFloat; };
	inline char * getSymPtr( int _at ){
		_ASSERTE( _at >= 0 && _at < getTokenCount() );
		_ASSERTE( getTokenType( _at ) == TOK_SYMBOL );
		return scrBuffer + scrTokens[_at].tokFrom; };
	inline int getSymSize( int _at ){
		_ASSERTE( _at >= 0 && _at < getTokenCount() );
		_ASSERTE( getTokenType( _at ) == TOK_SYMBOL );
		return scrTokens[_at].tokChars; };
protected:
	int cPtr;
	bool trytoken();
	bool trynumber();
	bool trysymbol();
	bool skipspaces();
};

class XTokDef {
public:
	char *S; int Value;
	XTokDef( char *_S, int _V ){
		S = _S; Value = _V; };
};

class XVarDef {
public:
	char *S; CTYPECONV Type;
	union {
		void *Ptr;
		union {
			float *PtrFloat;
			int *PtrInt;
			char *PtrChar; };};
	XVarDef( char *_S, CTYPECONV _Type, void *_Ptr ){
		S = _S; Type = _Type; Ptr = _Ptr; };
	XVarDef( ){
		S = 0; Type = CTE_FORCE_DWORD; Ptr = 0; };
};


class XScrBlock{
protected:
	XScr *sbScr;
	XScrBlock *sbParent;
	int sbFrom;
	int sbSize;

public:

	XScrBlock(XScr *_fullScript){
		_ASSERTE( _fullScript );
		sbScr = _fullScript;
		sbParent = 0; sbFrom = 0;
		sbSize = sbScr->getTokenCount(); };

	XScrBlock(XScrBlock *_block,int _at){
		_ASSERTE( _block && _block->sbScr );
		_ASSERTE( _block->sbScr->getTokenType( _block->sbFrom + _at ) == TOK_DELIMITER );
		_ASSERTE( _block->sbScr->getDelim( _block->sbFrom + _at ) == '{' );
		sbScr = _block->sbScr;
		sbParent = _block;
		sbFrom = _block->sbFrom + _at;
		sbSize = 0;
		sbFrom++;
		int sbcnt = -1;
		while( (sbFrom + sbSize) < sbScr->getTokenCount() ){
			if( sbScr->getTokenType( sbFrom + sbSize ) == TOK_DELIMITER ){
				if( sbScr->getDelim( sbFrom + sbSize ) == '{' ){
					sbcnt--; }
				else if( sbScr->getDelim( sbFrom + sbSize ) == '}' ){
					sbcnt++; }
			}
			if( sbcnt==0 )
				break;
			sbSize++; }
	};

	__inline int getTokenCount(){
		return sbSize; };
	__inline TOKENTYPES getTokenType( int _at ){
		_ASSERTE( _at >= 0 && _at < getTokenCount() );
		return sbScr->getTokenType(sbFrom+_at); };
	__inline char getDelim( int _at ){
		_ASSERTE( _at >= 0 && _at < getTokenCount() );
		_ASSERTE( getTokenType( _at ) == TOK_DELIMITER );
		return sbScr->getDelim(sbFrom+_at); };
	__inline int getInt( int _at ){
		_ASSERTE( _at >= 0 && _at < getTokenCount() );
		_ASSERTE( getTokenType( _at ) == TOK_INT );
		return sbScr->getInt(sbFrom+_at); };
	__inline float getFloat( int _at ){
		_ASSERTE( _at >= 0 && _at < getTokenCount() );
		_ASSERTE( getTokenType( _at ) == TOK_FLOAT );
		return sbScr->getFloat(sbFrom+_at); };
	__inline char * getSymPtr( int _at ){
		_ASSERTE( _at >= 0 && _at < getTokenCount() );
		_ASSERTE( getTokenType( _at ) == TOK_SYMBOL );
		return sbScr->getSymPtr(sbFrom+_at); };
	__inline int getSymSize( int _at ){
		_ASSERTE( _at >= 0 && _at < getTokenCount() );
		_ASSERTE( getTokenType( _at ) == TOK_SYMBOL );
		return sbScr->getSymSize(sbFrom+_at); };
	__inline void getSymbol( int _at, char *_Buffer ){
		_ASSERTE( _at >= 0 && _at < getTokenCount() );
		_ASSERTE( getTokenType( _at ) == TOK_SYMBOL );
		memcpy( _Buffer, sbScr->getSymPtr(sbFrom+_at), sbScr->getSymSize(sbFrom+_at) );
		_Buffer[sbScr->getSymSize(sbFrom+_at)] = 0;   };

	__inline int nextPth( int _from ){
		while( _from<getTokenCount() ){
			if( getTokenType( _from ) == TOK_DELIMITER ){
				if( getDelim( _from ) == '{' ){
					int idd = -1;
					_from++;
					while( _from<getTokenCount() ){
						if( getTokenType( _from ) == TOK_DELIMITER ){
							if( getDelim( _from ) == '{' ) idd--;
							else if( getDelim( _from ) == '}' ) idd++; }
						if( idd == 0 )
							break;
						_from++; };
				}
				else
					if( getDelim( _from ) == ';' )
						return (_from+1);
			}
			_from++; };
		return _from; };

	__inline bool delimCompare( int _at, char _cmpTo ){
		return ((getTokenType(_at)==TOK_DELIMITER) && (getDelim(_at)==_cmpTo) ); }

	__inline bool symCompare( int _at, char *_cmpTo ){
		if( getTokenType(_at)!=TOK_SYMBOL )
			return false;
		char *p = getSymPtr(_at);
		int lCmp = getSymSize(_at);
		int lOrg = strlen(_cmpTo);
		return ( lOrg == lCmp && 0==memcmp(_cmpTo,p,lOrg) ); }

	__inline int symSearch( int _at, XTokDef *_P ){
		if( getTokenType(_at)==TOK_SYMBOL ){
			char *p = getSymPtr(_at);
			int lCmp = getSymSize(_at);
			do{
				int lOrg = strlen(_P->S);
				if( lOrg == lCmp && 0==memcmp(_P->S,p,lOrg) )
					return _P->Value;
			}
			while( (++_P)->S );
		}
		return -1; }

	__inline bool varSearch( int _at, XVarDef *_P ){
		if( getTokenType(_at)==TOK_SYMBOL ){
			if( getTokenType(_at+1)==TOK_DELIMITER ){
				do{
					if( symCompare( _at, _P->S ) ){
						if( _P->Type == CTE_FLOAT && getTokenType(_at+2)==TOK_FLOAT ){
							(*_P->PtrFloat) = getFloat( _at + 2 );
							return true; }
						if( _P->Type == CTE_INT && getTokenType(_at+2)==TOK_INT ){
							(*_P->PtrInt) = getInt( _at + 2 );
							return true; }
						if( _P->Type == CTE_CHARPTR && getTokenType(_at+2)==TOK_SYMBOL ){
							memcpy(_P->PtrChar, getSymPtr(_at+2), getSymSize(_at+2) );
							_P->PtrChar[getSymSize(_at+2)] = 0;
							return true; }
					}
				}
				while( (++_P)->S );
			}
		}
		return false; }

	void ReadRect( RECT *r ){
		XVarDef VarDef[] = {
			XVarDef( "left", CTE_INT, &r->left ),
			XVarDef( "right", CTE_INT, &r->right ),
			XVarDef( "top", CTE_INT, &r->top ),
			XVarDef( "bottom", CTE_INT, &r->bottom ),
			XVarDef( )
		};
		for(int i=0;i<getTokenCount();i=nextPth(i))
			varSearch(i,VarDef);
	};

};

//------------
// C-like constructions:
//   1. <varname> = <value>;
//   2. { ... tokens ... }
//------------


#endif /*_INCDEF_XSCRIPT_H_*/

