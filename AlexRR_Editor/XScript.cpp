// file: XScript.cpp

#include "pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"


XScr::XScr( char *_fileName, bool _bLoad ){

	_ASSERTE( _fileName );
	strcpy( scrName, _fileName );
	scrBuffer = 0;
	scrBufferSize = 0;
	scrCompiled = false;

	if( _bLoad ){
		if( !LoadFile() ){
			NConsole.print( "Can't load '%s' !", _fileName );
			return; }
		if( !scrCompiled )
			if( !Compile() ){
				NConsole.print( "Can't compile '%s' !", _fileName );
				return; }
	}
}

XScr::XScr( char *_Buffer, int _bufferSize, bool _Text ){

	_ASSERTE( _Buffer && _bufferSize );
	strcpy( scrName, "AutoScript" );

	if( _Text ){
		scrBuffer = new char[_bufferSize+1];
		memcpy(scrBuffer,_Buffer,_bufferSize);
		scrBufferSize = _bufferSize;
		scrCompiled = false;
		if( !Compile() ){
			NConsole.print( "Can't compile '%s' !", scrName );
			return; }
	}
	else{

		XBinScrHeader *_H = (XBinScrHeader *)_Buffer;
		if( _H->magic != sizeof(_H) ){
			NConsole.print( "Script '%s' is not valid !", scrName );
			return; };

		scrBufferSize = _H->dicSize;
		scrBuffer = new char[scrBufferSize];
		memcpy( scrBuffer, _Buffer + _H->dicOffset, scrBufferSize );

		scrTokens.resize( _H->tokenCount );
		memcpy( & (scrTokens[0]), _Buffer + _H->tokenOffset,  scrTokens.size() * sizeof(XToken) );

		scrCompiled = true;
	}
}

XScr::~XScr(){
	scrTokens.clear();
	if( scrBufferSize && scrBuffer )
		delete [] scrBuffer;
}


bool XScr::LoadFile(){

	char _ext[MAX_PATH];
	char _fil[MAX_PATH];
	char _pat[MAX_PATH];
	char _drv[MAX_PATH];

	_splitpath( scrName, _drv, _pat, _fil, _ext );

	if( 0==stricmp(_ext,".script") ){
		if( LoadText() ){
			scrCompiled = false;
			return true; }
		else {
			_makepath( scrName, _drv, _pat, _fil, ".bin" );
			if( LoadBinary() ){
				scrCompiled = true;
				return true; };
		}
	}
	else if( 0==stricmp(_ext,".bin") ){
		if( LoadBinary() ){
			scrCompiled = true;
			return true; };
	}

	return false;
}

bool XScr::LoadBinary(){

	XBinScrHeader _H;
	
	int hfile = _open( scrName, _O_RDONLY|_O_BINARY );
	if( hfile <=0 ){
		return false;
	}	
	
	_read( hfile, &_H, sizeof(_H) );
	
	if( _H.magic != sizeof(_H) ){
		_close( hfile );
		return false;
	}
	
	scrBufferSize = _H.dicSize;
	scrBuffer = new char[scrBufferSize];
	
	_lseek( hfile, _H.dicOffset, SEEK_SET );
	_read( hfile, scrBuffer, scrBufferSize );
	
	scrTokens.resize( _H.tokenCount );
	
	_lseek( hfile, _H.tokenOffset, SEEK_SET );
	_read( hfile, scrTokens.begin(), scrTokens.size() * sizeof(XToken) );
	
	_close( hfile );

	return true;
}

bool XScr::LoadText(){

	int hfile = _open( scrName, _O_RDONLY|_O_BINARY );
	if( hfile <=0 ){
		return false; }
	scrBufferSize = _filelength( hfile );
	if( scrBufferSize<=0 ){
		NConsole.print( "zero-length file: '%s'...", scrName );
		_close(hfile); return false; }
	scrBuffer = new char[scrBufferSize];
	if( scrBufferSize != _read( hfile, scrBuffer, scrBufferSize ) ){
		NConsole.print( "error reading file '%s'...", scrName );
		_close(hfile); return false; }
	_close( hfile );
	return true;

}

