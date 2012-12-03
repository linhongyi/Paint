#include "StdAfx.h"
#include "CMouseControl.h"
#include "CFileControl.h"
#include "PaintInfo.h"
#include "ToolKits.h"
#include "MyListController.h"
#include "AddRedoUndoPaintInfo.h"
#include "MyListNode.h"
#include "DebugMsgOutputHandle.h"

extern MyListController gMyList;
extern AddRedoUndoPaintInfo gAddRedoUndoPaintInfo;
CMouseControl::CMouseControl(void)
{
	_posBeg.x = 0;
	_posBeg.y = 0;
	_posEnd.x = 0;
	_posEnd.y = 0;
}


CMouseControl::~CMouseControl(void)
{
}

void CMouseControl::SetParent(PaintInfo *ptr)
{
	_parent = ptr;
}

/**滑鼠左鍵點擊處理*/
void CMouseControl::WmLbuttonDownEvent(HWND hWnd,int mouseX,int mouseY)
{	
	_mouseIsMove = FALSE;
	//記錄滑鼠點下去的座標
	_iWindowMouseDownPosX = mouseX;
	_iWindowMouseDownPosY = mouseY;

	//紀錄目前x、y方向的座標位置
	int x =_parent->GetScrollBarHandle()->GetHorzPos();
	int y =_parent->GetScrollBarHandle()->GetVertPos(); 
	_parent->GetScrollBarHandle()->SetBeforeHorzPos (x);
	_parent->GetScrollBarHandle()->SetBeforeVertPos (y);

	//反白取消
	_parent->GetCInverseWordInfo().ResetInverseWordScope();

	//紀錄此次滑鼠游標形狀，判斷滑鼠點擊意義
	int directionMouse = _parent->GetDirectionMouse();
	// 一般畫圖
	if( directionMouse == CursorDirection_ARROW )
	{
		if( _parent->GetSelection() == IDM_WORD )
		{
			_parent->GetCTextOut()->CaretHide();

			//若此次為一段文字的起頭，則必需檢查上個節點是否為文字的起頭 (TCHAR) -1 ，cha ==65535
			if ( gMyList.CheckIsRemoveNullWordNode(this->_parent->GetHitID()) == TRUE )
			{
				this->_parent->GetCSelectRecInfo().SetIsShow(false);
				this->_parent->SetHitID(-1);
			}
		}
		else
		{
			//把實體框用不見 = 在貼一次畫布DC
			_parent->GetCSelectRecInfo().SetIsShow(FALSE);
			_parent->SetHitID( -1 );
			_parent->GetDrawTool()->MapDCToMonitor();

			_parent->GetCTextOut()->CaretHide();
		}
	}
	//修改文字
	else if( directionMouse == CursorDirection_IBEAM )
	{
	
		int ID =  gMyList.GetCurrentNo();
		this->_parent->SetHitID( ID );


		//設定目前滑鼠鼠標位置
		this->_parent->GetCTextOut()->CaretHide();
		this->_parent->GetCTextOut()->SetNearCaretPosInClickingMouse( mouseX, mouseY );

		POINT pt; 
		pt.x = mouseX;
		pt.y = mouseY;

		this->_parent->GetCTextOut()->CaretSetPos(pt);
		this->_parent->GetCTextOut()->CaretShow();

		//設定反白文字起始文字範圍
		this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos(pt.x,pt.y);
		this->_parent->GetCInverseWordInfo().SetCaretPt1(pt);
		this->_parent->GetCInverseWordInfo().SetCaretPt2(pt);

		//設定選取框大小範圍
		POINT ptStart, ptEnd;

		ptStart = gMyList.GetCurrentPtrPosBeg();
		ptEnd   = gMyList.GetCurrentPtrPosEnd();
		_parent->GetCSelectRecInfo().SetType(IDM_WORD_REC);
		_parent->GetCSelectRecInfo().SetIsShow(TRUE);
		_parent->GetCSelectRecInfo().SetPosStart(ptStart);
		_parent->GetCSelectRecInfo().SetPosEnd(ptEnd);
	}
	//非一般畫圖
	else
	{
		this->_parent->GetCTextOut()->CaretHide();
		
		int nType = gMyList.GetCurrentPtrType();

		//若此次點擊為文字框，且文字框為空文字框
		if( nType = IDM_WORD_REC )
		{
			//如果是空文字框則移除
			if ( gMyList.CheckIsRemoveNullWordNode(this->_parent->GetHitID()) == TRUE )
			{
				this->_parent->GetCTextOut()->CaretHide();
				this->_parent->GetCSelectRecInfo().SetIsShow(false);
				this->_parent->SetHitID(-1);
				_parent->SetDirectionMouse(CursorDirection_ARROW);
				_parent->GetDrawTool()->MapDCToMonitor();
				return;
			}
		}
		
		/**********************
		**設定確定選取框資訊
		**********************/
		POINT ptStart, ptEnd;

		ptStart = gMyList.GetCurrentPtrPosBeg();
		ptEnd   = gMyList.GetCurrentPtrPosEnd();


		_parent->GetCSelectRecInfo().SetType(nType);
		_parent->GetCSelectRecInfo().SetIsShow(TRUE);
		_parent->GetCSelectRecInfo().SetPosStart(ptStart);
		_parent->GetCSelectRecInfo().SetPosEnd(ptEnd);

		this->_parent->SetHitID( gMyList.GetCurrentNo() );
		//設定鼠標情形
		this->_parent->GetDrawTool()->SetMouseCurrsorInMouseEvent(directionMouse);
		//做修改圖案起始點修正，方便滑鼠移動時，拖移正確
		this->InitialPosFixForDragOrScale(mouseX,mouseY);
	}
}
/** 滑鼠移動繪圖選擇 */
void CMouseControl::MouseMoveEventForDrawSomething(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	_mouseIsMove = TRUE;
	this->_iWindowMouseMovePosX = LOWORD(lParam);
	this->_iWindowMouseMovePosY = HIWORD(lParam);

	//初始化DC
	HDC hdc = GetDC(hWnd);	
	RECT rectClient;
	::GetClientRect(hWnd,&rectClient);

	HDC hdcMem = CreateCompatibleDC(hdc); 
	HBITMAP bmpMem = CreateCompatibleBitmap(hdc,rectClient.right-rectClient.left , rectClient.bottom-rectClient.top);
	SelectObject(hdcMem, bmpMem); 

	if( !hdcMem )
		MessageBox(hWnd,_T("fatal error"),NULL,NULL);

	//紀錄此次滑鼠游標形狀，判斷滑鼠拖移的意義
	int directionMouse = _parent->GetDirectionMouse();


	/************************************
	* 進行滑鼠移動時，滑鼠座標邊界修正
	**************************************/

	// 正常畫圖情形
	if( directionMouse == CursorDirection_ARROW )
	{
		//座標移出視窗外修正。
		this->GraphBoundaryPosFix( hWnd, this->_iWindowMouseMovePosX, this->_iWindowMouseMovePosY,TRUE,TRUE,CommonVar::SHIFT_PIXEL);
		//重繪, 將畫布DC 貼至 視窗DC上
		this->_parent->GetDrawTool()->MapDcPasterToWindowDC(hdcMem);
	}
	else
	{
		//看目前修改圖的種類
		int type = gMyList.GetCurrentPtrType();

		// 若是平移的話，滑鼠座標可能不是在邊界上，所有要對滑鼠座標點作修正
		if ( this->IsGraphShiftForMouseMoving() == TRUE )
		{
			this->InputPosFixForShiftGraph();
		}
		// 某方向移動
		else if( this->IsGraphShiftSingleDirectionForMouseMoving() == TRUE ) 
		{
			// to do... 捲動軸不能在某些方向移動。 
			BOOL Is_ScrollX = FALSE;
			BOOL Is_ScrollY = FALSE;

			//上下移動，只能有Y能Scroll
			if( directionMouse == CursorDirection_N || directionMouse == CursorDirection_S )
				Is_ScrollY = TRUE;
			else
				Is_ScrollX = TRUE;

			//座標移出視窗外修正。
			this->GraphBoundaryPosFix( hWnd, this->_iWindowMouseMovePosX, this->_iWindowMouseMovePosY,Is_ScrollX,Is_ScrollY,CommonVar::SHIFT_PIXEL);
		}
		// 非平移的情形直接抓滑鼠座標，因為座標都在框上
		else
		{
			//座標移出視窗外修正。
			this->GraphBoundaryPosFix( hWnd, this->_iWindowMouseMovePosX, this->_iWindowMouseMovePosY,TRUE,TRUE,CommonVar::SHIFT_PIXEL);
		}
	}

	//滑鼠移動時，所畫圖的開始座標 = 滑鼠按下的座標 -  相鄰視窗捲動軸座標相減 （捲軸往右，圖案往左)　 
	_posBeg.x   = this->_iWindowMouseDownPosX - (_parent->GetScrollBarHandle()->GetHorzPos() -_parent->GetScrollBarHandle()->GetBeforeHorzPos() );
	_posBeg.y   = this->_iWindowMouseDownPosY - (_parent->GetScrollBarHandle()->GetVertPos() -_parent->GetScrollBarHandle()->GetBeforeVertPos() );

	//決定繪圖的種類
	if(  directionMouse != CursorDirection_ARROW )
	{
		_posEnd.x   = this->_iWindowMouseMovePosX;
		_posEnd.y   = this->_iWindowMouseMovePosY;

		//看目前修改圖的種類
		int type = gMyList.GetCurrentPtrType();


		// 若為平移某方向，圖形開始修正
		if( this->IsGraphShiftSingleDirectionForMouseMoving() == TRUE )
		{	
			this->TuneGraphForShiftSingleDirectionGraph( type );


			if( type == IDM_WORD_REC )
			{
				this->WordRecBoundaryPosFixForSingleDirectoin();
			}
		}
		//若為任意方向平移，滑鼠位置開始修正
		else if ( this->IsGraphShiftForMouseMoving() == TRUE )
		{
			this->TuneGraphForShiftiGraph();
		}
		// 點在斜對角，進行滑鼠顯示座標變換
		else
		{
			if( type == IDM_WORD_REC )
			{
				this->WordRecBoundaryPosFixForScaling();
			}
			else
				this->ChangeCursorShapeInScaleGraph();   
		}

		// 將視窗座標轉成畫布座標，才方便修改畫布
		this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos( _posBeg.x,_posBeg.y);
		this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos( _posEnd.x,_posEnd.y);
		
		
		//找出目前畫的圖形和存在佇列的圖形兩次圖形所成最小正方形，進行 ClipRgn重繪
		POINT recPt1, recPt2, recPt3, recPt4 ;
		recPt1 = _posBeg;
		recPt2 = _posEnd;
		recPt3 = gMyList.GetCurrentPtrPosBeg();
		recPt4 = gMyList.GetCurrentPtrPosEnd();

		ToolKits::FindMaxScopeWithFourPoint( recPt1, recPt2 , recPt3, recPt4);



		//修改佇列中的目標圖的資訊，方便之後重建
		gMyList.ModifyCurrNodeInfo(
			gMyList.GetCurrentPtrType()
			,_posBeg
			,_posEnd
			,gMyList.GetCurrentPenColor()
			,gMyList.GetCurrentFigBg()
			,gMyList.GetCurrentPenWidth());

	


		// 若此次修改為文字選擇框，則必須修改文字
		if( type == IDM_WORD_REC )
		{
			_parent->GetCTextOut()->CaretHide();
			_parent->GetCTextOut()->WordReputationForInsertingWord(_parent->GetHitID());
		}

	    // 畫布塗白
		this->_parent->GetDrawTool()->FillWhileRec( recPt1, recPt4);
		// 重建畫布DC
		this->_parent->GetDrawTool()->RePaintWithClipRgn(recPt1, recPt4);
		//重繪, 將畫布DC 貼至 視窗DC上
		this->_parent->GetDrawTool()->MapDcPasterToWindowDC(hdcMem);

		// 將畫布座標轉成視窗座標，才能在視窗DC上畫畫
		this->_parent->GetDrawTool()->ScreenToWindowCoordinate( _posBeg.x,_posBeg.y);
		this->_parent->GetDrawTool()->ScreenToWindowCoordinate( _posEnd.x,_posEnd.y);


		// 視窗DC上畫上八個點
		_parent->GetDrawTool()->FigSenSorDraw( 
			hdcMem
			,gMyList.GetCurrentPtrType()
			,_posBeg.x, _posBeg.y,_posEnd.x, _posEnd.y
			,CommonVar::COLOR_ORANGE);
	}
	else
	{
		_posEnd.x   = this->_iWindowMouseMovePosX;
		_posEnd.y   = this->_iWindowMouseMovePosY;


		_parent->GetDrawTool()->FigSenSorDraw(
			hdcMem
			,_parent->GetSelection()
			, _posBeg.x, _posBeg.y,_posEnd.x, _posEnd.y 
			,CommonVar::COLOR_ORANGE);

		//為畫新圖，所以看目前功能表上點選的種類
		_parent->GetDrawTool()->DrawFig( hdcMem,_parent->GetSelection(),  _posBeg.x, _posBeg.y,_posEnd.x, _posEnd.y,_parent->GetColorSelection(),_parent->GetFigBg(),_parent->GetPenWidth() );

		
	}

	BitBlt(hdc, 0, 0, rectClient.right-rectClient.left, rectClient.bottom-rectClient.top, hdcMem,0,0, SRCCOPY);

	DeleteObject(bmpMem);
	DeleteDC(hdcMem);
	ReleaseDC(hWnd,hdc);

}
/** 反白文字選取*/
void CMouseControl::MouseMoveEventForInverseWord(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	this->_parent->GetCTextOut()->CaretHide();

	this->_iWindowMouseMovePosX = LOWORD(lParam);
	this->_iWindowMouseMovePosY = HIWORD(lParam);
	

	//取文字框上下座標
	MyListNode *ptr = gMyList.visitID_Node( _parent->GetHitID() );
	POINT recPt1 = ptr->GetPtBeg();
	POINT recPt2 = ptr->GetPtEnd();
	ToolKits::Normalization(recPt1,recPt2);
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate(recPt1.x,recPt1.y);
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate(recPt2.x,recPt2.y);

	//視窗外座標修正
	this->GraphBoundaryPosFix(hWnd,this->_iWindowMouseMovePosX,this->_iWindowMouseMovePosY,TRUE,TRUE,CommonVar::SHIFT_PIXEL);

	POINT lastWordPt, iniWordPt;
	ptr->FindIniWordPosInWordRec( iniWordPt );
	ptr->FindLastWordPosInWordRec( lastWordPt );

	POINT pt; 

	//檢查鼠標是否在方框內

	//方框內
	if( this->_iWindowMouseMovePosX >= recPt1.x && this->_iWindowMouseMovePosX <= recPt2.x 
		&& this->_iWindowMouseMovePosY >= recPt1.y && this->_iWindowMouseMovePosY <= recPt2.y )
	{
		this->_parent->GetCTextOut()->SetNearCaretPosInClickingMouse(this->_iWindowMouseMovePosX,this->_iWindowMouseMovePosY);
		
		pt.x = this->_iWindowMouseMovePosX;
		pt.y = this->_iWindowMouseMovePosY;
		this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos( pt.x, pt.y );

		this->_parent->GetCInverseWordInfo().SetCaretPt2(pt);
	}
	//鼠標超過文字上界
	else if( this->_iWindowMouseMovePosY < iniWordPt.y )
	{
		pt.x = iniWordPt.x;
		pt.y = iniWordPt.y;
	
		this->_parent->GetCInverseWordInfo().SetCaretPt2(pt);
	}
	//鼠標超過文字下界
	else if( this->_iWindowMouseMovePosY > lastWordPt.y )
	{
		pt.x = lastWordPt.x;
		pt.y = lastWordPt.y;
	
		this->_parent->GetCInverseWordInfo().SetCaretPt2(pt);
	}

	//初始化DC
	HDC hdc = GetDC(hWnd);	
	RECT rectClient;
	::GetClientRect(hWnd,&rectClient);

	HDC hdcMem = CreateCompatibleDC(hdc); 
	HBITMAP bmpMem = CreateCompatibleBitmap(hdc,rectClient.right-rectClient.left , rectClient.bottom-rectClient.top);
	SelectObject(hdcMem, bmpMem); 

	//重繪, 將畫布DC 貼至 視窗DC上
	this->_parent->GetDrawTool()->MapDcPasterToWindowDC(hdcMem);

	this->_parent->GetCTextOut()->InverseTextWordToDC(hdcMem);

	// 視窗DC上畫上八個點
	_parent->GetDrawTool()->FigSenSorDraw( 
		hdcMem
		,IDM_WORD_REC
		,recPt1.x, recPt1.y,recPt2.x, recPt2.y
		,CommonVar::COLOR_ORANGE);

	BitBlt(hdc, 0, 0, rectClient.right-rectClient.left, rectClient.bottom-rectClient.top, hdcMem,0,0, SRCCOPY);

	DeleteObject(bmpMem);
	DeleteDC(hdcMem);
	ReleaseDC(hWnd,hdc);
}


