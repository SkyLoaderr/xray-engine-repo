/*
www.sourceforge.net/projects/tinyxml
Original code (2.0 and earlier )copyright (c) 2000-2002 Lee Thomason (www.grinninglizard.com)

This software is provided 'as-is', without any express or implied 
warranty. In no event will the authors be held liable for any 
damages arising from the use of this software.

Permission is granted to anyone to use this software for any 
purpose, including commercial applications, and to alter it and 
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must 
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and 
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source 
distribution.
*/

#include "stdafx.h"
#include "tinyxml.h"
#include <ctype.h>

//#define DEBUG_PARSER

// Note tha "PutString" hardcodes the same list. This
// is less flexible than it appears. Changing the entries
// or order will break putstring.	
TiXmlBase::Entity TiXmlBase::entity[ NUM_ENTITY ] = 
{
	{ "&amp;",  5, '&' },
	{ "&lt;",   4, '<' },
	{ "&gt;",   4, '>' },
	{ "&quot;", 6, '\"' },
	{ "&apos;", 6, '\'' }
};

// Bunch of unicode info at:
//		http://www.unicode.org/faq/utf_bom.html
// Including the basic of this table, which determines the #bytes in the
// sequence from the lead byte. 0 placed for invalid sequences.
// Beware of the non-characters in UTF-8:	
//				ef bb bf (Microsoft "lead bytes")
//				ef bf be
//				ef bf bf 



const int TiXmlBase::utf8ByteTable[256] = 
{
	//	0	1	2	3	4	5	6	7	8	9	a	b	c	d	e	f
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x00
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x10
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x20
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x30
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x40
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x50
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x60
		1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	// 0x70	End of ASCII range
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	// 0x80 0x80 to 0xc1 invalid
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	// 0x90 
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	// 0xa0 
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	// 0xb0 
		0,	0,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	// 0xc0 0xc2 to 0xdf 2 byte
		2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	// 0xd0
		3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	// 0xe0 0xe0 to 0xef 3 byte
		4,	4,	4,	4,	4,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0	// 0xf0 0xf0 to 0xf4 4 byte, 0xf5 and higher invalid
};


