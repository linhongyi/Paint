#include "StdAfx.h"
#include "MyListNode.h"
#include "ToolKits.h"
#include "CommonVar.h"
#include "SgWordDataModelControler.h"
#include "DebugMsgOutputHandle.h"
#include "PaintInfo.h"
#include <assert.h>
using namespace std;

extern PaintInfo gPaintInfo;

MyListNode::MyListNode(void)
{
	this->_head = NULL;
	this->_last = NULL;
	this->_curr = NULL;
	this->_rlink = NULL;
	this->_llink = NULL;
	this->_iTotalWordNum = 0;

}

MyListNode::~MyListNode(void)
{
}

/****************
* 加入文字節點
*****************/
void MyListNode::AppendWordNode(POINT pt1, TCHAR cha)
{
	WordNodeInfo *newNode = new WordNodeInfo();
	newNode->_cha = cha;
	newNode->_pt  = pt1;

	this->AppendNode( newNode );

	newNode = NULL;


	_iTotalWordNum++;
	//this->TraceWordInfo();
}

/****************
* 插入文字節點
*****************/
void MyListNode::InsertWordNode(POINT pt1, TCHAR cha)
{
	WordNodeInfo *newNode = new WordNodeInfo();
	newNode->_cha = cha;
	newNode->_pt  = pt1;

	this->InsertNode( newNode );

	newNode = NULL;
}

/***********************************
* 此文字框中，找尋最後一個文字的位置
* return: 下個文字的起始位置(遊標位置) 
**************************************/
POINT MyListNode::FindLastWordPosInWordRec(POINT &pt)
{
	POINT caretPt;

	if( _head == NULL )
	{
		caretPt = this->GetPtBeg();
		caretPt.x += CommonVar::WORD_REC_MARGIN;
		caretPt.y += CommonVar::WORD_REC_MARGIN;
	}
	else
	{
		pt = _last->_pt;
		caretPt = pt;

		if( _last->_cha == '\r' )
		{
			caretPt.x = _head->_pt.x;
			caretPt.y = pt.y + gPaintInfo.GetCTextOut()->GetCharY();;
		}
		else
		{
			caretPt.x += gPaintInfo.GetCTextOut()->GetCharX() + CommonVar::WORD_GAP;
		}
	}
	return caretPt;
}

/***********************************
* 此文字框中，找尋第一個文字的位置
* return: 第二個文字的起始位置(遊標位置) 
**************************************/
POINT MyListNode::FindIniWordPosInWordRec(POINT &pt)
{
	POINT caretPt;

	if( _head == NULL )
	{
		caretPt = this->GetPtBeg();
		caretPt.x += CommonVar::WORD_REC_MARGIN;
		caretPt.y += CommonVar::WORD_REC_MARGIN;
		pt = caretPt;
	}
	else
	{
		pt = _head->_pt;
		caretPt = pt;

		if( _head->_cha == '\r' )
		{
			caretPt.x = _head->_pt.x;
			caretPt.y = pt.y + gPaintInfo.GetCTextOut()->GetCharY();;
		}
		else
		{
			caretPt.x += gPaintInfo.GetCTextOut()->GetCharX() + CommonVar::WORD_GAP;
		}
	}
	return caretPt;
}

/***********************************
* 此文字框中，找尋目前文字的位置
* return: 下個文字的起始位置(遊標位置) 
**************************************/
POINT MyListNode::FindCurrWordPosInWordRec(POINT &pt)
{
	POINT caret;

	if( this->_curr == this->_head )
	{
		caret = this->FindIniWordPosInWordRec(pt);
	}
	else if( this->_curr == this->_last )
	{
		caret = this->FindLastWordPosInWordRec(pt);
	}
	else if( this->_curr == NULL )
	{
		caret = this->GetPtBeg();
		caret.x += CommonVar::WORD_REC_MARGIN;
		caret.y += CommonVar::WORD_REC_MARGIN;
		pt = caret;
	}
	else
	{
		pt = _curr->_pt;
		caret = pt;

		if( _curr->_cha == '\r' )
		{
			caret.x = this->GetPtBeg().x + CommonVar::WORD_REC_MARGIN;
			caret.y = pt.y + gPaintInfo.GetCTextOut()->GetCharY();;
		}
		else
		{
			caret.x += gPaintInfo.GetCTextOut()->GetCharX() + CommonVar::WORD_GAP;
		}
	}
	return caret;
}

