#pragma once

class PaintInfo;
class CMouseControl
{
public:
	CMouseControl(void);
	~CMouseControl(void);
	void SetParent(PaintInfo *ptr);

	void MouseMoveEventForDrawSomething(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void MouseMoveEventForInverseWord(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void WmLbuttonDownEvent(HWND hWnd,int mouseX, int mouseY);
    void WmLbuttonUpEvent(HWND hWnd,int mouseX,int mouseY);


	void SetThisTurnPosEndX(LONG x);
	void SetThisTurnPosEndY(LONG y);
	void IsHitGraphAndChangeCursorShape(int mouseX, int mouseY);
	void InputPosFixForShiftGraph();
	void TuneGraphForShiftiGraph();
	void ChangeCursorShapeInScaleGraph();
	void TuneGraphForShiftSingleDirectionGraph(int type);

private:
	BOOL IsGraphShiftForMouseMoving();
	BOOL IsGraphShiftSingleDirectionForMouseMoving();
	void InitialPosFixForDragOrScale(int mouseX,int mouseY);
	void GraphBoundaryPosFix(HWND hWnd, int &iEndPosX, int &iEndPosY,BOOL IS_SCROOLX, BOOL IS_SCROOLY, int offset);
	void InverseWordBoundaryFix(HWND hWnd, POINT ptStart, POINT ptEnd, int &iEndPosX, int &iEndPosY, int offset);
	void WordRecBoundaryPosFixForSingleDirectoin();
	void WordRecBoundaryPosFixForScaling();
	PaintInfo *_parent;
	POINT _posBeg , _posEnd;
	int _iWindowMouseDownPosX;
    int _iWindowMouseDownPosY;
	int _iWindowMouseMovePosX;
	int _iWindowMouseMovePosY;

	int _iDisX_Cursor_PosEnd;
	int _iDisY_Cursor_PosEnd;
	int _iDisX_Cursor_PosStart;
	int _iDisY_Cursor_PosStart;
	BOOL _mouseIsMove;
};