void TiXmlBase::ConvertUTF32ToUTF8( unsigned long input, char* output, int* length )
{
	const unsigned long BYTE_MASK = 0xBF;
	const unsigned long BYTE_MARK = 0x80;
	const unsigned long FIRST_BYTE_MARK[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

	if (input < 0x80) 
		*length = 1;
	else if ( input < 0x800 )
		*length = 2;
	else if ( input < 0x10000 )
		*length = 3;
	else if ( input < 0x200000 )
		*length = 4;
	else
		{ *length = 0; return; }	// This code won't covert this correctly anyway.

	output += *length;

	// Scary scary fall throughs.
	switch (*length) 
	{
		case 4:
			--output; 
			*output = (char)((input | BYTE_MARK) & BYTE_MASK); 
			input >>= 6;
		case 3:
			--output; 
			*output = (char)((input | BYTE_MARK) & BYTE_MASK); 
			input >>= 6;
		case 2:
			--output; 
			*output = (char)((input | BYTE_MARK) & BYTE_MASK); 
			input >>= 6;
		case 1:
			--output; 
			*output = (char)(input | FIRST_BYTE_MARK[*length]);
	}
}


/*static*/ int TiXmlBase::IsAlphaUTF8( unsigned char anyByte )
{
	if ( anyByte < 127 )
		return isalpha( anyByte );
	else
		return 1;	// What else to do? The unicode set is huge...get the english ones right.
}


/*static*/ int TiXmlBase::IsAlphaNumUTF8( unsigned char anyByte )
{
	if ( anyByte < 127 )
		return isalnum( anyByte );
	else
		return 1;	// What else to do? The unicode set is huge...get the english ones right.
}

class TiXmlParsingData
{
	friend class TiXmlDocument;
  public:
	//TiXmlParsingData( const char* now, const TiXmlParsingData* prevData );
	void Stamp( const char* now );

	const TiXmlCursor& Cursor()	{ return cursor; }
	//void Update( const char* now );

  private:
	// Only used by the document!
	TiXmlParsingData( const char* start, int _tabsize, int row, int col )
	{
		assert( start );
		stamp = start;
		tabsize = _tabsize;
		cursor.row = row;
		cursor.col = col;
	}

	TiXmlCursor		cursor;
	const char*		stamp;
	int				tabsize;
};


void TiXmlParsingData::Stamp( const char* now )
{
	assert( now );

	// Do nothing if the tabsize is 0.
	if ( tabsize < 1 )
	{
		return;
	}

	// Get the current row, column.
	int row = cursor.row;
	int col = cursor.col;
	const char* p = stamp;
	assert( p );

	while ( p < now )
	{
		// Code contributed by Fletcher Dunn: (modified by lee)
		switch (*p) {
			case 0:
				// We *should* never get here, but in case we do, don't
				// advance past the terminating null character, ever
				return;

			case '\r':
				// bump down to the next line
				++row;
				col = 0;				
				// Eat the character
				++p;

				// Check for \r\n sequence, and treat this as a single character
				if (*p == '\n') {
					++p;
				}
				break;

			case '\n':
				// bump down to the next line
				++row;
				col = 0;

				// Eat the character
				++p;

				// Check for \n\r sequence, and treat this as a single
				// character.  (Yes, this bizarre thing does occur still
				// on some arcane platforms...)
				if (*p == '\r') {
					++p;
				}
				break;

			case '\t':
				// Eat the character
				++p;

				// Skip to next tab stop
				col = (col / tabsize + 1) * tabsize;
				break;

#pragma warning (disable: 4310)

			case (char)(0xef):
				if ( *(p+1) && *(p+2) )
				{
					// In these cases, don't advance the column. These are
					// 0-width spaces.
					if ( *(p+1)==(char)(0xbb) && *(p+2)==(char)(0xbf) )
						p += 3;	
					else if ( *(p+1)==(char)(0xbf) && *(p+2)==(char)(0xbe) )
						p += 3;	
					else if ( *(p+1)==(char)(0xbf) && *(p+2)==(char)(0xbf) )
						p += 3;	
					else
						{ p +=3; ++col; }	// A normal character.
				}
				break;

			default:
				// Eat the 1 to 4 byte utf8 character.
				int step = TiXmlBase::utf8ByteTable[*((unsigned char*)p)];
				if ( step == 0 )
					step = 1;		// Error case from bad encoding, but handle gracefully.
				p += step;

				// Just advance one column, of course.
				++col;
				break;
		}
	}
	cursor.row = row;
	cursor.col = col;
	assert( cursor.row >= -1 );
	assert( cursor.col >= -1 );
	stamp = p;
	assert( stamp );
}


const char* TiXmlBase::SkipWhiteSpace( const char* p )
{
	if ( !p || !*p )
	{
		return 0;
	}
	while ( p && *p )
	{
		// Skip the stupid Microsoft UTF-8 Byte order marks
		if (	*(p+0)==(char) 0xef 
			 && *(p+1)==(char) 0xbb 
			 && *(p+2)==(char) 0xbf )
		{
			p += 3;
			continue;
		}
		else if(*(p+0)==(char) 0xef
			 && *(p+1)==(char) 0xbf
			 && *(p+2)==(char) 0xbe )
		{
			p += 3;
			continue;
		}
		else if(*(p+0)==(char) 0xef
			 && *(p+1)==(char) 0xbf
			 && *(p+2)==(char) 0xbf )
		{
			p += 3;
			continue;
		}

		if ( IsWhiteSpace( *p ) || *p == '\n' || *p =='\r' )		// Still using old rules for white space.
			++p;
		else
			break;
	}

	return p;
}

#pragma warning (default: 4310)

#ifdef TIXML_USE_STL
/*static*/ bool TiXmlBase::StreamWhiteSpace( TIXML_ISTREAM * in, TIXML_STRING * tag )
{
	for( ;; )
	{
		if ( !in->good() ) return false;

		int c = in->peek();
		if ( !IsWhiteSpace( c ) )
			return true;
		*tag += (char) in->get();
	}
}

/*static*/ bool TiXmlBase::StreamTo( TIXML_ISTREAM * in, int character, TIXML_STRING * tag )
{
	assert( character > 0 && character < 128 );	// else it won't work in utf-8
	while ( in->good() )
	{
		int c = in->peek();
		if ( c == character )
			return true;

		in->get();
		*tag += (char) c;
	}
	return false;
}
#endif

const char* TiXmlBase::ReadName( const char* p, TIXML_STRING * name )
{
	*name = "";
	assert( p );

	// Names start with letters or underscores.
	// Of course, in unicode, tinyxml has no idea what a letter *is*. The
	// algorithm is generous.
	//
	// After that, they can be letters, underscores, numbers,
	// hyphens, or colons. (Colons are valid ony for namespaces,
	// but tinyxml can't tell namespaces from names.)
	if (    p && *p 
		 && ( IsAlphaUTF8( (unsigned char) *p ) || *p == '_' ) )
	{
		while(		p && *p
				&&	(		IsAlphaNumUTF8( (unsigned char ) *p ) 
						 || *p == '_'
						 || *p == '-'
						 || *p == '.'
						 || *p == ':' ) )
		{
			(*name) += *p;
			++p;
		}
		return p;
	}
	return 0;
}

const char* TiXmlBase::GetEntity( const char* p, char* value, int* length )
{
	// Presume an entity, and pull it out.
    TIXML_STRING ent;
	int i;
	*length = 0;

	if ( *(p+1) && *(p+1) == '#' && *(p+2) )
	{
		unsigned long ucs = 0;
		unsigned delta = 0;
		unsigned mult = 1;

		if ( *(p+2) == 'x' )
		{
			// Hexadecimal.
			if ( !*(p+3) ) return 0;

			const char* q = p+3;
			q = strchr( q, ';' );

			if ( !q || !*q ) return 0;

			delta = static_cast<unsigned int>(q-p);
			--q;

			while ( *q != 'x' )
			{
				if ( *q >= '0' && *q <= '9' )
					ucs += mult * (*q - '0');
				else if ( *q >= 'a' && *q <= 'f' )
					ucs += mult * (*q - 'a' + 10);
				else if ( *q >= 'A' && *q <= 'F' )
					ucs += mult * (*q - 'A' + 10 );
				else 
					return 0;
				mult *= 16;
				--q;
			}
		}
		else
		{
			// Decimal.
			if ( !*(p+2) ) return 0;

			const char* q = p+2;
			q = strchr( q, ';' );

			if ( !q || !*q ) return 0;

			delta = static_cast<unsigned int>(q-p);
			--q;

			while ( *q != '#' )
			{
				if ( *q >= '0' && *q <= '9' )
					ucs += mult * (*q - '0');
				else 
					return 0;
				mult *= 10;
				--q;
			}
		}
		// convert the UCS to UTF-8
		ConvertUTF32ToUTF8( ucs, value, length );
		return p + delta + 1;
	}

	// Now try to match it.
	for( i=0; i<NUM_ENTITY; ++i )
	{
		if ( strncmp( entity[i].str, p, entity[i].strLength ) == 0 )
		{
			assert( xr_strlen( entity[i].str ) == entity[i].strLength );
			*value = entity[i].chr;
			*length = 1;
			return ( p + entity[i].strLength );
		}
	}

	// So it wasn't an entity, its unrecognized, or something like that.
	*value = *p;	// Don't put back the last one, since we return it!
	return p+1;
}


bool TiXmlBase::StringEqual( const char* p,
							 const char* tag,
							 bool ignoreCase )
{
	assert( p );
	assert( tag );
	if ( !p || !*p )
	{
		assert( 0 );
		return false;
	}

	const char* q = p;

	if ( ignoreCase )
	{
		while ( *q && *tag && ToLowerUTF8( *q ) == ToLowerUTF8( *tag ) )
		{
			++q;
			++tag;
		}

		if ( *tag == 0 )
			return true;
	}
	else
	{
		while ( *q && *tag && *q == *tag )
		{
			++q;
			++tag;
		}

		if ( *tag == 0 )		// Have we found the end of the tag, and everything equal?
			return true;
	}
	return false;
}

const char* TiXmlBase::ReadText(	const char* p, 
									TIXML_STRING * text, 
									bool trimWhiteSpace, 
									const char* endTag, 
									bool caseInsensitive )
{
    *text = "";
	if (    !trimWhiteSpace			// certain tags always keep whitespace
		 || !condenseWhiteSpace )	// if true, whitespace is always kept
	{
		// Keep all the white space.
		while (	   p && *p
				&& !StringEqual( p, endTag, caseInsensitive )
			  )
		{
			int len;
			char cArr[4];
			if (UTF_8 == docEncoding)
			{
				p = GetCharUTF8( p, cArr, &len );
				R_ASSERT2(p, "Prohibitive character in UTF-8 encoding");
			}	
			else
			{
				cArr[0] = *p++;
				len = 1;
			}
			text->append( cArr, len );
		}
	}
	else
	{
		bool whitespace = false;

		// Remove leading white space:
		p = SkipWhiteSpace( p );
		while (	   p && *p
				&& !StringEqual( p, endTag, caseInsensitive ) )
		{
			if ( *p == '\r' || *p == '\n' )
			{
				whitespace = true;
				++p;
			}
			else if ( IsWhiteSpace( *p ) )
			{
				whitespace = true;
				++p;
			}
			else
			{
				// If we've found whitespace, add it before the
				// new character. Any whitespace just becomes a space.
				if ( whitespace )
				{
					(*text) += ' ';
					whitespace = false;
				}
				char cArr[4];
				int len;
				if (UTF_8 == docEncoding)
				{
					p = GetCharUTF8( p, cArr, &len );
					R_ASSERT2(p, "Prohibitive character in UTF-8 encoding");
				}
				else
				{
					cArr[0] = *p++;
					len = 1;
				}
				if ( len == 1 )
					(*text) += cArr[0];	// more efficient
				else
					text->append( cArr, len );
			}
		}
	}
	return p + xr_strlen( endTag );
}

#ifdef TIXML_USE_STL

void TiXmlDocument::StreamIn( TIXML_ISTREAM * in, TIXML_STRING * tag )
{
	// The basic issue with a document is that we don't know what we're
	// streaming. Read something presumed to be a tag (and hope), then
	// identify it, and call the appropriate stream method on the tag.
	//
	// This "pre-streaming" will never read the closing ">" so the
	// sub-tag can orient itself.

	if ( !StreamTo( in, '<', tag ) ) 
	{
		SetError( TIXML_ERROR_PARSING_EMPTY, 0, 0 );
		return;
	}

	while ( in->good() )
	{
		int tagIndex = (int) tag->length();
		while ( in->good() && in->peek() != '>' )
		{
			int c = in->get();
			(*tag) += (char) c;
		}

		if ( in->good() )
		{
			// We now have something we presume to be a node of 
			// some sort. Identify it, and call the node to
			// continue streaming.
			TiXmlNode* node = Identify( tag->c_str() + tagIndex );

			if ( node )
			{
				node->StreamIn( in, tag );
				bool isElement = node->ToElement() != 0;
				delete node;
				node = 0;

				// If this is the root element, we're done. Parsing will be
				// done by the >> operator.
				if ( isElement )
				{
					return;
				}
			}
			else
			{
				SetError( TIXML_ERROR, 0, 0 );
				return;
			}
		}
	}
	// We should have returned sooner.
	SetError( TIXML_ERROR, 0, 0 );
}

#endif

const char* TiXmlDocument::Parse( const char* p, TiXmlParsingData* prevData )
{
	ClearError();

	// Parse away, at the document level. Since a document
	// contains nothing but other tags, most of what happens
	// here is skipping white space.
	if ( !p || !*p )
	{
		SetError( TIXML_ERROR_DOCUMENT_EMPTY, 0, 0 );
		return 0;
	}

	// Note that, for a document, this needs to come
	// before the while space skip, so that parsing
	// starts from the pointer we are given.
	location.Clear();
	if ( prevData )
	{
		location.row = prevData->cursor.row;
		location.col = prevData->cursor.col;
	}
	else
	{
		location.row = 0;
		location.col = 0;
	}
	TiXmlParsingData data( p, TabSize(), location.row, location.col );
	location = data.Cursor();

    p = SkipWhiteSpace( p );
	if ( !p )
	{
		SetError( TIXML_ERROR_DOCUMENT_EMPTY, 0, 0 );
		return 0;
	}

	while ( p && *p )
	{
		TiXmlNode* node = Identify( p );
		if ( node )
		{
			p = node->Parse( p, &data );
			LinkEndChild( node );
		}
		else
		{
			break;
		}
		p = SkipWhiteSpace( p );
	}

	// All is well.
	return p;
}

void TiXmlDocument::SetError( int err, const char* pError, TiXmlParsingData* data )
{	
	// The first error in a chain is more accurate - don't set again!
	if ( error )
		return;

	assert( err > 0 && err < TIXML_ERROR_STRING_COUNT );
	error   = true;
	errorId = err;
	errorDesc = errorString[ errorId ];

	errorLocation.Clear();
	if ( pError && data )
	{
		//TiXmlParsingData data( pError, prevData );
		data->Stamp( pError );
		errorLocation = data->Cursor();
	}
}


TiXmlNode* TiXmlNode::Identify( const char* p )
{
	TiXmlNode* returnNode = 0;

	p = SkipWhiteSpace( p );
	if( !p || !*p || *p != '<' )
	{
		return 0;
	}

	TiXmlDocument* doc = GetDocument();
	p = SkipWhiteSpace( p );

	if ( !p || !*p )
	{
		return 0;
	}

	// What is this thing? 
	// - Elements start with a letter or underscore, but xml is reserved.
	// - Comments: <!--
	// - Decleration: <?xml
	// - Everthing else is unknown to tinyxml.
	//

	const char* xmlHeader = { "<?xml" };
	const char* commentHeader = { "<!--" };
	const char* dtdHeader = { "<!" };

	if ( StringEqual( p, xmlHeader, true ) )
	{
		#ifdef DEBUG_PARSER
			TIXML_LOG( "XML parsing Declaration\n" );
		#endif
		returnNode = xr_new<TiXmlDeclaration>();
	}
	else if ( StringEqual( p, commentHeader, false ) )
	{
		#ifdef DEBUG_PARSER
			TIXML_LOG( "XML parsing Comment\n" );
		#endif
		returnNode = xr_new<TiXmlComment>();
	}
	else if ( StringEqual( p, dtdHeader, false ) )
	{
		#ifdef DEBUG_PARSER
			TIXML_LOG( "XML parsing Unknown(1)\n" );
		#endif
		returnNode = xr_new<TiXmlUnknown>();
	}
	else if (    IsAlphaUTF8( *(p+1) )
			  || *(p+1) == '_' )
	{
		#ifdef DEBUG_PARSER
			TIXML_LOG( "XML parsing Element\n" );
		#endif
		returnNode = xr_new<TiXmlElement>( "" );
	}
	else
	{
		#ifdef DEBUG_PARSER
			TIXML_LOG( "XML parsing Unknown(2)\n" );
		#endif
		returnNode = xr_new<TiXmlUnknown>();
	}

	if ( returnNode )
	{
		// Set the parent, so it can report errors
		returnNode->parent = this;
	}
	else
	{
		if ( doc )
			doc->SetError( TIXML_ERROR_OUT_OF_MEMORY, 0, 0 );
	}
	return returnNode;
}

#ifdef TIXML_USE_STL

void TiXmlElement::StreamIn (TIXML_ISTREAM * in, TIXML_STRING * tag)
{
	// We're called with some amount of pre-parsing. That is, some of "this"
	// element is in "tag". Go ahead and stream to the closing ">"
	while( in->good() )
	{
		int c = in->get();
		(*tag) += (char) c ;
		
		if ( c == '>' )
			break;
	}

	if ( tag->length() < 3 ) return;

	// Okay...if we are a "/>" tag, then we're done. We've read a complete tag.
	// If not, identify and stream.

	if (    tag->at( tag->length() - 1 ) == '>' 
		 && tag->at( tag->length() - 2 ) == '/' )
	{
		// All good!
		return;
	}
	else if ( tag->at( tag->length() - 1 ) == '>' )
	{
		// There is more. Could be:
		//		text
		//		closing tag
		//		another node.
		for ( ;; )
		{
			StreamWhiteSpace( in, tag );

			// Do we have text?
			if ( in->good() && in->peek() != '<' ) 
			{
				// Yep, text.
				TiXmlText text( "" );
				text.StreamIn( in, tag );

				// What follows text is a closing tag or another node.
				// Go around again and figure it out.
				continue;
			}

			// We now have either a closing tag...or another node.
			// We should be at a "<", regardless.
			if ( !in->good() ) return;
			assert( in->peek() == '<' );
			int tagIndex = tag->length();

			bool closingTag = false;
			bool firstCharFound = false;

			for( ;; )
			{
				if ( !in->good() )
					return;

				int c = in->peek();
				
				if ( c == '>' )
					break;

				*tag += (char) c;
				in->get();

				if ( !firstCharFound && c != '<' && !IsWhiteSpace( c ) )
				{
					firstCharFound = true;
					if ( c == '/' )
						closingTag = true;
				}
			}
			// If it was a closing tag, then read in the closing '>' to clean up the input stream.
			// If it was not, the streaming will be done by the tag.
			if ( closingTag )
			{
				int c = in->get();
				assert( c == '>' );
				*tag += (char) c;

				// We are done, once we've found our closing tag.
				return;
			}
			else
			{
				// If not a closing tag, id it, and stream.
				const char* tagloc = tag->c_str() + tagIndex;
				TiXmlNode* node = Identify( tagloc );
				if ( !node )
					return;
				node->StreamIn( in, tag );
				delete node;
				node = 0;

				// No return: go around from the beginning: text, closing tag, or node.
			}
		}
	}
}
#endif

const char* TiXmlElement::Parse( const char* p, TiXmlParsingData* data )
{
	p = SkipWhiteSpace( p );
	TiXmlDocument* document = GetDocument();

	if ( !p || !*p )
	{
		if ( document ) document->SetError( TIXML_ERROR_PARSING_ELEMENT, 0, 0 );
		return 0;
	}

//	TiXmlParsingData data( p, prevData );
	if ( data )
	{
		data->Stamp( p );
		location = data->Cursor();
	}

	if ( *p != '<' )
	{
		if ( document ) document->SetError( TIXML_ERROR_PARSING_ELEMENT, p, data );
		return 0;
	}

	p = SkipWhiteSpace( p+1 );

	// Read the name.
	const char* pErr = p;

    p = ReadName( p, &value );
	if ( !p || !*p )
	{
		if ( document )	document->SetError( TIXML_ERROR_FAILED_TO_READ_ELEMENT_NAME, pErr, data );
		return 0;
	}

    TIXML_STRING endTag ("</");
	endTag += value;
	endTag += ">";

	// Check for and read attributes. Also look for an empty
	// tag or an end tag.
	while ( p && *p )
	{
		pErr = p;
		p = SkipWhiteSpace( p );
		if ( !p || !*p )
		{
			if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES, pErr, data );
			return 0;
		}
		if ( *p == '/' )
		{
			++p;
			// Empty tag.
			if ( *p  != '>' )
			{
				if ( document ) document->SetError( TIXML_ERROR_PARSING_EMPTY, p, data );		
				return 0;
			}
			return (p+1);
		}
		else if ( *p == '>' )
		{
			// Done with attributes (if there were any.)
			// Read the value -- which can include other
			// elements -- read the end tag, and return.
			++p;
			p = ReadValue( p, data );		// Note this is an Element method, and will set the error if one happens.
			if ( !p || !*p )
				return 0;

			// We should find the end tag now
			if ( StringEqual( p, endTag.c_str(), false ) )
			{
				p += endTag.length();
				return p;
			}
			else
			{
				if ( document ) document->SetError( TIXML_ERROR_READING_END_TAG, p, data );
				return 0;
			}
		}
		else
		{
			// Try to read an attribute:
			TiXmlAttribute* attrib = xr_new<TiXmlAttribute>();
			if ( !attrib )
			{
				if ( document ) document->SetError( TIXML_ERROR_OUT_OF_MEMORY, pErr, data );
				return 0;
			}

			attrib->SetDocument( document );
			const char* pErr = p;
			p = attrib->Parse( p, data );

			if ( !p || !*p )
			{
				if ( document ) document->SetError( TIXML_ERROR_PARSING_ELEMENT, pErr, data );
				delete attrib;
				return 0;
			}

			// Handle the strange case of double attributes:
			TiXmlAttribute* node = attributeSet.Find( attrib->Name() );
			if ( node )
			{
				node->SetValue( attrib->Value() );
				delete attrib;
				return 0;
			}

			attributeSet.Add( attrib );
		}
	}
	return p;
}


