//---------------------------------------------------------------------------

#ifndef StatisticH
#define StatisticH
//---------------------------------------------------------------------------
class CStatistic{
public:
    long lRenderPolyCount;
    long lTotalLight;
    long lLightInScene;
    long lTotalFrame;

    CStatistic();
    void Reset();
};
#endif
