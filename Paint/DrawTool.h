#pragma once
#include "resource.h"
#include "DrawLine.h"
#include "DrawCircular.h"
#include "DrawRec.h"


class PaintInfo;
class DrawTool
{
public:
       DrawTool(void);
       ~DrawTool(void);
	  
        void SetParent(PaintInfo *ptr);
      

        void ClearMonitor();
        void ScrollBarEventForDrawSomething(HWND hWnd);
		void ScreenToWindowCoordinate(LONG &x, LONG &y );
		void WindowToScreenCoordinateForStartPos(LONG &x, LONG &y );
		void WindowToScreenCoordinateForEndPos(LONG &x, LONG &y );
		
		void SetMouseCurrsorInMouseEvent(int direction);
		void RePaintWithClipRgn(POINT rect1, POINT rect2);
		void MapDcPasterToWindowDC(HDC &hdcMem);
		void DrawFig(HDC &hdcMem,int iType,int iStartPosX,int iStartPosY, int iEndPosX, int iEndPosY, int penColor, int figBg, int penWidth);
		void FigSenSorDraw(HDC &hdcMem,int iType,int iStartPosX,int iStartPosY, int iEndPosX, int iEndPosY,COLORREF bgColor);
		void CurrentSelectRecInfoShow(int iDetectionIndex);
		void MapDCToMonitor();
		
	
		void FillWhileRec(POINT pt1,POINT pt2);
		void ShowTarGetRecForTypingWord();
	
		DrawCircular* GetDrawCircular(){ return _drawCircular; };
		DrawLine* GetDrawLine(){ return _drawLine; };
		DrawRec* GetDrawRec(){ return _drawRec; };
		void SetDrawCircular(DrawCircular *ptr){_drawCircular =  ptr;};
		void SetDrawLine(DrawLine *ptr){_drawLine =  ptr;};
		void SetDrawRec(DrawRec *ptr){_drawRec =  ptr;};

		
		PaintInfo* GetPaintInfo();
private: 
	   
	    PaintInfo*    _parent;
		DrawCircular* _drawCircular;
		DrawLine*     _drawLine;
		DrawRec*      _drawRec;
};
