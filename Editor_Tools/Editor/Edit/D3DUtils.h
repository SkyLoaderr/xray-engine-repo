//----------------------------------------------------
// file: D3DUtils.h
//----------------------------------------------------

#ifndef _INCDEF_D3DUtils_H_
#define _INCDEF_D3DUtils_H_
//----------------------------------------------------

namespace DU{
    void InitUtilLibrary();
    void UninitUtilLibrary();

	void UpdateGrid(int number_of_cell, float square_size, int subdiv=10);

    //----------------------------------------------------
    void DrawCross(const Fvector& p, float szx1, float szy1, float szz1, float szx2, float szy2, float szz2, DWORD clr, bool bRot45=false);
    void DrawFlag(const Fvector& p, float heading, float height, float sz, float sz_fl, DWORD clr);
    void DrawRomboid(const Fvector& p, float radius, DWORD clr);
    void DrawDirectionalLight(const Fvector& p, const Fvector& d, float radius, float range, DWORD clr);
    void DrawPointLight(const Fvector& p, float radius, DWORD clr);
    void DrawSound(const Fvector& p, float radius, DWORD clr);
    void DrawLineSphere(const Fvector& p, float radius, DWORD clr, bool bCross);
    void DrawLine(const Fvector& p1,	const Fvector& p2, DWORD clr);
    IC void DrawLine(const Fvector* p, DWORD clr){DrawLine(p[0],p[1],clr);}
    IC void DrawFaceNormal(const Fvector& p1, const Fvector& p2, const Fvector& p3, float size, DWORD clr){
        Fvector N,C,P; N.mknormal(p1,p2,p3); C.set(p1);C.add(p2);C.add(p3);C.div(3);
        P.direct(C,N,size);
        DrawLine(C,P,clr);}
    IC void DrawFaceNormal(const Fvector* p, float size, DWORD clr){DrawFaceNormal(p[0],p[1],p[2],size,clr);}
    IC void DrawFaceNormal(const Fvector& C, const Fvector& N, float size, DWORD clr){
        Fvector P; P.direct(C,N,size);DrawLine(C,P,clr);}
    void DrawSelectionBox(const Fvector& center, const Fvector& size, DWORD* c=0);
    IC void DrawSelectionBox(const Fbox& box, DWORD* c=0){
        Fvector S,C;
        box.getsize(S);
        box.getcenter(C);
        DrawSelectionBox(C,S,c);
    }
    void DrawIdentBox(bool bSolid, bool bWire, DWORD* c=0);
    void DrawBox    (const Fvector& offs, const Fvector& Size, bool bWire, DWORD c);
    void DrawPlane  (const Fvector& center, const Fvector2& scale, const Fvector& rotate, DWORD c, bool bCull, bool bBorder, DWORD cb);

    void DrawGrid	();
    void DrawPivot	(const Fvector& pos, float sz=5.f);
	void DrawAxis	();
	void DrawSelectionRect(const Ipoint& m_SelStart, const Ipoint& m_SelEnd);

    void DrawPrimitiveL(D3DPRIMITIVETYPE pt, DWORD pc, Fvector* vertices, int vc, DWORD color, bool bCull, bool bCycle);
    void DrawPrimitiveTL(D3DPRIMITIVETYPE pt, DWORD pc, FVF::TL* vertices, int vc, bool bCull, bool bCycle);
    void DrawPrimitiveLIT(D3DPRIMITIVETYPE pt, DWORD pc, FVF::LIT* vertices, int vc, bool bCull, bool bCycle);
};
//----------------------------------------------------
#endif /*_INCDEF_D3DUtils_H_*/

