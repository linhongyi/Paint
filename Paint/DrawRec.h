#pragma once
#include "basisgraph.h"
#include "CommonVar.h"

class DrawTool;
class DrawRec :
	public BasisGraph
{
public:
	DrawRec(void);
	~DrawRec(void);
	void FixPosForSingleMovingGraph(int cursorShape);
	void RecDraw(HDC &copyMemHDC,int posStartX, int posStartY, int posEndX, int posEndY, COLORREF color,  COLORREF bgColor,int PenWidth);
	void SetParent(DrawTool *ptr);
	
private: 
	DrawTool* _parent;
};

