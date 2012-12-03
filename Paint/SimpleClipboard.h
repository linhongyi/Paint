#include "CommonVar.h"
#pragma once


class CClipboardControl;

class SimpleClipboard
{
public:
	SimpleClipboard(void);
	virtual ~SimpleClipboard(void);
	
	void MoveSelectRecIntoLastTarGetRecByDrawMap();
	void SetParent(CClipboardControl *ptr){ _parent = ptr; }; 
	void RestOffset();

protected:
	CClipboardControl* _parent;
	clipObj *_buffer;
	char*   _pText;
	int _xOffset;
	int _yOffset;
		
	void X_BoundaryDetection(HWND hWnd,POINT &pt);
	void Y_BoundaryDetection(HWND hWnd,POINT &pt);
	void DecidePasteGraphPos();
	
	
};