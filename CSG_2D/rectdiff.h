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

#ifndef RECTDIFF_H
#define RECTDIFF_H

typedef struct
{
    int xmin, xmax;
    int ymin, ymax;
}
IRectangle;

typedef struct IRectangleList
{
    IRectangle R;
    IRectangleList* next;
}
IRectangleList;

IRectangleList* CopyList (const IRectangleList* A);
void DeleteList (IRectangleList*& list);

IRectangleList* Difference (const IRectangle& R0, const IRectangle& R1);
IRectangleList* Difference (const IRectangleList* A, const IRectangle& R);
IRectangleList* Difference (const IRectangleList* A, const IRectangleList* B);

#endif
