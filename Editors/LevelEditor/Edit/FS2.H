#ifndef FS2_H
#define FS2_H

#pragma once

class ENGINE_API CCustomStream {
private:
	std::stack<DWORD>	chunk_pos;
	std::stack<WORD>	subchunk_pos;
	void		revbytes		( void *bp, int elsize, int elcount )
	{
		register unsigned char *p, *q;
		
		p = ( unsigned char * ) bp;
		
		if ( elsize == 2 ) {
			q = p + 1;
			while ( elcount-- ) {
				*p ^= *q;
				*q ^= *p;
				*p ^= *q;
				p += 2;
				q += 2;
			}
			return;
		}
		
		while ( elcount-- ) {
			q = p + elsize - 1;
			while ( p < q ) {
				*p ^= *q;
				*q ^= *p;
				*p ^= *q;
				++p;
				--q;
			}
			p += elsize >> 1;
		}
	}
public:
	virtual ~CCustomStream	()
	{
		R_ASSERT(subchunk_pos.empty());
		while (!chunk_pos.empty())
			close_chunk();
	}

	// kernel
	virtual void	write	(const void* ptr, DWORD count)	= 0;
	virtual void	seek	(DWORD pos)						= 0;
	virtual DWORD	tell	()								= 0;

	// generalized writing functions
	IC void			Wdword	(DWORD d)			{	revbytes(&d,4,1); write(&d,sizeof(DWORD));	}
	IC void			Wword	(WORD d)			{	revbytes(&d,2,1); write(&d,sizeof(WORD));	}
	IC void			Wfloat	(float d)			{	revbytes(&d,4,1); write(&d,sizeof(float));	}
	IC void			Wbyte	(BYTE d)			{	write(&d,sizeof(BYTE));		}
	IC void			Wstring	(const char *p)
	{
    	write(p,strlen(p));
		Wbyte(13);
		Wbyte(10);
    }
	IC void			WstringZ(const char *p)		{	write(p,strlen(p)+1); if (!(strlen(p)&1)) Wbyte(0);}
	IC void			Wvector	(Fvector v)			{	revbytes(&v,4,3); write(&v,3*sizeof(float));	}
	IC void			Wcolor	(Fcolor c)			{	revbytes(&c,4,4); write(&c,4*sizeof(float));	}

	// generalized chunking
	IC void			open_chunk	(DWORD type)
	{
		Wdword(type);
		chunk_pos.push(tell());
		Wdword(0);	// the place for 'size'
	}
	IC void			close_chunk	()
	{
		VERIFY(!chunk_pos.empty());

		int pos  = tell();
		seek		(chunk_pos.top());
		Wdword		(pos-chunk_pos.top()-4);
		seek		(pos);
		chunk_pos.pop();
	}
	// generalized chunking
	IC void			open_subchunk	(DWORD type)
	{
		Wdword(type);
		subchunk_pos.push((WORD)tell());
		Wword(0);	// the place for 'size'
	}
	IC void			close_subchunk	()
	{
		VERIFY(!subchunk_pos.empty());

		int pos  = tell();
		seek		(subchunk_pos.top());
		Wword		(pos-subchunk_pos.top()-2);
		seek		(pos);
		subchunk_pos.pop();
	}
	IC void			write_chunk	(DWORD type, void* data, DWORD size)
	{
		open_chunk	(type);
		write		(data,size);
		close_chunk	();
	}
};

class ENGINE_API CMemoryStream : public CCustomStream
{
	BYTE*		data;
	DWORD		position;
	DWORD		mem_size;
	DWORD		file_size;
public:
	CMemoryStream() {
		data		= 0;
		position	= 0;
		mem_size	= 0;
		file_size	= 0;
	}
	virtual ~CMemoryStream();

	// kernel
	virtual void	write	(const void* ptr, DWORD count);

	virtual void	seek	(DWORD pos)	
	{	position = pos;		}
	virtual DWORD	tell	() 
	{	return position;	}

	// specific
	BYTE*	pointer	()	{ return data; }
	DWORD	size	()	{ return file_size;	}
	void	clear	()  { file_size=0; position=0;	}
	void	SaveTo	(const char* fn, const char* sign)
	{
    	if (sign) FileCompress(fn,sign,pointer(),size());
        else {
        #ifdef M_BORLAND
        	int H = open(fn,O_WRONLY|O_CREAT|O_TRUNC|O_BINARY,S_IREAD|S_IWRITE);
        #else
        	int H = _open(fn,O_WRONLY|O_CREAT|O_TRUNC|O_BINARY,S_IREAD|S_IWRITE);
        #endif
            R_ASSERT(H>0);
            _write(H,pointer(),size());
            _close(H);
        }
	}
};