/**滑鼠左鍵放開點擊處理*/
void CMouseControl::WmLbuttonUpEvent(HWND hWnd,int mouseX,int mouseY)
{	
	// 直線、方框、橢圓、文字
	int iType = _parent->GetSelection();
     //紀錄此次滑鼠游標形狀，判斷滑鼠點擊意義
	int directionMouse = _parent->GetDirectionMouse();
	
	// 新增圖形
    if( directionMouse == CursorDirection_ARROW )
    {
        //此次選擇非文字
	    if( iType != IDM_WORD)
	    {
	         //太小, 設成最小單位大小
	        if( abs( mouseX - this->_iWindowMouseDownPosX ) <=4 && abs( mouseY - this->_iWindowMouseDownPosY) <=4  )
	        {
	            _posBeg.x = this->_iWindowMouseDownPosX;
			    _posBeg.y = this->_iWindowMouseDownPosY;
			    _posEnd.x = _posBeg.x + 4;
			    _posEnd.y = _posBeg.y + 4;
    		
	        }
	    }
		//文字
	    else
	    {
			//檢查文字框是否不符合最小文字框大小( 5個文字 ，左右留白， 字與字的間格)
	        int minWidth  =  5*_parent->GetCTextOut()->GetCharX() + 2* CommonVar::WORD_REC_MARGIN + 4* CommonVar::WORD_GAP;
	        int minHeight =  _parent->GetCTextOut()->GetCharY()   + 2* CommonVar::WORD_REC_MARGIN;
    	    
	        if( abs(mouseX - this->_iWindowMouseDownPosX) < minWidth || abs(mouseY - this->_iWindowMouseDownPosY) < minHeight)
	        {
	              _posBeg.x = this->_iWindowMouseDownPosX;
                  _posBeg.y = this->_iWindowMouseDownPosY;
                  _posEnd.x =  _posBeg.x + minWidth;
                  _posEnd.y =  _posBeg.y + minHeight;
            		
             
	        }
	    }
    }
    else
    {
          //點下又放開 
          //if( abs( mouseX - this->_iWindowMouseDownPosX ) <=0 && abs( mouseY - this->_iWindowMouseDownPosY) <=0  )
		  if( this->_mouseIsMove == FALSE )
		  {
			  _parent->GetCSelectRecInfo().SetIsShow( true );
			  _parent->GetCSelectRecInfo().SetType( gMyList.GetCurrentPtrType() );
			  return;
	      }
    }
	        

	// 判定畫面被修改過
	_parent->GetCFileControl()->newChangeWithFile( TRUE );

	// 平移情形
	if ( this->IsGraphShiftForMouseMoving() == TRUE )
	{	
		//將視窗座標，轉成畫布座標
		_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos(_posBeg.x,_posBeg.y);
		//將視窗座標，轉成畫布座標
		_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos(_posEnd.x,_posEnd.y);
	}
	else
	{
		_posBeg.x = this->_iWindowMouseDownPosX;
		_posBeg.y = this->_iWindowMouseDownPosY;


		//將視窗座標，轉成畫布座標
		_parent->GetDrawTool()->WindowToScreenCoordinateForStartPos(_posBeg.x,_posBeg.y);
		//將視窗座標，轉成畫布座標
		_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos(_posEnd.x,_posEnd.y);
	}

	// 一般畫圖
	if( directionMouse == CursorDirection_ARROW )
	{
		if( iType == IDM_WORD  )
		{
		     ////若此次為一段文字的起頭，則必需檢查上個節點是否為文字的起頭 (TCHAR) -1 ，cha ==65535
	        if ( gMyList.CheckIsRemoveNullWordNode(this->_parent->GetHitID()) == TRUE )
	        {
		         this->_parent->SetHitID(-1);
	        }
	        
			_parent->GetCTextOut()->AddWordRecIntoQueue(_posBeg,_posEnd);
			_parent->GetDrawTool()->ShowTarGetRecForTypingWord();

		}
		else if( iType != IDM_SELECT)
		{
			//// 儲存畫布資訊
			gMyList.AddGraphNode( iType
				,_posBeg
				,_posEnd
				,_parent->GetColorSelection()
				,_parent->GetFigBg()
				,_parent->GetPenWidth());


			gAddRedoUndoPaintInfo.AddNewNodeInfo(  gMyList.GetCurrentNo() );
			_parent->SetHitID( gMyList.GetCurrentNo() );

			// 設定此次選取框位置
			_parent->GetCSelectRecInfo().SetIsShow(TRUE);
			_parent->GetCSelectRecInfo().SetPosStart(_posBeg);
			_parent->GetCSelectRecInfo().SetPosEnd(_posEnd);
			_parent->GetCSelectRecInfo().SetType(iType);
			_parent->GetDrawTool()->DrawFig( _parent->GetMemDC(),iType,_posBeg.x, _posBeg.y, _posEnd.x , _posEnd.y, _parent->GetColorSelection(),_parent->GetFigBg(),_parent->GetPenWidth() );


		}

	}
	//非一般畫圖
	else
	{
		MyListNode *ptr = gMyList.visitID_Node( _parent->GetHitID() );
		
		iType = gMyList.GetCurrentPtrType();
		_parent->GetCSelectRecInfo().SetType(iType);

        // 加入改變圖像範圍的LOG
		POINT oldGraphSrcPt1, oldGraphSrcPt2;
		oldGraphSrcPt1 = _parent->GetCSelectRecInfo().GetPosStart();
		oldGraphSrcPt2 = _parent->GetCSelectRecInfo().GetPosEnd();
	
		if( iType == IDM_WORD_REC )
		{
			ToolKits::Normalization( _posBeg, _posEnd );
			//修改佇列中的目標圖的資訊，方便之後重建
			gMyList.ModifyCurrNodeInfo(
				gMyList.GetCurrentPtrType()
				,_posBeg
				,_posEnd
				,gMyList.GetCurrentPenColor()
				,gMyList.GetCurrentFigBg()
				,gMyList.GetCurrentPenWidth());


			// set pos
			POINT caretPt = ptr->FindCurrWordPosInWordRec(caretPt);
			this->_parent->GetDrawTool()->ScreenToWindowCoordinate( caretPt.x, caretPt.y);

			_parent->GetCTextOut()->CaretSetPos(caretPt);
			_parent->GetCTextOut()->CaretShow();

			
			//若有文字才紀錄變動文字方框變動的資訊
			if( ptr->GetWordNum() > 0 )
			   gAddRedoUndoPaintInfo.AddModifySizeInfo( _parent->GetHitID(), oldGraphSrcPt1 , oldGraphSrcPt2);
		}
        else
        {
            gAddRedoUndoPaintInfo.AddModifySizeInfo( _parent->GetHitID(), oldGraphSrcPt1 , oldGraphSrcPt2);
        }
		

		// 設定此次選取框位置
		_parent->GetCSelectRecInfo().SetIsShow(TRUE);
		_parent->GetCSelectRecInfo().SetPosStart(_posBeg);
		_parent->GetCSelectRecInfo().SetPosEnd(_posEnd);
		_parent->GetCSelectRecInfo().SetType(iType);
		
		ptr = NULL;
	}
}

