#pragma once
#include "SimpleClipboard.h"



class CGraphClipboardControl:
	public SimpleClipboard
{
public:
	CGraphClipboardControl(void);
	~CGraphClipboardControl(void);

	void Copy(HWND hWnd,int type, POINT pt1, POINT pt2, int penColor, int figBg, int penWidth);
	void Paste(clipObj* pBuffer);
	void CutGraph();
};

