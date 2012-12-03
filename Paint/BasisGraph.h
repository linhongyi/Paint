#pragma once
#include "CommonVar.h"
class BasisGraph
{
public:
	BasisGraph(void);
	~BasisGraph(void);

	void ShowRec(HDC &copyMemDC, int posStartX, int posStartY, int posEndX, int posEndY);
	void ShowSmallRec(HDC &copyMemDC, int posStartX, int posStartY, int posEndX, int posEndY,COLORREF bgColor);

	void DetectPointOutput(POINT* arrDetectPointSet,int posStartX, int posStartY, int posEndX, int posEndY);
	void DecideCursorShape(int direction);
	void SetEndPosForFixingPoint(POINT pt);
	POINT GetEndPosForFixingPoint();

	void ResetClipRgnPt();
	void SetClipRgnPt1(POINT pt);
	void SetClipRgnPt2(POINT pt);

protected:
	POINT _clipRgnPt1;
	POINT _clipRgnPt2;
private:
	//修改圖時，變更初始座標後，終點座標的紀錄 
	POINT _EndPosForFixingPoint;


};