/**邊界座標修正*/
void CMouseControl::GraphBoundaryPosFix(HWND hWnd, int &iEndPosX, int &iEndPosY,BOOL IS_SCROOLX, BOOL IS_SCROOLY, int offset)
{
	direction x =_parent->GetScrollBarHandle()->IsXScroll(iEndPosX);
	direction y =_parent->GetScrollBarHandle()->IsYScroll(iEndPosY);

	//x 邊界判定修正
	if( x == Left )
	{
		iEndPosX = 0;

		if( IS_SCROOLX )
			_parent->GetScrollBarHandle()->HorizontalScrollBarAutoEvent(hWnd, -1* offset);
	}
	else if( x == LeftBoundary )
	{
		iEndPosX = 0;
	}
	else if( x == Right )
	{
		if( IS_SCROOLX )
			_parent->GetScrollBarHandle()->HorizontalScrollBarAutoEvent(hWnd, offset );

		if( iEndPosX >  _parent->GetClientWidth() )
			iEndPosX =  _parent->GetClientWidth();
	}
	else if( x == RightBoundary )
	{

		iEndPosX = _parent->GetClientWidth();
	}

	// y邊界判定修正
	if( y == Up )
	{
		iEndPosY = 0;

		if( IS_SCROOLY )
			_parent->GetScrollBarHandle()->VerticalScrollBarAutoEvent(hWnd, -1* offset  );
	}
	else if( y == UpBoundary )
	{
		iEndPosY = 0;
	}
	else if ( y == Down )
	{
		if( IS_SCROOLY )
			_parent->GetScrollBarHandle()->VerticalScrollBarAutoEvent(hWnd, offset );

		if( iEndPosY >  _parent->GetClientHeight() )
			iEndPosY =  _parent->GetClientHeight();

	}
	else if( y == DownBoundary )
	{
		iEndPosY =  _parent->GetClientHeight();
	}
}