bool XScr::Compile(){
	_ASSERTE( !scrCompiled );

	cPtr = 0;
	while( cPtr<scrBufferSize )
		if( !trytoken() )
			return false;

	scrCompiled = true;
	return true;
}

bool XScr::SaveText  ( char *_fileName ) {

	_ASSERTE( _fileName );

	return true;
}

bool XScr::SaveBinary( char *_fileName ){

	_ASSERTE( _fileName );

	vector<XToken> _NewTokens;
	int _DictionarySize = 0;
	char *_NewBuffer = 0;
	
	for( int i=0; i<scrTokens.size(); i++){
		
		switch( scrTokens[i].tokType ){
		
		case TOK_INT:
		case TOK_FLOAT:
		case TOK_DELIMITER:
			_NewTokens.push_back( scrTokens[i] );
			break;

		case TOK_SYMBOL:{
			
			XToken _TmpTok = scrTokens[i];

			bool _AlreadyExist = false;
			for( int j=0; j<(_DictionarySize - _TmpTok.tokChars - 1); j++ )
				if( 0==memcmp( scrBuffer + scrTokens[i].tokFrom, _NewBuffer+j, _TmpTok.tokChars ) ){
					_TmpTok.tokFrom = j;
					_NewTokens.push_back( _TmpTok );
					_AlreadyExist = true;
					break; }

			if( !_AlreadyExist ){
				_TmpTok.tokFrom = _DictionarySize;
				_NewBuffer = (char*)realloc( _NewBuffer, _DictionarySize + _TmpTok.tokChars );
				memcpy( _NewBuffer+_DictionarySize, scrBuffer+scrTokens[i].tokFrom, _TmpTok.tokChars );
				_DictionarySize += _TmpTok.tokChars;
				_NewTokens.push_back( _TmpTok ); }
			break; }

		default:
			return false; }
	}

	if( _NewBuffer == 0 )
		return false;

	XBinScrHeader _H;
	_H.magic = sizeof(_H);
	_H.dicSize = _DictionarySize;
	_H.dicOffset = sizeof(_H);
	_H.tokenCount = _NewTokens.size();
	_H.tokenOffset = sizeof(_H) + _DictionarySize;
	
	int hfile = _open( _fileName, 
		_O_WRONLY|_O_BINARY|_O_CREAT|_O_TRUNC,
		_S_IREAD|_S_IWRITE );
	
	if( hfile <= 0 )
		return false;

	_write( hfile, &_H, sizeof(_H) );
	_write( hfile, _NewBuffer, _DictionarySize );
	_write( hfile, _NewTokens.begin(), _NewTokens.size() * sizeof(XToken) );
	_close( hfile );

	free( _NewBuffer );
	_NewTokens.clear();

	return true;
}

bool XScr::Merge( XScr *_S ){
	_ASSERTE( _S );
	if( _S->scrTokens.size() <= 0 )
		return false;
	for( int i=0; i<_S->scrTokens.size(); i++)
		AddToken( & _S->scrTokens[i], _S->scrBuffer );
	return true;
}

bool XScr::AddText( char *_Text, int _TextSize ){
	_ASSERTE( _Text && _TextSize );
	char *_Tmp = new char[scrBufferSize+_TextSize];
	memcpy( _Tmp, scrBuffer, scrBufferSize );
	memcpy( _Tmp + scrBufferSize, _Text, _TextSize );
	SAFE_DELETE_A( scrBuffer );
	scrBuffer = _Tmp;
	scrBufferSize += _TextSize;
	return true;
}

