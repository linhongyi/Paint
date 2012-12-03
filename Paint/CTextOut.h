#pragma once
#include <string>
#include "MyListNode.h"

using namespace std;


class PaintInfo;
class CTextOut
{
public:
    CTextOut(void);
    ~CTextOut(void);
    void SetParent(PaintInfo *ptr);
	void ResetCharSize();
    void ClickKeyBoardEventHandle(WPARAM wParam, LPARAM lParam);
	void ReleaseKeyBoardEventHandle(WPARAM wParam, LPARAM lParam);
    void NoSystemClickKeyBoardEventHandle(WPARAM wParam, LPARAM lParam);
    void ReCauculateTextBuffer();
     
    void SetDwCharSet(int val);

    void CaretShow();
    void CaretHide();
    void CaretSetPos(POINT pt);

	int  GetCharX();
	int  GetCharY();

	void Type_WordHandle(int ID,TCHAR cha, int penColor, int bgColor);
	void ReturnKeyHandle();

	void SetNearCaretPosInClickingMouse(int &x , int &y);
	void TextWordToDC(HDC& copyMemHDC,int posX,int posY, TCHAR cha[], int penColor, int bgColor);
	void TextLineWordToDcInWindowView(MyListNode *ptr,HDC &copyMemHDC, POINT clipRgnPt1, POINT clipRgnPt2);
	
	BOOL IsTheWordInTheWindowView(POINT ptStart,POINT ptEnd);
	void InverseTextWordToDC(HDC & copyMemHDC);
	
	void WordReputationForInsertingWord(int ID);
    void DeleteWordTokenForWordReputation(POINT pt);
    
	void AddWordRecIntoQueue(POINT recPt1,POINT recPt2);
	
	BOOL IsInverseWordScope();
	BOOL IsTheWordInTheInverseWord(POINT candicateWordPt,POINT inverseWordStartPt, POINT inverseWordEndPt);


	void IsAddWordRecForWordContent(int ID);

	void ResetClipRgnPt();
	void RenewNowCurScope();
	void InverseWordHandle(BOOL isAction);
	BOOL IsChangeThisWordRecScope(int ID, POINT pt1, POINT pt2);
private:


	BOOL IsLastLine();
	BOOL IsXLineEnd();
	BOOL IsYLineEnd();

	PaintInfo* _parent;
	POINT _clipRgnPt1;
	POINT _clipRgnPt2;
	
    int dwCharSet;
	int _cxChar;
    int _cyChar;
  
 
    BOOL IS_LOCK;
	BOOL _IS_CLICK_CONTROL;
	
    void DeleteHandle(BOOL bAction);
	void BackSpaceHandle(BOOL bAction);
	
	void VkLeftHandle();
	void VkDownHandle();
	void VkRightHandle();
	void VkUpHandle();
	void VkEndHandle();
	void VkHomeHandle();
 
	int _hideCounter;
	int _showCounter;
};
