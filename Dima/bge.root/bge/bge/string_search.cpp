#include "stdafx.h"
#include "cpu.h"
#include "ui.h"

//#define smart_cast_list_file "smart_cast_list0.h"
//#include "smart_cast.h"

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

class CBoyerMooreSimple {
private:
	char		m_table[256];
	LPCSTR		m_pattern;
	int			m_length;

public:
				CBoyerMooreSimple	(LPCSTR pattern)
	{
		m_pattern		= pattern;
		m_length		= (int)strlen(m_pattern);

		for (int i=0; i<256; ++i)
			m_table[i]	= (char)m_length;

		for (int i=m_length - 1; i>=0; --i)
			if ((int)m_table[m_pattern[i]] == m_length)
				m_table[m_pattern[i]] = char(m_length - i - 1);
	}

	IC	LPCSTR	search				(LPCSTR str, int length, int start = 0) const
	{
		int				pos = start + m_length - 1;
		while (pos < length) {
			if (m_pattern[m_length - 1] != str[pos]) {
				pos		+= m_table[str[pos]];
				continue;
			}
			for (int i = m_length - 2; i>=0; --i) {
				if (m_pattern[i] != str[pos - m_length + i + 1]) {
					++pos;
					break;
				}
				if (!i)
					return	(str + (pos - m_length + 1));
			}
		}
		return				(0);
	}
};

/**
class CBoyerMooreComplex {
private:
	typedef char TABLE[256];

private:
	xr_vector<TABLE>	m_table;
	LPCSTR				m_pattern;
	int					m_length

public:
				CBoyerMooreComplex	(LPCSTR pattern)
	{
		m_pattern		= pattern;
		m_length		= strlen(pattern);
		LPSTR			suffix = (LPSTR)malloc(m_length*sizeof(char));

		m_table.resize	(m_length);

		for (int i=0; i<256; ++i)
			m_table[m_length - 1][i]	= (char)m_length;

		for (int i=m_length, max_shift; i>0; --i) {
			if (m_table[m_length - 1][m_pattern[i - 1]] == m_length)
				m_table[m_length - 1][m_pattern[i - 1]]	= m_length - i;

			max_shift	= m_length;

			for (int k=m_length - 1; k>=0; --k) {
				strcpy	(suffix,m_pattern + k);
				if (strstr(suffix,m_pattern) == m_pattern)
					max_shift	= k;

				for (int j=0, cur_shift; j<256; ++j) {
					cur_shift	= max_shift;

				}
			}
		}

		free			(suffix);
	}

	IC	LPCSTR	search				(LPCSTR str, int length, int start = 0) const
	{
		int					pos = start + m_length - 1;
		while (pos < length) {
			for (int i = m_length - 1; i>=0; --i) {
				if (m_table[i-1][str[pos - m_length + i]]) {
					pos		+= m_table[i-1][str[pos - m_length + i]];
					break;
				}
				if (!i)
					return	(str + (pos - m_length + 1));
			}
		}
		return				(0);
	}
};
/**/

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

const int ASIZE = 256;
const int XSIZE = 16;

IC void PRE_KMP( char *x, int m, int kmp_next[] ) {
 int i, j; 
 
 i=0; 
 j=kmp_next[0]=-1; 
 while ( i < m ) {
    while ( j > -1 && x[ i ] != x[ j ] ) j=kmp_next[ j ]; 
   i++; 
   j++; 
   if ( x[ i ] == x[ j ] ) kmp_next[i]=kmp_next[j]; 
      else kmp_next[ i ] = j; 
   }
}

