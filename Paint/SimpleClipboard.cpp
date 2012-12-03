#include "StdAfx.h"
#include "SimpleClipboard.h"
#include "CommonVar.h"
#include "ClipboardControl.h"
#include "PaintInfo.h"
#include <math.h>

#include "BaseListNode.h"
#include "Memento.h"


extern MyListController gMyList;
SimpleClipboard::SimpleClipboard(void)
{
	this->RestOffset();
	this->_buffer = NULL;
}


SimpleClipboard::~SimpleClipboard(void)
{
	this->_parent = NULL;
	
	if( !_pText)
		delete _pText;
	this->_pText = NULL;

	if( !_buffer)
		delete _buffer;
	this->_buffer = NULL;
	
}

/*********************************
* 決定貼上圖形往哪方向貼
**********************************/
void SimpleClipboard::DecidePasteGraphPos()
{
	HWND hWnd = _parent->GetPaintInfo()->GetMainHwnd();
	POINT testPt;
	POINT tmpPt;

	clipObj data;
	data = *_buffer;
	/*******************
	* 決定方向
	*********************/
	//拿左上的點判斷
	if( _xOffset < 0 && _yOffset < 0 )
	{
		testPt.x = min ( data.ptBeg.x , data.ptEnd.x );
	    testPt.y = min ( data.ptBeg.y , data.ptEnd.y );
	    tmpPt = testPt;

		this->X_BoundaryDetection(hWnd,testPt );
		this->Y_BoundaryDetection(hWnd,testPt );

		data.ptEnd.x -= abs(testPt.x - tmpPt.x);
		data.ptEnd.y -= abs(testPt.y - tmpPt.y);
		data.ptBeg.x -= abs(testPt.x - tmpPt.x);
        data.ptBeg.y -= abs(testPt.y - tmpPt.y);

	}
	//拿左下的點判斷
	else if( _xOffset < 0 && _yOffset > 0 )
	{
		testPt.x = min ( data.ptBeg.x , data.ptEnd.x );
	    testPt.y = max ( data.ptBeg.y , data.ptEnd.y );
	    tmpPt = testPt;

		this->X_BoundaryDetection(hWnd,testPt );
		this->Y_BoundaryDetection(hWnd,testPt );
	
		data.ptEnd.x -= abs(testPt.x - tmpPt.x);
		data.ptEnd.y += abs(testPt.y - tmpPt.y);
		data.ptBeg.x -= abs(testPt.x - tmpPt.x);
        data.ptBeg.y += abs(testPt.y - tmpPt.y);

		
	}
	//拿右下
	else if( _xOffset > 0 && _yOffset >0 )
	{
		testPt.x = max ( data.ptBeg.x , data.ptEnd.x );
	    testPt.y = max ( data.ptBeg.y , data.ptEnd.y );
	    tmpPt = testPt;

		this->X_BoundaryDetection(hWnd,testPt );
		this->Y_BoundaryDetection(hWnd,testPt );
	
		data.ptEnd.x += abs(testPt.x - tmpPt.x);
		data.ptEnd.y += abs(testPt.y - tmpPt.y);
		data.ptBeg.x += abs(testPt.x - tmpPt.x);
        data.ptBeg.y += abs(testPt.y - tmpPt.y);

	
	}
	//拿右上
	else
	{
		testPt.x = max ( data.ptBeg.x , data.ptEnd.x );
	    testPt.y = min ( data.ptBeg.y , data.ptEnd.y );
	    tmpPt = testPt;

		this->X_BoundaryDetection(hWnd,testPt );
		this->Y_BoundaryDetection(hWnd,testPt );
	
		data.ptEnd.x += abs(testPt.x - tmpPt.x);
		data.ptEnd.y -= abs(testPt.y - tmpPt.y);
		data.ptBeg.x += abs(testPt.x - tmpPt.x);
        data.ptBeg.y -= abs(testPt.y - tmpPt.y);
	}

	
		_buffer->wordIniPt.x += ( data.ptBeg.x - _buffer->ptBeg.x );
		_buffer->wordIniPt.y += ( data.ptBeg.y - _buffer->ptBeg.y );
		_buffer->ptBeg = data.ptBeg;
		_buffer->ptEnd = data.ptEnd;

}

