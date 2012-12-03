#include "StdAfx.h"
#include "DrawTool.h"
#include "PaintInfo.h"
#include "PenSetting.h"
#include "DebugMsgOutputHandle.h"
#include "CommonVar.h"
#include "math.h"

#include "ToolKits.h"
#include <list>
#include <deque>
#include <assert.h>
#include "MyListController.h"
#include "SgWordDataModelControler.h"
#include "MyListNode.h"

extern MyListController gMyList;


DrawTool::DrawTool()
{ 

}

DrawTool::~DrawTool()
{
}

void DrawTool::SetParent(PaintInfo *ptr)
{
	_parent = ptr;
}

/**捲動時螢幕重繪*/
void DrawTool::ScrollBarEventForDrawSomething(HWND hwnd)
{


	int cx = _parent->GetClientWidth();
	int cy = _parent->GetClientHeight();
	int gx = _parent->GetScrollBarHandle()->GetHorzPos();
	int gy = _parent->GetScrollBarHandle()->GetVertPos();


	if( cx + gx > CommonVar::DRAW_MAP_WIDTH )
	{
		gx =  CommonVar::DRAW_MAP_WIDTH - cx;

		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask  = SIF_ALL;
		GetScrollInfo(hwnd, SB_HORZ , &si);
		si.nPos = gx;
		si.fMask = SIF_POS;
		SetScrollInfo(hwnd,SB_HORZ,&si,TRUE);

		_parent->GetScrollBarHandle()->SetHorzPos( si.nPos );
	}

	if( cy + gy > CommonVar::DRAW_MAP_HEIGHT )
	{
		gy =  CommonVar::DRAW_MAP_HEIGHT - cy;

		SCROLLINFO sy;
		sy.cbSize = sizeof(sy);
		sy.fMask  = SIF_ALL;
		GetScrollInfo(hwnd, SB_VERT , &sy);
		sy.nPos = gy;
		sy.fMask = SIF_POS;
		SetScrollInfo(hwnd,SB_VERT,&sy,TRUE);

		_parent->GetScrollBarHandle()->SetVertPos( sy.nPos );
	}

	//this->MapDCToMonitor();
}


/** 清除螢幕*/
void DrawTool::ClearMonitor()
{
	HDC hdc = GetDC(_parent->GetMainHwnd());
	RECT rectClient;

	GetClientRect(_parent->GetMainHwnd(), &rectClient);
	int cx = rectClient.right - rectClient.left;
	int cy = rectClient.bottom - rectClient.top;

	HDC hdcMem = CreateCompatibleDC(hdc); // memory object for hdc, whose size is 1X1.
	HBITMAP bmpMem = CreateCompatibleBitmap(hdc, cx, cy);

	SelectObject(hdcMem, bmpMem); // Let hdcMem's size become bigger as bmpMem

	FillRect(hdcMem, &rectClient, (HBRUSH) GetStockObject(WHITE_BRUSH));

	BitBlt(hdc, 0, 0, cx, cy, hdcMem, 0, 0, SRCCOPY);

	DeleteObject(bmpMem);
	DeleteDC(hdcMem);
	ReleaseDC(_parent->GetMainHwnd(),hdc);
}

