#pragma once
#include "BasisGraph.h"
#include "CommonVar.h"

class DrawTool;
class DrawLine: public BasisGraph
{
public:
	DrawLine(void);
	~DrawLine(void);
	void LineDraw(HDC& copyMemHDC, int posStartX, int posStartY, int posEndX, int posEndY, COLORREF color, int PenWidth);
	void FixPosForSingleMovingGraph(int cursorShape);
	void SetParent(DrawTool *ptr);
private: 
	DrawTool* _parent;
};

