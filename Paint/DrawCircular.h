#pragma once
#include "basisgraph.h"
#include "CommonVar.h"


class DrawTool;
class DrawCircular :public BasisGraph
{
public:
	DrawCircular();
	~DrawCircular(void);
    void SetParent(DrawTool *ptr);
	void CirDraw(HDC &copyMemHDC,int posStartX, int posStartY, int posEndX, int posEndY, COLORREF color, COLORREF bgColor,int PenWidth);
	void FixPosForSingleMovingGraph(int cursorShape);
	
private:
	DrawTool *_parent;
};

