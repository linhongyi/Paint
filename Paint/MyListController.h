#pragma once
#include "BaseListNode.h"
#include "CommonVar.h"
#include "MyListNode.h"


class MyListController
{
public:
	MyListController(void);
	~MyListController(void);

	void AddGraphNode(int type, POINT posBeg, POINT posEnd, int penColor,int figBg, int penWidth);
	void ModifyCurrNodeInfo(int type, POINT posBeg, POINT posEnd, int penColor,int bgColor, int penWidth);
	bool CheckIsRemoveNullWordNode(int ID);
	
	InsertAction InsertGraphIDNode(int ID,int type, POINT posBeg, POINT posEnd, int penColor,int figBg, int penWidth);

	bool DeleteID_Node(int ID);

	MyListNode* visitID_Node(int ID);
	MyListNode* GetHead();
	MyListNode* GetLast();


	void TraceAllNode();
	void ClearAllListNode();
	void CurrDataToRecordData(RecordObj *data);
	

	BOOL IsLastTypeIsWordRec(int ID);
	BOOL IsID_NodeExit(int ID);
	BOOL IsNull();
	BOOL IsWordStart();

	int   GetTotalNum();
	int   GetCurrentPtrType();
	POINT GetCurrentPtrPosBeg();
	POINT GetCurrentPtrPosEnd();
	int   GetCurrentPenColor();
	int   GetCurrentPenWidth();
	int   GetCurrentFigBg();
	int   GetCurrentNo();

private:
	void PopNode();
	void AppendNode(MyListNode *node);
	bool DeleteNode(MyListNode *node);

	
	MyListNode *_head;
	MyListNode *_last;
	MyListNode *_curr;

	int _iTotalNum;
	int _currentID;
};

