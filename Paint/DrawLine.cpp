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

/** �e���u*/
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
*  �Ϲ������V����  
*  mouseEndX, mouseEndY �ƹ������y�СA�D�ϧΪ��﨤�u�y��
*  �N�y�СA�ন�ϧγ��V�᪺�y��
**********************/
void DrawLine::FixPosForSingleMovingGraph(int cursorShape)
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