#pragma once
#include "SimpleClipboard.h"
#include "CommonVar.h"

class CWordClipboardControl :
	public SimpleClipboard
{
public:
	CWordClipboardControl(void);
	~CWordClipboardControl(void);

	void CopyWord(HWND hWnd,int ID, int type, POINT ptBeg, POINT ptEnd, POINT wordIniPt,int iPenColor, int iBgColor,const vector<char>& tmp,BOOL IsSegment);
	void Paste(clipObj *pBuffer,char* pText);
	void CutWordRec();
	void CutSegmentWord();
	void Paste_NewWord();

	void Paste_SegmentWord();
	void SetPtext(char* pText);

};

