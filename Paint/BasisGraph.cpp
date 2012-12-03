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


// 顯示虛線偵測的方形範圍
void BasisGraph::ShowRec(HDC &copyMemDC,int posStartX, int posStartY, int posEndX, int posEndY)
{
	//若x方向 向量為0  或 y方向 向量為0 則不畫方形範圍
	if( abs(posStartX - posEndX ) < 3)
		return;
	else if( abs(posStartY - posEndY ) < 3 )
		return;

	//一個矩型
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

//顯示偵測的八個小正方形項點
void BasisGraph::ShowSmallRec(HDC &copyMemDC,int posStartX, int posStartY, int posEndX, int posEndY,COLORREF bgColor)
{
	//9個小矩型
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

/** 產生偵測的八個點*/
void BasisGraph::DetectPointOutput(POINT* arrDetectPointSet,int posStartX, int posStartY, int posEndX, int posEndY)
{
	//先決定起始點相對組合( 左上->右下 or 右下->左下)，確定滑鼠方向的正確性
	
	if( posStartX > posEndX )
	{
	   //起點在終點的右下
	   if( posStartY > posEndY )
	   {
	       ToolKits::Swap(posStartX,posEndX);
	       ToolKits::Swap(posStartY,posEndY);
	   }
	   //起點在終點的右上
	   else
	   {
	       ToolKits::Swap(posStartX,posEndX);
	   }
	}
	else
	{
	  //起點在終點的左下
	   if( posStartY > posEndY )
	   {
	      ToolKits::Swap(posStartY,posEndY);
	   }
	   //起點在終點的左上
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

// 決定滑鼠形狀
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

// 重設 clipRgn 的範圍
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