void CMouseControl::InverseWordBoundaryFix(HWND hWnd, POINT ptStart, POINT ptEnd, int &iEndPosX, int &iEndPosY, int offset)
{
	direction x =_parent->GetScrollBarHandle()->IsXScroll(iEndPosX);
	direction y =_parent->GetScrollBarHandle()->IsYScroll(iEndPosY);


	//x 邊界判定修正
	if( x == Left )
	{
		iEndPosX = 0;

		if( _parent->GetScrollBarHandle()->GetNowScrollBarPos().x - offset >= ptStart.x )
			_parent->GetScrollBarHandle()->HorizontalScrollBarAutoEvent(hWnd, -1* offset);
	}
	else if( x == LeftBoundary )
	{
		iEndPosX = 0;
	}
	else if( x == Right )
	{
		if( _parent->GetScrollBarHandle()->GetNowScrollBarPos().x + offset >= ptEnd.x  )
			_parent->GetScrollBarHandle()->HorizontalScrollBarAutoEvent(hWnd, offset );

		if( iEndPosX >  _parent->GetClientWidth() )
			iEndPosX =  _parent->GetClientWidth();
	}
	else if( x == RightBoundary )
	{

		iEndPosX = _parent->GetClientWidth();
	}

	// y邊界判定修正
	if( y == Up )
	{
		iEndPosY = 0;

		if( _parent->GetScrollBarHandle()->GetNowScrollBarPos().y - offset >= ptStart.y )
			_parent->GetScrollBarHandle()->VerticalScrollBarAutoEvent(hWnd, -1* offset  );
	}
	else if( y == UpBoundary )
	{
		iEndPosY = 0;
	}
	else if ( y == Down )
	{
		if( _parent->GetScrollBarHandle()->GetNowScrollBarPos().y + offset <= ptEnd.y  )
			_parent->GetScrollBarHandle()->VerticalScrollBarAutoEvent(hWnd, offset );

		if( iEndPosY >  _parent->GetClientHeight() )
			iEndPosY =  _parent->GetClientHeight();

	}
	else if( y == DownBoundary )
	{
		iEndPosY =  _parent->GetClientHeight();
	}
}