/**************
* 獲得文字數量
***************/
int MyListNode::GetWordNum()
{  
	return this->_iTotalWordNum;
}

/***************
* 釋放此文字節點
****************/
void MyListNode::FreeObj()
{
	this->_rlink = NULL;
	this->_llink = NULL;
	this->ClearAllNode();
}

/*******************************
* 方向鍵左，實際上佇列操作的對應
********************************/
POINT MyListNode::ShiftLeftArrChaIdx()
{
	POINT caretPt;

	if( _curr == NULL )
	{
		caretPt = this->GetPtBeg();
		caretPt.x += CommonVar::WORD_REC_MARGIN;
		caretPt.y += CommonVar::WORD_REC_MARGIN;
	}
	else 
	{
		caretPt = _curr->_pt;
		_curr = _curr->_llink;
	
	}

	return caretPt;
}

/*******************************
* 方向鍵右，實際上佇列操作的對應
********************************/
POINT MyListNode::ShiftRightArrChaIdx()
{
	POINT caretPt;

	if( _head == NULL )
	{
		caretPt = this->GetPtBeg();
		_curr = NULL;
		
		caretPt.x += CommonVar::WORD_REC_MARGIN;
		caretPt.y += CommonVar::WORD_REC_MARGIN;
		
	}
	else if( _curr == NULL )
	{
		caretPt = this->GetPtBeg();
		_curr = _head;

		if( _curr->_cha == '\r')
		{
			caretPt.x += CommonVar::WORD_REC_MARGIN;
			caretPt.y += CommonVar::WORD_REC_MARGIN +gPaintInfo.GetCTextOut()->GetCharY();
		}
		else
		{
			caretPt.x += CommonVar::WORD_REC_MARGIN + gPaintInfo.GetCTextOut()->GetCharX() + CommonVar::WORD_GAP;
			caretPt.y += CommonVar::WORD_REC_MARGIN;
		}

	}
	else 
	{
		if( _curr->_rlink != NULL )
		{
			_curr = _curr->_rlink;
		
		}
		caretPt = _curr->_pt;

		if( _curr->_cha == '\r')
		{
			caretPt.x =  this->GetPtBeg().x + CommonVar::WORD_REC_MARGIN;
			caretPt.y += gPaintInfo.GetCTextOut()->GetCharY();;
		}
		else
		{
			caretPt.x += gPaintInfo.GetCTextOut()->GetCharX() + CommonVar::WORD_GAP;
		}
	}

	return caretPt;
}

/*******************************
* 方向鍵上，實際上佇列操作的對應
********************************/
POINT MyListNode::ShiftUpArrChaIdx(POINT pt)
{
	POINT caret = pt;

	// this->TraceWordInfo();

	while( this->_curr != NULL)
	{
		// hit
		if( _curr->_pt.y == pt.y 
			&& _curr->_pt.x < pt.x )
		{
			caret = _curr->_pt;

			if( _curr->_cha != '\r')
				caret.x += gPaintInfo.GetCTextOut()->GetCharX() + CommonVar::WORD_GAP;
			break;
		}
		else if( _curr->_pt.y < pt.y )
		{
			break;
		}
		caret = _curr->_pt;
		this->_curr = this->_curr->_llink;
	

		//跳行符且鼠標位置不在開頭
		if( this->_curr != NULL && this->_curr->_cha == '\r' && pt.x != this->_head->_pt.x )
		{
			caret = _curr->_pt;
			this->_curr = this->_curr->_llink;
		
		}
	}

	return caret;
}

