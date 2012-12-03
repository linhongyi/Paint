#include "StdAfx.h"
#include "MyListController.h"
#include "DebugMsgOutputHandle.h"
#include "Resource.h"
#include "MyListNode.h"
#include <assert.h>
#include "ToolKits.h"

MyListController::MyListController(void)
{
	_head = NULL;
	_last = NULL;
	_curr = NULL;
	
	_iTotalNum = 0;
	_currentID = 0;

}


MyListController::~MyListController(void)
{
	_head = NULL;
	_last = NULL;
	_curr = NULL;
}

/*************************
* 加入圖形資料節點至佇列
**************************/
void MyListController::AddGraphNode(int type, POINT posBeg, POINT posEnd,int penColor,int figBg, int penWidth)
{
	MyListNode *newNode = new MyListNode();
	::SecureZeroMemory(newNode, sizeof(MyListNode));

	newNode->SetnObjType(type);
	newNode->SetPtBeg(posBeg.x,posBeg.y);
	newNode->SetPtEnd(posEnd.x,posEnd.y);
	newNode->SetPenColor(penColor);
	newNode->SetFigBg(figBg);
	newNode->SetPenWidth(penWidth);
	newNode->SetNo( this->_currentID );
	AppendNode(newNode);
    
	newNode = NULL;
	
	this->_iTotalNum++;
	this->_currentID++;

}

/*************************
* 修改圖形資料節點至佇列
**************************/
void MyListController::ModifyCurrNodeInfo(int type, POINT posBeg, POINT posEnd,int penColor,int bgColor, int penWidth)
{
	////正規化
	//if( type != IDM_LINE )
	//{
	//	ToolKits::Normalization( posBeg, posEnd );
	//}

	
	_curr->SetnObjType(type);
	_curr->SetPtBeg( posBeg.x , posBeg.y);
	_curr->SetPtEnd( posEnd.x , posEnd.y);

	_curr->SetPenColor( penColor );
	_curr->SetFigBg( bgColor );
	_curr->SetPenWidth( penWidth );

	//this->TraceAllNode();
}

/*****************************************
* 檢查是否最後一個節點(last) 是空文字節點
*******************************************/
bool MyListController::CheckIsRemoveNullWordNode(int ID)
{
	bool answer = false;

	if(this->IsNull() ==FALSE && this->IsWordStart() == TRUE )
	{
		//已是文字起頭 又沒有其它文字資料，移除節點
		this->PopNode();
		answer = true;
	}
	return answer;
}

/****************************
* 佇列中找尋此ID節點，並刪除
*****************************/
bool MyListController::DeleteID_Node(int ID)
{
	bool answer;
	int buff = 0;

	MyListNode* tmp = this->visitID_Node( ID ) ;

    // 上一個
    if( tmp->_llink != NULL )
       _curr = tmp->_llink;
    else 
       _curr = tmp->_rlink;
	answer = this->DeleteNode( tmp );

	if( answer == true )
	{
		this->_iTotalNum--;
	}

	return answer;
}

//插入ID節點
InsertAction MyListController::InsertGraphIDNode(int ID,int type, POINT posBeg, POINT posEnd, int penColor,int figBg, int penWidth)
{
	InsertAction insertAction = InsertNone;

	MyListNode *newNode  = new MyListNode();
	::SecureZeroMemory(newNode, sizeof(MyListNode));

	newNode->SetnObjType(type);
	newNode->SetPtBeg(posBeg.x,posBeg.y);
	newNode->SetPtEnd(posEnd.x,posEnd.y);
	newNode->SetPenColor(penColor);
	newNode->SetFigBg(figBg);
	newNode->SetPenWidth(penWidth);
	newNode->SetNo(ID);

	if( _head == NULL )
	{
		this->AppendNode(newNode);
		insertAction = InsertTail;
	}
	else if ( _head == _last )
	{
		//插頭
		if( _head->GetNo() > ID )
		{
			newNode->_rlink = _head;
			_head->_llink = newNode;
			newNode->_llink = NULL;
			_head = newNode;
			insertAction = InsertHead;
		}
		//插尾
		else
		{
			this->AppendNode(newNode);
			insertAction = InsertTail;
		}
	}
	else
	{
		MyListNode *tmp = this->_head;
		
		for( ;tmp != NULL ; tmp = tmp->_rlink )
		{
			if( tmp->GetNo() > ID)
			{
				if( tmp == _head )
				{	
					newNode->_rlink = _head;
					_head->_llink = newNode;
					newNode->_llink = NULL;
					_head = newNode;
					insertAction = InsertHead;
					break;
				}
				else
				{

					tmp = tmp->_llink;
					if( tmp != _last )
					{
						newNode->_rlink = tmp->_rlink;
						tmp->_rlink->_llink = newNode;
						tmp->_rlink = newNode;
						newNode->_llink = tmp;
						insertAction = InsertMid;

					}
					else
					{
						this->AppendNode(newNode);
						insertAction = InsertTail;
					}
					break;
				}

			}
			//插尾
			else if(  tmp->_rlink == NULL)
			{
				this->AppendNode(newNode);
				insertAction = InsertTail;
				break;
			}
		}
	}
	this->_iTotalNum++;
	//this->TraceAllNode();
	return insertAction;
}


