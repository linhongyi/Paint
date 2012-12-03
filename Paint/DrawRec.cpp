#include "StdAfx.h"
#include "DrawRec.h"
#include "DrawTool.h"
#include "PaintInfo.h"

DrawRec::DrawRec(void)
{
	
}


DrawRec::~DrawRec(void)
{
}

void DrawRec::SetParent(DrawTool *ptr)
{
	_parent = ptr;
}

/** 畫方形 */
void DrawRec::RecDraw(HDC& copyMemHDC, int posStartX, int posStartY, int posEndX, int posEndY, COLORREF color, COLORREF bgColor, int PenWidth)
{
     HPEN recPen = CreatePen(PS_SOLID, PenWidth, color);
	 HRGN recRgn = ::CreateRectRgn( this->_clipRgnPt1.x , this->_clipRgnPt1.y , this->_clipRgnPt2.x , this->_clipRgnPt2.y );
   
	 if( posStartX == posEndX )
		 posEndX++;
	 else if( posStartY == posEndY )
		 posEndY++;

	 if( !recRgn )
		MessageBox(_parent->GetPaintInfo()->GetMainHwnd(),_T("fail to create recRgn In RecDraw\n"),NULL,NULL);
	 
	 HBRUSH recBrush;

	  if( bgColor == CommonVar::BG_TRANSPARENT )
		recBrush = (HBRUSH)GetStockObject(NULL_BRUSH);  
	 else
	    recBrush = CreateSolidBrush(bgColor);

    int saveDC = SaveDC(copyMemHDC);
   

    SelectObject( copyMemHDC, recBrush);
    SelectObject( copyMemHDC, recPen);
	SelectClipRgn(copyMemHDC, recRgn);

    Rectangle(copyMemHDC, posStartX, posStartY, posEndX , posEndY);
   
    RestoreDC(copyMemHDC,saveDC);
    DeleteObject( recPen );
    DeleteObject( recBrush );
	DeleteObject( recRgn );
}

/********************
*  圖像往單方向移動  
*  mouseEndX, mouseEndY 滑鼠指的座標，非圖形的對角線座標
*  將座標，轉成圖形單方向後的座標
**********************/
void DrawRec::FixPosForSingleMovingGraph(int cursorShape)
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