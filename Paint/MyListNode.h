#pragma once
#include "BaseListNode.h"
#include "CommonVar.h"
#include "WordNodeInfo.h"

using namespace std;

class MyListNode: public BaseListNode
{
public:
	MyListNode(void);
	~MyListNode(void);
	void AppendWordNode(POINT pt1,TCHAR cha);
	void InsertWordNode(POINT pt1,TCHAR cha);

    POINT FindLastWordPosInWordRec(POINT &pt);
	POINT FindIniWordPosInWordRec(POINT &pt);
	POINT FindCurrWordPosInWordRec(POINT &pt);

	int  GetWordNum();
	
	
	void TraceWordInfo();
	void ClearAllNode();

    void FreeObj();
   
    POINT ShiftLeftArrChaIdx();
    POINT ShiftRightArrChaIdx();
    POINT ShiftUpArrChaIdx(POINT cursorPt);
    POINT ShiftDownArrChaIdx(POINT cursorPt); 
	
	POINT ShiftHomeArrChaIdx(POINT cursorPt); 
	POINT ShiftCtrlHomeArrChaIdx(); 
	POINT ShiftEndArrChaIdx(POINT cursorPt);
	POINT ShiftCtrlEndArrChaIdx();



    BOOL  IsArrChaIdxBelongEnd();
    void  FixLastWordRecPosPt(POINT pt);
    
	WordNodeInfo* GetHead();
    WordNodeInfo* GetCurrWordPtr();
	WordNodeInfo* GetLast();
	void ShiftCurrPtrToLast();
    void SetCurrWordPtr(WordNodeInfo *ptr);
 
	POINT BackSpaceEvent();
    POINT DelEvent();

	
	bool DeleteNode(WordNodeInfo *node);



    MyListNode *_rlink;
	MyListNode *_llink;
private:

	WordNodeInfo *_head;
	WordNodeInfo *_last;
	WordNodeInfo *_curr;

	void AppendNode(WordNodeInfo *node);
	void InsertNode(WordNodeInfo *node);
	

	int _iTotalWordNum;
	
};

