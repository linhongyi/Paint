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
* �e���
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
*  �Ϲ������V����  
*  mouseEndX, mouseEndY �ƹ������y�СA�D�ϧΪ��﨤�u�y��
*  �N�y�СA�ন�ϧγ��V�᪺�y��
**********************/
void DrawCircular::FixPosForSingleMovingGraph(int cursorShape)
{

	 switch( cursorShape )
	 {
		   // �W�U���ʡA���k���p
		   case CursorDirection_N:
		   case CursorDirection_S:
			   _parent->GetPaintInfo()->GetCMouseControl()->SetThisTurnPosEndX( this->GetEndPosForFixingPoint().x);
			   break;
		   // ���k���ʡA�W�U���p
		   case CursorDirection_E:
		   case CursorDirection_W:
			   _parent->GetPaintInfo()->GetCMouseControl()->SetThisTurnPosEndY( this->GetEndPosForFixingPoint().y);
			   break;
		   default:
			   //to do nothing
			   break;
	  }
}


