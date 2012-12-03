#pragma once

	enum direction 
    {
     None  = 0,
     Left  = 1,
     Right = 2,
     Up    = 3,
     Down  = 4,
     
     LeftBoundary = 5,
     RightBoundary = 6,
     UpBoundary = 7,
     DownBoundary =8
    };


class PaintInfo;

class ScrollBarHandle
{
public:


    ScrollBarHandle(void);
    ~ScrollBarHandle(void);

  //  static ScrollBarHandle* Instance();

    void TurningScrollBar(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void VerticalScrollBarEvent(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void HorizontalScrollBarEvent(HWND hWnd, WPARAM wParam, LPARAM lParam);

    direction IsXScroll(int mouseX);
    direction IsYScroll(int mouseY);

    void VerticalScrollBarAutoEvent(HWND hWnd, int offset);
    void HorizontalScrollBarAutoEvent(HWND hWnd, int offset);
	void TuneWindowView();

    int GetHorzPos();
    int GetVertPos();

  
    int GetBeforeHorzPos();
    int GetBeforeVertPos();

    void SetBeforeHorzPos(int val);
    void SetBeforeVertPos(int val);
    void SetVertPos(int val);
    void SetHorzPos(int val);

	POINT GetNowScrollBarPos();
  
	void SetParent(PaintInfo* ptr);
	PaintInfo* GetParent(){return _parent;};
private:
    static ScrollBarHandle* _instance;
	PaintInfo* _parent;
 

    int _iHorzPos;
    int _iVertPos;

    int _iBeforeHorzPos;
    int _iBeforeVertPos;

	
};
