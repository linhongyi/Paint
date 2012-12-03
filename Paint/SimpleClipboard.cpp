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
* �M�w�K�W�ϧΩ�����V�K
**********************************/
void SimpleClipboard::DecidePasteGraphPos()
{
	HWND hWnd = _parent->GetPaintInfo()->GetMainHwnd();
	POINT testPt;
	POINT tmpPt;

	clipObj data;
	data = *_buffer;
	/*******************
	* �M�w��V
	*********************/
	//�����W���I�P�_
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
	//�����U���I�P�_
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
	//���k�U
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
	//���k�W
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

/** �qredoUndo��C��U�@�ӥؼ�*/
void SimpleClipboard::MoveSelectRecIntoLastTarGetRecByDrawMap()
{
	if( gMyList.IsNull() == FALSE )
	{
		POINT ptStart, ptEnd;
	
		ptStart = gMyList.GetCurrentPtrPosBeg();
		ptEnd   = gMyList.GetCurrentPtrPosEnd();
		int type = gMyList.GetCurrentPtrType();

		/** �]�w������*/
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
* �٭�C���K�ϮɡA���ʪ�x�By��V�V�q
************************************/
void SimpleClipboard::RestOffset()
{
	_xOffset = -CommonVar::SHIFT_PIXEL;
	_yOffset = -CommonVar::SHIFT_PIXEL;
}

/****************************
* �K�ϬO�_�I�� x��V�����
*****************************/
void SimpleClipboard::X_BoundaryDetection(HWND hWnd,POINT &pt)
{
	//this->Tune_X_View(hWnd,pt.x);

	//x ��ɧP�w�ץ�
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
* �K�ϬO�_�I�� y��V�����
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
	// y��ɧP�w�ץ�
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