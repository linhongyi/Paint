#include "StdAfx.h"
#include "PaintInfo.h"
#include "CommonVar.h"
#include "SgWordDataModelControler.h"
#include "MyListController.h"
extern MyListController   gMyList;
extern RedoUndoController *gRedoUndoController;

PaintInfo::PaintInfo()
{
	// initialize
	_iSelection      = -1;
	_iColorSelection = -1;
	_PenWidth        = 1;
	_FigBg           = IDM_BG_TRANSPARENT;
	_HitIndex        = -1;
	_directionMouse     = CursorDirection_ARROW;
	this->_clientHeight = 0;
	this->_clientWidth  = 0;


	_CFileControl    = NULL;
	_scrollBarHandle = NULL;
	_drawTool = NULL;
	_CTextOut = NULL;
	_cHitTest = NULL;

	_bAction = false;
}

PaintInfo::~PaintInfo(void)
{
}

void PaintInfo::ConstrunctionAllObjRelation()
{
	DrawTool         *pDrawTool = new DrawTool();
	ScrollBarHandle  *pScrollBarHandle = new ScrollBarHandle();
	CTextOut         *pCTextOut        = new CTextOut();
	CFileControl     *pCFileControl    = new CFileControl();
	DrawCircular     *pDrawCircular    = new DrawCircular();
	DrawRec          *pDrawRec         = new DrawRec();
	DrawLine         *pDrawLine        = new DrawLine();
	CHitTest         *pCHitTest        = new CHitTest();
	CMouseControl    *pCMouseControl   = new CMouseControl();
	CClipboardControl *pCClipboardControl = new CClipboardControl();
	CGraphClipboardControl *pCGraphClipboardControl = new CGraphClipboardControl();
	CWordClipboardControl  *pCWordClipboardControl = new CWordClipboardControl();

	//this->SetMyList(&gDoubleLinkList);
	this->SetDrawTool(pDrawTool);
	this->SetScrollBarHandle(pScrollBarHandle);
	this->SetCTextOut(pCTextOut);
	this->SetCFileControl(pCFileControl);
	this->SetCHitTest(pCHitTest);
	this->SetCMouseControl(pCMouseControl);
	this->SetClipboardControl(pCClipboardControl);

	//gDoubleLinkList.SetParent(&this);
	pDrawTool->SetParent(this);
	pDrawTool->SetDrawCircular(pDrawCircular);
	pDrawTool->SetDrawLine(pDrawLine);
	pDrawTool->SetDrawRec(pDrawRec);

	pDrawCircular->SetParent(pDrawTool);
	pDrawLine->SetParent(pDrawTool);
	pDrawRec->SetParent(pDrawTool);

	pCHitTest->SetParent(this);
	pScrollBarHandle->SetParent(this);
	pCTextOut->SetParent(this);
	pCFileControl->SetParent(this);
	pCMouseControl->SetParent(this);

	pCClipboardControl->SetParent(this);
	pCClipboardControl->SetCGraphClipboardControl(pCGraphClipboardControl);
	pCClipboardControl->SetCWordClipboardControl(pCWordClipboardControl);

	pCGraphClipboardControl->SetParent(pCClipboardControl);
	pCWordClipboardControl->SetParent(pCClipboardControl);

	pDrawTool = NULL;
	pScrollBarHandle = NULL;
	pCTextOut        = NULL;
	pCFileControl    = NULL;
	pDrawCircular    = NULL;
	pDrawRec         = NULL;
	pDrawLine        = NULL;
	pCHitTest        = NULL;
	pCMouseControl   = NULL;
	pCClipboardControl = NULL;
	pCGraphClipboardControl = NULL;
	pCWordClipboardControl = NULL;
}


void PaintInfo::DelMemDC()
{
	DeleteObject(_bmpMem);
	DeleteDC(_memDC);
	_memDC = NULL;
}