const char* TiXmlElement::ReadValue( const char* p, TiXmlParsingData* data )
{
	TiXmlDocument* document = GetDocument();

	const char* pWithWhiteSpace = p;
	// Read in text and elements in any order.
	p = SkipWhiteSpace( p );
	while ( p && *p )
	{
		if ( *p != '<' )
		{
			// Take what we have, make a text element.
			TiXmlText* textNode = xr_new<TiXmlText>( "" );

			if ( !textNode )
			{
				if ( document ) document->SetError( TIXML_ERROR_OUT_OF_MEMORY, 0, 0 );
				    return 0;
			}

			if ( TiXmlBase::IsWhiteSpaceCondensed() )
			{
				p = textNode->Parse( p, data );
			}
			else
			{
				// Special case: we want to keep the white space
				// so that leading spaces aren't removed.
				p = textNode->Parse( pWithWhiteSpace, data );
			}

			if ( !textNode->Blank() )
				LinkEndChild( textNode );
			else
				delete textNode;
		} 
		else 
		{
			// We hit a '<'
			// Have we hit a new element or an end tag?
			if ( StringEqual( p, "</", false ) )
			{
				return p;
			}
			else
			{
				TiXmlNode* node = Identify( p );
				if ( node )
				{
					p = node->Parse( p, data );
					LinkEndChild( node );
				}				
				else
				{
					return 0;
				}
			}
		}
		p = SkipWhiteSpace( p );
	}

	if ( !p )
	{
		if ( document ) document->SetError( TIXML_ERROR_READING_ELEMENT_VALUE, 0, 0 );
	}	
	return p;
}