/*******************************
* 方向鍵下，實際上佇列操作的對應
********************************/
POINT MyListNode::ShiftDownArrChaIdx(POINT pt)
{
	

	POINT caret;

	if( this->_curr == NULL )
		_curr = _head;


	while( this->_curr != NULL)
	{
		// hit
		if( _curr->_pt.y == pt.y && _curr->_pt.x == pt.x )
		{
			_curr = _curr->_llink;
			break;
		}
		else if( _curr->_pt.y == pt.y && _curr->_pt.x > pt.x )
		{
			_curr = _curr->_llink->_llink;
			break;
		}
		else if( _curr->_pt.y > pt.y )
		{
			_curr = _curr->_llink;

			if( _curr->_cha == '\r' )
				_curr = _curr->_llink;
			break;
		}

		this->_curr = this->_curr->_rlink;
	
	}

	if( _curr != NULL )
	{
		caret = this->FindCurrWordPosInWordRec( caret);
	}
	else
	{
		_curr = _last;

		caret = _curr->_pt; 
		if( _curr->_cha != '\r' )
		{
			caret.x += gPaintInfo.GetCTextOut()->GetCharX() + CommonVar::WORD_GAP;
		}
		else
		{
			caret.x = _head->_pt.x;
			caret.y +=  gPaintInfo.GetCTextOut()->GetCharY(); 
		}
	}

	return caret;
}

/*******************************
* Home鍵，實際上佇列操作的對應
********************************/
POINT MyListNode::ShiftHomeArrChaIdx(POINT cursorPt)
{
	POINT caret;
	
	if( _head == NULL )
	{
		return cursorPt;
	}
	//最上一行
	else if( cursorPt.y == _head->_pt.y )
	{
		_curr = NULL;
		this->FindIniWordPosInWordRec( caret );
		return caret;
	}
	//最左
	else if( cursorPt.x == _head->_pt.x )
	{
		caret = cursorPt;
		return caret;
	}
	else
	{
		if( _curr == NULL )
			_curr = _head;
		


		for( ; _curr != NULL ; _curr = _curr->_llink )
		{
			if( _curr->_cha == '\r' )
			{
				caret = this->FindCurrWordPosInWordRec(caret);

				break;
			}
		    else if( cursorPt.y == _curr->_pt.y )
			{
				this->FindCurrWordPosInWordRec(caret);
			}
			else
			{
				break;
			}
		}

		return caret;
	}
}

/************************************
* CTRL + Home鍵，實際上佇列操作的對應
*************************************/
POINT MyListNode::ShiftCtrlHomeArrChaIdx()
{
	this->_curr = NULL;

	POINT caret;
	this->FindIniWordPosInWordRec( caret );
	return caret;
}

/**鼠標向下移 input :目標鼠標位置*/
POINT MyListNode::ShiftEndArrChaIdx(POINT cursorPt)
{
	POINT caret;

	if( _last == NULL )
	{
		return cursorPt;
	}
	//最後一行
	else if(  cursorPt.y == _last->_pt.y )
	{
		_curr = _last;
		caret = this->FindLastWordPosInWordRec( caret );
		return caret;
	}
	//最後一行
	else if(  cursorPt.y > _last->_pt.y )
	{
		_curr = _last;
		return cursorPt;
	}
	else
	{
		if( _curr == NULL )
			_curr = _head;
		
		
		for( ; _curr != NULL ; _curr = _curr->_rlink )
		{
			if( _curr->_cha == '\r' )
			{
				//插在此行跳行結尾前
				if( _curr != _last && cursorPt.x != _head->_pt.x)
				{
					this->FindCurrWordPosInWordRec(caret);
					_curr = _curr->_llink;
					break;
				}
				else if( _curr != _last && _curr->_rlink->_pt.y != cursorPt.y )
				{
					this->FindCurrWordPosInWordRec(caret);
					_curr = _curr->_llink;
					break;
				}
				else if( _curr == _last) 
				{
					caret = this->FindCurrWordPosInWordRec(caret);
				}
			
			}
		    else if( cursorPt.y == _curr->_pt.y )
			{
				caret = this->FindCurrWordPosInWordRec(caret);
			}
			else
			{
				_curr = _curr->_llink;
				break;
			}
		}

		return caret;
	}
}

/************************************
* CTRL + END鍵，實際上佇列操作的對應
*************************************/
POINT MyListNode::ShiftCtrlEndArrChaIdx()
{
	this->_curr = this->_last;

	POINT caret = this->FindLastWordPosInWordRec( caret );
	return caret;
}


/*****************************************
* 此文字框目前指標位置是否是指向最後一個(last)
******************************************/
BOOL MyListNode::IsArrChaIdxBelongEnd()
{
	if( this->GetWordNum() <= 0 )
		return TRUE;
	else if( this->_curr == this->_last )
		return TRUE;
	else 
		return FALSE;
}
/** 修改文字選取框位置*/
void MyListNode::FixLastWordRecPosPt(POINT pt)
{
	this->SetPtEnd(pt.x,pt.y);
}