/**初始畫布DC*/
void PaintInfo::InitializeMemDC()
{
	HDC hdc = GetDC( _mainHwnd );
	RECT rectClient;
	rectClient.bottom = CommonVar::DRAW_MAP_HEIGHT;
	rectClient.top = 0;
	rectClient.left = 0;
	rectClient.right = CommonVar::DRAW_MAP_WIDTH;

	if( _memDC != NULL )
	{
		DeleteDC( _memDC );
	    DeleteObject(_bmpMem);
	}
	_memDC = CreateCompatibleDC(hdc);
	_bmpMem = CreateCompatibleBitmap(hdc, CommonVar::DRAW_MAP_WIDTH, CommonVar::DRAW_MAP_HEIGHT);
	SelectObject( _memDC, _bmpMem); // Let hdcMem's size become bigger as bmpMem

	// draw
	FillRect(_memDC, &rectClient , (HBRUSH) GetStockObject(WHITE_BRUSH)  );
	ReleaseDC(_mainHwnd ,hdc);
}

/** 建立顏色、線寬、底色圖示功能表*/
void PaintInfo::ConstructPopMenu(HWND hWnd, HINSTANCE hInst)
{
	HBITMAP hbit;
	HMENU hMainMenu, hSubMenu, hPopupMenu,hPopupBgMenu, hWidthMenu;

	int iMenuID;

	hMainMenu = LoadMenu( hInst, MAKEINTRESOURCE(IDC_PAINT) );
	hSubMenu  = GetSubMenu(hMainMenu,2);

	//建出顏色列表
	hPopupMenu = CreatePopupMenu();

	for( int i = 0 ; i < 8 ; i++ )
	{
		hbit = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_BITMAP5+i) );
		iMenuID = IDM_GREEN +i;

		AppendMenu( hPopupMenu, MF_BITMAP , iMenuID, (PTSTR)(LONG)hbit ); 
	}

	AppendMenu( hSubMenu, MF_STRING | MF_POPUP  , (UINT) hPopupMenu, _T("顏色(&Color)"));

	//建出線寬列表
	hWidthMenu = CreatePopupMenu();

	for( int i = 0 ; i < 5 ; i++ )
	{
		hbit = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_BITMAP19+i) );
		iMenuID = IDM_WIDTH_1 +i;

		AppendMenu( hWidthMenu, MF_BITMAP , iMenuID, (PTSTR)(LONG)hbit ); 
	}

	AppendMenu( hSubMenu, MF_STRING | MF_POPUP  , (UINT) hWidthMenu, _T("線寬(&Width)"));

	//建出底色列表
	hPopupBgMenu = CreatePopupMenu();

	AppendMenu( hPopupBgMenu, MF_STRING , IDM_BG_TRANSPARENT, _T("Transparent") ); 

	for( int i = 0 ; i < 8 ; i++ )
	{
		hbit = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_BITMAP5+i) );
		iMenuID = IDM_BG_GREEN +i;

		AppendMenu( hPopupBgMenu, MF_BITMAP , iMenuID, (PTSTR)(LONG)hbit ); 
	}

	AppendMenu( hSubMenu, MF_STRING | MF_POPUP  , (UINT) hPopupBgMenu, _T("底色(&Bg)"));

	SetMenu(hWnd,hMainMenu);
}


/** 建立畫布按鈕*/
void PaintInfo::ConstructBtn(HWND hWnd, HINSTANCE hInst)
{
	HWND button_hWnd;
	HBITMAP hbit;
	int iBitMapID;
	int iButtonID;

	for( int i = 0 ; i < 5 ; i++ )
	{
		switch( i)
		{
		case 0:
			iBitMapID = IDB_BITMAP1;
			iButtonID = IDB_LINE;
			break;
		case 1:
			iBitMapID = IDB_BITMAP2;
			iButtonID = IDB_REC;
			break;
		case 2:
			iBitMapID = IDB_BITMAP3;
			iButtonID = IDB_CIR;
			break;
		case 3:
			iBitMapID = IDB_BITMAP4;
			iButtonID = IDB_WORD;
			break;
		case 4:
			iBitMapID = IDB_BITMAP17;
			iButtonID = IDB_SELECT;
			break;
		}

		button_hWnd = CreateWindow(_T("BUTTON") ,NULL,
			WS_CHILD | WS_VISIBLE | BS_BITMAP | BS_DEFPUSHBUTTON,
			0, 0+ i*50, 48, 48,
			hWnd,
			(HMENU) iButtonID,
			hInst,
			NULL);

		hbit = LoadBitmap( hInst, MAKEINTRESOURCE(iBitMapID) );

		if( !hbit )
			MessageBox(hWnd,_T("fail bitmap"),_T("Fail"),NULL);

		SendMessage( button_hWnd , BM_SETIMAGE , (WPARAM) IMAGE_BITMAP , (LPARAM) hbit );

		if( !button_hWnd )
			MessageBox(hWnd,_T("fail create button"),_T("Fail"),NULL);

		switch( i)
		{
		case 0:
			SetLineBtnHWND(button_hWnd);
			break;
		case 1:
			SetRecBtnHWND(button_hWnd);
			break;
		case 2:
			SetCirBtnHWND(button_hWnd);
			break;
		case 3:
			SetWordBtnHWND(button_hWnd);
			break;
		case 4:
			SetSelectBtnHWND(button_hWnd);
			break;
		}

	}
}

