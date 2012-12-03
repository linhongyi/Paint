#include "StdAfx.h"
#include "ScrollBarHandle.h"
#include "CommonVar.h"
#include "PaintInfo.h"

#include "ToolKits.h"
#include "MyListController.h"

extern MyListController gMyList;
ScrollBarHandle::ScrollBarHandle(void)
{

	_iHorzPos = 0;
	_iVertPos = 0;

	_iBeforeHorzPos = 0;
	_iBeforeVertPos = 0;
}

ScrollBarHandle::~ScrollBarHandle(void)
{
}

void ScrollBarHandle::SetParent(PaintInfo *ptr)
{
	_parent = ptr;
}

/** 調整ScrollBar*/
void ScrollBarHandle::TurningScrollBar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

	SCROLLINFO si;

    RECT rectClient;
	::GetClientRect(hWnd, &rectClient );

	_parent->SetClientWidth(  LOWORD(lParam) );
	_parent->SetClientHeight( HIWORD(lParam) );

	// Set vertical scroll bar range and page size
	::GetScrollInfo(hWnd, SB_VERT , &si );
	
	si.nPos = this->_iVertPos;
	si.fMask  = SIF_RANGE | SIF_PAGE;
	si.nMin   = 0;
	si.nPage = rectClient.bottom - rectClient.top;
		
	si.nMax = CommonVar::DRAW_MAP_HEIGHT ;

	if ( si.nPos > (int ) (si.nMax - si.nPage) )
	{
		this->_iVertPos =  si.nMax - si.nPage;
		si.nPos = this->_iVertPos;
	}

   
	SetScrollInfo(hWnd, SB_VERT , &si , TRUE );


	// set horizontal scroll bar range and page size

	::GetScrollInfo(hWnd, SB_HORZ , &si );

	si.fMask  = SIF_RANGE | SIF_PAGE;
	si.nMin   = 0;

	si.nPage = rectClient.right - rectClient.left;
	si.nMax   = CommonVar::DRAW_MAP_WIDTH;

	si.nPos = this->_iHorzPos;
	if ( si.nPos > (int) (si.nMax - si.nPage) )
	{
		this->_iHorzPos =  si.nMax - si.nPage;
		si.nPos = this->_iHorzPos;
	}

	SetScrollInfo(hWnd,SB_HORZ,&si, TRUE);


}

/**************************
* y方向捲軸捲動處理
***************************/
void ScrollBarHandle::VerticalScrollBarEvent(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	SCROLLINFO si;

	si.cbSize = sizeof(si);
	si.fMask  = SIF_ALL;
	GetScrollInfo(hWnd, SB_VERT , &si);



	switch( LOWORD( wParam ) )
	{
	case SB_TOP:
		si.nPos = si.nMin;
		break;
	case SB_BOTTOM:
		si.nPos = si.nMax;
		break;
	case SB_LINEUP:
		OutputDebugString(_T("SB_LINEUP\n"));
		si.nPos -= 1;
		break;
	case SB_LINEDOWN:
		OutputDebugString(_T("SB_LINEDOWN\n"));
		si.nPos += 1;
		break;
	case SB_PAGEUP:
		si.nPos -= si.nPage;
		break;
	case SB_PAGEDOWN:
		si.nPos += si.nPage;
		break;
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos;
		break;
	case SB_THUMBPOSITION:
		si.nPos = si.nTrackPos;
		break;

	default:
		break;
	}

	si.fMask = SIF_POS;
	SetScrollInfo(hWnd,SB_VERT,&si,TRUE);

	_iVertPos = si.nPos ;
}