IC LPCSTR KMP( LPCSTR y, LPCSTR x, int n, int m , int kmp_next[XSIZE]) {
   int i, j; 
   
   /* Searching */
   
   i=j=0; 
   while ( i < n ) {
      while ( j > -1 && x[ j ] != y[ i ] ) j = kmp_next[ j ];
     i++; 
     j++; 
     if ( j >= m ) {
	   return (y + i - j);
//       OUTPUT( i - j ); 
//     j = kmp_next[ j ]; 
       }
   }

   return		(0);
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

void PRE_BC( LPCSTR x, int m, int bm_bc[] ) {
   int a, j; 
   
   for ( a=0; a < ASIZE; a++ ) bm_bc[ a ] = m; 
   for ( j=0; j < m-1;  j++ ) 
	 if (bm_bc[ (unsigned char)x[ j ] ] == m)
		bm_bc[ (unsigned char)x[ j ] ] = m - j - 1; 
  }
 
 /* Preprocessing of the Good Suffix function shift  */
void  PRE_GS( LPCSTR x, int m,  int bm_gs[] ) {
   int i, j, p, f[XSIZE];
   
   memset( bm_gs, 0, ( m + 1 ) * sizeof( int ) );
   f[ m ] = j = m + 1; 
   for (i=m; i > 0; i-- ) {
      while ( j <= m && x[ i - 1 ] != x[ j - 1 ] ) {
        if ( bm_gs[ j ] == 0 ) bm_gs[ j ] = j - 1;
       j = f[ j ];
     }
     f[ i - 1 ] = --j; 
   }
   p=f[ 0 ]; 
   for ( j=0; j <= m; ++j ) {
      if ( bm_gs[ j ] == 0 ) bm_gs[ j ] = p; 
     if ( j == p ) p = f[ p ]; 
   }
}

#define MAX(a,b) (a)>=(b) ? (a) : (b)
#define MIN(a,b) (a)<=(b) ? (a) : (b)

LPCSTR BM( LPCSTR y, LPCSTR x, int n, int m, int bm_gs[XSIZE], int bm_bc[ASIZE]) {
  int i, j;
  
  i=0; 
  while ( i <= n-m ) {
     for ( j=m-1; j >= 0 && x[j] == y[ i+j ];  --j ); 
    if ( j < 0 )
       return(y + i); 
    else i += MAX(( bm_gs[ j+1 ]), ( bm_bc[ (unsigned char)y[ i+j ] ] - m + j + 1 ) ); 
   }
  return 0;
}

/* Boyer-Moore string matching algorithm */
LPCSTR TBM( LPCSTR y, LPCSTR x, int n, int m, int bm_gs[XSIZE], int bm_bc[ASIZE]) {
  int i, j, u, shift, v, turbo_shift, bc_shift;
  
  i = u = 0;
  shift = m;
  while ( i <= n-m ) {
     j = m - 1;
    while ( j >= 0 && x[ j ] == y[ i + j ] ) {
       --j;
      if ( u != 0 && j == m - 1 - shift ) j -= u;
   }
   if ( j < 0 )
      return (y + i);
   else {
      v = m - 1 - j;
      turbo_shift = u - v;
      bc_shift = bm_bc[ y[ i+j ] ] - m + j + 1;
      shift = MAX ( turbo_shift, bc_shift );
      shift = MAX ( shift, bm_gs[ j+1 ] );
      if ( shift == bm_gs[ j+1 ]) u = MIN( (m - shift), v );
      else {
       if ( turbo_shift < bc_shift ) shift = MAX(shift, (u+1) );
       u = 0;
      }
   }
   i += shift;
 }
  return 0;
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

struct State {
   int length, suf, pos;
   char terminal;
};

typedef struct State STATE;

int state_counter;
/* Creates and returns a clone of state p. */
int COPY ( int p , STATE states[] , int trans[ XSIZE ][ ASIZE ])
{
 int r;
 
 r = state_counter++;
 memcpy( trans[ r ] , trans[ p ] , ASIZE * sizeof( int ));
 states[ r ].suf = states[ p ].suf;
 states[ r ].pos = states[ p ].pos;
 return (r);
}
/* Creates the suffix automation for the reverse of the word x and */
/*  returns its initial state.                              */
int SUFF( LPCSTR x, int m, STATE states[], int trans[ XSIZE ][ ASIZE ])
{
 int i, art, init, last, p, q, r;
 char a;
 
 art = state_counter++;
 init = state_counter++;
 states[ init ].suf = art;
 last = init;
 for ( i = m - 1; i >= 0; --i) {
    a = x[ i ];
   p = last;
   q = state_counter++;
   states[ q ].length = states[ p ].length + 1;
   states[ q ].pos = states[ p ].pos + 1;
   while ( p != init && trans[ p ][ a ] == 0 ) {
      trans[ p ][ a ] = q;
      p = states[ p ].suf;
   }
   if ( trans[ p ][ a ] == 0 ) {
      trans[ init ][ a ] = q;
      states[ q ].suf = init;
   }
   else
     if ( states[ p ].length + 1 == states[ trans[ p ][ a ] ].length)
        states[ q ].suf = trans[ p ][ a ];
     else {
 r = COPY( trans[ p ][ a ], states, trans );
 states[ r ].length = states[ p ].length + 1;
 states[ trans[ p ][ a ] ].suf = r;
 states[ q ].suf = r;
 while ( p != art && states[ trans[ p ][ a ] ].length >= 
 									states[ r ].length) {
    trans[ p ][ a ] = r;
   p = states[ p ].suf;
 }
    }
   last = q;
 }
 states[ last ].terminal = 1;
 p = last;
 while ( p != init ) {
    p = states[ p ].suf;
   states[ p ].terminal = 1;
 }
 return ( init );
}
/* Reverse Factor Algorithm. */
IC	LPCSTR RF( LPCSTR y, LPCSTR x, int n, int m, STATE states[ 2 * XSIZE + 2 ], int trans[ XSIZE ][ ASIZE ], int init)
{
 int i, j, shift, period, state, state_aux;
 
 /* Preprocessing */
 i = 0; period = m;
 
 /* Searching */
 while ( i <= n - m ) {
    j = m - 1;
   state = init;
   shift = m;
   while ( ( state_aux = trans[ state ][ y[ i+j ] ]) != 0) {
      state = state_aux;
      if ( states[ state ].terminal) {
         period = shift;
         shift = j;
      }
      j--;
   }
   if ( j < 0 )
      return	(y + i);
   i += shift;
  }
  return 0;
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

IC LPCSTR QS( LPCSTR y, LPCSTR x, int n, int m, int qs_bc[ASIZE])
{
 int i;
 
 /* Searching */
 i = 0; 
 while ( i <= n - m ) {
    if ( memcmp( &y[ i ], x, m ) == 0 ) return (y + i );
   i += qs_bc[ y[ i + m ] ];             /* shift */
 }
 return 0;
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

IC	LPCSTR AUT(LPCSTR y, LPCSTR x, int n, int m, int delta[XSIZE][ASIZE])
{
 int i, j;
 
 /* Searching */
 j=0;
 for (i=0; i < n; i++) {
    j=delta[j][y[i]];
   if (j >= m) return (y+i-m+1);
   }
 return 0;
}
 
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

#ifdef _DEBUG
const int TEST_COUNT = 10000;
#else
const int TEST_COUNT = 1000000;
#endif

void string_test()
{
	LPCSTR			str = "asdndmnjiojoakDOIJJSasjdfksjalfjlasjfkljaskldfjklsadjfklsadjfklsadjfklsadjfklsdajfklsdajfsdaoifjsadijfoiasdjfs_google_yahoo_andex";

	u64				start, finish;
	
#ifndef _DEBUG
	SetPriorityClass		(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
#endif

	// STANDARD
	{
		Sleep			(1);
		start			= CPU::cycles();

		for (int i=0; i<TEST_COUNT; ++i) {
			LPCSTR		r_google = strstr(str,"google");
			LPCSTR		r_yahoo  = strstr(str,"yahoo");
			LPCSTR		r_yandex = strstr(str,"yandex");

			printf		("",r_google,r_yahoo,r_yandex,i);
		}

		finish			= CPU::cycles();
		
		ui().log		("strstr : %f\n",(finish-start)*CPU::cycles2milisec);
	}

	// BM
	{
		CBoyerMooreSimple	google	("google");
		CBoyerMooreSimple	yahoo	("yahoo");
		CBoyerMooreSimple	yandex	("yandex");

		Sleep			(1);
		start			= CPU::cycles();

		for (int i=0; i<TEST_COUNT; ++i) {
			int			length = (int)strlen(str);
			LPCSTR		b_google = google.search(str,length);
			LPCSTR		b_yahoo  = yahoo.search	(str,length);
			LPCSTR		b_yandex = yandex.search(str,length);

			printf		("",b_google,b_yahoo,b_yandex,i);
		}

		finish			= CPU::cycles();
		
		ui().log		("search : %f\n",(finish-start)*CPU::cycles2milisec);
	}

	// KMP
	{
		int				google[XSIZE];
		int				yahoo[XSIZE];
		int				yandex[XSIZE];

		/* Preprocessing */
		PRE_KMP			("google", (int)strlen("google"), google); 
		PRE_KMP			("yahoo",  (int)strlen("yahoo"),  yahoo); 
		PRE_KMP			("yandex", (int)strlen("yandex"), yandex); 
	   
		Sleep			(1);
		start			= CPU::cycles();

		for (int i=0; i<TEST_COUNT; ++i) {
			int			length = (int)strlen(str);
			LPCSTR		r_google = KMP(str,"google",length,6,google);
			LPCSTR		r_yahoo  = KMP(str,"yahoo",length,5,yahoo);
			LPCSTR		r_yandex = KMP(str,"yandex",length,6,yandex);

			printf		("",r_google,r_yahoo,r_yandex,i);
		}

		finish			= CPU::cycles();
		
		ui().log		("KMP : %f\n",(finish-start)*CPU::cycles2milisec);
	}

	/**/
	// BM/TBM
	{
		int				google0[XSIZE], google1[ASIZE];
		int				yahoo0[XSIZE], yahoo1[ASIZE];
		int				yandex0[XSIZE], yandex1[ASIZE];

		PRE_GS			("google", (int)strlen("google"), google0); 
		PRE_BC			("google", (int)strlen("google"), google1); 
		  
		PRE_GS			("yahoo", (int)strlen("yahoo"), yahoo0); 
		PRE_BC			("yahoo", (int)strlen("yahoo"), yahoo1); 
		  
		PRE_GS			("yandex", (int)strlen("yandex"), yandex0); 
		PRE_BC			("yandex", (int)strlen("yandex"), yandex1); 
		  
		Sleep			(1);
		start			= CPU::cycles();

		for (int i=0; i<TEST_COUNT; ++i) {
			int			length = (int)strlen(str);
			LPCSTR		r_google = TBM(str,"google",length,6,google0,google1);
			LPCSTR		r_yahoo  = TBM(str,"yahoo", length,5,yahoo0, yahoo1);
			LPCSTR		r_yandex = TBM(str,"yandex",length,6,yandex0,yandex1);

			printf		("",r_google,r_yahoo,r_yandex,i);
		}

		finish			= CPU::cycles();
		
		ui().log		("TBM : %f\n",(finish-start)*CPU::cycles2milisec);
	}
	/**/

	// RF
	{
		/* Construction of the minimal suffix of the reverse of x */
		int				m;
		m				= (int)strlen("google");
		STATE			google_states[ 2 * XSIZE + 2 ];
		int				google_trans[ XSIZE ][ ASIZE ];
		memset			( google_trans, 0, 2 * ( m + 2 ) * ASIZE * sizeof ( int ) );
		memset			( google_states, 0, 2 * ( m + 2 ) * sizeof( STATE ) );
		state_counter	= 1;
		int				google_init	= SUFF( "google" , m , google_states , google_trans );
		 
		m				= (int)strlen("yahoo");
		STATE			yahoo_states[ 2 * XSIZE + 2 ];
		int				yahoo_trans[ XSIZE ][ ASIZE ];
		memset			( yahoo_trans, 0, 2 * ( m + 2 ) * ASIZE * sizeof ( int ) );
		memset			( yahoo_states, 0, 2 * ( m + 2 ) * sizeof( STATE ) );
		state_counter	= 1;
		int				yahoo_init	= SUFF( "yahoo" , m , yahoo_states , yahoo_trans );
		 
		m				= (int)strlen("yandex");
		STATE			yandex_states[ 2 * XSIZE + 2 ];
		int				yandex_trans[ XSIZE ][ ASIZE ];
		memset			( yandex_trans, 0, 2 * ( m + 2 ) * ASIZE * sizeof ( int ) );
		memset			( yandex_states, 0, 2 * ( m + 2 ) * sizeof( STATE ) );
		state_counter	= 1;
		int				yandex_init	= SUFF( "yandex" , m , yandex_states , yandex_trans );
	   
		Sleep			(1);
		start			= CPU::cycles();

		for (int i=0; i<TEST_COUNT; ++i) {
			int			length = (int)strlen(str);
			LPCSTR		r_google = RF(str,"google",length,6,google_states, google_trans, google_init);
			LPCSTR		r_yahoo  = RF(str,"yahoo",length,5,yahoo_states, yahoo_trans, yahoo_init);
			LPCSTR		r_yandex = RF(str,"yandex",length,6,yandex_states, yandex_trans, yandex_init);

			printf		("",r_google,r_yahoo,r_yandex,i);
		}

		finish			= CPU::cycles();
		
		ui().log		("RF : %f\n",(finish-start)*CPU::cycles2milisec);
	}

	// QS
	{
		int				m;
		LPCSTR			x;
		/* Construction of the minimal suffix of the reverse of x */
		/* Preprocessing */
		int				google_qs_bc[ASIZE];
		x				= "google";
		m				= (int)strlen(x);
		for (int i = 0; i < ASIZE; i++ ) google_qs_bc[ i ] = m + 1;
		for (int i = 0; i < m; i ++ ) google_qs_bc[ x[i] ] = m - i;
		 
		int				yahoo_qs_bc[ASIZE];
		x				= "yahoo";
		m				= (int)strlen(x);
		for (int i = 0; i < ASIZE; i++ ) yahoo_qs_bc[ i ] = m + 1;
		for (int i = 0; i < m; i ++ ) yahoo_qs_bc[ x[i] ] = m - i;
		 
		int				yandex_qs_bc[ASIZE];
		x				= "yandex";
		m				= (int)strlen(x);
		for (int i = 0; i < ASIZE; i++ ) yandex_qs_bc[ i ] = m + 1;
		for (int i = 0; i < m; i ++ ) yandex_qs_bc[ x[i] ] = m - i;
		 
		Sleep			(1);
		start			= CPU::cycles();

		for (int i=0; i<TEST_COUNT; ++i) {
			int			length = (int)strlen(str);
			LPCSTR		r_google = QS(str,"google",length,6,google_qs_bc);
			LPCSTR		r_yahoo  = QS(str,"yahoo",length,5,yahoo_qs_bc);
			LPCSTR		r_yandex = QS(str,"yandex",length,6,yandex_qs_bc);

			printf		("",r_google,r_yahoo,r_yandex,i);
		}

		finish			= CPU::cycles();
		
		ui().log		("QS : %f\n",(finish-start)*CPU::cycles2milisec);
	}

	// AUT
	{
		int				m,i,j;
		LPCSTR			x;
		
		int				google[XSIZE][ASIZE];
		x				= "google";
		m				= (int)strlen(x);
		memset(google, 0, ASIZE*sizeof(int));
		for (j=0; j < m; j++) {
			i=google[j][x[j]];
		google[j][x[j]]=j+1;
		memcpy(google[j+1], google[i], ASIZE*sizeof(int));
		}

		int				yahoo[XSIZE][ASIZE];
		x				= "yahoo";
		m				= (int)strlen(x);
		memset(yahoo, 0, ASIZE*sizeof(int));
		for (j=0; j < m; j++) {
			i=yahoo[j][x[j]];
		yahoo[j][x[j]]=j+1;
		memcpy(yahoo[j+1], yahoo[i], ASIZE*sizeof(int));
		}

		int				yandex[XSIZE][ASIZE];
		x				= "yandex";
		m				= (int)strlen(x);
		memset(yandex, 0, ASIZE*sizeof(int));
		for (j=0; j < m; j++) {
			i=yandex[j][x[j]];
		yandex[j][x[j]]=j+1;
		memcpy(yandex[j+1], yandex[i], ASIZE*sizeof(int));
		}

		Sleep			(1);
		start			= CPU::cycles();

		for (int i=0; i<TEST_COUNT; ++i) {
			int			length = (int)strlen(str);
			LPCSTR		r_google = AUT(str,"google",length,6,google);
			LPCSTR		r_yahoo  = AUT(str,"yahoo",length,5,yahoo);
			LPCSTR		r_yandex = AUT(str,"yandex",length,6,yandex);

			printf		("",r_google,r_yahoo,r_yandex,i);
		}

		finish			= CPU::cycles();
		
		ui().log		("AUT : %f\n",(finish-start)*CPU::cycles2milisec);
	}

	SetThreadPriority		(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	SetPriorityClass		(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
}