/** 小畫家顏色選單觸發事件 */
void PaintInfo::ColorMenuClickEvent(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	CheckMenuItem(GetMenu(hWnd), GetColorSelection() ,MF_UNCHECKED );
	SetColorSelection(wParam);
	CheckMenuItem(GetMenu(hWnd),wParam,MF_CHECKED);

	
}

/** 小畫家筆細選擇*/
void PaintInfo::PenWidthMenuClickEvent(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	CheckMenuItem(GetMenu(hWnd), this->GetPenWidth() ,MF_UNCHECKED );
	this->SetPenWidth(wParam);
	CheckMenuItem(GetMenu(hWnd),wParam,MF_CHECKED);

	
}

/** 小畫家圖案底色選擇*/
void PaintInfo::FigBgMenuClickEvent(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	CheckMenuItem(GetMenu(hWnd), this->GetFigBg() ,MF_UNCHECKED );
	this->SetFigBg(wParam);
	CheckMenuItem(GetMenu(hWnd),wParam,MF_CHECKED);

	
}

/** 畫布上按鈕觸發事件*/
void PaintInfo::BtnClickEvent(HWND hWnd,WPARAM wParam,LPARAM lParam)
{

	switch( LOWORD(wParam))
	{
	case IDB_LINE:

		SendMessage(this->GetMainHwnd() , WM_COMMAND, IDM_LINE, lParam);

		break;
	case IDB_CIR:


		SendMessage(this->GetMainHwnd() , WM_COMMAND, IDM_CIR, lParam);

		break;
	case IDB_REC:


		SendMessage(this->GetMainHwnd() , WM_COMMAND, IDM_REC, lParam);

		break;
	case IDB_WORD:

		SendMessage(this->GetMainHwnd() , WM_COMMAND, IDM_WORD, lParam);
		break;
	case IDB_SELECT:

		////檢查工作圖案是否為文字選取框
		// int ID = this->GetHitID();
		// gDoubleLinkList.visitID_Node( ID );
		// if( gDoubleLinkList.GetCurrentPtrType() != IDM_WORD_REC )

		SendMessage(this->GetMainHwnd() , WM_COMMAND, IDM_SELECT, lParam);
		break;
	}
}