/**************************
* x方向捲軸捲動處理
***************************/
void ScrollBarHandle::HorizontalScrollBarEvent(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	SCROLLINFO si;

	si.cbSize = sizeof(si);
	si.fMask  = SIF_ALL;

	GetScrollInfo(hWnd,SB_HORZ,&si);
	

	switch( LOWORD (wParam))
	{
	case SB_LINELEFT:
		si.nPos -= 1;
		break;
	case SB_LINERIGHT:
		si.nPos += 1;
		break;
	case SB_PAGELEFT:
		si.nPos -= si.nPage;
		break;
	case SB_PAGERIGHT:
		si.nPos += si.nPage;
		break;
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos;
		break;
	case SB_THUMBPOSITION:
		si.nPos = si.nTrackPos;
		break;
	default:
		break;
	}

	si.fMask = SIF_POS;

	SetScrollInfo( hWnd, SB_HORZ, &si , TRUE );

	_iHorzPos = si.nPos;

}

/***************************************
* x方向是否捲動，讓此座標仍然落在視窗內
****************************************/
direction ScrollBarHandle::IsXScroll(int mouseX)
{
	direction result;

	RECT rectClient;
	GetClientRect(_parent->GetMainHwnd(),&rectClient);


	// 達左界
	if ( mouseX > CommonVar::OUT_WINDOW_CONSTANT )
	{

		if( GetHorzPos() > 0)
			result = Left;
		else
			result = LeftBoundary;
	}
	// 右界
	else if( mouseX - (rectClient.right - rectClient.left) + GetHorzPos() > CommonVar::DRAW_MAP_WIDTH )
		result = RightBoundary;
	// 在可視範圍內
	else if( mouseX < (rectClient.right - rectClient.left) )
		result = None;
	// 在可視範圍外
	else
		result = Right;

	return result;
}

/***************************************
* y方向是否捲動，讓此座標仍然落在視窗內
****************************************/
direction ScrollBarHandle::IsYScroll(int mouseY )
{

	direction result;


	//上界
	if ( mouseY >  CommonVar::OUT_WINDOW_CONSTANT  )
	{
		if( GetVertPos() > 0 )
			result = Up;
		else
			result = UpBoundary;
	}
	//下界
	else if(   mouseY - _parent->GetClientHeight()  + GetVertPos() > CommonVar::DRAW_MAP_HEIGHT )
		result = DownBoundary;
	// 在可視範圍內
	else if( mouseY < _parent->GetClientHeight() )
		result = None;
	// 在可視範圍外
	else
		result = Down;

	return result;
}

/*****************************
* 目前捲動軸的位置
******************************/
POINT ScrollBarHandle::GetNowScrollBarPos()
{
	SCROLLINFO si;
	POINT pos;

	si.cbSize = sizeof(si);
	si.fMask  = SIF_ALL;

	GetScrollInfo(_parent->GetMainHwnd(),SB_HORZ,&si);
	pos.x = si.nPos;
	GetScrollInfo(_parent->GetMainHwnd(),SB_VERT,&si);
	pos.y = si.nPos;

	return pos;
}

/** 垂直捲動列自動捲動*/
void ScrollBarHandle::VerticalScrollBarAutoEvent(HWND hWnd,int offset)
{

	// OutputDebugString(_T("-----------------------VerticalScrollBarAutoEvent\n"));
	if( offset == 0 ) return ;

	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask  = SIF_ALL;
	GetScrollInfo(hWnd, SB_VERT , &si);;


	si.nPos += offset;

	if( si.nPos < 0 )
		si.nPos = 0;
	else if( si.nPos >(int) (si.nMax - si.nPage  ))
		si.nPos =  si.nMax - si.nPage ;


	si.fMask = SIF_POS;
	SetScrollInfo(hWnd,SB_VERT,&si,TRUE);

	_iVertPos = si.nPos;   

}