//判定此次滑鼠移動事件是否為圖形平移
BOOL CMouseControl::IsGraphShiftForMouseMoving()
{
	int direction = _parent->GetDirectionMouse();

	if( direction == CursorDirection_SIZEALL  )
		return TRUE;
	else
	{
		return FALSE;
	}
}

//判定此次滑鼠移動事件是否為圖形單方向平移
BOOL CMouseControl::IsGraphShiftSingleDirectionForMouseMoving()
{
	int direction = _parent->GetDirectionMouse();

	if( direction == CursorDirection_N 
		|| direction == CursorDirection_W 
		|| direction == CursorDirection_E 
		|| direction == CursorDirection_S )
		return TRUE;
	else
	{
		return FALSE;
	}
}

/**修正圖案時，起始點座標修正*/
void CMouseControl::InitialPosFixForDragOrScale(int mouseX,int mouseY)
{
	gMyList.visitID_Node( _parent->GetHitID());
	//取得目前所找到要修改的圖案，在佇列中位置的型態(直線、橢圓、方框)
	int type = gMyList.GetCurrentPtrType();
	//此次游標位於哪個點上
	int directionMouse = _parent->GetDirectionMouse();

	POINT currNodeStartPt, currNodeEndPt;

	currNodeStartPt.x = gMyList.GetCurrentPtrPosBeg().x;
	currNodeStartPt.y = gMyList.GetCurrentPtrPosBeg().y;
	currNodeEndPt.x   = gMyList.GetCurrentPtrPosEnd().x;
	currNodeEndPt.y   = gMyList.GetCurrentPtrPosEnd().y;

	//畫布座標轉視窗座標
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate(currNodeStartPt.x,currNodeStartPt.y);
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate(currNodeEndPt.x,currNodeEndPt.y);

	POINT tarGetPt;  tarGetPt.x = mouseX; tarGetPt.y = mouseY;

	POINT arrTarGetPt[4]; RECT tarGetRect;


	if( directionMouse == CursorDirection_NW ||  directionMouse == CursorDirection_SE || directionMouse == CursorDirection_NE ||  directionMouse == CursorDirection_SW)
	{
		//把目標四個點，算出來
		arrTarGetPt[0].x = currNodeStartPt.x;   
		arrTarGetPt[0].y = currNodeStartPt.y;

		arrTarGetPt[1].x = currNodeEndPt.x;
		arrTarGetPt[1].y = currNodeEndPt.y;

		arrTarGetPt[2].x = currNodeEndPt.x;
		arrTarGetPt[2].y = currNodeStartPt.y;

		arrTarGetPt[3].x = currNodeStartPt.x;
		arrTarGetPt[3].y = currNodeEndPt.y;


		//所點滑鼠是否命中端點
		for( int iNumPt = 0 ; iNumPt < 4 ; iNumPt++ )
		{
			tarGetRect.left   = arrTarGetPt[iNumPt].x - CommonVar::SMALL_RECT_BUFFER;
			tarGetRect.right  = arrTarGetPt[iNumPt].x + CommonVar::SMALL_RECT_BUFFER;
			tarGetRect.top    = arrTarGetPt[iNumPt].y - CommonVar::SMALL_RECT_BUFFER;
			tarGetRect.bottom = arrTarGetPt[iNumPt].y + CommonVar::SMALL_RECT_BUFFER;

			//滑鼠游標命中點擊佇列中此圖案的起點，做對角線對調( 起始點 對調 )
			if( ::PtInRect(&tarGetRect, tarGetPt ) == TRUE )
			{
				switch( iNumPt )
				{
				case 0:
					this->_iWindowMouseDownPosX = currNodeEndPt.x;
					this->_iWindowMouseDownPosY = currNodeEndPt.y;
					break;
				case 1:
					this->_iWindowMouseDownPosX = currNodeStartPt.x;
					this->_iWindowMouseDownPosY = currNodeStartPt.y;
					break;
				case 2:
					this->_iWindowMouseDownPosX = currNodeStartPt.x;
					this->_iWindowMouseDownPosY = currNodeEndPt.y;
					break;
				case 3:
					this->_iWindowMouseDownPosX = currNodeEndPt.x;
					this->_iWindowMouseDownPosY = currNodeStartPt.y;
					break;
				}
				break;
			}
		}
	}
	//針對滑鼠遊標為上下
	else if ( directionMouse == CursorDirection_N ||  directionMouse == CursorDirection_S)
	{
		//滑鼠座標比較靠目標圖案的起點，上下起始點對換
		if( abs(tarGetPt.y - currNodeEndPt.y) >  abs(tarGetPt.y - currNodeStartPt.y) )
		{
			this->_iWindowMouseDownPosX = currNodeEndPt.x;
			this->_iWindowMouseDownPosY = currNodeEndPt.y;

			if( type == IDM_CIR )
				this->_parent->GetDrawTool()->GetDrawCircular()->SetEndPosForFixingPoint(currNodeStartPt);
			else if ( type == IDM_LINE )
				this->_parent->GetDrawTool()->GetDrawLine()->SetEndPosForFixingPoint(currNodeStartPt);
			else if ( type == IDM_REC)
				this->_parent->GetDrawTool()->GetDrawRec()->SetEndPosForFixingPoint(currNodeStartPt);
			else if( type == IDM_WORD_REC )
				this->_parent->GetDrawTool()->GetDrawRec()->SetEndPosForFixingPoint(currNodeStartPt);
		}
		else
		{
			this->_iWindowMouseDownPosX = currNodeStartPt.x;
			this->_iWindowMouseDownPosY = currNodeStartPt.y;

			if( type == IDM_CIR )
				this->_parent->GetDrawTool()->GetDrawCircular()->SetEndPosForFixingPoint(currNodeEndPt);
			else if ( type == IDM_LINE )
				this->_parent->GetDrawTool()->GetDrawLine()->SetEndPosForFixingPoint(currNodeEndPt);
			else if ( type == IDM_REC)
				this->_parent->GetDrawTool()->GetDrawRec()->SetEndPosForFixingPoint(currNodeEndPt);
			else if( type == IDM_WORD_REC )
				this->_parent->GetDrawTool()->GetDrawRec()->SetEndPosForFixingPoint(currNodeEndPt);

		}
	}
	//針對滑鼠游標左右
	else if ( directionMouse == CursorDirection_W || directionMouse == CursorDirection_E)
	{
	
		//滑鼠座標比較靠目標圖案的起點，左右起始點對換
		if( abs(tarGetPt.x - currNodeEndPt.x) >  abs(tarGetPt.x - currNodeStartPt.x) )
		{
			this->_iWindowMouseDownPosX =  currNodeEndPt.x ;
			this->_iWindowMouseDownPosY =  currNodeEndPt.y ;

			if( type == IDM_CIR )
				this->_parent->GetDrawTool()->GetDrawCircular()->SetEndPosForFixingPoint(currNodeStartPt);
			else if ( type == IDM_LINE )
				this->_parent->GetDrawTool()->GetDrawLine()->SetEndPosForFixingPoint(currNodeStartPt);
			else if ( type == IDM_REC)
				this->_parent->GetDrawTool()->GetDrawRec()->SetEndPosForFixingPoint(currNodeStartPt);
			else if( type == IDM_WORD_REC )
				this->_parent->GetDrawTool()->GetDrawRec()->SetEndPosForFixingPoint(currNodeStartPt);
		}
		else
		{
			this->_iWindowMouseDownPosX = currNodeStartPt.x;
			this->_iWindowMouseDownPosY = currNodeStartPt.y;


			if( type == IDM_CIR )
				this->_parent->GetDrawTool()->GetDrawCircular()->SetEndPosForFixingPoint(currNodeEndPt);
			else if ( type == IDM_LINE )
				this->_parent->GetDrawTool()->GetDrawLine()->SetEndPosForFixingPoint(currNodeEndPt);
			else if ( type == IDM_REC)
				this->_parent->GetDrawTool()->GetDrawRec()->SetEndPosForFixingPoint(currNodeEndPt);
			else if( type == IDM_WORD_REC )
				this->_parent->GetDrawTool()->GetDrawRec()->SetEndPosForFixingPoint(currNodeEndPt);
		}


	}

	//平移，算差量，滑鼠座標點和終點的距離差
	else
	{  
	 

		_iDisX_Cursor_PosStart = abs(tarGetPt.x - currNodeStartPt.x);
		_iDisY_Cursor_PosStart = abs(tarGetPt.y - currNodeStartPt.y);
		_iDisX_Cursor_PosEnd = abs(currNodeEndPt.x - tarGetPt.x);
		_iDisY_Cursor_PosEnd = abs(currNodeEndPt.y - tarGetPt.y);
	}
}