/** 小畫家功能選單觸發事件處理*/
void PaintInfo::MenuClickEvent(HWND hWnd,HINSTANCE hInst,WPARAM wParam, LPARAM lParam)
{  
	HBITMAP hbit;
	int lastSelection =  this->GetSelection();

	if( wParam != IDM_WORD )
	{
		//若此次為一段文字的起頭，則必需檢查上個節點是否為文字的起頭 (TCHAR) -1 ，cha ==65535
		if ( gMyList.CheckIsRemoveNullWordNode(this->GetHitID()) == TRUE )
		{
			this->GetCSelectRecInfo().SetIsShow(false);
			this->SetHitID(-1);
			this->GetDrawTool()->MapDCToMonitor();
		}
	
	}

	// 此選項沒被選過
	if( lastSelection != wParam )
	{
		CheckMenuItem(GetMenu(hWnd), lastSelection ,MF_UNCHECKED );
		this->SetSelection(wParam);
		CheckMenuItem(GetMenu(hWnd),wParam,MF_CHECKED);

		switch( lastSelection )
		{
		case IDM_LINE:
			hbit = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_BITMAP1) );
			SendMessage( this->GetLineBtnHWND(), BM_SETIMAGE , (WPARAM) IMAGE_BITMAP , (LPARAM) hbit );
			break;
		case IDM_REC:
			hbit = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_BITMAP2) );
			SendMessage( this->GetRecBtnHWND() , BM_SETIMAGE , (WPARAM) IMAGE_BITMAP , (LPARAM) hbit );
			break;
		case IDM_CIR:
			hbit = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_BITMAP3) );
			SendMessage( this->GetCirBtnHWND() , BM_SETIMAGE , (WPARAM) IMAGE_BITMAP , (LPARAM) hbit );
			break;
		case IDM_WORD:
			hbit = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_BITMAP4) );
			SendMessage( this->GetWordBtnHWND() , BM_SETIMAGE , (WPARAM) IMAGE_BITMAP , (LPARAM) hbit );
			break;
		case IDM_SELECT:
			hbit = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_BITMAP17) );
			SendMessage( this->GetSelectBtnHWND() , BM_SETIMAGE , (WPARAM) IMAGE_BITMAP , (LPARAM) hbit );


			break;
		default:
			break;
		}


		switch( wParam )
		{
		case IDM_LINE:
		
			hbit = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_BITMAP13) );
			SendMessage( this->GetLineBtnHWND() , BM_SETIMAGE , (WPARAM) IMAGE_BITMAP , (LPARAM) hbit );
			break;
		case IDM_REC:

			hbit = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_BITMAP14) );
			SendMessage( this->GetRecBtnHWND() , BM_SETIMAGE , (WPARAM) IMAGE_BITMAP , (LPARAM) hbit );
			break;
		case IDM_CIR:
		
			hbit = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_BITMAP15) );
			SendMessage( this->GetCirBtnHWND(), BM_SETIMAGE , (WPARAM) IMAGE_BITMAP , (LPARAM) hbit );
			break;
		case IDM_WORD:
			hbit = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_BITMAP16) );	
			SendMessage( this->GetWordBtnHWND() , BM_SETIMAGE , (WPARAM) IMAGE_BITMAP , (LPARAM) hbit );
			break;
		case IDM_SELECT:
			
			hbit = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_BITMAP18) );
			SendMessage( this->GetSelectBtnHWND() , BM_SETIMAGE , (WPARAM) IMAGE_BITMAP , (LPARAM) hbit );
			break;
		default:
			break;
		}
		//	DebugMsgOutputHandle::Instance()->ErrMsg();

	}
}

void PaintInfo::ClickClearMenuItem()
{
	gMyList.ClearAllListNode();


	this->DelMemDC();
	//this->InitializeMemDC();
	this->GetCTextOut()->CaretHide();
}

void PaintInfo::CheckMenuIsAvailable(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	UINT format = RegisterClipboardFormat(_T("MY_CUSTOM_FORMAT"));
	int iEnable = MF_GRAYED;

	switch(lParam)
	{
	case 0: // edit File Menu
		TCHAR szProgramTitle[MAX_PATH];
		::GetWindowText(hWnd,szProgramTitle,MAX_PATH);

		int iLength;
		iLength = ::_tcslen(szProgramTitle);
		//找尋程式標題有無*號
		for( int i = 0 ; i < iLength ; i ++ )
		{
			if( szProgramTitle[i] == '*')
			{
				iEnable = MF_ENABLED;
				break;
			}
		}

		::EnableMenuItem((HMENU)wParam,IDM_SAVE,iEnable);
		::EnableMenuItem((HMENU)wParam,IDM_SAVE_AS,iEnable);

		if( gMyList.IsNull() == FALSE )
			::EnableMenuItem((HMENU)wParam,IDM_CLEAR,MF_ENABLED);
		else
			::EnableMenuItem((HMENU)wParam,IDM_CLEAR,MF_GRAYED);
		break;
	case 1: // edit Edit Menu

		if( ::IsClipboardFormatAvailable(format) == TRUE )
		{
			EnableMenuItem((HMENU)wParam,IDM_PASTE,MF_ENABLED);
		}
		else if( ::IsClipboardFormatAvailable(CF_TEXT) == TRUE ) 
		{
			EnableMenuItem((HMENU)wParam,IDM_PASTE,MF_ENABLED);
		}
		else
			EnableMenuItem((HMENU)wParam,IDM_PASTE,MF_GRAYED);
		


		// Enable Cut, Copy, And Del 
		if( this->GetHitID() >= 0 )
		{
			iEnable = MF_ENABLED;
		}
		::EnableMenuItem((HMENU)wParam,IDM_COPY,iEnable);
		::EnableMenuItem((HMENU)wParam,IDM_CUT,iEnable);
		::EnableMenuItem((HMENU)wParam,IDM_DELETE,iEnable);



		if( gRedoUndoController->IsRedo() == TRUE)
			iEnable = MF_ENABLED;
		else
			iEnable = MF_GRAYED;
		::EnableMenuItem((HMENU)wParam,IDM_REDO,iEnable);


		if( gRedoUndoController->IsUndo() == TRUE)
			iEnable = MF_ENABLED;
		else
			iEnable = MF_GRAYED;
		::EnableMenuItem((HMENU)wParam,IDM_UNDO,iEnable);
		break;
	}

}
/*************************
*  Getter // setter
***************************/
HDC& PaintInfo::GetMemDC()
{
	if( !_memDC) 
		this->InitializeMemDC();
	return _memDC;
}