#ifdef TIXML_USE_STL
void TiXmlUnknown::StreamIn( TIXML_ISTREAM * in, TIXML_STRING * tag )
{
	while ( in->good() )
	{
		int c = in->get();	
		(*tag) += (char) c;

		if ( c == '>' )
		{
			// All is well.
			return;		
		}
	}
}
#endif


const char* TiXmlUnknown::Parse( const char* p, TiXmlParsingData* data )
{
	TiXmlDocument* document = GetDocument();
	p = SkipWhiteSpace( p );

//	TiXmlParsingData data( p, prevData );
	if ( data )
	{
		data->Stamp( p );
		location = data->Cursor();
	}
	if ( !p || !*p || *p != '<' )
	{
		if ( document ) document->SetError( TIXML_ERROR_PARSING_UNKNOWN, p, data );
		return 0;
	}
	++p;
    value = "";

	while ( p && *p && *p != '>' )
	{
		value += *p;
		++p;
	}

	if ( !p )
	{
		if ( document )	document->SetError( TIXML_ERROR_PARSING_UNKNOWN, 0, 0 );
	}
	if ( *p == '>' )
		return p+1;
	return p;
}

#ifdef TIXML_USE_STL
void TiXmlComment::StreamIn( TIXML_ISTREAM * in, TIXML_STRING * tag )
{
	while ( in->good() )
	{
		int c = in->get();	
		(*tag) += (char) c;

		if ( c == '>' 
			 && tag->at( tag->length() - 2 ) == '-'
			 && tag->at( tag->length() - 3 ) == '-' )
		{
			// All is well.
			return;		
		}
	}
}
#endif