/** 水平捲動列自動捲動*/
void ScrollBarHandle::HorizontalScrollBarAutoEvent(HWND hWnd,int offset)
{
	// OutputDebugString(_T("-----------------------HorizontalScrollBarAutoEvent\n"));
	if( offset == 0 ) return;

	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask  = SIF_ALL;
	GetScrollInfo(hWnd, SB_HORZ , &si);


	si.nPos += offset;

	if( si.nPos < 0 )
		si.nPos = 0;
	else if( si.nPos > (int) (si.nMax -  si.nPage  ))
		si.nPos = si.nMax -  si.nPage ;

	si.fMask = SIF_POS;
	SetScrollInfo(hWnd,SB_HORZ,&si,TRUE);
	_iHorzPos = si.nPos;
}

/*****************************************
* 目標圖形是否落在視窗內; 不在視窗內的話，
* 則調整捲動位置
******************************************/
void ScrollBarHandle::TuneWindowView()
{
	int ID = _parent->GetHitID();
	MyListNode *ptr = gMyList.visitID_Node(ID);

	POINT pt1,pt2;
	pt1 = ptr->GetPtBeg();
	pt2 = ptr->GetPtEnd();

	pt1.x -= CommonVar::SMALL_RECT_BUFFER;
	pt1.y -= CommonVar::SMALL_RECT_BUFFER;
	
	pt2.x += CommonVar::SMALL_RECT_BUFFER;
	pt2.y += CommonVar::SMALL_RECT_BUFFER;
	ToolKits::Normalization(pt1,pt2);

	HWND hWnd = _parent->GetMainHwnd();
	//x
	if( pt1.x > this->GetHorzPos() + _parent->GetClientWidth() )
	{
		this->HorizontalScrollBarAutoEvent( hWnd, pt1.x - this->GetHorzPos() - _parent->GetClientWidth() );
	}
	else if ( pt1.x < this->GetHorzPos()  )
	{
		this->HorizontalScrollBarAutoEvent( hWnd, pt1.x -  this->GetHorzPos() );
	}

	if( pt2.x > this->GetHorzPos() + _parent->GetClientWidth() )
	{
		this->HorizontalScrollBarAutoEvent( hWnd, pt2.x - this->GetHorzPos() - _parent->GetClientWidth() );
	}
	else if( pt2.x < this->GetHorzPos()  )
	{
		this->HorizontalScrollBarAutoEvent( hWnd, pt2.x -  this->GetHorzPos() );
	}
	//y

	//若貼上去的圖層不落在可視視窗內 ( 目標y座標 不大於目前y方向捲動軸 + 可視視窗高度
	if( pt1.y > this->GetVertPos() + _parent->GetClientHeight() )
	{
		this->VerticalScrollBarAutoEvent( hWnd, pt1.y - this->GetVertPos() - _parent->GetClientHeight() );
	}
	else if( pt1.y < this->GetVertPos()  )
	{
		this->VerticalScrollBarAutoEvent( hWnd, pt1.y - this->GetVertPos());
	}

	if( pt2.y > this->GetVertPos() + _parent->GetClientHeight() )
	{
		this->VerticalScrollBarAutoEvent( hWnd, pt2.y - this->GetVertPos() - _parent->GetClientHeight() );
	}
	else if( pt2.y < this->GetVertPos()  )
	{
		this->VerticalScrollBarAutoEvent( hWnd, pt2.y - this->GetVertPos());
	}
}
/*********************
*  Getter // setter
**********************/
int ScrollBarHandle::GetHorzPos()
{
	return _iHorzPos;
}

int ScrollBarHandle::GetVertPos()
{
	return _iVertPos;
}


int ScrollBarHandle::GetBeforeHorzPos()
{
	return _iBeforeHorzPos;
}

int ScrollBarHandle::GetBeforeVertPos()
{
	return _iBeforeVertPos;
}

void ScrollBarHandle::SetBeforeHorzPos(int val)
{
	_iBeforeHorzPos = val;
} 

void ScrollBarHandle::SetBeforeVertPos(int val)
{
	_iBeforeVertPos = val;
} 

void ScrollBarHandle::SetHorzPos(int val)
{
	_iHorzPos = val;
} 

void ScrollBarHandle::SetVertPos(int val)
{
	_iVertPos = val;
} 