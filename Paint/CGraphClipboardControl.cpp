#include "StdAfx.h"
#include "CGraphClipboardControl.h"
#include "CommonVar.h"
#include "PaintInfo.h"
#include "ToolKits.h"
#include "MyListController.h"
#include <assert.h>

extern MyListController gMyList;
CGraphClipboardControl::CGraphClipboardControl(void)
{
	
}


CGraphClipboardControl::~CGraphClipboardControl(void)
{
		_parent = NULL;
}

/*******************
* 剪貼簿複製圖形
********************/
void CGraphClipboardControl::Copy(HWND hWnd,int type, POINT pt1, POINT pt2, int penColor, int figBg, int penWidth)
{
    UINT format = RegisterClipboardFormat(_T("MY_CUSTOM_FORMAT"));

	::OpenClipboard(hWnd);
	::EmptyClipboard();

	clipObj data;
	data.nObjType = type;
	data.ptBeg = pt1;
	data.ptEnd = pt2;
	data.iPenColor = penColor;
	data.iPenWidth = penWidth;
	data.iFigBg    = figBg;
	

	//allocate some global memory
	HGLOBAL clipbuffer;
	clipbuffer = GlobalAlloc(GMEM_DDESHARE, sizeof(clipObj));
	clipObj * buffer = (clipObj*)GlobalLock(clipbuffer);

	//put the data into that memory
	*buffer = data;

	//Put it on the clipboard
	GlobalUnlock(clipbuffer);
	SetClipboardData(format,clipbuffer);
	CloseClipboard();

	//reset direction
	_xOffset = -CommonVar::SHIFT_PIXEL;
	_yOffset = -CommonVar::SHIFT_PIXEL;
}

/*******************************
剪下圖
********************************/
void CGraphClipboardControl::CutGraph()
{
	int ID = _parent->GetPaintInfo()->GetHitID();
	
	MyListNode *ptr = gMyList.visitID_Node(ID);

	POINT pt1;
	pt1.x = ptr->GetPtBeg().x;
	pt1.y = ptr->GetPtBeg().y;

	POINT pt2;
	pt2.x = ptr->GetPtEnd().x;
	pt2.y = ptr->GetPtEnd().y;

	/**塗白*/
	_parent->GetPaintInfo()->GetDrawTool()->FillWhileRec(pt1,pt2);
	
	bool bDeleted = gMyList.DeleteID_Node(ID);

	assert ( bDeleted == true );
	

	this->MoveSelectRecIntoLastTarGetRecByDrawMap();
	

	//若刪除是最後一個節點則不用調整視窗
	if( _parent->GetPaintInfo()->GetHitID() >= 0 ) 
	{
		_parent->GetPaintInfo()->GetScrollBarHandle()->TuneWindowView();
	}
	
	_parent->GetPaintInfo()->GetDrawTool()->RePaintWithClipRgn(pt1,pt2);

}


/*******************
* 剪貼簿貼上圖形
********************/
void CGraphClipboardControl::Paste(clipObj* pBuffer)
{
	HDC drawMapDc = _parent->GetPaintInfo()->GetMemDC();
	
	
	//Get the buffer
	this->_buffer = pBuffer;
	this->DecidePasteGraphPos( );

	///**畫新圖在畫布DC上*/
	_parent->GetPaintInfo()->GetDrawTool()->DrawFig(
		_parent->GetPaintInfo()->GetMemDC(),_buffer->nObjType,_buffer->ptBeg.x,_buffer->ptBeg.y,_buffer->ptEnd.x,_buffer->ptEnd.y,_buffer->iPenColor,_buffer->iFigBg,_buffer->iPenWidth);


	//若此次為一段文字的起頭，則必需檢查上個節點是否為文字的起頭 (TCHAR) -1 ，cha ==65535
	if ( gMyList.CheckIsRemoveNullWordNode(this->_parent->GetPaintInfo()->GetHitID()) == TRUE )
	{
		this->_parent->GetPaintInfo()->GetCSelectRecInfo().SetIsShow(false);
		this->_parent->GetPaintInfo()->SetHitID(-1);
		this->_parent->GetPaintInfo()->GetDrawTool()->MapDCToMonitor();
	}

	gMyList.AddGraphNode(
		_buffer->nObjType
		,_buffer->ptBeg
		,_buffer->ptEnd
		,_buffer->iPenColor
		,_buffer->iFigBg
		,_buffer->iPenWidth
		);
	
	/** 設定選取方框*/
	_parent->GetPaintInfo()->GetCSelectRecInfo().SetType(_buffer->nObjType);
	_parent->GetPaintInfo()->GetCSelectRecInfo().SetIsShow(TRUE);
	_parent->GetPaintInfo()->GetCSelectRecInfo().SetPosStart(_buffer->ptBeg);
	_parent->GetPaintInfo()->GetCSelectRecInfo().SetPosEnd(_buffer->ptEnd);

	_parent->GetPaintInfo()->SetHitID( gMyList.GetCurrentNo() );

	_parent->GetPaintInfo()->GetScrollBarHandle()->TuneWindowView();
	
	_buffer = NULL;

}

