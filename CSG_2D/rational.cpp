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

#include "rational.h"

//---------------------------------------------------------------------------
Rational::Rational (Int iNumer)
{
    m_iNumer = iNumer;
    m_iDenom = 1;
}
//---------------------------------------------------------------------------
Rational::Rational (Int iNumer, Int iDenom)
{
    assert( iDenom != 0 );
    m_iNumer = iNumer;
    m_iDenom = iDenom;
}
//---------------------------------------------------------------------------
Rational::Rational (const Rational& r)
{
    m_iNumer = r.m_iNumer;
    m_iDenom = r.m_iDenom;
}
//---------------------------------------------------------------------------
void Rational::Reduce ()
{
    if ( m_iNumer != 0 )
    {
        // compute greatest common divisor of iNumerator and iDenominator
        Int iAbsNumer = ( m_iNumer > 0 ? m_iNumer : -m_iNumer );
        Int iAbsDenom = ( m_iDenom > 0 ? m_iDenom : -m_iDenom );
        Int iRemainder;
        do
        {
            iRemainder = iAbsNumer;
            while ( iRemainder >= iAbsDenom )
                iRemainder -= iAbsDenom;
            if ( iRemainder > 0 )
            {
                iAbsNumer = iAbsDenom;
                iAbsDenom = iRemainder;
            }
        }
        while ( iRemainder > 0 );

        // iAbsDenom is the greatest common divisor
        if ( iAbsDenom > 1 )
        {
            m_iNumer /= iAbsDenom;
            m_iDenom /= iAbsDenom;
        }
    }
}
//---------------------------------------------------------------------------
int Rational::Ordering (const Rational& r)
{
    Int prod0 = m_iNumer*r.m_iDenom;
    Int prod1 = m_iDenom*r.m_iNumer;

    if ( prod0 == prod1 )
        return 0;

    if ( m_iDenom > 0 )
    {
        if ( r.m_iDenom > 0 )
            return (prod0 < prod1 ? -1 : +1);
        else
            return (prod0 > prod1 ? -1 : +1);
    }
    else
    {
        if ( r.m_iDenom > 0 )
            return (prod0 > prod1 ? -1 : +1);
        else
            return (prod0 < prod1 ? -1 : +1);
    }
}
//---------------------------------------------------------------------------
Rational& Rational::operator= (const Rational& r)
{
    m_iNumer = r.m_iNumer;
    m_iDenom = r.m_iDenom;
    return *this;
}
//---------------------------------------------------------------------------
Rational& Rational::operator= (Int i)
{
    m_iNumer = i;
    m_iDenom = 1;
    return *this;
}
//---------------------------------------------------------------------------
bool Rational::operator== (const Rational& r) const
{
    return ( m_iNumer*r.m_iDenom == m_iDenom*r.m_iNumer );
}
//---------------------------------------------------------------------------
bool Rational::operator!= (const Rational& r) const
{
    return ( m_iNumer*r.m_iDenom != m_iDenom*r.m_iNumer );
}
//---------------------------------------------------------------------------
bool Rational::operator<= (const Rational& r) const
{
    Int prod0 = m_iNumer*r.m_iDenom;
    Int prod1 = m_iDenom*r.m_iNumer;

    if ( m_iDenom > 0 )
    {
        if ( r.m_iDenom > 0 )
            return prod0 <= prod1;
        else
            return prod0 >= prod1;
    }
    else
    {
        if ( r.m_iDenom > 0 )
            return prod0 >= prod1;
        else
            return prod0 <= prod1;
    }
}
//---------------------------------------------------------------------------
bool Rational::operator< (const Rational& r) const
{
    Int prod0 = m_iNumer*r.m_iDenom;
    Int prod1 = m_iDenom*r.m_iNumer;

    if ( m_iDenom > 0 )
    {
        if ( r.m_iDenom > 0 )
            return prod0 < prod1;
        else
            return prod0 > prod1;
    }
    else
    {
        if ( r.m_iDenom > 0 )
            return prod0 > prod1;
        else
            return prod0 < prod1;
    }
}
//---------------------------------------------------------------------------
Rational Rational::operator+ (const Rational& r) const
{
    Rational sum;
    sum.m_iNumer = m_iNumer*r.m_iDenom+m_iDenom*r.m_iNumer;
    sum.m_iDenom = m_iDenom*r.m_iDenom;
    sum.Reduce();
    return sum;
}
//---------------------------------------------------------------------------
Rational Rational::operator- (const Rational& r) const
{
    Rational diff;
    diff.m_iNumer = m_iNumer*r.m_iDenom-m_iDenom*r.m_iNumer;
    diff.m_iDenom = m_iDenom*r.m_iDenom;
    diff.Reduce();
    return diff;
}
//---------------------------------------------------------------------------
Rational Rational::operator* (const Rational& r) const
{
    Rational prod;
    prod.m_iNumer = m_iNumer*r.m_iNumer;
    prod.m_iDenom = m_iDenom*r.m_iDenom;
    prod.Reduce();
    return prod;
}
//---------------------------------------------------------------------------
Rational Rational::operator/ (const Rational& r) const
{
    Rational quot;
    quot.m_iNumer = m_iNumer*r.m_iDenom;
    quot.m_iDenom = m_iDenom*r.m_iNumer;
    quot.Reduce();
    return quot;
}
//---------------------------------------------------------------------------
