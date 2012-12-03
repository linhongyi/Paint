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

/** �e��� */
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
*  �Ϲ������V����  
*  mouseEndX, mouseEndY �ƹ������y�СA�D�ϧΪ��﨤�u�y��
*  �N�y�СA�ন�ϧγ��V�᪺�y��
**********************/
void DrawRec::FixPosForSingleMovingGraph(int cursorShape)
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