/** 碰撞偵測*/
void CMouseControl::IsHitGraphAndChangeCursorShape(int mouseX, int mouseY)
{
	POINT tarGetPt; 
	tarGetPt.x = mouseX;
	tarGetPt.y = mouseY;

	//佇列為空
	if( gMyList.GetTotalNum() < 0 )
		return;

	//按選擇選單
	if( _parent->GetSelection() == IDM_SELECT )
	{
		//存在目標圖形的話
		if( _parent->GetHitID() >= 0 )
		{
			int ID = _parent->GetHitID();

			MyListNode* ptr = gMyList.visitID_Node( _parent->GetHitID() );

			if( !ptr )
			{
				::OutputDebugString(_T("  畫布中不存在此對象 \n"));
			}
			else if( _parent->GetCHitTest()->PtHitTestRegion(ID, tarGetPt, ptr->GetPtBeg() , ptr->GetPtEnd()  ) == TRUE )
			{
				this->_parent->GetDrawTool()->CurrentSelectRecInfoShow(_parent->GetHitID());
				return ;
			}

		}
	
		int ID;
		MyListNode* last = gMyList.GetLast();

		//從佇列後面開始找
		for( ; last != NULL ; last = last->_llink )
		{
			ID = last->GetNo();

			if( _parent->GetCHitTest()->PtHitTestRegion(ID,tarGetPt, last->GetPtBeg(), last->GetPtEnd() ) == TRUE )
			{
				this->_parent->GetDrawTool()->CurrentSelectRecInfoShow(ID);
				break;
			}
			//最後仍然沒找到
			else if( last->_llink == NULL )
			{
				this->_parent->GetDrawTool()->MapDCToMonitor();
			}
		}
	}
	// 一般情況只檢查佇列最後一個( 畫完新原件後，移動滑鼠)
	else if( _parent->GetHitID() >= 0 )
	{
		int ID = _parent->GetHitID();

		MyListNode* ptr = gMyList.visitID_Node( _parent->GetHitID() );

        if( !ptr )
        {
             ::OutputDebugString(_T("  畫布中不存在此對象 \n"));
        }
		else if( _parent->GetCHitTest()->PtHitTestRegion(ID, tarGetPt, ptr->GetPtBeg() , ptr->GetPtEnd()  ) == TRUE )
		{
			this->_parent->GetDrawTool()->CurrentSelectRecInfoShow(_parent->GetHitID());
		}

	}
}

