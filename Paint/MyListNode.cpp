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
* �[�J��r�`�I
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
* ���J��r�`�I
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
* ����r�ؤ��A��M�̫�@�Ӥ�r����m
* return: �U�Ӥ�r���_�l��m(�C�Ц�m) 
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
* ����r�ؤ��A��M�Ĥ@�Ӥ�r����m
* return: �ĤG�Ӥ�r���_�l��m(�C�Ц�m) 
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
* ����r�ؤ��A��M�ثe��r����m
* return: �U�Ӥ�r���_�l��m(�C�Ц�m) 
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
* ��o��r�ƶq
***************/
int MyListNode::GetWordNum()
{  
	return this->_iTotalWordNum;
}

/***************
* ���񦹤�r�`�I
****************/
void MyListNode::FreeObj()
{
	this->_rlink = NULL;
	this->_llink = NULL;
	this->ClearAllNode();
}

/*******************************
* ��V�䥪�A��ڤW��C�ާ@������
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
* ��V��k�A��ڤW��C�ާ@������
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
* ��V��W�A��ڤW��C�ާ@������
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
	

		//����ťB���Ц�m���b�}�Y
		if( this->_curr != NULL && this->_curr->_cha == '\r' && pt.x != this->_head->_pt.x )
		{
			caret = _curr->_pt;
			this->_curr = this->_curr->_llink;
		
		}
	}

	return caret;
}

/*******************************
* ��V��U�A��ڤW��C�ާ@������
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
* Home��A��ڤW��C�ާ@������
********************************/
POINT MyListNode::ShiftHomeArrChaIdx(POINT cursorPt)
{
	POINT caret;
	
	if( _head == NULL )
	{
		return cursorPt;
	}
	//�̤W�@��
	else if( cursorPt.y == _head->_pt.y )
	{
		_curr = NULL;
		this->FindIniWordPosInWordRec( caret );
		return caret;
	}
	//�̥�
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
* CTRL + Home��A��ڤW��C�ާ@������
*************************************/
POINT MyListNode::ShiftCtrlHomeArrChaIdx()
{
	this->_curr = NULL;

	POINT caret;
	this->FindIniWordPosInWordRec( caret );
	return caret;
}

/**���ЦV�U�� input :�ؼй��Ц�m*/
POINT MyListNode::ShiftEndArrChaIdx(POINT cursorPt)
{
	POINT caret;

	if( _last == NULL )
	{
		return cursorPt;
	}
	//�̫�@��
	else if(  cursorPt.y == _last->_pt.y )
	{
		_curr = _last;
		caret = this->FindLastWordPosInWordRec( caret );
		return caret;
	}
	//�̫�@��
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
				//���b������浲���e
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
* CTRL + END��A��ڤW��C�ާ@������
*************************************/
POINT MyListNode::ShiftCtrlEndArrChaIdx()
{
	this->_curr = this->_last;

	POINT caret = this->FindLastWordPosInWordRec( caret );
	return caret;
}


/*****************************************
* ����r�إثe���Ц�m�O�_�O���V�̫�@��(last)
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
/** �ק��r����ئ�m*/
void MyListNode::FixLastWordRecPosPt(POINT pt)
{
	this->SetPtEnd(pt.x,pt.y);
}

WordNodeInfo* MyListNode::GetHead()
{
	return this->_head;
}

/***********************
* �N�ثe���в��̫ܳ�@��
***********************/
void MyListNode::ShiftCurrPtrToLast()
{
	this->_curr = this->_last;
}

/*************************
* �]�w�ثe���Ъ����V
**************************/
void MyListNode::SetCurrWordPtr( WordNodeInfo* ptr )
{
	this->_curr = ptr;
}

/**************************
* ��o�ثe���Ъ����V
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
* �h����B�z
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
* �R����B�z
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
* �L�X����r�ؤ��t�Ҧ���r�`�I���
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
* �R������r�ةҧt�Ҧ���r����T
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

// �[�J��r�`�I�̫ܳ�
void MyListNode::AppendNode(WordNodeInfo *node)
{
	// �쵲��C����
	if (_head == NULL )
	{
		_head = node;
		_last = node;

		node->_rlink = NULL;
		node->_llink = NULL;
	}
	//���b��
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

//�R����r�`�I
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

//���J��r�`�I
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