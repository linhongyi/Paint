#include "StdAfx.h"
#include "DrawCircular.h"

#include "DrawTool.h"
#include "PaintInfo.h"


DrawCircular::DrawCircular()
{
}


DrawCircular::~DrawCircular(void)
{
}

void DrawCircular::SetParent(DrawTool *ptr)
{
	_parent = ptr;
}

/*****************
* 畫圓形
******************/
void  DrawCircular::CirDraw(HDC &copyMemHDC,int posStartX, int posStartY, int posEndX, int posEndY, COLORREF color, COLORREF bgColor,int PenWidth)
{
	 HPEN cirPen = CreatePen(PS_SOLID, PenWidth, color);

	 if( this->_clipRgnPt1.x == this->_clipRgnPt2.x )
		 this->_clipRgnPt2.x++;
	 if( this->_clipRgnPt1.y == this->_clipRgnPt2.y )
		 this->_clipRgnPt2.y++;
	 if( posStartX == posEndX )
		 posEndX++;
	 if( posStartY == posEndY )
		 posEndY++;

	 HRGN cirRgn = ::CreateRectRgn( this->_clipRgnPt1.x , this->_clipRgnPt1.y , this->_clipRgnPt2.x , this->_clipRgnPt2.y );

	 if( !cirRgn )
	 {
		MessageBox(_parent->GetPaintInfo()->GetMainHwnd(),_T("fail to create cirRgn In CirDraw\n"),NULL,NULL);
		return;
	 }

	 HBRUSH cirBrush;
	 if( bgColor == CommonVar::BG_TRANSPARENT )
		cirBrush = (HBRUSH)GetStockObject(NULL_BRUSH);  
	 else
	    cirBrush = CreateSolidBrush(bgColor);

    int saveDC = SaveDC(copyMemHDC);
   
    SelectObject(copyMemHDC, cirBrush);
    SelectObject(copyMemHDC, cirPen);
    SelectClipRgn(copyMemHDC, cirRgn);

    Ellipse(copyMemHDC, posStartX, posStartY, posEndX , posEndY);

    RestoreDC(copyMemHDC,saveDC);
    DeleteObject( cirPen );
    DeleteObject( cirBrush );
	DeleteObject( cirRgn );
}

/********************
*  圖像往單方向移動  
*  mouseEndX, mouseEndY 滑鼠指的座標，非圖形的對角線座標
*  將座標，轉成圖形單方向後的座標
**********************/
void DrawCircular::FixPosForSingleMovingGraph(int cursorShape)
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