WordNodeInfo* MyListNode::GetHead()
{
	return this->_head;
}

/***********************
* 將目前指標移至最後一個
***********************/
void MyListNode::ShiftCurrPtrToLast()
{
	this->_curr = this->_last;
}

/*************************
* 設定目前指標的指向
**************************/
void MyListNode::SetCurrWordPtr( WordNodeInfo* ptr )
{
	this->_curr = ptr;
}

/**************************
* 獲得目前指標的指向
****************************/
WordNodeInfo* MyListNode::GetCurrWordPtr()
{
	if( this->_curr == NULL )
		return _head;
	else
		return this->_curr;
}

WordNodeInfo* MyListNode::GetLast()
{
	return this->_last;
}

/**********************
* 退位鍵處理
**********************/
POINT MyListNode::BackSpaceEvent()
{
	WordNodeInfo *deleteNode = _curr;

	POINT caretPt = _curr->_pt;

	_curr = _curr->_llink;

	BOOL foo = this->DeleteNode(deleteNode);

	assert( foo == TRUE );

	deleteNode = NULL;

	return caretPt;
}

/***************************
* 刪除鍵處理
****************************/
POINT MyListNode::DelEvent()
{
	assert( _curr != _last);

	POINT caretPt;
	WordNodeInfo *deleteNode;

	if( _curr != NULL )
	{  
		deleteNode = _curr;
		deleteNode = deleteNode->_rlink;
	}
	else
	{
		deleteNode = _head;
	}

	caretPt =deleteNode->_pt;

	BOOL foo = this->DeleteNode(deleteNode);

	assert( foo == TRUE );

	deleteNode = NULL;

	return caretPt;
}

/*****************************
* 印出此文字框內含所有文字節點資料
*****************************/
void MyListNode::TraceWordInfo()
{
	TCHAR cha[100];
	int size = this->GetWordNum();

	WordNodeInfo *ptr = _head;

	OutputDebugString(_T("---TraceWordInfo rlink\n"));
	for( int i = 0  ; i < size ; i++ ,ptr = ptr->_rlink )
	{
		cha[0] = ptr->_cha;
		cha[1] = '\0';
		OutputDebugString(cha);

		wsprintf(cha, _T(" (ascii: %d, pt1.x %d, sizeOfCha:%d )\n"),  ptr->_cha , ptr->_pt.x, sizeof(ptr->_cha ));
		OutputDebugString(cha);
	}

	OutputDebugString(_T("\n"));
}

/********************************
* 刪除此文字框所含所有文字的資訊
*********************************/
void MyListNode::ClearAllNode()
{
	WordNodeInfo *deleteNode;

	if (_head == NULL )
		return;
	else
	{
		while( _head != _last)
		{
			deleteNode = _head;
			_head = _head->_rlink;

			deleteNode->FreeObj();
			delete deleteNode;
		}

		delete _head;
		_head = NULL;
		_last = NULL;
	}
	this->_iTotalWordNum = 0;
	deleteNode = NULL;
}

/******************
*private function
*******************/

// 加入文字節點至最後
void MyListNode::AppendNode(WordNodeInfo *node)
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

	node = NULL;
	_curr = _last;
}

//刪除文字節點
bool MyListNode::DeleteNode(WordNodeInfo *node)
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

	if( answer == TRUE )
		_iTotalWordNum--;
	return answer;
}

//插入文字節點
void MyListNode::InsertNode(WordNodeInfo *node)
{
	if( _head == NULL )
	{
		this->AppendNode(node);
	}
	else if( _curr == NULL )
	{
		node->_rlink = _head;
		_head->_llink = node;
		node->_llink = NULL;
		_head = node;
	}
	else if( _curr == _last )
	{
		_last->_rlink = node;
		node->_llink = _last;
		node->_rlink = NULL;
		_last = node;
	}
	else
	{
		node->_rlink = _curr->_rlink;
		_curr->_rlink->_llink = node;
		_curr->_rlink = node;
		node->_llink = _curr;
	}
	_curr = node;
	_iTotalWordNum++;
}