// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000, All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// FREE SOURCE CODE
// http://www.magic-software.com/License/free.pdf

#ifndef RATIONAL_H
#define RATIONAL_H

#include <assert.h>

typedef __int64 Int;
//typedef float Int;

class Rational
{
public:
    Rational () { /**/ }
    Rational (Int iNumer);
    Rational (Int iNumer, Int iDenom);
    Rational (const Rational& r);

    Int& Numerator () { return m_iNumer; }
    Int& Denominator () { return m_iDenom; }
    void Reduce ();

    int Ordering (const Rational& r);

    Rational& operator= (const Rational& r);
    Rational& operator= (Int i);
    operator float () const { return float(m_iNumer)/float(m_iDenom); }

    bool operator== (const Rational& r) const;
    bool operator!= (const Rational& r) const;
    bool operator<= (const Rational& r) const;
    bool operator<  (const Rational& r) const;
    bool operator>= (const Rational& r) const { return r <= (*this); }
    bool operator>  (const Rational& r) const { return r <  (*this); }
    bool operator== (Int i) const { return (*this) == Rational(i,1); }
    bool operator!= (Int i) const { return (*this) != Rational(i,1); }
    bool operator<= (Int i) const { return (*this) <= Rational(i,1); }
    bool operator<  (Int i) const { return (*this) <  Rational(i,1); }
    bool operator>= (Int i) const { return (*this) >= Rational(i,1); }
    bool operator>  (Int i) const { return (*this) >  Rational(i,1); }
    friend bool operator== (Int i, const Rational& r) { return Rational(i,1) == r; }
    friend bool operator!= (Int i, const Rational& r) { return Rational(i,1) != r; }
    friend bool operator<= (Int i, const Rational& r) { return Rational(i,1) <= r; }
    friend bool operator<  (Int i, const Rational& r) { return Rational(i,1) <  r; }
    friend bool operator>= (Int i, const Rational& r) { return Rational(i,1) >= r; }
    friend bool operator>  (Int i, const Rational& r) { return Rational(i,1) >  r; }

    Rational operator+ (const Rational& r) const;
    Rational operator- (const Rational& r) const;
    Rational operator* (const Rational& r) const;
    Rational operator/ (const Rational& r) const;
    Rational operator+ (Int i) const { return (*this)+Rational(i,1); }
    Rational operator- (Int i) const { return (*this)-Rational(i,1); }
    Rational operator* (Int i) const { return (*this)*Rational(i,1); }
    Rational operator/ (Int i) const { return (*this)/Rational(i,1); }
    friend Rational operator+ (Int i, const Rational& r) { return Rational(i,1)+r; }
    friend Rational operator- (Int i, const Rational& r) { return Rational(i,1)-r; }
    friend Rational operator* (Int i, const Rational& r) { return Rational(i,1)*r; }
    friend Rational operator/ (Int i, const Rational& r) { return Rational(i,1)/r; }

    Rational operator- () const { return Rational(-m_iNumer,m_iDenom); }

protected:
    Int m_iNumer, m_iDenom;
};

#endif
