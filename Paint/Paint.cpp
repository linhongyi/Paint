// Paint.cpp : Defines the entry point for the application.

/*********************************************
* The painter program
*@ author : hongyiLin
*@ date   : 2012.09.27
* All rights reserved
**********************************************/

#include "stdafx.h"
#include "Paint.h"	
#include "PaintInfo.h"

#include "Originator.h"
#include "RedoUndoController.h"
#include "Invoker.h"
#include "MyListController.h"
#include "AddRedoUndoPaintInfo.h"
#include "SgWordDataModelControler.h"
#include <assert.h>
#include "UnitTest.h"

#define MAX_LOADSTRING 100

// add by Hongyi
TCHAR szChildWindowClass[20] = _T("szChildWindow");
HINSTANCE subhInst; // draw tool instance

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

PaintInfo gPaintInfo;

MyListController gMyList;
AddRedoUndoPaintInfo gAddRedoUndoPaintInfo;
Originator          *gOriginator;
RedoUndoController  *gRedoUndoController;
CInvoker             gCInvoer;


// Forward declarations of functions included in this code module:
void				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    childWndProc(HWND,UINT,WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings

	LoadString(hInstance, IDS_INIAPP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PAINT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PAINT));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will Get 'well formed' small icons associated
//    with it.
//
void MyRegisterClass(HINSTANCE hInstance)
{

	WNDCLASSEX wcex;

	// main window
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW ;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PAINT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PAINT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassEx(&wcex);

	// sub window

	subhInst = hInstance +1;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style  = CS_HREDRAW | CS_VREDRAW;
	wcex.hInstance   = subhInst;
	wcex.lpfnWndProc = childWndProc;
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcex.cbWndExtra  = sizeof(long);
	wcex.hIcon       = NULL;
	wcex.lpszClassName = szChildWindowClass;

	RegisterClassEx(&wcex);

	// return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL | WS_CLIPCHILDREN,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;   
	static HWND child_hWnd;
	PAINTSTRUCT ps;
	static BOOL fBlocking;
	UnitTest *unitTest = NULL;
//	CUPPHttpTest *cUPPHttpTest = NULL;
//	myTest *test = NULL;
//	COperation *pObj = NULL;

	static int mouseMovePosX = 0;
	static int mouseMovePosY = 0;


	switch (message)
	{
	case WM_INPUTLANGCHANGE:
		OutputDebugString(_T("WM_INPUTLANGCHANGE\n"));
		gPaintInfo.GetCTextOut()->SetDwCharSet(wParam);
		break;
	case WM_SETFOCUS:
		break;
	case WM_KILLFOCUS:
		break;
	case WM_CREATE:
		OutputDebugString(_T("WM_CREATE\n"));

		gPaintInfo.ConstrunctionAllObjRelation();
		gOriginator = new Originator();
	    gRedoUndoController = new RedoUndoController(gOriginator);
		
	    //�إߵe��DC
		//gPaintInfo.InitializeMemDC();
		//�إߵe���u�����
		child_hWnd = CreateWindow(szChildWindowClass ,TEXT("�e��"),
			WS_CAPTION | WS_VISIBLE | WS_CHILD |WS_CLIPCHILDREN,
			CW_USEDEFAULT, CW_USEDEFAULT, 52, 280,
			hWnd,
			NULL,
			subhInst,
			NULL);

		if( !child_hWnd )
			MessageBox(hWnd,_T("fail create draw map"),_T("Fail"),NULL);

		//�x�s�D���� hWnd
		gPaintInfo.SetMainHwnd(hWnd);
		//�إ�pop_up �\���
		gPaintInfo.ConstructPopMenu(hWnd,hInst);
		//�w�]�¦�
		gPaintInfo.ColorMenuClickEvent(hWnd,IDM_BLACK,NULL);
		//�w�]���� =1 
		gPaintInfo.PenWidthMenuClickEvent(hWnd,IDM_WIDTH_1,NULL);
		//�w�]����z��
		gPaintInfo.FigBgMenuClickEvent(hWnd,IDM_BG_TRANSPARENT,NULL);
		//�w�]���u
		gPaintInfo.MenuClickEvent(hWnd,hInst,IDM_LINE,NULL);
		//���o��r�j�p
		gPaintInfo.GetCTextOut()->ResetCharSize();
		// create Caret
		::CreateCaret(hWnd,NULL,1,gPaintInfo.GetCTextOut()->GetCharY());
		// ��l���ɮ׺޲z����
		gPaintInfo.GetCFileControl()->PopFileInitialize(hWnd);
	
		ToolKits::IniDLL();
		
	//	unitTest->AutoGenerateObj();
	/*	pObj->ParamAdd(1,2);
		pObj->ParamMultiplicate(2,4);*/
	/*	cUPPHttpTest = new CUPPHttpTest();
		cUPPHttpTest->OnButton1();*/
	
	/*	test = new myTest();
		test->Test();*/
		break;

	case WM_SIZE:
		 OutputDebugString(_T("WM_SIZE\n"));
		//�ھڵ����j�p�ܤơA�վ㱲�ʦC
		gPaintInfo.GetScrollBarHandle()->TurningScrollBar(hWnd,wParam,lParam);
		gPaintInfo.GetCTextOut()->CaretHide();
		
	
		break;

	case WM_CLOSE:
		gPaintInfo.GetCFileControl()->CloseProgram(hWnd);
		//DestroyWindow(hWnd);
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_NEW:
			gPaintInfo.GetCFileControl()->FileCreate(hWnd);
			break;
		case IDM_SAVE:
			gPaintInfo.GetCFileControl()->SaveFile(hWnd);
			break;
		case IDM_SAVE_AS:
			gPaintInfo.GetCFileControl()->SaveFileAs(hWnd);
			break;
		case IDM_OPEN:
			gPaintInfo.GetCFileControl()->OpenFile(hWnd);
			break;
		case IDM_EXIT:
			gPaintInfo.GetCFileControl()->CloseProgram(hWnd);
			break;
		case IDM_CLEAR:
			gPaintInfo.GetCFileControl()->ClickClearMenu(hWnd);
			break;
			// edit Handle
		case IDM_COPY:
			gPaintInfo.GetClipboardControl()->CopyControl();
			break;
		case IDM_PASTE:
			gPaintInfo.GetClipboardControl()->PasteControl();
			break;
		case IDM_CUT:
			gPaintInfo.GetClipboardControl()->CutControl();
			break;
		case IDM_DELETE:
			gPaintInfo.GetClipboardControl()->DeleteControl();
			break;
		case IDM_REDO:
			gRedoUndoController->Redo();
			break;
		case IDM_UNDO:
			gRedoUndoController->Undo();
			break;
			// menu handle  
		case IDM_LINE:
		case IDM_REC:
		case IDM_CIR:
		case IDM_WORD:
		case IDM_SELECT:
			gPaintInfo.MenuClickEvent(hWnd,hInst,wParam,lParam);
			gPaintInfo.GetCTextOut()->CaretHide();

			if( wmId != IDM_WORD && gMyList.IsWordStart() == TRUE )
				gPaintInfo.GetCSelectRecInfo().SetIsShow(false);
			gPaintInfo.GetDrawTool()->MapDCToMonitor();
			break;
			// color menu handle
		case IDM_GREEN:
		case IDM_RED:
		case IDM_YELLOW:
		case IDM_BLUE:
		case IDM_PINK:
		case IDM_BLACK:
		case IDM_GRAY:
		case IDM_SKYBLUE:
			gPaintInfo.ColorMenuClickEvent(hWnd,wParam,lParam);
			if( gPaintInfo.GetHitID() >=0 )
			{
				MyListNode *ptr  = gMyList.visitID_Node( gPaintInfo.GetHitID() );
				gAddRedoUndoPaintInfo.AddModifyPenColorInfo( wParam );
				ptr->SetPenColor( wParam );

				gPaintInfo.GetDrawTool()->RePaintWithClipRgn( ptr->GetPtBeg(), ptr->GetPtEnd() );
				gPaintInfo.GetDrawTool()->MapDCToMonitor();

				if( ptr->GetnObjType() == IDM_WORD_REC )
					SgWordDataModelControler::Instance()->SaveAllWordIniInThisWordRec( gPaintInfo.GetHitID() );
			}
			break;
			// pen width menu handle
		case IDM_WIDTH_1:
		case IDM_WIDTH_2:
		case IDM_WIDTH_3:
		case IDM_WIDTH_4:
		case IDM_WIDTH_5:
			gPaintInfo.PenWidthMenuClickEvent(hWnd,wParam,lParam);
			if( gPaintInfo.GetHitID() >=0  )
			{
				MyListNode *ptr  = gMyList.visitID_Node( gPaintInfo.GetHitID());
				gAddRedoUndoPaintInfo.AddModifyPenWidthInfo( wParam );
				ptr->SetPenWidth( wParam );

				gPaintInfo.GetDrawTool()->RePaintWithClipRgn( ptr->GetPtBeg(), ptr->GetPtEnd() );
				gPaintInfo.GetDrawTool()->MapDCToMonitor();

			}
			break;
			// Fig bg menu handle
		case IDM_BG_TRANSPARENT:
		case IDM_BG_GREEN:
		case IDM_BG_RED:
		case IDM_BG_YELLOW:
		case IDM_BG_BLUE:
		case IDM_BG_PINK:
		case IDM_BG_BLACK:
		case IDM_BG_GRAY:
		case IDM_BG_SKYBLUE:
			gPaintInfo.FigBgMenuClickEvent(hWnd,wParam,lParam);
			//���ܥؼйϮת�����
			if( gPaintInfo.GetHitID() >=0 )
			{

				MyListNode *ptr = gMyList.visitID_Node(gPaintInfo.GetHitID());
				gAddRedoUndoPaintInfo.AddModifyBgColorInfo( wParam );
				ptr->SetFigBg( wParam );

				gPaintInfo.GetDrawTool()->FillWhileRec( ptr->GetPtBeg(), ptr->GetPtEnd() );
				gPaintInfo.GetDrawTool()->RePaintWithClipRgn( ptr->GetPtBeg(), ptr->GetPtEnd() );
				gPaintInfo.GetDrawTool()->MapDCToMonitor();

				if( ptr->GetnObjType() == IDM_WORD_REC )
					SgWordDataModelControler::Instance()->SaveAllWordIniInThisWordRec( gPaintInfo.GetHitID() );
			}

			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		//OutputDebugString(_T("WM_PAINT\n"));
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
	
		//���e
		gPaintInfo.GetDrawTool()->MapDCToMonitor();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		// mouseEventHandle
	case WM_LBUTTONDOWN:
		//OutputDebugString(_T("�ƹ�����b��ܰϰ����\n"));

		gPaintInfo.GetCMouseControl()->WmLbuttonDownEvent(hWnd, LOWORD(lParam) ,  HIWORD(lParam) );

		SetCapture(hWnd);
		fBlocking = TRUE;

		break;
	case WM_LBUTTONUP:
		if( fBlocking == TRUE )
		{

			//  OutputDebugString(_T("�ƹ�����b��ܰϰ��}\n"));

			if(  gPaintInfo.GetDirectionMouse() == CursorDirection_IBEAM )
			{
				POINT caretPt = gPaintInfo.GetCInverseWordInfo().GetCaretPt2();
				gPaintInfo.GetDrawTool()->ScreenToWindowCoordinate( caretPt.x, caretPt.y );

				gPaintInfo.GetCTextOut()->CaretSetPos(caretPt);
				gPaintInfo.GetCTextOut()->CaretShow();
			}
			else
				gPaintInfo.GetCMouseControl()->WmLbuttonUpEvent( hWnd,LOWORD(lParam) ,  HIWORD(lParam) );

			SetCursor(LoadCursor(NULL,IDC_ARROW));
			fBlocking = FALSE;
			ReleaseCapture();
		}
		::SetFocus(hWnd);

		break;
	case WM_MOUSEMOVE:

		if( fBlocking == TRUE )
		{
			if(  gPaintInfo.GetDirectionMouse() == CursorDirection_IBEAM )
			{
				gPaintInfo.GetCMouseControl()->MouseMoveEventForInverseWord(hWnd, wParam, lParam);
			}
			else
			{
				gPaintInfo.GetCMouseControl()->MouseMoveEventForDrawSomething( hWnd, wParam , lParam) ;
			}
			
		}
		else if( ::abs( (int) wParam - mouseMovePosX ) > 0 ||  abs( (int) lParam - mouseMovePosY ) > 0 )
		{
			//�P�_�O�_�R���ؼйϮסA�åB���ܴ�ЧΪ��A���x�s�ƹ��ثe���欰
			gPaintInfo.GetCMouseControl()->IsHitGraphAndChangeCursorShape( LOWORD(lParam) ,  HIWORD(lParam));
			mouseMovePosX = (int) wParam;
			mouseMovePosY = (int) lParam;
		
		}
		else 
		{
			//�]�w���б���
		    gPaintInfo.GetDrawTool()->SetMouseCurrsorInMouseEvent(gPaintInfo.GetDirectionMouse());

		}
		break;

		// scrollBarHandle
	case WM_VSCROLL:
		gPaintInfo.GetScrollBarHandle()->VerticalScrollBarEvent(hWnd,wParam,lParam);
		gPaintInfo.GetDrawTool()->ScrollBarEventForDrawSomething(hWnd);
		//���e
		gPaintInfo.GetDrawTool()->MapDCToMonitor();
		gPaintInfo.GetCTextOut()->CaretHide();
		break;
	case WM_HSCROLL: 
		gPaintInfo.GetScrollBarHandle()->HorizontalScrollBarEvent(hWnd,wParam,lParam);
		gPaintInfo.GetDrawTool()->ScrollBarEventForDrawSomething(hWnd);
		//���e
		gPaintInfo.GetDrawTool()->MapDCToMonitor();
		gPaintInfo.GetCTextOut()->CaretHide();
		break;

		// Keyword Handle
	case WM_KEYDOWN:

		//OutputDebugString(_T("WM_KEYDOWN\n"));
		gPaintInfo.GetCTextOut()->ClickKeyBoardEventHandle(wParam,lParam);
		break;
	case WM_KEYUP:
		// OutputDebugString(_T("WM_KEYUP\n"));
		gPaintInfo.GetCTextOut()->ReleaseKeyBoardEventHandle(wParam,lParam);
		break;
	case WM_CHAR:

		if( gPaintInfo.GetHitID() < 0 )
		{
			OutputDebugString(_T(" isnt focus on wordrec\n"));
		}
		else if ( gMyList.GetCurrentPtrType() == IDM_WORD_REC )
		{	 
			gPaintInfo.GetCTextOut()->NoSystemClickKeyBoardEventHandle(wParam,lParam);
		}
		break;
	case WM_INITMENUPOPUP:
		gPaintInfo.CheckMenuIsAvailable(hWnd,wParam,lParam);
		break;
	case WM_TIMER:
		//::OutputDebugString(_T("WM_TIMER\n"));
		gPaintInfo.GetClipboardControl()->PasteControl();
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

LRESULT CALLBACK childWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC	 hdc;
	PAINTSTRUCT ps;

	// TCHAR debugBuf[200];

	switch( message )
	{
	case WM_CREATE:
		// �إߵe���W�����s
		gPaintInfo.ConstructBtn(hWnd,hInst);
		break;
	case WM_COMMAND:
		// Ĳ�o���s�ƥ�
		gPaintInfo.BtnClickEvent(hWnd,wParam,lParam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps );

		EndPaint(hWnd, &ps);

		break;

	}
	return DefWindowProc( hWnd, message, wParam, lParam );
}