/**畫布DC至視窗*/
void DrawTool::MapDCToMonitor()
{

	HWND hWnd = _parent->GetMainHwnd();
	HDC hdc = GetDC(hWnd);	

	RECT rectClient;
	if ( ::GetClientRect(hWnd, &rectClient ) == FALSE )
	{
		DebugMsgOutputHandle::Instance()->ErrMsg();
	}

	HDC hdcMem = CreateCompatibleDC(hdc); // memory object for hdc, whose size is 1X1.
	HBITMAP bmpMem = CreateCompatibleBitmap(hdc,rectClient.right-rectClient.left , rectClient.bottom-rectClient.top);
	SelectObject(hdcMem, bmpMem); // Let hdcMem's size become bigger as bmpMem

	if( !hdcMem )
	{
		DebugMsgOutputHandle::Instance()->ErrMsg();
		MessageBox(hWnd,_T("fatal error"),NULL,NULL);
	}
	//重繪, 將畫布DC 貼至 視窗DC上
	MapDcPasterToWindowDC(hdcMem);

	if( _parent->GetCSelectRecInfo().GetIsShow() == TRUE )
	{
		POINT sensorPosPt1, sensorPosPt2;

		sensorPosPt1.x = _parent->GetCSelectRecInfo().GetPosStart().x;
		sensorPosPt1.y = _parent->GetCSelectRecInfo().GetPosStart().y;

		sensorPosPt2.x = _parent->GetCSelectRecInfo().GetPosEnd().x;
		sensorPosPt2.y = _parent->GetCSelectRecInfo().GetPosEnd().y;

		this->ScreenToWindowCoordinate(sensorPosPt1.x,sensorPosPt1.y);
		this->ScreenToWindowCoordinate(sensorPosPt2.x,sensorPosPt2.y);


		int type = _parent->GetCSelectRecInfo().GetType();

		if( type != IDM_WORD)
		{
			this->FigSenSorDraw(
				hdcMem
				,type
				,sensorPosPt1.x
				,sensorPosPt1.y
				,sensorPosPt2.x
				,sensorPosPt2.y
				,CommonVar::COLOR_ORANGE);

			if( type == IDM_WORD_REC )
				_parent->GetCTextOut()->InverseTextWordToDC(hdcMem);
		}

	}
	BitBlt(hdc, 0, 0, rectClient.right-rectClient.left, rectClient.bottom-rectClient.top, hdcMem,0,0, SRCCOPY);

	ReleaseDC(hWnd,hdc);
	DeleteObject(bmpMem);
	DeleteDC(hdcMem);
	//DeleteDC(hdc);
}

//偵測命中時螢幕畫面控制
void DrawTool::CurrentSelectRecInfoShow(int iDetectionIndex)
{
	HWND hWnd = _parent->GetMainHwnd();

	//初始化DC
	HDC hdc = GetDC(hWnd);	
	RECT rectClient;
	::GetClientRect(hWnd,&rectClient);

	HDC hdcMem = CreateCompatibleDC(hdc); // memory object for hdc, whose size is 1X1.
	HBITMAP bmpMem = CreateCompatibleBitmap(hdc,rectClient.right-rectClient.left , rectClient.bottom-rectClient.top);
	SelectObject(hdcMem, bmpMem); 

	//產生的螢幕DC，讓畫布DC選適當位置貼上( 畫布DC中不含 等待選取的方框 )
	this->MapDcPasterToWindowDC(hdcMem);

	/****************************
	*從佇列中拿目前命中的圖形
	****************************/
	//取得目前所找到要修改的圖案，在佇列中位置的型態(直線、橢圓、方框)

	gMyList.visitID_Node(iDetectionIndex);

	int type = gMyList.GetCurrentPtrType();

	POINT currNodeStartPt, currNodeEndPt;
	currNodeStartPt.x = gMyList.GetCurrentPtrPosBeg().x;
	currNodeStartPt.y = gMyList.GetCurrentPtrPosBeg().y;
	currNodeEndPt.x   = gMyList.GetCurrentPtrPosEnd().x;
	currNodeEndPt.y   = gMyList.GetCurrentPtrPosEnd().y;

	this->ScreenToWindowCoordinate(currNodeStartPt.x ,currNodeStartPt.y);
	this->ScreenToWindowCoordinate(currNodeEndPt.x, currNodeEndPt.y);

	//產生等待選取的方框
	this->FigSenSorDraw(hdcMem,type,currNodeStartPt.x,currNodeStartPt.y,currNodeEndPt.x ,currNodeEndPt.y,CommonVar::COLOR_PURPLE);

	//目前已選取的方框
	if( _parent->GetCSelectRecInfo().GetIsShow() == TRUE )
	{
		POINT sensorPosPt1, sensorPosPt2;

		sensorPosPt1.x = _parent->GetCSelectRecInfo().GetPosStart().x;
		sensorPosPt1.y = _parent->GetCSelectRecInfo().GetPosStart().y;

		sensorPosPt2.x = _parent->GetCSelectRecInfo().GetPosEnd().x;
		sensorPosPt2.y = _parent->GetCSelectRecInfo().GetPosEnd().y;

		this->ScreenToWindowCoordinate(sensorPosPt1.x,sensorPosPt1.y);
		this->ScreenToWindowCoordinate(sensorPosPt2.x,sensorPosPt2.y);


		this->FigSenSorDraw(hdcMem
			,_parent->GetCSelectRecInfo().GetType()
			,sensorPosPt1.x
			,sensorPosPt1.y
			,sensorPosPt2.x
			,sensorPosPt2.y
			,CommonVar::COLOR_ORANGE);

		if( _parent->GetCSelectRecInfo().GetType() == IDM_WORD_REC )
			_parent->GetCTextOut()->InverseTextWordToDC(hdcMem);
	}


	//將螢幕DC貼至螢幕上
	BitBlt(hdc, 0, 0, rectClient.right-rectClient.left, rectClient.bottom-rectClient.top, hdcMem,0,0, SRCCOPY);

	DeleteObject(hdcMem);
	DeleteObject(bmpMem);
	ReleaseDC(hWnd,hdc);
}