/** 從redoUndo佇列找下一個目標*/
void SimpleClipboard::MoveSelectRecIntoLastTarGetRecByDrawMap()
{
	if( gMyList.IsNull() == FALSE )
	{
		POINT ptStart, ptEnd;
	
		ptStart = gMyList.GetCurrentPtrPosBeg();
		ptEnd   = gMyList.GetCurrentPtrPosEnd();
		int type = gMyList.GetCurrentPtrType();

		/** 設定選取方框*/
		_parent->GetPaintInfo()->GetCSelectRecInfo().SetType(type);
		_parent->GetPaintInfo()->GetCSelectRecInfo().SetIsShow(TRUE);
		_parent->GetPaintInfo()->GetCSelectRecInfo().SetPosStart(ptStart);
		_parent->GetPaintInfo()->GetCSelectRecInfo().SetPosEnd(ptEnd);

		_parent->GetPaintInfo()->SetHitID( gMyList.GetCurrentNo() );
	}

	else
	{
		_parent->GetPaintInfo()->SetHitID( -1 );
		_parent->GetPaintInfo()->GetCSelectRecInfo().SetIsShow(FALSE);
	}

}

/***********************************
* 還原每次貼圖時，移動的x、y方向向量
************************************/
void SimpleClipboard::RestOffset()
{
	_xOffset = -CommonVar::SHIFT_PIXEL;
	_yOffset = -CommonVar::SHIFT_PIXEL;
}

/****************************
* 貼圖是否碰到 x方向的邊界
*****************************/
void SimpleClipboard::X_BoundaryDetection(HWND hWnd,POINT &pt)
{
	//this->Tune_X_View(hWnd,pt.x);

	//x 邊界判定修正
	if( pt.x + _xOffset <= 0 )
    {
         pt.x = 0;

		 if( _xOffset > 0 )
			 _xOffset = -CommonVar::SHIFT_PIXEL;
		 else
			 _xOffset = CommonVar::SHIFT_PIXEL;
    }
	else if( pt.x + _xOffset - _parent->GetPaintInfo()->GetScrollBarHandle()->GetHorzPos() < 0 )
    {
		pt.x += _xOffset;
		//_parent->GetPaintInfo()->GetScrollBarHandle()->HorizontalScrollBarAutoEvent( hWnd, - _xOffset );

    }
	else if(  pt.x + _xOffset >= CommonVar::DRAW_MAP_WIDTH)
    {
		  pt.x = CommonVar::DRAW_MAP_WIDTH;

		 if( _xOffset > 0 )
			 _xOffset = -CommonVar::SHIFT_PIXEL;
		 else
			 _xOffset = CommonVar::SHIFT_PIXEL;
    }
	else if( pt.x + _xOffset - _parent->GetPaintInfo()->GetScrollBarHandle()->GetHorzPos() > _parent->GetPaintInfo()->GetClientWidth() )
    {
		//_parent->GetPaintInfo()->GetScrollBarHandle()->HorizontalScrollBarAutoEvent( hWnd, _xOffset);
		pt.x += _xOffset;
    }
	else
	{
		pt.x += _xOffset;
	}

}

/****************************
* 貼圖是否碰到 y方向的邊界
*****************************/
void SimpleClipboard::Y_BoundaryDetection(HWND hWnd,POINT &pt)
{
	//this->Tune_Y_View(hWnd,pt.y);

	if( pt.y + _yOffset <= 0 )
	{
		   pt.y = 0;

		 if( _yOffset > 0 )
			 _yOffset = -CommonVar::SHIFT_PIXEL;
		 else
			 _yOffset = CommonVar::SHIFT_PIXEL;
	}
	// y邊界判定修正
	else if(  pt.y + _yOffset - _parent->GetPaintInfo()->GetScrollBarHandle()->GetVertPos() < 0  )
    {
	    pt.y += _yOffset;
		//_parent->GetPaintInfo()->GetScrollBarHandle()->VerticalScrollBarAutoEvent( hWnd, _yOffset );
    }
  
	else if (  pt.y + _yOffset >= CommonVar::DRAW_MAP_HEIGHT)
    {
		  pt.y  =  CommonVar::DRAW_MAP_HEIGHT;

		  if( _yOffset > 0 )
			 _yOffset = -CommonVar::SHIFT_PIXEL;
		 else
			 _yOffset = CommonVar::SHIFT_PIXEL;
    }
	else if( pt.y + _yOffset - _parent->GetPaintInfo()->GetScrollBarHandle()->GetVertPos() > _parent->GetPaintInfo()->GetClientHeight() )
    {
       //_parent->GetPaintInfo()->GetScrollBarHandle()->VerticalScrollBarAutoEvent( hWnd, _yOffset);
	   pt.y += _yOffset;
    }
	else
		pt.y += _yOffset;
}