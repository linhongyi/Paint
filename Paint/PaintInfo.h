#pragma once
#include "CFileControl.h"
#include "ScrollBarHandle.h"
#include "DrawTool.h"
#include "CTextOut.h"
#include "CommonVar.h"
#include "CHitTest.h"
#include "CMouseControl.h"
#include "ClipboardControl.h"
#include "RedoUndoController.h"
#include "Originator.h"
#include "CSelectRecInfo.h"
#include "CInverseWordInfo.h"
#include "CFileControl.h"

class PaintInfo {
public:
    
    PaintInfo(void);
    ~PaintInfo(void);
	void ConstrunctionAllObjRelation();
    //static PaintInfo* Instance();
   
    void InitializeMemDC();
	void ConstructPopMenu(HWND hWnd, HINSTANCE hInst);
	void ConstructBtn(HWND hWnd,HINSTANCE hInst  );
    void ColorMenuClickEvent(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void PenWidthMenuClickEvent(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void FigBgMenuClickEvent(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void MenuClickEvent(HWND hWnd,HINSTANCE hInst,WPARAM wParam, LPARAM lParam);
	void BtnClickEvent(HWND hWnd,WPARAM wParam,LPARAM lParam);
	void SetMainHwnd(HWND hWnd);
	void ClickClearMenuItem();
	void CheckMenuIsAvailable(HWND hWnd, WPARAM wParam, LPARAM lParam);

    HWND GetMainHwnd();

    void SetSelection(int val);
    int  GetSelection();

    void SetColorSelection(int val);
    int  GetColorSelection();

 

    void SetMemDC(HDC dc);
    HDC&  GetMemDC();

    void SetClientWidth(int val);
    void SetClientHeight(int val);
    int GetClientWidth();
    int GetClientHeight();

    void DelMemDC();

	void SetLineBtnHWND(HWND hWnd);
    HWND GetLineBtnHWND();
	void SetRecBtnHWND(HWND hWnd);
    HWND GetRecBtnHWND();
	void SetWordBtnHWND(HWND hWnd);
    HWND GetWordBtnHWND();
	void SetCirBtnHWND(HWND hWnd);
    HWND GetCirBtnHWND();
	void SetSelectBtnHWND(HWND hWnd);
	HWND GetSelectBtnHWND();

	int GetDirectionMouse();
	void SetDirectionMouse(int val);

	int GetPenWidth();
	void SetPenWidth(int val);

	int GetFigBg();
	void SetFigBg(int val);

	int GetHitID();
	void SetHitID(int val);

	void SetAction(BOOL foo);
	BOOL GetAction();

	CFileControl*     GetCFileControl();
	ScrollBarHandle*  GetScrollBarHandle();
	CTextOut*         GetCTextOut();
	DrawTool*         GetDrawTool();
	CHitTest*         GetCHitTest();
	CMouseControl*    GetCMouseControl();
	CClipboardControl* GetClipboardControl();
	
	CInverseWordInfo&  GetCInverseWordInfo(){ return _cInverseWordInfo; };
	CSelectRecInfo&    GetCSelectRecInfo(){ return _cSelectRecInfo; };

	void SetCHitTest(CHitTest *ptr){_cHitTest =  ptr;};
	void SetCFileControl(CFileControl *ptr){_CFileControl =  ptr;};
	void SetScrollBarHandle(ScrollBarHandle *ptr){_scrollBarHandle =  ptr;};
	void SetCTextOut(CTextOut *ptr){_CTextOut =  ptr;};
	void SetDrawTool(DrawTool *ptr){_drawTool =  ptr;}
	void SetCMouseControl(CMouseControl *ptr){_cMouseControl =  ptr;};
	void SetClipboardControl(CClipboardControl *ptr){_cClipboardControl = ptr;};
	

private:

	 CFileControl*      _CFileControl;
	 ScrollBarHandle*   _scrollBarHandle;
	 DrawTool*          _drawTool;
	 CTextOut*          _CTextOut;
	 CHitTest*          _cHitTest;
	 CMouseControl*     _cMouseControl;
	 CClipboardControl* _cClipboardControl;
	 CSelectRecInfo     _cSelectRecInfo;
	 CInverseWordInfo   _cInverseWordInfo;
     
     HWND _mainHwnd;  
     HDC  _memDC;

     HBITMAP _bmpMem;

     int _iSelection; // 直線 矩形 橢圓 文字
     int _iColorSelection; // 綠、黃、紅、藍、粉紅、灰、黑、天空藍
	 int _directionMouse;
   
	 int _PenWidth;   // 筆寬
     int _FigBg;      // 圖案底色
	 int _clientWidth;
     int _clientHeight;
     int _HitIndex;

	 BOOL _bAction;

     HWND _LineBtnHwnd;
     HWND _CirBtnHwnd;
     HWND _RecBtnHwnd;
     HWND _WordBtnHwnd;
	 HWND _SelectBtnHwnd;

	
};