/** 初始化打字方框*/
void DrawTool::ShowTarGetRecForTypingWord()
{
	POINT pt1 = _parent->GetCSelectRecInfo().GetPosStart();
	POINT pt2 = _parent->GetCSelectRecInfo().GetPosEnd();

	this->ScreenToWindowCoordinate(pt1.x,pt1.y);
	this->ScreenToWindowCoordinate(pt2.x,pt2.y);
	//初始化DC
	HWND hWnd = _parent->GetMainHwnd();

	HDC hdc = GetDC(hWnd);	
	RECT rectClient;
	::GetClientRect(hWnd,&rectClient);

	HDC hdcMem = CreateCompatibleDC(hdc); 
	HBITMAP bmpMem = CreateCompatibleBitmap(hdc,rectClient.right-rectClient.left , rectClient.bottom-rectClient.top);
	SelectObject(hdcMem, bmpMem); 

	this->MapDcPasterToWindowDC(hdcMem);
	this->FigSenSorDraw(hdcMem,IDM_REC, pt1.x, pt1.y, pt2.x, pt2.y, CommonVar::COLOR_ORANGE);
	BitBlt(hdc, 0, 0, rectClient.right-rectClient.left, rectClient.bottom-rectClient.top, hdcMem,0,0, SRCCOPY);

	::DeleteObject(bmpMem);
	::DeleteDC(hdcMem);
	::ReleaseDC(hWnd,hdc);
}



/** 設定滑鼠游標*/
void DrawTool::SetMouseCurrsorInMouseEvent(int direction)
{
	if( gMyList.GetTotalNum() <= 0 )
		return;

	//取得目前所找到要修改的圖案，在佇列中位置的型態(直線、橢圓、方框)
	int type = gMyList.GetCurrentPtrType();
	//獲得滑鼠方向，並要設定游標
	switch(type)
	{
	case IDM_LINE:
		_drawLine->DecideCursorShape( direction );
		break;
	case IDM_REC:
		_drawRec->DecideCursorShape( direction );
		break;
	case IDM_CIR:
		_drawCircular->DecideCursorShape( direction );
		break;
	case IDM_WORD_REC:
		_drawRec->DecideCursorShape( direction );
		break;
	}
}


/** 畫布DC貼至螢幕DC上*/
void DrawTool::MapDcPasterToWindowDC(HDC &hdcMem)
{
	RECT rectClient;
	GetClientRect(_parent->GetMainHwnd(),&rectClient);
	int cx = rectClient.right - rectClient.left;
	int cy = rectClient.bottom - rectClient.top;

	SCROLLINFO sx, sy;
	GetScrollInfo( _parent->GetMainHwnd() , SB_HORZ, &sx);
	GetScrollInfo( _parent->GetMainHwnd() , SB_VERT, &sy);

	BitBlt( hdcMem
		, 0
		, 0
		, cx
		, cy
		, _parent->GetMemDC()
		,_parent->GetScrollBarHandle()->GetHorzPos()
		,_parent->GetScrollBarHandle()->GetVertPos()
		, SRCCOPY);
}