CFileControl* PaintInfo::GetCFileControl()
{
	return _CFileControl;
}

ScrollBarHandle* PaintInfo::GetScrollBarHandle()
{
	return _scrollBarHandle;
}

CTextOut* PaintInfo::GetCTextOut()
{
	return _CTextOut;
}

DrawTool* PaintInfo::GetDrawTool()
{
	return _drawTool;
}

CHitTest* PaintInfo::GetCHitTest()
{
	return _cHitTest;
}

CMouseControl* PaintInfo::GetCMouseControl()
{
	return _cMouseControl;
}

CClipboardControl* PaintInfo::GetClipboardControl()
{
	return this->_cClipboardControl;
}
void PaintInfo::SetMainHwnd(HWND hWnd)
{
	_mainHwnd = hWnd;
}

HWND PaintInfo::GetMainHwnd()
{
	return _mainHwnd;
}

void PaintInfo::SetLineBtnHWND(HWND hWnd)
{
	_LineBtnHwnd = hWnd;
}

HWND PaintInfo::GetLineBtnHWND()
{
	return _LineBtnHwnd;
}

void PaintInfo::SetRecBtnHWND(HWND hWnd)
{
	_RecBtnHwnd = hWnd;
}

HWND PaintInfo::GetRecBtnHWND()
{
	return _RecBtnHwnd;
}

void PaintInfo::SetWordBtnHWND(HWND hWnd)
{
	_WordBtnHwnd = hWnd;
}

HWND PaintInfo::GetWordBtnHWND()
{
	return _WordBtnHwnd;
}

void PaintInfo::SetCirBtnHWND(HWND hWnd)
{
	_CirBtnHwnd = hWnd;
}

HWND PaintInfo::GetCirBtnHWND()
{
	return _CirBtnHwnd;
}

void PaintInfo::SetSelectBtnHWND(HWND hWnd)
{
	_SelectBtnHwnd = hWnd;
}

HWND PaintInfo::GetSelectBtnHWND()
{
	return _SelectBtnHwnd;
}


void PaintInfo::SetSelection(int val)
{
	_iSelection = val;
}

int PaintInfo::GetSelection()
{
	return _iSelection ;
}

void PaintInfo::SetColorSelection(int val)
{
	_iColorSelection = val;
}

int PaintInfo::GetColorSelection()
{
	return _iColorSelection ;
}


void PaintInfo::SetClientHeight(int val)
{
	_clientHeight = val;
}

int PaintInfo::GetClientHeight()
{
	return _clientHeight;
}

void PaintInfo::SetClientWidth(int val)
{
	_clientWidth = val;
}

int PaintInfo::GetClientWidth()
{
	return _clientWidth;
}

void PaintInfo::SetDirectionMouse(int val)
{
	_directionMouse = val;
}

int PaintInfo::GetDirectionMouse()
{
	return _directionMouse;
}

void PaintInfo::SetPenWidth(int val)
{
	this->_PenWidth = val;
}

int PaintInfo::GetPenWidth()
{
	return this->_PenWidth;
}

void PaintInfo::SetFigBg(int val)
{
	this->_FigBg = val;
}

int PaintInfo::GetFigBg()
{
	return this->_FigBg;
}

void PaintInfo::SetHitID(int val)
{
	this->_HitIndex = val;
}

int PaintInfo::GetHitID()
{
	return this->_HitIndex;
}

void PaintInfo::SetAction(BOOL foo)
{
	this->_bAction = foo;
}

BOOL PaintInfo::GetAction()
{
	return this->_bAction;
}