/** 拜訪某個ID節點*/
MyListNode* MyListController::visitID_Node(int ID )
{

	if( _curr != NULL && _curr->GetNo() == ID )
	{
	}
	else
	{
		_curr = _last;

		while ( _curr)
		{
			if( _curr->GetNo() == ID )
			{
				break;
			}
			else
			{
				_curr = _curr->_llink;
			}
		} 
	}

	if( !_curr )
		::OutputDebugString(_T("isn't find correct node\n"));
	return _curr;
}

MyListNode* MyListController::GetHead()
{
	return _head;
}

MyListNode* MyListController::GetLast()
{
	return _last;
}

/********************
* 清從所有佇列資料
*********************/
void MyListController::ClearAllListNode()
{
	MyListNode *deleteNode = _head;
	
	BOOL bDeleted;

	while( deleteNode != NULL )
	{
		bDeleted = this->DeleteNode(deleteNode);
		assert(bDeleted == TRUE);

		deleteNode = _head;
	}
	deleteNode = NULL;

	this->_iTotalNum = 0;
	this->_currentID = 0;
}

/*********************
* 尋訪所有節點
***********************/
void MyListController::TraceAllNode()
{
	MyListNode *curNode;

	OutputDebugString(_T("--------------------------------ListInfo-------------------------------------\n"));

	if (_head == NULL )
	{
		OutputDebugString(_T("LIST IS Null\n"));
	}
	else
	{
		curNode = _head;
		while( curNode != NULL)
		{
			//	OutputDebugString(_T("->\n"));
			if( curNode->GetnObjType() == IDM_WORD_REC)
			{
				OutputDebugString(_T("IDM_WORD_REC\n"));
				DebugMsgOutputHandle::Instance()->SetSzDebugBuffer(curNode->GetNo(), curNode->GetnObjType(), curNode->GetPtBeg().x ,curNode->GetPtBeg().y, curNode->GetPtEnd().x, curNode->GetPtEnd().y);
				OutputDebugString(DebugMsgOutputHandle::Instance()->GetSzDebugBuffer());

				curNode->TraceWordInfo();
			}
			else
			{
				OutputDebugString(_T("IDM_GRAPH\n"));
				DebugMsgOutputHandle::Instance()->SetSzDebugBuffer(curNode->GetNo(),curNode->GetnObjType(),curNode->GetPtBeg().x,curNode->GetPtBeg().y, curNode->GetPtEnd().x, curNode->GetPtEnd().y);
				OutputDebugString(DebugMsgOutputHandle::Instance()->GetSzDebugBuffer());
			}

			curNode = curNode->_rlink;
		}

		OutputDebugString(_T("--------------------------------ListInfo end-------------------------------------\n"));
	}
	curNode = NULL;

}

/************************
* 拷貝目前節點資訊
*************************/
void MyListController::CurrDataToRecordData(RecordObj *data)
{
	data->nObjType  = _curr->GetnObjType();
	data->ID        = _curr->GetNo();

	data->ptBeg     = _curr->GetPtBeg();
	data->ptEnd     = _curr->GetPtEnd();
	data->iFigBg    = _curr->GetFigBg();
	data->iPenWidth = _curr->GetPenWidth();
	data->iPenColor = _curr->GetPenColor();

	data->ptNewBeg     = data->ptBeg;
	data->ptNewEnd     = data->ptEnd;
	data->iNewFigBg    = data->iFigBg;
	data->iNewPenColor = data->iPenColor;
	data->iNewPenWidth = data->iPenWidth;
}

/***************************
* 最後一個節點是否為文字節點
****************************/
BOOL MyListController::IsLastTypeIsWordRec(int ID)
{
	if( _last->GetnObjType() == IDM_WORD_REC )
		return TRUE;
	else
		return FALSE;
}