/*************************
* clipRgn 下重繪所有圖形
**************************/
void DrawTool::RePaintWithClipRgn(POINT recPt1, POINT recPt2)
{
	 if( _parent->GetHitID() < 0 )
    {
        return ;
    }

	ToolKits::Normalization(recPt1,recPt2);
	( recPt1.x - CommonVar::MAX_PEN_WIDTH > 0)? recPt1.x -= CommonVar::MAX_PEN_WIDTH :  recPt1.x = 0;
	( recPt1.y - CommonVar::MAX_PEN_WIDTH > 0)? recPt1.y -= CommonVar::MAX_PEN_WIDTH :  recPt1.y = 0;


	( recPt2.x + CommonVar::MAX_PEN_WIDTH > CommonVar::DRAW_MAP_WIDTH)
		? recPt2.x = CommonVar::DRAW_MAP_WIDTH :  recPt2.x += CommonVar::MAX_PEN_WIDTH;
	( recPt2.y + CommonVar::MAX_PEN_WIDTH > CommonVar::DRAW_MAP_HEIGHT)
		? recPt2.y = CommonVar::DRAW_MAP_HEIGHT :  recPt2.y += CommonVar::MAX_PEN_WIDTH;

	//設定剪貼區域
	this->GetDrawCircular()->SetClipRgnPt1( recPt1 );
	this->GetDrawCircular()->SetClipRgnPt2( recPt2 );
	this->GetDrawLine()->SetClipRgnPt1( recPt1 );
	this->GetDrawLine()->SetClipRgnPt2( recPt2 );
	this->GetDrawRec()->SetClipRgnPt1( recPt1 );
	this->GetDrawRec()->SetClipRgnPt2( recPt2 );

	POINT ptStart, ptEnd;	
	int type;

	int currNodePenColor;
	int currNodePenWidth;
	int currNodeFigBg;

	MyListNode* ptr = gMyList.GetHead();
    
  
	for( ;ptr != NULL;	ptr = ptr->_rlink )
	{ 
		type  = ptr->GetnObjType();
		currNodePenColor  = ptr->GetPenColor();
		currNodeFigBg     = ptr->GetFigBg();

	    ptStart.x = ptr->GetPtBeg().x;
		ptStart.y = ptr->GetPtBeg().y;
		ptEnd.x   = ptr->GetPtEnd().x;
		ptEnd.y   = ptr->GetPtEnd().y;
		
	
		ToolKits::Normalization(ptStart,ptEnd);
		if ( _parent->GetCHitTest()->RegionHitRegion(ptStart,ptEnd,recPt1,recPt2) == FALSE )
		{
			continue;
		}
		//非文字輸出
		else if( type != IDM_WORD_REC)
		{
			currNodePenWidth  = ptr->GetPenWidth();

			
			this->DrawFig(_parent->GetMemDC()
				,type
				,ptr->GetPtBeg().x
				,ptr->GetPtBeg().y
				,ptr->GetPtEnd().x 
				,ptr->GetPtEnd().y 
				,currNodePenColor 
				,currNodeFigBg
				,currNodePenWidth );
		}
		//文字
		else
		{
			this->_parent->GetCTextOut()->TextLineWordToDcInWindowView(ptr,_parent->GetMemDC(), recPt1, recPt2);
		}
		
	}

	//重設剪貼區域
	this->GetDrawCircular()->ResetClipRgnPt();
	this->GetDrawLine()->ResetClipRgnPt();
	this->GetDrawRec()->ResetClipRgnPt();
	this->_parent->GetCTextOut()->ResetClipRgnPt();
	ptr = NULL;
}


/*********************
* 塗白方框
**********************/
void DrawTool::FillWhileRec( POINT pt1, POINT pt2)
{
	ToolKits::Normalization(pt1,pt2);
	( pt1.x - CommonVar::MAX_PEN_WIDTH > 0)? pt1.x -= CommonVar::MAX_PEN_WIDTH :  pt1.x = 0;
	( pt1.y - CommonVar::MAX_PEN_WIDTH > 0)? pt1.y -= CommonVar::MAX_PEN_WIDTH :  pt1.y = 0;

	( pt2.x + CommonVar::MAX_PEN_WIDTH > CommonVar::DRAW_MAP_WIDTH)
		? pt2.x = CommonVar::DRAW_MAP_WIDTH :  pt2.x += CommonVar::MAX_PEN_WIDTH;
	( pt2.y + CommonVar::MAX_PEN_WIDTH > CommonVar::DRAW_MAP_HEIGHT)
		? pt2.y = CommonVar::DRAW_MAP_HEIGHT :  pt2.y += CommonVar::MAX_PEN_WIDTH;

	this->GetDrawRec()->RecDraw( _parent->GetMemDC()
		, pt1.x
		, pt1.y
		, pt2.x
		, pt2.y
		, RGB(255,255,255)
		, RGB(255,255,255)
		, 1 );
	
}