const char* TiXmlComment::Parse( const char* p, TiXmlParsingData* data )
{
	TiXmlDocument* document = GetDocument();
	value = "";

	p = SkipWhiteSpace( p );

//	TiXmlParsingData data( p, prevData );
	if ( data )
	{
		data->Stamp( p );
		location = data->Cursor();
	}
	const char* startTag = "<!--";
	const char* endTag   = "-->";

	if ( !StringEqual( p, startTag, false ) )
	{
		document->SetError( TIXML_ERROR_PARSING_COMMENT, p, data );
		return 0;
	}
	p += xr_strlen( startTag );
	p = ReadText( p, &value, false, endTag, false );
	return p;
}


const char* TiXmlAttribute::Parse( const char* p, TiXmlParsingData* data )
{
	p = SkipWhiteSpace( p );
	if ( !p || !*p ) return 0;

	int tabsize = 4;
	if ( document )
		tabsize = document->TabSize();

//	TiXmlParsingData data( p, prevData );
	if ( data )
	{
		data->Stamp( p );
		location = data->Cursor();
	}
	// Read the name, the '=' and the value.
	const char* pErr = p;
	p = ReadName( p, &name );
	if ( !p || !*p )
	{
		if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES, pErr, data );
		return 0;
	}
	p = SkipWhiteSpace( p );
	if ( !p || !*p || *p != '=' )
	{
		if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES, p, data );
		return 0;
	}

	++p;	// skip '='
	p = SkipWhiteSpace( p );
	if ( !p || !*p )
	{
		if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES, p, data );
		return 0;
	}
	
	const char* end;

	if ( *p == '\'' )
	{
		++p;
		end = "\'";
		p = ReadText( p, &value, false, end, false );
	}
	else if ( *p == '"' )
	{
		++p;
		end = "\"";
		p = ReadText( p, &value, false, end, false );
	}
	else
	{
		// All attribute values should be in single or double quotes.
		// But this is such a common error that the parser will try
		// its best, even without them.
		value = "";
		while (    p && *p										// existence
				&& !IsWhiteSpace( *p ) && *p != '\n' && *p != '\r'	// whitespace
				&& *p != '/' && *p != '>' )						// tag end
		{
			value += *p;
			++p;
		}
	}
	return p;
}

