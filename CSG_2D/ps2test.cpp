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

#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include "psolid2.h"

#define WSIZE 256
#define RAND (rand() % WSIZE)

typedef struct
{
    int xmin, ymin, xmax, ymax;
}
Viewport;

//---------------------------------------------------------------------------
void ConstructPolygon (PolySolid2& poly)
{
    RPoint2 start, r0, r1;

    r0.x = start.x = WSIZE/8;
    r0.y = start.y = WSIZE/8;
    r1.x = r0.x;
    r1.y = 7*WSIZE/8;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = 7*WSIZE/8;
    r1.y = r0.y;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = r0.x;
    r1.y = 5*WSIZE/8;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = 5*WSIZE/8;
    r1.y = r0.y;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = r0.x;
    r1.y = 6*WSIZE/8;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = 2*WSIZE/8;
    r1.y = r0.y;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = r0.x;
    r1.y = 3*WSIZE/8;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = 3*WSIZE/8;
    r1.y = r0.y;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = r0.x;
    r1.y = WSIZE/8;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1 = start;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));
}
//---------------------------------------------------------------------------
void ConstructPentagon (PolySolid2& poly)
{
    double pi = 4.0*atan(1.0);
    double angle = -2.0*pi/5.0;
    double radius = 0.35*WSIZE;
    double cx = WSIZE/2, cy = WSIZE/2;

    RPoint2 start, r0, r1;
    r0.x = start.x = int(cx+radius);
    r0.y = start.y = int(cy);
    r1.x = int(cx+radius*cos(angle));
    r1.y = int(cy+radius*sin(angle));
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = int(cx+radius*cos(2.0*angle));
    r1.y = int(cy+radius*sin(2.0*angle));
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = int(cx+radius*cos(3.0*angle));
    r1.y = int(cy+radius*sin(3.0*angle));
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = int(cx+radius*cos(4.0*angle));
    r1.y = int(cy+radius*sin(4.0*angle));
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1 = start;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));
}
//---------------------------------------------------------------------------
void ConstructSquare (PolySolid2& poly)
{
    RPoint2 r0, r1;

    r0.x = 2*WSIZE/8;
    r0.y = 2*WSIZE/8;
    r1.x = r0.x;
    r1.y = 6*WSIZE/8;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = 6*WSIZE/8;
    r1.y = r0.y;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = r0.x;
    r1.y = 2*WSIZE/8;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = 2*WSIZE/8;
    r1.y = 2*WSIZE/8;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));
}
//---------------------------------------------------------------------------
void ConstructSShape (PolySolid2& poly)
{
    RPoint2 r0, r1;

    r0.x = 6*WSIZE/8;
    r0.y = Int(2.5*WSIZE/8);
    r1.x = r0.x;
    r1.y = 3*WSIZE/8;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = Int(6.5*WSIZE/8);
    r1.y = r0.y;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = r0.x;
    r1.y = Int(3.25*WSIZE/8);
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = 5*WSIZE/8;
    r1.y = r0.y;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = r0.x;
    r1.y = Int(5.5*WSIZE/8);
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = 6*WSIZE/8;
    r1.y = r0.y;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = r0.x;
    r1.y = Int(4.75*WSIZE/8);
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = Int(5.5*WSIZE/8);
    r1.y = r0.y;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = r0.x;
    r1.y = 4*WSIZE/8;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = 7*WSIZE/8;
    r1.y = r0.y;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = r0.x;
    r1.y = Int(2.5*WSIZE/8);
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = 6*WSIZE/8;
    r1.y = r0.y;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));
}
//---------------------------------------------------------------------------
void ConstructPolyWithHoles (PolySolid2& poly)
{
    RPoint2 r0, r1;

    // outer boundary
    r0.x = WSIZE/8;
    r0.y = WSIZE/8;
    r1.x = r0.x;
    r1.y = 7*WSIZE/8;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = 7*WSIZE/8;
    r1.y = WSIZE/8;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = WSIZE/8;
    r1.y = WSIZE/8;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    // inner hole
    r0.x = 2*WSIZE/8;
    r0.y = 3*WSIZE/16;
    r1.x = 3*WSIZE/8;
    r1.y = r0.y;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = r0.x;
    r1.y = 3*WSIZE/8;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = 2*WSIZE/8;
    r1.y = 3*WSIZE/16;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));
}
//---------------------------------------------------------------------------
void ConstructAnotherSquare (PolySolid2& poly)
{
    RPoint2 r0, r1;

    // clockwise: (16,16),(16,128),(128,128),(128,16),(16,16)

    r0.x = 16;
    r0.y = 16;
    r1.x = 16;
    r1.y = 128;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = 128;
    r1.y = 128;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = 128;
    r1.y = 16;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = 16;
    r1.y = 16;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));
}
//---------------------------------------------------------------------------
void ConstructAnotherOne (PolySolid2& poly)
{
    RPoint2 r0, r1;

    // clockwise: (128,64),(16,64),(16,128),(256,128),(128,64)

    r0.x = 128;
    r0.y = 64;
    r1.x = 16;
    r1.y = 64;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = 16;
    r1.y = 128;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = 256;
    r1.y = 128;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

    r0 = r1;
    r1.x = 128;
    r1.y = 64;
    poly.AddEdge(poly.AddVertex(r0),poly.AddVertex(r1));

}
//---------------------------------------------------------------------------
void DrawPolySolid (HDC hDC, PolySolid2& poly, COLORREF color, bool fill)
{
    HPEN hPen = CreatePen(PS_SOLID,2,color);
    HGDIOBJ hOldPen = SelectObject(hDC,hPen);
    for (Edge* pEdge = poly.GetEdges(); pEdge; pEdge = pEdge->next)
    {
        const RPoint2& r0 = pEdge->v0->r;
        const RPoint2& r1 = pEdge->v1->r;
        
        int x0 = int(r0.x);
        int y0 = WSIZE-1-int(r0.y);
        int x1 = int(r1.x);
        int y1 = WSIZE-1-int(r1.y);
        
        MoveToEx(hDC,x0,y0,NULL);
        LineTo(hDC,x1,y1);
    }
    SelectObject(hDC,hOldPen);
    DeleteObject(hPen);

    // draw polysolid interior
    if ( fill )
    {
        HBRUSH hBrush = CreateSolidBrush(RGB(255,255,0));
        HBRUSH hOldBrush = (HBRUSH) SelectObject(hDC,hBrush);
        for (Edge* pEdge = poly.GetEdges(); pEdge; pEdge = pEdge->next)
        {
            const RPoint2& r0 = pEdge->v0->r;
            const RPoint2& r1 = pEdge->v1->r;

            // draw tick mark pointing to polysolid interior
            RPoint2 mid, norm;
            mid.x = (r0.x+r1.x)/Int(2);
            mid.y = (r0.y+r1.y)/Int(2);
            norm.x =  r1.y-r0.y;
            norm.y =  r0.x-r1.x;
            float nx = float(norm.x);
            float ny = float(norm.y);
            float length = float(sqrt(nx*nx+ny*ny));
            nx /= length;
            ny /= length;
            
            int x0 = int(mid.x);
            int y0 = WSIZE-1-int(mid.y);
            int x1 = int(x0+5*nx);
            int y1 = int(y0-5*ny);

#if 1
            if ( GetPixel(hDC,x1,y1) != color )
                ExtFloodFill(hDC,x1,y1,color,FLOODFILLBORDER);
#else
            MoveToEx(hDC,x0,y0,NULL);
            LineTo(hDC,x1,y1);
#endif
        }
        SelectObject(hDC,hOldBrush);
        DeleteObject(hBrush);
    }
    
}
//---------------------------------------------------------------------------
long FAR PASCAL 
WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static PolySolid2 poly0, poly1;
    static PolySolid2 INTERSECTION, UNION, DIFF, EXCLUSIVE_OR;
    static PolySolid2* pDraw = 0;
    static Viewport v;

    switch ( message ) 
    {
    case WM_CREATE:
    {
#if 0
        ConstructPolygon(poly0);
        ConstructPentagon(poly1);
#endif
#if 0
        ConstructSquare(poly0);
        ConstructSShape(poly1);
#endif
#if 1
        ConstructPolyWithHoles(poly0);
        ConstructPentagon(poly1);
#endif
#if 0
        ConstructAnotherSquare(poly0);
        ConstructAnotherOne(poly1);
#endif
        INTERSECTION = poly0 & poly1;
        UNION = poly0 | poly1;
        DIFF = poly0 - poly1;
        EXCLUSIVE_OR = poly0 ^ poly1;
        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(hWnd,&ps);
            
        DrawPolySolid(hDC,poly0,RGB(255,0,0),false);
        DrawPolySolid(hDC,poly1,RGB(0,255,0),false);
        if ( pDraw )
            DrawPolySolid(hDC,*pDraw,RGB(0,0,255),true);
            
        EndPaint(hWnd,&ps);
        return 0;
    }
    case WM_CHAR:
    {
        switch ( wParam )
        {
        case 'p':
        case 'P':
            pDraw = 0;
            InvalidateRect(hWnd,NULL,TRUE);
            break;
        case 'u':
        case 'U':
            pDraw = &UNION;
            InvalidateRect(hWnd,NULL,TRUE);
            break;
        case 'i':
        case 'I':
            pDraw = &INTERSECTION;
            InvalidateRect(hWnd,NULL,TRUE);
            break;
        case 'd':
        case 'D':
            pDraw = &DIFF;
            InvalidateRect(hWnd,NULL,TRUE);
            break;
        case 'x':
        case 'X':
            pDraw = &EXCLUSIVE_OR;
            InvalidateRect(hWnd,NULL,TRUE);
            break;
        case 'q':
        case 'Q':
        case VK_ESCAPE:
            PostMessage(hWnd,WM_DESTROY,0,0);
        }
        return 0;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProc(hWnd,message,wParam,lParam);
}
//---------------------------------------------------------------------------
int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
{
    static char szAppName[] = "Polysolid2 Test";

    WNDCLASS wc;
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL,IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = szAppName;
    RegisterClass(&wc);

    RECT rect = { 0, 0, WSIZE-1, WSIZE-1 };
    AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,FALSE);

    HWND hWnd = CreateWindow (
        szAppName,
        "Polysolid2 Test",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right-rect.left+1,
        rect.bottom-rect.top+1,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    ShowWindow(hWnd,nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while ( TRUE )
    {
        if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            if ( msg.message == WM_QUIT )
                break;

            HACCEL hAccel = NULL;
            if ( !TranslateAccelerator( hWnd, hAccel, &msg ) )
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    return msg.wParam;
}
//---------------------------------------------------------------------------
