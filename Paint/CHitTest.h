#pragma once

class PaintInfo;
class CHitTest
{
public:
	CHitTest(void);
	~CHitTest(void);
	void SetParent(PaintInfo *ptr);
	BOOL PtHitTestRegion(int ID, POINT tarGetPt, POINT posStart , POINT posEnd); 
    BOOL RegionHitRegion(POINT ptSrc1, POINT ptSrc2, POINT ptDes1, POINT ptDes2);
	BOOL PtHitClimax(POINT tarGetPt, int ID); 

private:
	BOOL HitEightPoint(POINT tarGetPt,int ID);
	PaintInfo*   _parent;

	void GenerateRecRegion(BOOL isTransparent);
	void GenerateCirRegion(BOOL isTransparent);
	void GenerateEightPt(int iType,int posStartX,int posStartY, int posEndX, int posEndY );

	BOOL HitLine(POINT tarGetPt,int posStartX,int posStartY, int posEndX, int posEndY);
	POINT _pt[8];
	HRGN _tarGetCirHrgn;
	HRGN _tarGetRecHrgn;
};