bool XScr::AddToken( XToken *_T, char *_Text ){
	_ASSERTE( _T && _Text );
	XToken _T2 = (*_T);
	_T2.tokFrom = scrBufferSize;
	AddText( _Text+_T->tokFrom, _T->tokChars );
	scrTokens.push_back( _T2 );
	return true;
}

//-------------- parsing

bool XScr::trytoken(){
	
	skipspaces();
	
	if( cPtr >= scrBufferSize )
		return true;

	if( trynumber() )
		return true;

	if( trysymbol() )
		return true;

	XToken tok;
	tok.tokType = TOK_DELIMITER;
	tok.tokFrom = cPtr;
	tok.tokChars = 1;
	tok.tokDelim = scrBuffer[cPtr++];
	
	scrTokens.push_back( tok );
	return true;
}


bool XScr::trynumber(){
	
	int cc = cPtr;
	int cctok = 0;
	bool bfloat = false;
	char tokbuf[256];

	if( scrBuffer[cc] == '.' || 
		scrBuffer[cc] == '+' ||
		scrBuffer[cc] == '-' ||
		isdigit( scrBuffer[cc] ) ){
			while( cc<=scrBufferSize ){
				if( scrBuffer[cc] == '.' || 
					scrBuffer[cc] == '+' ||
					scrBuffer[cc] == '-' ||
					isdigit( scrBuffer[cc] ) ){
						tokbuf[cctok++] = scrBuffer[cc];
						if( scrBuffer[cc] == '.' ) bfloat = true;
						cc++;
				}
				else
					break;
			}

			tokbuf[cctok] = 0;

			XToken tok;
			tok.tokFrom = cPtr;
			tok.tokChars = cctok;
			if( bfloat ){
				tok.tokFloat = (float)atof(tokbuf);
				tok.tokType = TOK_FLOAT;
			}
			else{
				tok.tokInt = (int)atoi(tokbuf);
				tok.tokType = TOK_INT;
			}

			cPtr = cc;
			scrTokens.push_back( tok );

			return true;
	}
	else
		return false;
}

bool XScr::trysymbol(){

	int cc = cPtr;
	int cctok = 0;
	
	if( scrBuffer[cc] == '\"' ){
		// until next quote ...
		cc++;
		while( cc<scrBufferSize ){
			if( scrBuffer[cc] == '\"' ){
				XToken tok;
				tok.tokFrom = cPtr+1;
				tok.tokChars = cctok;
				tok.tokType = TOK_SYMBOL;
				scrTokens.push_back( tok );
				cPtr = cc+1;
				return true;
			}
			cctok++;
			cc++;
		}
		return false;
	}
	else if( scrBuffer[cc] == '_' || isalpha(scrBuffer[cc])){
		// while allowed chars
		while( cc<scrBufferSize )
			if( scrBuffer[cc] == '_' || 
				isalpha(scrBuffer[cc]) ){
					cctok++;
					cc++;
			}
			else{
				XToken tok;
				tok.tokFrom = cPtr;
				tok.tokChars = cctok;
				tok.tokType = TOK_SYMBOL;
				scrTokens.push_back( tok );
				cPtr = cc;
				return true;
			}
	}

	return false;
}


bool XScr::skipspaces(){
	while( cPtr < scrBufferSize && (scrBuffer[cPtr] == ' ' ||
		scrBuffer[cPtr] == '\r' || scrBuffer[cPtr]=='\n' || scrBuffer[cPtr]=='\t') ) cPtr++;
	if( cPtr < (scrBufferSize-1) && scrBuffer[cPtr] == '/' && scrBuffer[cPtr+1] == '/' ){
		while( cPtr < scrBufferSize && scrBuffer[cPtr] != '\r' && scrBuffer[cPtr] != '\n' ) cPtr++;
		while( cPtr < scrBufferSize && (scrBuffer[cPtr] == '\r' || scrBuffer[cPtr] == '\n') ) cPtr++;
		return skipspaces(); }
	return true;
}