#include "lwchunks.h"

class ENGINE_API CLWMemoryStream: public CMemoryStream{
	IC void make_ord( int nbloks, int index, unsigned char *ord )
	{
		int i, d;
		
		for ( i = 8, d = 16; i < 128; i *= 2 ){
			if ( i >= nbloks ) break;
			d /= 2;
		}
		ord[ 0 ] = 128 + index * d;
		ord[ 1 ] = 0;
	}
	
public:
	IC void		begin_save		()				{ open_chunk(ID_FORM); Wdword(ID_LWO2);}
	IC void		end_save		(LPCSTR name)	{ close_chunk(); SaveTo(name,0);}
	IC void		Wlayer			(WORD number, LPCSTR name){
		open_chunk(ID_LAYR);
			Wword	(number);	// num
			Wword	(0);
			Wfloat	(0);		// pivot
			Wfloat	(0);
			Wfloat	(0);
			WstringZ(name);
		close_chunk();
	}
	IC void		Wvx				(int idx)
	{	if (idx>0xFF00){ idx|=0xff000000; Wdword(idx);} else Wword(idx);}

	IC void		Wface3			(int i0, int i1, int i2)
	{	Wword(3); Wvx(i0); Wvx(i1); Wvx(i2); }

	IC void		begin_vmap		(BOOL polymap, DWORD type, int dim, LPCSTR name){	
		open_chunk(polymap?ID_VMAD:ID_VMAP);
		Wdword	(type);
		Wword	(dim);
		R_ASSERT(name&&name[0]);//,"Empty vmap name!");
		WstringZ(name);
	}
	IC void		end_vmap		()
	{	close_chunk();}

	IC void		Wvmap			(int v_index, int dim, float* uv)
	{	Wword(v_index); Wfloat(uv[0]); if (dim==2) Wfloat(1.f-uv[1]); }

	IC void		Wvmad			(int v_index, int f_index, int dim, float* uv)
	{	Wword(v_index); Wword(f_index); Wfloat(uv[0]); if (dim==2) Wfloat(1.f-uv[1]); }

	IC void Wsurface(LPCSTR name, BOOL b2Sided, WORD image, LPCSTR vmap, LPCSTR sh_eng, LPCSTR sh_comp){
		// surf screen 
		char ord[2];
		open_chunk(ID_SURF);
			WstringZ(name);
			WstringZ("");
			open_subchunk(ID_COLR);
 				Wfloat(1.f);Wfloat(1.f);Wfloat(1.f); Wword(0);
			close_subchunk();
			open_subchunk(ID_SIDE); 
				Wword(b2Sided?3:1); 
			close_subchunk();
			open_subchunk(ID_BLOK);
				open_subchunk(ID_IMAP);
					make_ord(128,0,(BYTE*)ord);
					WstringZ(ord);
					open_subchunk(ID_CHAN);
						Wdword(ID_COLR);
					close_subchunk();
					open_subchunk(ID_OPAC);
						Wword(0);
						Wfloat(1.f);
						Wword(0);
					close_subchunk();
					open_subchunk(ID_ENAB);
						Wword(1);
					close_subchunk();
				close_subchunk();
				open_subchunk(ID_PROJ);
					Wword(5);
				close_subchunk();
				open_subchunk(ID_IMAG);
					Wword(image);
				close_subchunk();
				open_subchunk(ID_VMAP);
					WstringZ(vmap);
				close_subchunk();
				open_subchunk(ID_AAST);
					Wword(1); Wfloat(1.f);
				close_subchunk();
				open_subchunk(ID_PIXB);
					Wword(1);
				close_subchunk();
				open_subchunk(ID_STCK);
					Wfloat(0.f); Wword(0);
				close_subchunk();
				open_subchunk(ID_TAMP);
					Wfloat(1.f); Wword(0);
				close_subchunk();
			close_subchunk();
			// plugins
			open_subchunk(ID_BLOK);
				open_subchunk(ID_SHDR);
					make_ord(128,0,(BYTE*)ord);
					WstringZ(ord);
					open_subchunk(ID_ENAB);
						Wword(1);
					close_subchunk();
					open_subchunk(ID_FUNC);
						WstringZ("!XRayShader");
						write(sh_eng,64);
						Wdword(-1);
						write(sh_comp,64);
						Wdword(-1);
						write("",128);
					close_subchunk();
				close_subchunk();
			close_subchunk();
		close_chunk();
	}
};

#endif
