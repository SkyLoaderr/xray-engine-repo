#ifndef _INCDEF_PortalUtils_H_
#define _INCDEF_PortalUtils_H_

//refs
class CEditMesh;
class CEditObject;
class CSector;

struct SVertex: public Fvector{
	INTVec link[2];
    INTVec ulink;
    int portal;
    SVertex(const Fvector& v){set(v);portal=-1;}
    IC void SetLink(int id, int i0, int i1){if (i0>=0)link[id].push_back(i0); if (i1>=0)link[id].push_back(i1);}
    IC void ConsolidateLink(){
    	sort(link[0].begin(),link[0].end());
    	sort(link[1].begin(),link[1].end());
        set_intersection(	link[0].begin(),link[0].end(),
        					link[1].begin(),link[1].end(),
                        	inserter(ulink,ulink.begin()));
        VERIFY(ulink.size()<=2);
    }
};

DEFINE_VECTOR(SVertex,SVertexVec,SVertexIt)

class CPortalUtils{
	void FindSVertexLinks(int id, CSector* S, SVertexVec& V);
	int  CalculateSelectedPortals(vector<CSector*>& sectors);
public:
	bool CalculateConvexHull(FvectorVec& points);
	int  CalculatePortals(CSector* SF, CSector* SB);
	int  CalculatePortals(char* s_front, char* s_back);
	int  CalculateAllPortals();
	int  CalculateAllPortals2();
	void RemoveAllPortals();
	void RemoveSectorPortal(CSector* S);
	bool CreateDefaultSector();
	bool RemoveDefaultSector();

	bool IsFaceUsed(CEditObject* o, CEditMesh* m, DWORD f_id);
	void TestUsedFaces(CEditObject* o, CEditMesh* m, DWORDVec& fl); // remove used from list

	void CreateDebugCollection();
    bool Validate(bool bMsg);
};

extern CPortalUtils PortalUtils;

#endif /*_INCDEF_PortalUtils_H_*/
