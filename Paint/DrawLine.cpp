#include "StdAfx.h"
#include "DrawLine.h"
#include "CommonVar.h"
#include "DrawTool.h"
#include "PaintInfo.h"
#include <assert.h>
using namespace std;

DrawLine::DrawLine(void)
{
}


DrawLine::~DrawLine(void)
{
}

void DrawLine::SetParent(DrawTool *ptr)
{
	_parent = ptr;
}

/** 畫直線*/
void DrawLine::LineDraw(HDC& copyMemHDC, int posStartX, int posStartY, int posEndX, int posEndY, COLORREF color, int PenWidth)
{
    HPEN linePen = CreatePen(PS_SOLID, PenWidth, color);
    HRGN recRgn = ::CreateRectRgn( this->_clipRgnPt1.x , this->_clipRgnPt1.y , this->_clipRgnPt2.x , this->_clipRgnPt2.y );
	
    if( !recRgn )
		MessageBox(_parent->GetPaintInfo()->GetMainHwnd(),_T("fail to create recRgn In Line Draw\n"),NULL,NULL);
	
	int saveDC = SaveDC(copyMemHDC);

    SelectObject(copyMemHDC, linePen);
	SelectClipRgn(copyMemHDC, recRgn);

    MoveToEx(copyMemHDC,posStartX,posStartY,NULL);
    LineTo(copyMemHDC,posEndX,posEndY);


    RestoreDC(copyMemHDC,saveDC);
    DeleteObject( linePen );
	DeleteObject( recRgn );
}

/********************
*  圖像往單方向移動  
*  mouseEndX, mouseEndY 滑鼠指的座標，非圖形的對角線座標
*  將座標，轉成圖形單方向後的座標
**********************/
void DrawLine::FixPosForSingleMovingGraph(int cursorShape)
{

	 switch( cursorShape )
	 {
		   // 上下移動，左右不計
		   case CursorDirection_N:
		   case CursorDirection_S:
			   
			    _parent->GetPaintInfo()->GetCMouseControl()->SetThisTurnPosEndX( this->GetEndPosForFixingPoint().x);
			   break;
		   // 左右移動，上下不計
		   case CursorDirection_E:
		   case CursorDirection_W:
			    _parent->GetPaintInfo()->GetCMouseControl()->SetThisTurnPosEndY( this->GetEndPosForFixingPoint().y);
			   break;
		   default:
			   //to do nothing
			   break;
	  }


}