/*****************************
* 此ID編號節點是否存在於節點中
******************************/
BOOL MyListController::IsID_NodeExit(int ID)
{
	BOOL bAnswer = FALSE;

	_curr = _last;

	while ( _curr)
	{
		if( _curr->GetNo() == ID )
		{
			bAnswer = TRUE;
			break;
		}
		else
		{
			_curr = _curr->_llink;
		}
	} 
	return bAnswer;
}

/***********************
* 是否為空佇列
************************/
BOOL MyListController::IsNull()
{
	if( _head == NULL )
		return TRUE;
	else
		return FALSE;
}

/*****************************
* 最後一個節點是否為空文字框節點
******************************/
BOOL MyListController::IsWordStart()
{
	if( _last == NULL )
		return FALSE;
	else if( _last->GetnObjType() == IDM_WORD_REC )
	{
		if( _last->GetWordNum() == 0 )
		{
			//空字串
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	
	}
	else
		return FALSE;
}

/*********************
* 目前節點個數
**********************/
int MyListController::GetTotalNum()
{
	return this->_iTotalNum;
}


/********************
* private function 
*********************/

/********************
* 加入節點(從尾巴加入)
**********************/
void MyListController::AppendNode(MyListNode *node)
{
	// 鏈結串列為空
	if (_head == NULL )
	{
		_head = node;
		_last = node;

		node->_rlink = NULL;
		node->_llink = NULL;
	}
	//插在尾
	else
	{
		if( _last != NULL) 
			_last->_rlink = node;

		node->_rlink = NULL;
		node->_llink = _last;
		_last = node;
	}

	_curr = _last;
	node = NULL;

}

/********************
* 刪除此節點
*********************/
bool MyListController::DeleteNode(MyListNode *node)
{
	bool answer = false;

	if( node != NULL )
	{
		answer = true;

		if( node == _head && node == _last )
		{
			_head->FreeObj();
			_last->FreeObj();
			node->FreeObj();

			_head = NULL;
			_last = NULL;
			delete node;
		}
		else if ( node == _head )
		{
			_head = _head->_rlink;
			_head->_llink = NULL;
			node->FreeObj();
			delete node;
		}
		else if( node == _last )
		{
			_last = _last->_llink;
			_last->_rlink = NULL;
			node->FreeObj();
			delete node;
		}
		else
		{
			node->_llink->_rlink = node->_rlink;
			node->_rlink->_llink = node->_llink;
			node->FreeObj();
			delete node;
		}
	}

	return answer;
}


/*************************
* 移除最後一個節點
**************************/
void MyListController::PopNode()
{
	MyListNode* tmp = _last;

	if( _last == _head )
	{
		_head->_rlink = NULL;
		_last->_llink = NULL;
		_head = NULL;
		_last = NULL;
		_curr = NULL;
	}
	else
	{
		_last = _last->_llink;
		_last->_rlink = NULL;
		_curr = _last;
	}

	tmp->FreeObj();

	if( tmp->GetnObjType() == IDM_WORD_REC) 
	{
		MyListNode* ptr = (MyListNode*) tmp;
		ptr->ClearAllNode();
		ptr = NULL;
	}

	delete tmp;
	tmp = NULL;

	_iTotalNum--;
}


int MyListController::GetCurrentPtrType()
{
	if( !_curr )
		OutputDebugString(_T("------_curr -null " ));
	return _curr->GetnObjType();
}

POINT MyListController::GetCurrentPtrPosBeg()
{
	if( !_curr )
		OutputDebugString(_T("------_curr -null " ));
	return _curr->GetPtBeg();
}

POINT MyListController::GetCurrentPtrPosEnd()
{
		if( !_curr )
		OutputDebugString(_T("------_curr -null " ));
	return _curr->GetPtEnd();
}

int MyListController::GetCurrentPenColor()
{
		if( !_curr )
		OutputDebugString(_T("------_curr -null " ));
	return _curr->GetPenColor();
}

int MyListController::GetCurrentPenWidth()
{
	if( !_curr )
		OutputDebugString(_T("------_curr -null " ));
	return _curr->GetPenWidth();
}

int MyListController::GetCurrentFigBg()
{
		if( !_curr )
		OutputDebugString(_T("------_curr -null " ));
	return _curr->GetFigBg();
}

int MyListController::GetCurrentNo()
{
	if( !_curr )
		OutputDebugString(_T("------_curr -null " ));
	return _curr->GetNo();
}