#ifdef TIXML_USE_STL
void TiXmlText::StreamIn( TIXML_ISTREAM * in, TIXML_STRING * tag )
{
	while ( in->good() )
	{
		int c = in->peek();	
		if ( c == '<' )
			return;

		(*tag) += (char) c;
		in->get();
	}
}
#endif

const char* TiXmlText::Parse( const char* p, TiXmlParsingData* data )
{
	value = "";
//	TiXmlParsingData data( p, prevData );
	if ( data )
	{
		data->Stamp( p );
		location = data->Cursor();
	}
	bool ignoreWhite = true;

	const char* end = "<";
	p = ReadText( p, &value, ignoreWhite, end, false );
	if ( p )
		return p-1;	// don't truncate the '<'
	return 0;
}

#ifdef TIXML_USE_STL
void TiXmlDeclaration::StreamIn( TIXML_ISTREAM * in, TIXML_STRING * tag )
{
	while ( in->good() )
	{
		int c = in->get();
		(*tag) += (char) c;

		if ( c == '>' )
		{
			// All is well.
			return;
		}
	}
}
#endif

const char* TiXmlDeclaration::Parse( const char* p, TiXmlParsingData* data )
{
	p = SkipWhiteSpace( p );
	// Find the beginning, find the end, and look for
	// the stuff in-between.
	TiXmlDocument* document = GetDocument();
	if ( !p || !*p || !StringEqual( p, "<?xml", true ) )
	{
		if ( document ) document->SetError( TIXML_ERROR_PARSING_DECLARATION, 0, 0 );
		return 0;
	}
//	TiXmlParsingData data( p, prevData );
	if ( data )
	{
		data->Stamp( p );
		location = data->Cursor();
	}
	p += 5;

	version = "";
	encoding = "";
	standalone = "";

	while ( p && *p )
	{
		if ( *p == '>' )
		{
			++p;
			return p;
		}

		p = SkipWhiteSpace( p );
		if ( StringEqual( p, "version", true ) )
		{
			TiXmlAttribute attrib;
			p = attrib.Parse( p, data );		
			version = attrib.Value();
		}
		else if ( StringEqual( p, "encoding", true ) )
		{
			TiXmlAttribute attrib;
			p = attrib.Parse( p, data );		
			encoding = attrib.Value();
			// Store encoding id
			if (encoding == "windows-1251") docEncoding = windows_1251;
			else docEncoding = UTF_8;
		}
		else if ( StringEqual( p, "standalone", true ) )
		{
			TiXmlAttribute attrib;
			p = attrib.Parse( p, data );		
			standalone = attrib.Value();
		}
		else
		{
			// Read over whatever it is.
			while( p && *p && *p != '>' && !IsWhiteSpace( *p ) )
				++p;
		}
	}
	return 0;
}

bool TiXmlText::Blank() const
{
	for ( unsigned i=0; i<value.length(); i++ )
		if ( !IsWhiteSpace( value[i] ) )
			return false;
	return true;
}