/*****************
* 畫圖
*******************/
void DrawTool::DrawFig(HDC &hdcMem,int iType,int iStartPosX,int iStartPosY, int iEndPosX, int iEndPosY, int penColor, int figBg, int penWidth)
{
	// 直線、方框、橢圓、文字
	switch( iType)
	{
	case IDM_LINE:
		_drawLine->LineDraw( hdcMem
			, iStartPosX
			, iStartPosY
			, iEndPosX 
			, iEndPosY
			, PenSetting::TransferIDToPenColor( penColor ) 
			, PenSetting::TransferIDToPenWidth( penWidth ));

		break;
	case IDM_REC:
		_drawRec->RecDraw(hdcMem
			, iStartPosX
			, iStartPosY
			, iEndPosX
			, iEndPosY
			, PenSetting::TransferIDToPenColor( penColor ) 
			, PenSetting::TransferIDToBg( figBg )
			, PenSetting::TransferIDToPenWidth( penWidth ));
		break;
	case IDM_CIR:
		_drawCircular->CirDraw(hdcMem
			, iStartPosX
			, iStartPosY
			, iEndPosX
			, iEndPosY
			, PenSetting::TransferIDToPenColor(  penColor )
			, PenSetting::TransferIDToBg( figBg )
			, PenSetting::TransferIDToPenWidth( penWidth ));
		break;
	case IDM_WORD:
		_drawRec->ShowRec(hdcMem, iStartPosX, iStartPosY, iEndPosX, iEndPosY);
		_drawRec->ShowSmallRec(hdcMem, iStartPosX, iStartPosY, iEndPosX, iEndPosY,CommonVar::COLOR_ORANGE);
	default:
		// MessageBox( _parent->GetMainHwnd() , _T("繪圖選擇格式錯誤"),_T("MouseMoveEventForDrawSomething"),NULL);
		break;
	}
}

/**偵測範圍大矩形和頂點的重繪*/
void DrawTool::FigSenSorDraw(HDC &hdcMem,int iType,int iStartPosX,int iStartPosY, int iEndPosX, int iEndPosY, COLORREF bgColor)
{
	// 直線、方框、橢圓、文字
	switch( iType)
	{
	case IDM_LINE:
		_drawLine->ShowRec(hdcMem, iStartPosX, iStartPosY, iEndPosX, iEndPosY);
		_drawLine->ShowSmallRec(hdcMem, iStartPosX, iStartPosY, iEndPosX, iEndPosY,bgColor);
		break;
	case IDM_REC:
		/*if( iStartPosX > iEndPosX )
			ToolKits::Swap(iStartPosX,iEndPosX);
		if( iStartPosY > iEndPosY )
			ToolKits::Swap(iStartPosY,iEndPosY);*/
	
		_drawRec->ShowRec(hdcMem, iStartPosX-1, iStartPosY-1, iEndPosX+1, iEndPosY+1);
		_drawRec->ShowSmallRec(hdcMem, iStartPosX, iStartPosY, iEndPosX, iEndPosY,bgColor);
		break;
	case IDM_CIR:
		
		_drawCircular->ShowRec(hdcMem, iStartPosX, iStartPosY, iEndPosX, iEndPosY);
		_drawCircular->ShowSmallRec(hdcMem, iStartPosX, iStartPosY, iEndPosX, iEndPosY,bgColor);

		break;
	case IDM_WORD_REC:
		
		_drawRec->ShowRec(hdcMem, iStartPosX, iStartPosY, iEndPosX, iEndPosY);
		_drawRec->ShowSmallRec(hdcMem, iStartPosX, iStartPosY, iEndPosX, iEndPosY,bgColor);
		break;
	case IDM_SELECT:
	case IDM_WORD:
		break;
	default:
		OutputDebugString(_T("不存在此類別能畫感應範圍\n"));

		break;
	}
}

void DrawTool::WindowToScreenCoordinateForStartPos(LONG &x, LONG &y )
{
	x+= _parent->GetScrollBarHandle()->GetBeforeHorzPos();
	y+= _parent->GetScrollBarHandle()->GetBeforeVertPos();
}

void DrawTool::WindowToScreenCoordinateForEndPos(LONG &x, LONG &y )
{
	x+= _parent->GetScrollBarHandle()->GetHorzPos();
	y+= _parent->GetScrollBarHandle()->GetVertPos();
}

void DrawTool::ScreenToWindowCoordinate(LONG &x, LONG &y )
{
	x-= _parent->GetScrollBarHandle()->GetHorzPos();
	y-= _parent->GetScrollBarHandle()->GetVertPos(); 
}


PaintInfo* DrawTool::GetPaintInfo()
{
	return _parent;
}


