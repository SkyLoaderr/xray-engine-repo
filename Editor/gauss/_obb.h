#ifndef FOBB_H
#define FOBB_H

struct	Fobb
{
	Fmatrix33	m_rotate;
	Fvector		m_translate;
	Fvector		m_halfsize;

	IC void		invalidate() {
		m_rotate.identity	();
		m_translate.set		(0,0,0);
		m_halfsize.set		(0,0,0);
	}
	IC void		xform_get(Fmatrix& D)	
	{
		D.i.set(m_rotate.i); D._14_ = 0;
		D.j.set(m_rotate.j); D._24_ = 0;
		D.k.set(m_rotate.k); D._34_ = 0;
		D.c.set(m_translate);D._44_ = 1;
	}
	IC void		xform_set(const Fmatrix& S)
	{
		m_rotate.i.set	(S.i);
		m_rotate.j.set	(S.j);
		m_rotate.k.set	(S.k);
		m_translate.set	(S.c);
	}
	IC void		xform_full(Fmatrix& D)
	{
		Fmatrix		R,S;
		xform_get	(R);
		S.scale		(m_halfsize);
		D.mul_43	(R,S);
	}

	// NOTE: Unoptimized
	IC void		transform(Fobb& src, Fmatrix& M)	
	{
		Fmatrix	srcR,destR;

		src.xform_get(srcR);
		destR.mul_43(M,srcR);
		xform_set(destR);
		m_halfsize.set	(src.m_halfsize);
	}
};

#endif