/**圖像平移時，滑鼠座標的調整*/
void CMouseControl::InputPosFixForShiftGraph()
{

	POINT currNodeStartPt, currNodeEndPt;
	gMyList.visitID_Node( _parent->GetHitID());
	currNodeStartPt.x = gMyList.GetCurrentPtrPosBeg().x;
	currNodeStartPt.y = gMyList.GetCurrentPtrPosBeg().y;
	currNodeEndPt.x   = gMyList.GetCurrentPtrPosEnd().x;
	currNodeEndPt.y   = gMyList.GetCurrentPtrPosEnd().y;



	//畫布座標轉視窗座標
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate(currNodeStartPt.x,currNodeStartPt.y);
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate(currNodeEndPt.x,currNodeEndPt.y);

	//y方向往下
	if( this->_iWindowMouseMovePosY >=  this->_iWindowMouseDownPosY )
	{   
		/*OutputDebugString(_T("---detecion y方向往下\n"));*/
		if( currNodeEndPt.y >= currNodeStartPt.y )
			this->_iWindowMouseMovePosY += this->_iDisY_Cursor_PosEnd;
		else
			this->_iWindowMouseMovePosY += this->_iDisY_Cursor_PosStart;
	}
	//y方向往上
	else
	{   
		//OutputDebugString(_T("---detecion y方向往上\n"));
		if( currNodeEndPt.y >= currNodeStartPt.y )
			this->_iWindowMouseMovePosY -= this->_iDisY_Cursor_PosStart;
		else
			this->_iWindowMouseMovePosY -= this->_iDisY_Cursor_PosEnd;
	}

	//x方向往右
	if( this->_iWindowMouseMovePosX >=  this->_iWindowMouseDownPosX )
	{
		//OutputDebugString(_T("---detecion x方向往右\n"));
		if( currNodeEndPt.x >= currNodeStartPt.x )
			this->_iWindowMouseMovePosX += this->_iDisX_Cursor_PosEnd;
		else
			this->_iWindowMouseMovePosX += this->_iDisX_Cursor_PosStart;
	}
	else
	{
		//OutputDebugString(_T("---detecion x方向往左\n"));
		if( currNodeEndPt.x >= currNodeStartPt.x )
			this->_iWindowMouseMovePosX -= this->_iDisX_Cursor_PosStart;
		else
			this->_iWindowMouseMovePosX -= this->_iDisX_Cursor_PosEnd;
	}

	if(   this->_iWindowMouseMovePosX < 0 )
		this->_iWindowMouseMovePosX =  CommonVar::OUT_WINDOW_CONSTANT+1;
	if (  this->_iWindowMouseMovePosY < 0 )
		this->_iWindowMouseMovePosY =  CommonVar::OUT_WINDOW_CONSTANT+1 ;
	//座標移出視窗外修正。
	this->GraphBoundaryPosFix( _parent->GetMainHwnd(), this->_iWindowMouseMovePosX, this->_iWindowMouseMovePosY,TRUE,TRUE,CommonVar::SHIFT_PIXEL);
}

/**圖像平移時，圖像的調整*/
void CMouseControl::TuneGraphForShiftiGraph()
{
	// OutputDebugString(_T("/若為任意方向平移，滑鼠位置開始修正\n"));

	gMyList.visitID_Node( _parent->GetHitID());

	POINT posEndPt = gMyList.GetCurrentPtrPosEnd();
	_parent->GetDrawTool()->ScreenToWindowCoordinate(posEndPt.x,posEndPt.y);

	POINT posStartPt = gMyList.GetCurrentPtrPosBeg();
	_parent->GetDrawTool()->ScreenToWindowCoordinate(posStartPt.x,posStartPt.y);

	


	//算原始圖形起始點座標差
	int _disX = abs(posEndPt.x - posStartPt.x);
	int _disY = abs(posEndPt.y - posStartPt.y);


	//y方向往下
	if( this->_iWindowMouseMovePosY >=  this->_iWindowMouseDownPosY )
	{   

		if( posEndPt.y >= posStartPt.y )
		{
			//   OutputDebugString(_T("y方向往下，起點在終點上方\n"));
			_posEnd.y = this->_iWindowMouseMovePosY;
			_posBeg.y = _posEnd.y - _disY;
		}
		else
		{
			//  OutputDebugString(_T("y方向往下，起點在終點下方\n"));
			_posBeg.y = this->_iWindowMouseMovePosY;
			_posEnd.y = _posBeg.y - _disY;
		}
	}
	//y方向往上
	else
	{   
		if( posEndPt.y >= posStartPt.y )
		{
			//  OutputDebugString(_T("y方向往上，起點在終點上方\n"));
			_posBeg.y = this->_iWindowMouseMovePosY;
			_posEnd.y = _posBeg.y + _disY;
		}
		else
		{
			//  OutputDebugString(_T("y方向往上，起點在終點下方\n"));
			_posEnd.y = this->_iWindowMouseMovePosY;
			_posBeg.y = _posEnd.y + _disY;
		}
	}

	//x方向往右
	if( this->_iWindowMouseMovePosX >=  this->_iWindowMouseDownPosX )
	{

		if( posEndPt.x >= posStartPt.x )
		{

			_posEnd.x = this->_iWindowMouseMovePosX;
			_posBeg.x = _posEnd.x - _disX;
		}
		else
		{

			_posBeg.x = this->_iWindowMouseMovePosX;
			_posEnd.x = _posBeg.x - _disX;
		}
	}
	else
	{
		if( posEndPt.x >= posStartPt.x )
		{
			//   OutputDebugString(_T("x方向往左，起點在終點右方\n"));
			_posBeg.x = this->_iWindowMouseMovePosX;
			_posEnd.x = _posBeg.x + _disX;
		}
		else
		{
			// OutputDebugString(_T("x方向往左，起點在終點左方\n"));
			_posEnd.x = this->_iWindowMouseMovePosX;
			_posBeg.x = _posEnd.x + _disX;
		}
	}
	
}

/** 圖像放大縮小時，鼠標形狀變換*/
void CMouseControl::ChangeCursorShapeInScaleGraph()
{
	//目前滑鼠座標在起點座標的右邊
	if( this->_posEnd.x > _posBeg.x )
	{
		//第4象限
		if( this->_posEnd.y < _posBeg.y )
		{
			_parent->GetDrawTool()->SetMouseCurrsorInMouseEvent(CursorDirection_NE);
		}
		//第1象限
		else
		{

			_parent->GetDrawTool()->SetMouseCurrsorInMouseEvent(CursorDirection_SE);
		}
	}
	//.. 左邊
	else
	{
		//第3象限
		if( this->_posEnd.y < _posBeg.y )
		{
			_parent->GetDrawTool()->SetMouseCurrsorInMouseEvent(CursorDirection_NW);
		}
		//第2象限
		else
		{
			_parent->GetDrawTool()->SetMouseCurrsorInMouseEvent(CursorDirection_SW);
		}
	}
}

