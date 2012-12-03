#include "StdAfx.h"
#include "BasisGraph.h"
#include "CommonVar.h"
#include "Toolkits.h"
BasisGraph::BasisGraph(void)
{
	this->ResetClipRgnPt();
}


BasisGraph::~BasisGraph(void)
{
}


// ��ܵ�u��������νd��
void BasisGraph::ShowRec(HDC &copyMemDC,int posStartX, int posStartY, int posEndX, int posEndY)
{
	//�Yx��V �V�q��0  �� y��V �V�q��0 �h���e��νd��
	if( abs(posStartX - posEndX ) < 3)
		return;
	else if( abs(posStartY - posEndY ) < 3 )
		return;

	//�@�ӯx��
	HPEN recPen = CreatePen(PS_DOT, 1, CommonVar::COLOR_PURPLE);
    HBRUSH recBrush = (HBRUSH) GetStockObject(NULL_BRUSH);

    int saveDC = SaveDC(copyMemDC);
    SelectObject(copyMemDC, recBrush);
    SelectObject(copyMemDC, recPen);
    Rectangle(copyMemDC, posStartX, posStartY, posEndX , posEndY);
    RestoreDC(copyMemDC,saveDC);	
    DeleteObject( recPen );
    DeleteObject( recBrush );
}

//��ܰ������K�Ӥp����ζ��I
void BasisGraph::ShowSmallRec(HDC &copyMemDC,int posStartX, int posStartY, int posEndX, int posEndY,COLORREF bgColor)
{
	//9�Ӥp�x��
    HBRUSH recBrush = CreateSolidBrush(bgColor);
	
    int saveDC = SaveDC(copyMemDC);
    SelectObject(copyMemDC, recBrush);


	POINT pt[8];

	DetectPointOutput(pt, posStartX, posStartY, posEndX, posEndY);


	for( int detectPointCounter = 0; detectPointCounter < 8 ; detectPointCounter++ )
	{
		Rectangle( copyMemDC
			,pt[detectPointCounter].x - CommonVar::SMALL_RECT_BUFFER
			,pt[detectPointCounter].y - CommonVar::SMALL_RECT_BUFFER
			,pt[detectPointCounter].x + CommonVar::SMALL_RECT_BUFFER
			,pt[detectPointCounter].y + CommonVar::SMALL_RECT_BUFFER);
	}

	RestoreDC(copyMemDC,saveDC);

    DeleteObject( recBrush );
}

/** ���Ͱ������K���I*/
void BasisGraph::DetectPointOutput(POINT* arrDetectPointSet,int posStartX, int posStartY, int posEndX, int posEndY)
{
	//���M�w�_�l�I�۹�զX( ���W->�k�U or �k�U->���U)�A�T�w�ƹ���V�����T��
	
	if( posStartX > posEndX )
	{
	   //�_�I�b���I���k�U
	   if( posStartY > posEndY )
	   {
	       ToolKits::Swap(posStartX,posEndX);
	       ToolKits::Swap(posStartY,posEndY);
	   }
	   //�_�I�b���I���k�W
	   else
	   {
	       ToolKits::Swap(posStartX,posEndX);
	   }
	}
	else
	{
	  //�_�I�b���I�����U
	   if( posStartY > posEndY )
	   {
	      ToolKits::Swap(posStartY,posEndY);
	   }
	   //�_�I�b���I�����W
	   else
	   {
	       //do nothing
	   }
	} 
	//POINT arrDetectPointSet[8];
	POINT pt;

	pt.x = posStartX; pt.y = posStartY; // NW             
	arrDetectPointSet[0] = pt;

    pt.x = posEndX; pt.y = posEndY;     // SE                 
	arrDetectPointSet[1] = pt;

	pt.x = posStartX; pt.y = posEndY;   // SW              
	arrDetectPointSet[2] = pt;

	pt.x = posEndX; pt.y = posStartY;   // NE
	arrDetectPointSet[3] = pt;

	pt.x = (posStartX+posEndX)/2; pt.y = posStartY;  // N
	arrDetectPointSet[4] = pt;

	pt.x = posStartX; pt.y = (posStartY+posEndY)/2;  // W
	arrDetectPointSet[5] = pt;

	pt.x = posEndX; pt.y = (posStartY+posEndY)/2;    // E
	arrDetectPointSet[6] = pt;

	pt.x = (posStartX+posEndX)/2; pt.y = posEndY;    // S
	arrDetectPointSet[7] = pt;

}

// �M�w�ƹ��Ϊ�
void BasisGraph::DecideCursorShape(int direction)
{
	  switch( direction )
	 {
		   case CursorDirection_NW:
			    SetCursor( LoadCursor(NULL,IDC_SIZENWSE)); 
			   break;
		   case CursorDirection_N:
			    SetCursor( LoadCursor(NULL,IDC_SIZENS)); 
			   break;
		   case CursorDirection_NE:
			    SetCursor( LoadCursor(NULL,IDC_SIZENESW)); 
			   break;
		   case CursorDirection_E:
			    SetCursor( LoadCursor(NULL,IDC_SIZEWE)); 
			   break;
		   case CursorDirection_SE:
			    SetCursor( LoadCursor(NULL,IDC_SIZENWSE)); 
			   break;
		   case CursorDirection_S:
			    SetCursor( LoadCursor(NULL,IDC_SIZENS)); 
			   break;
		   case CursorDirection_SW:
			    SetCursor( LoadCursor(NULL,IDC_SIZENESW)); 
			   break;
		   case CursorDirection_W:
			    SetCursor( LoadCursor(NULL,IDC_SIZEWE)); 
			   break;
		   case CursorDirection_SIZEALL:
			    SetCursor( LoadCursor(NULL,IDC_SIZEALL)); 
			   break;
		   default:
			   //to do something
			   break;
	  }
}

// ���] clipRgn ���d��
void BasisGraph::ResetClipRgnPt()
{
	this->_clipRgnPt1.x = this->_clipRgnPt1.y = 0;
	this->_clipRgnPt2.x = CommonVar::DRAW_MAP_WIDTH;
	this->_clipRgnPt2.y = CommonVar::DRAW_MAP_HEIGHT;
}


/****************************************
* Getter // setter
*******************************************/

void BasisGraph::SetEndPosForFixingPoint(POINT pt)
{
	_EndPosForFixingPoint.x = pt.x;
	_EndPosForFixingPoint.y = pt.y;

}

POINT BasisGraph::GetEndPosForFixingPoint()
{
	return _EndPosForFixingPoint;
}

void BasisGraph::SetClipRgnPt1(POINT pt)
{
	this->_clipRgnPt1 = pt ;
}

void BasisGraph::SetClipRgnPt2(POINT pt)
{
	this->_clipRgnPt2 = pt;
}