/**圖像單方向移動時，圖像的調整*/
void CMouseControl::TuneGraphForShiftSingleDirectionGraph(int type)
{
	//OutputDebugString(_T("/若為平移某方向，滑鼠位置開始修正\n"));

	switch( type )
	{
	case IDM_LINE:
		_parent->GetDrawTool()->GetDrawLine()->FixPosForSingleMovingGraph( _parent->GetDirectionMouse());
	
		break;
	case IDM_REC:
		_parent->GetDrawTool()->GetDrawRec()->FixPosForSingleMovingGraph( _parent->GetDirectionMouse());
	
		break;
	case IDM_CIR:
		_parent->GetDrawTool()->GetDrawCircular()->FixPosForSingleMovingGraph( _parent->GetDirectionMouse());

		break;
	case IDM_WORD_REC:
		_parent->GetDrawTool()->GetDrawRec()->FixPosForSingleMovingGraph( _parent->GetDirectionMouse());
		break;
	}
}

/**文字選取框單方向拖移時，滑鼠座標修正*/
void CMouseControl::WordRecBoundaryPosFixForSingleDirectoin()
{

	int xBuffer = 2*CommonVar::WORD_REC_MARGIN + _parent->GetCTextOut()->GetCharX() * 5;

	MyListNode* ptr = gMyList.visitID_Node(_parent->GetHitID());
	POINT queuePtBeg = ptr->GetPtBeg();
	POINT queuePtEnd = ptr->GetPtEnd();
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate( queuePtBeg.x, queuePtBeg.y );
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate( queuePtEnd.x, queuePtEnd.y );

	int direction = _parent->GetDirectionMouse();
	if( direction == CursorDirection_E  || direction == CursorDirection_W)
	{

		if( direction == CursorDirection_E )
		{
			if( _posEnd.x < _posBeg.x + xBuffer )
				_posEnd.x = _posBeg.x + xBuffer;
		}
		else if( direction == CursorDirection_W )
		{
			if( _posEnd.x > _posBeg.x - xBuffer )
				_posEnd.x = _posBeg.x - xBuffer;
		}
		
	}

	else if( direction == CursorDirection_N )
	{
		int yBuffer = _parent->GetCTextOut()->GetCharY();

		if( _posEnd.y > _posBeg.y - yBuffer )
			_posEnd.y = _posBeg.y - yBuffer;
	}
	else 
	{
		int yBuffer = _parent->GetCTextOut()->GetCharY();
		if( _posEnd.y < _posBeg.y + yBuffer )
			_posEnd.y = _posBeg.y + yBuffer;
	}

	BOOL answer = this->_parent->GetCTextOut()->IsChangeThisWordRecScope( _parent->GetHitID() , _posBeg,_posEnd);
	
	if( answer == FALSE )
	{
		if( direction == CursorDirection_N )
		{
			_posEnd.y = queuePtBeg.y;
		}
		else if( direction == CursorDirection_W )
		{
			_posEnd.x = queuePtBeg.x;
		
		}
		else if( direction == CursorDirection_S )
		{
			_posEnd.y = queuePtEnd.y;
		}
		else
		{
			_posEnd.x = queuePtEnd.x;
		}
	}
	ptr = NULL;
}


/** 文字框座標修正，在scale情況下*/
void CMouseControl::WordRecBoundaryPosFixForScaling()
{

	MyListNode* ptr =  gMyList.visitID_Node(_parent->GetHitID());
	POINT queuePtBeg = ptr->GetPtBeg();
	POINT queuePtEnd = ptr->GetPtEnd();
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate( queuePtBeg.x, queuePtBeg.y );
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate( queuePtEnd.x, queuePtEnd.y );

	int xBuffer = 2*CommonVar::WORD_REC_MARGIN + 8 * 5;
	int yBuffer = _parent->GetCTextOut()->GetCharY();

	
	int direction = _parent->GetDirectionMouse();

	// 四個點邊界判斷
	if( direction == CursorDirection_SE  )
	{
		if( _posEnd.x < _posBeg.x + xBuffer )
			_posEnd.x = _posBeg.x + xBuffer;

		if( _posEnd.y < _posBeg.y +yBuffer)
		{
			_posEnd.y = _posBeg.y +yBuffer;
		}

	}
	else if( direction == CursorDirection_SW )
	{
		if( _posEnd.x > _posBeg.x - xBuffer)
			_posEnd.x = _posBeg.x - xBuffer;

		if( _posEnd.y < _posBeg.y + yBuffer)
			_posEnd.y = _posBeg.y + yBuffer;
	}
	else if( direction == CursorDirection_NW )
	{
		if( _posEnd.x > _posBeg.x - xBuffer )
			_posEnd.x = _posBeg.x - xBuffer ;

		if( _posEnd.y > _posBeg.y -yBuffer )
			_posEnd.y = _posBeg.y -yBuffer ;
	}
	else
	{
		if( _posEnd.x < _posBeg.x + xBuffer )
			_posEnd.x = _posBeg.x + xBuffer ;

		if( _posEnd.y > _posBeg.y - yBuffer )
			_posEnd.y = _posBeg.y - yBuffer ;
	}

	int diff_x = abs( _posEnd.x - _posBeg.x );
	int diff_y = abs( _posEnd.y - _posBeg.y );

	BOOL answer = this->_parent->GetCTextOut()->IsChangeThisWordRecScope( _parent->GetHitID() , _posBeg,_posEnd);
	
	if( answer == FALSE )
	{
		if( direction == CursorDirection_NE )
		{
			_posBeg.x = queuePtBeg.x;
			_posBeg.y = queuePtEnd.y;

			_posEnd.x = queuePtEnd.x;
			_posEnd.y = queuePtBeg.y;
		}
		else if( direction == CursorDirection_NW )
		{
			_posBeg = queuePtEnd;
			_posEnd = queuePtBeg;
		}
		else if( direction == CursorDirection_SW )
		{
		    _posEnd.x = queuePtBeg.x;
			_posEnd.y = queuePtEnd.y;

			_posBeg.x = queuePtEnd.x;
			_posBeg.y = queuePtBeg.y;
		}
		else
		{
			_posBeg = queuePtBeg;
			_posEnd = queuePtEnd;
		}
	}
		
	ptr = NULL;
}
/************************
*  Getter // setter 
***************************/

void CMouseControl::SetThisTurnPosEndX(LONG x)
{
	_posEnd.x = x;
}

void CMouseControl::SetThisTurnPosEndY(LONG y)
{
	_posEnd.y = y;
}