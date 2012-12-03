#include "StdAfx.h"
#include "CTextOut.h"
#include "Resource.h"
#include "PenSetting.h"
#include "ToolKits.h"
#include "CommonVar.h"
#include "PaintInfo.h"
#include "DebugMsgOutputHandle.h"
#include <assert.h>
#include "AddRedoUndoPaintInfo.h"
#include "MyListController.h"
#include "MyListNode.h"
#include "WordNodeInfo.h"

extern AddRedoUndoPaintInfo gAddRedoUndoPaintInfo;

extern MyListController gMyList;
extern RedoUndoController *gRedoUndoController;
CTextOut::CTextOut(void)
{

	dwCharSet = DEFAULT_CHARSET;

	 IS_LOCK = FALSE;
	_IS_CLICK_CONTROL = FALSE;

	_hideCounter = 0;
	_showCounter = 0;

	this->ResetClipRgnPt();
}

CTextOut::~CTextOut(void)
{
}

// �p���r�j�p�A������ܰʮ�
void CTextOut::ResetCharSize()
{
	HDC hdc = GetDC(_parent->GetMainHwnd());
	TEXTMETRIC tm;

	//��o�r�鰪��
	if( GetTextMetrics(hdc,&tm) == FALSE )
	{
		MessageBox( _parent->GetMainHwnd() , _T("��l���ʦC�A�L�k��o�r�鰪��"), _T("ResetCharSize"), NULL );
	}
	else
	{
		_cxChar = tm.tmAveCharWidth;
		_cyChar = tm.tmHeight + tm.tmExternalLeading;
	}

	ReleaseDC( _parent->GetMainHwnd(), hdc);
}

void CTextOut::SetParent(PaintInfo *ptr)
{
	_parent = ptr;
}


void CTextOut::SetDwCharSet(int val)
{
	dwCharSet = val;
}


/**WM_KEYDOWN �B�z*/
void CTextOut::ClickKeyBoardEventHandle(WPARAM wParam, LPARAM lParam)
{
	if (IS_LOCK == FALSE )
		return;
	int _key = LOWORD(wParam);
	int ID   = _parent->GetHitID();
	int iLastID = gRedoUndoController->GetFinalID();
	int bAction  = FALSE;

	switch (_key )
	{
	case VK_DELETE:

		//�@�Ӥ�r��X�ꪺ�_�Y
		if(  gMyList.IsWordStart() == TRUE )
			return;

		//�W�Ӱʧ@���@��Action�A�ҥH�n�N���ɪ���r��T���O��
		if( _parent->GetAction() == TRUE )
		{
			//�]�w���~�A�i��ACTION�������A
			bAction = TRUE;
			_parent->SetAction( FALSE );
			SgWordDataModelControler::Instance()->SaveAllWordIniInThisWordRec(ID);
		}
		//�ثe�s��ID�A����LOG�������̫�@��ID�A�h�����@�ӷs���ʧ@
		else if( iLastID != ID )
		{
			bAction = TRUE;
			SgWordDataModelControler::Instance()->SaveAllWordIniInThisWordRec(ID);
		}

		if( this->IsInverseWordScope() == TRUE )
		{
			this->InverseWordHandle(bAction);
			// ���صe��
		    this->RenewNowCurScope();
			this->_parent->GetDrawTool()->ShowTarGetRecForTypingWord();
		}
		else
		{
			this->DeleteHandle(bAction);
		}
		break;
	case VK_LEFT:
		this->VkLeftHandle();
		break;
	case VK_UP:
		this->VkUpHandle();
		break;
	case VK_DOWN:
		this->VkDownHandle();  
		break;
	case VK_RIGHT:
		this->VkRightHandle();
		break;
	case VK_END:
		this->VkEndHandle();
		break;
	case VK_HOME:
		this->VkHomeHandle();
		break;
	case VK_CONTROL:
		this->_IS_CLICK_CONTROL = TRUE;
		break;
	}
	this->_parent->GetDrawTool()->MapDCToMonitor();
}

/**WM_KEYUP �B�z*/
void CTextOut::ReleaseKeyBoardEventHandle(WPARAM wParam, LPARAM lParam)
{
	int _key = LOWORD(wParam);

	switch (_key )
	{
	case VK_CONTROL:
		this->_IS_CLICK_CONTROL = FALSE;
		break;
	}

}

/** WM_CHAR �B�z*/
void CTextOut::NoSystemClickKeyBoardEventHandle(WPARAM wParam, LPARAM lParam)
{
	int ID = _parent->GetHitID();
    int iLastCmd = gRedoUndoController->GetLastCmd();
    int iLastID  = gRedoUndoController->GetFinalID();
    int bAction  = FALSE;

	//�W�Ӱʧ@���@��Action�A�ҥH�n�N���ɪ���r��T���O��
	if( _parent->GetAction() == TRUE )
	{
		bAction = TRUE;
		//�]�w�����@��ACTION�A�i��ACTION�������A
		_parent->SetAction( FALSE );
		SgWordDataModelControler::Instance()->SaveAllWordIniInThisWordRec(ID);
	}
	//�ثe�s��ID�A����LOG�������̫�@��ID�A�h�����@�ӷs���ʧ@
	else if( iLastID != ID )
	{
		bAction = TRUE;
		SgWordDataModelControler::Instance()->SaveAllWordIniInThisWordRec(ID);
	}

	switch( wParam )
	{
	case '\b':
		if (IS_LOCK == FALSE )
			return;
		// �O�_���@�Ӥ�r��X�ꪺ�_�Y
		else if(  gMyList.IsWordStart() == TRUE )
			return;
		else if( this->IsInverseWordScope() == TRUE )
		{
			this->InverseWordHandle(bAction);
			// ���صe��
		    this->RenewNowCurScope();

			this->_parent->GetDrawTool()->ShowTarGetRecForTypingWord();
		}
		else
		{
			this->BackSpaceHandle(bAction);
	
		}
		break;
	case '\r':
		//OutputDebugString(_T("VK_RETURN\n"));

		if (this->IsLastLine() == TRUE )
			return;
		else if (IS_LOCK == FALSE )
			return;

		if( this->IsInverseWordScope() == TRUE )
		{
			this->InverseWordHandle(bAction);
			bAction = FALSE;
		}

		this->CaretHide();
		this->ReturnKeyHandle();
		this->CaretShow();

		// ���صe��
		this->RenewNowCurScope();

		_parent->GetDrawTool()->ShowTarGetRecForTypingWord();       
      
        iLastID  = gRedoUndoController->GetFinalID();
		iLastCmd = gRedoUndoController->GetLastCmd();
         // �����ª���r�s��
	    if( iLastCmd == COMMAND_EDIT_WORD && ID == iLastID && bAction == FALSE )
		{
		    gRedoUndoController->PopFinalWord();
		}
	    gAddRedoUndoPaintInfo.AddEditWordNodeInfo( ID);
       	
		break;
	default:
		if (this->IsLastLine() == TRUE && this->IsXLineEnd() == TRUE && this->IsYLineEnd() == TRUE)
			return;
		else if (IS_LOCK == FALSE )
			return;

        BOOL bInverseWord = this->IsInverseWordScope();

		if( bInverseWord == TRUE )
		{
			this->InverseWordHandle(bAction);
			bAction = FALSE;
		}
	
		this->CaretHide();
		this->Type_WordHandle(_parent->GetHitID(),(TCHAR) wParam,_parent->GetColorSelection(), _parent->GetFigBg());
		this->CaretShow();

		// ���صe��
		this->RenewNowCurScope();

		_parent->GetDrawTool()->ShowTarGetRecForTypingWord();

        iLastID  = gRedoUndoController->GetFinalID();
		iLastCmd = gRedoUndoController->GetLastCmd();
         // �����ª���r�s��
	    if( iLastCmd == COMMAND_EDIT_WORD && ID == iLastID && bAction == FALSE )
		{
		    gRedoUndoController->PopFinalWord();
		}
	    gAddRedoUndoPaintInfo.AddEditWordNodeInfo( ID);
		
		//gMyList.TraceAllNode();
		break;
	}
}

/******************
* ���r�ɡA��r���B�z
*******************/
void CTextOut::Type_WordHandle(int ID , TCHAR cha, int penColor, int bgColor)
{
	MyListNode* wordPtr =  gMyList.visitID_Node( ID );

	POINT recPt1,recPt2;

	recPt1 = gMyList.GetCurrentPtrPosBeg();
	_parent->GetDrawTool()->ScreenToWindowCoordinate( recPt1.x, recPt1.y );
	recPt2 = gMyList.GetCurrentPtrPosEnd();
	_parent->GetDrawTool()->ScreenToWindowCoordinate( recPt2.x, recPt2.y );

	POINT wordPt;
	POINT caretPt;
	::GetCaretPos( & caretPt );

	if( caretPt.x + _cxChar >  recPt2.x - CommonVar::WORD_REC_MARGIN )
	{
		wordPt.x = recPt1.x+ CommonVar::WORD_REC_MARGIN;
		wordPt.y = caretPt.y+ this->GetCharY();
	}
	else
	{
		wordPt.x = caretPt.x;
		wordPt.y = caretPt.y;
	}
	this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos( wordPt.x, wordPt.y );

	caretPt.x = wordPt.x + _cxChar + CommonVar::WORD_GAP;
	caretPt.y = wordPt.y;


	if( gMyList.IsLastTypeIsWordRec( ID ) == TRUE 
		&& wordPtr->IsArrChaIdxBelongEnd() == TRUE)  
	{
		wordPtr->AppendWordNode(wordPt,cha);
		TextWordToDC( _parent->GetMemDC(),wordPt.x ,wordPt.y,&cha,penColor, bgColor);
	}
	else
	{
		wordPtr->InsertWordNode(wordPt,cha);
		this->WordReputationForInsertingWord( ID );
	
	}

	// addWordRec
	this->IsAddWordRecForWordContent( ID );

	this->_parent->GetDrawTool()->ScreenToWindowCoordinate( caretPt.x , caretPt.y );
	this->CaretSetPos(caretPt);

	
	wordPtr = NULL;
}

/*******************
* ���J��ܲŸ�������
********************/
void CTextOut::CaretShow()
{ 

	if ( IS_LOCK == FALSE )
	{
		BOOL answer = ::ShowCaret( _parent->GetMainHwnd() );

		if( answer == FALSE )
		{
			//::HideCaret( _parent->GetMainHwnd() );
			::DestroyCaret();
			// create Caret
			::CreateCaret(_parent->GetMainHwnd(),NULL,1,this->GetCharY());

			answer = ::HideCaret( _parent->GetMainHwnd() );
			assert( answer == TRUE );
			answer = ::ShowCaret( _parent->GetMainHwnd() );
			assert( answer == TRUE );
		}

		IS_LOCK = TRUE;
		_showCounter ++;
		assert( _showCounter == _hideCounter+1);
	}
}

/*******************
* �]�w��ܲŸ�����m
********************/
void CTextOut::CaretSetPos(POINT pt)
{
	BOOL answer = ::SetCaretPos(pt.x,pt.y);
	if( answer == FALSE )
	{
		DebugMsgOutputHandle::Instance()->ErrMsg();
		::DestroyCaret();
		// create Caret
		::CreateCaret(_parent->GetMainHwnd(),NULL,1,this->GetCharY());
	
		answer = ::SetCaretPos( pt.x,pt.y );
		assert( answer == TRUE );

	}
}

/*******************
* ������ܲŸ�������
********************/
void CTextOut::CaretHide()
{
	if( IS_LOCK == TRUE )
	{
		BOOL answer = ::HideCaret( _parent->GetMainHwnd() );
	
		if( answer == FALSE )
		{
			//::ShowCaret( _parent->GetMainHwnd() );
			DebugMsgOutputHandle::Instance()->ErrMsg();
			::DestroyCaret();
			// create Caret
			::CreateCaret(_parent->GetMainHwnd(),NULL,1,this->GetCharY());

			answer = ::ShowCaret( _parent->GetMainHwnd() );
			assert( answer == TRUE );
			answer = ::HideCaret( _parent->GetMainHwnd() );
			assert( answer == TRUE );

		}

		IS_LOCK = FALSE;
		_hideCounter ++;
		assert( _showCounter == _hideCounter);
	}
}

/*************************************
* ���r���J�ɤ�r���ܰ�(�q�Y�}�l�p��)
**************************************/
void CTextOut::WordReputationForInsertingWord(int ID)
{
	MyListNode *ptr =  gMyList.visitID_Node(ID);

	POINT pt,pt1,pt2;
	pt1.x = ptr->GetPtBeg().x + CommonVar::WORD_REC_MARGIN;
	pt1.y = ptr->GetPtBeg().y + CommonVar::WORD_REC_MARGIN;
	pt2.x = ptr->GetPtEnd().x;
	pt2.y = ptr->GetPtEnd().y;

	ToolKits::Normalization(pt1,pt2);
	pt = pt1;

	int xChar;

	//�]�w�_�l�r��m
	WordNodeInfo *wordPtr = ptr->GetHead();

	while( wordPtr != NULL )
	{
		///�M�w�e��
		if( wordPtr->_cha == '\r' )
		{
			xChar = 0;
		}
		else
		{
			xChar = this->GetCharX();
		}

		if( pt.x + xChar > pt2.x - CommonVar::WORD_REC_MARGIN )
		{
			pt.x = pt1.x;
			pt.y += this->GetCharY();
		}

		wordPtr->_pt = pt;

		//�p��U�@�Ӥ�r�_�l�I		
		if( wordPtr->_cha == '\r' )
		{
			pt.x = pt1.x;
			pt.y += this->GetCharY();
		}
		else
		{
			pt.x += xChar + CommonVar::WORD_GAP;
		}

		wordPtr = wordPtr->_rlink;
	}

	ptr = NULL;
	wordPtr = NULL;
}


/** �]�w��Ц�m*/
void CTextOut::SetNearCaretPosInClickingMouse(int &x , int &y)
{
	if( gMyList.GetTotalNum() == 0 )
		return;

	//	this->CaretHide();
	int HitIndex = _parent->GetHitID();

	// �w�]�O��r����ت��_�I
	MyListNode *ptr = gMyList.visitID_Node( HitIndex );

	POINT pt;
	pt = ptr->FindLastWordPosInWordRec(pt);
	_parent->GetDrawTool()->ScreenToWindowCoordinate( pt.x, pt.y );

	//�̫�@��
	if( x >= pt.x && y >=pt.y )
	{
		x = pt.x;
	    y = pt.y;
		ptr->ShiftCurrPtrToLast();
	}
	else
	{
		WordNodeInfo* wordPtr = ptr->GetHead();

		while ( wordPtr != NULL )
		{
			pt = wordPtr->_pt;

			_parent->GetDrawTool()->ScreenToWindowCoordinate( pt.x, pt.y );

			if(  abs (pt.x - x) < _cxChar &&  abs (pt.y - y) < _cyChar)
			{
				ptr->SetCurrWordPtr( wordPtr->_llink );
				break;
			}
			else if(  pt.y > y )
			{
				if( wordPtr->_llink != NULL )
				{
					wordPtr = wordPtr->_llink;
					pt = wordPtr->_pt;
					ptr->SetCurrWordPtr(  wordPtr->_llink );
				}
				else
				{
					ptr->SetCurrWordPtr(  wordPtr->_llink );
				}
			
				break;
			}

			wordPtr = wordPtr->_rlink;
		}
		wordPtr = NULL;

	    x = pt.x;
	    y = pt.y;
	}
	ptr = NULL;
}


/********************************
* �R����r�ɡA��r��m�ܰʪ��B�z
*********************************/
void CTextOut::DeleteWordTokenForWordReputation(POINT pt)
{
	
	int hitID =  _parent->GetHitID();
	MyListNode *ptr = gMyList.visitID_Node(hitID);

	if( ptr->GetWordNum() <= 0)
	{
		return;
	}


	POINT pt1, pt2;
	pt1 = ptr->GetPtBeg();
	pt2 = ptr->GetPtEnd();
	ToolKits::Normalization(pt1,pt2);

	pt1.x += CommonVar::WORD_REC_MARGIN;
	pt1.y += CommonVar::WORD_REC_MARGIN;
	this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos(pt.x,pt.y);


	WordNodeInfo* wordPtr = ptr->GetCurrWordPtr();
	
	if( wordPtr == NULL )
	{
		wordPtr = ptr->GetHead();
	}
	else if( wordPtr == ptr->GetHead() )
	{
	}
	else if( wordPtr == ptr->GetLast() )
	{
		ptr = NULL;	
		wordPtr = NULL;
		return ;
	}
	else
	{
		wordPtr = wordPtr->_rlink;
	}

	POINT queuePt = pt;
	
	int xChar;

	while( wordPtr != NULL )
	{
		if( wordPtr->_pt.x < pt.x && wordPtr->_pt.y == pt.y )
		{
			wordPtr = wordPtr->_rlink;
			continue;
		}

		///�M�w�e��
		if( wordPtr->_cha == '\r' )
		{
			xChar = 0;
		}
		else
		{
			xChar = this->GetCharX();
		}

		if( pt.x + xChar  > pt2.x - CommonVar::WORD_REC_MARGIN  )
		{
			pt.x = pt1.x;
			pt.y += this->GetCharY();
		}

		wordPtr->_pt = pt;

		//�p��U�@�Ӥ�r�_�l�I		
		if( wordPtr->_cha == '\r' )
		{
			pt.x = pt1.x;
			pt.y += this->GetCharY();
		}
		else
		{
			pt.x += xChar + CommonVar::WORD_GAP;
		}

		wordPtr = wordPtr->_rlink;
	}

	wordPtr = NULL;
	ptr = NULL;	

}


/**********************
* Getter // Setter
***********************/
int CTextOut::GetCharX()
{
	return this->_cxChar;
}

int CTextOut::GetCharY()
{
	return this->_cyChar;
}

//�ϥզr��X
void CTextOut::InverseTextWordToDC(HDC& copyMemHDC)
{
	int ID = _parent->GetHitID();

	//�ˬd�ϥսd��
	POINT inverseWordPt1 = _parent->GetCInverseWordInfo().GetCaretPt1();
	POINT inverseWordPt2 = _parent->GetCInverseWordInfo().GetCaretPt2();
	if( inverseWordPt1.x == inverseWordPt2.x && inverseWordPt1.y == inverseWordPt2.y)
		return ;

	//���W�ơA�����W�k�U�C
	ToolKits::InverseWordNormalization( inverseWordPt1, inverseWordPt2);

	LONG wordStreamPosX = inverseWordPt1.x;

	POINT wordIniPt;
	POINT wordEndPt;

	int nCount = 0;

	MyListNode *ptr = gMyList.visitID_Node(ID);
	WordNodeInfo *wordPtr = ptr->GetHead();

	//�]�w clip region �d��
	POINT pt1 = ptr->GetPtBeg();
	POINT pt2 = ptr->GetPtEnd();
	ToolKits::Normalization( pt1, pt2);
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate(pt1.x,pt1.y);
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate(pt2.x,pt2.y);
	this->_clipRgnPt1 = pt1;
	this->_clipRgnPt2 = pt2;
	
	//�ϥզr�Ȧs
	TCHAR desTmp[CommonVar::MAX_WORD] = {'\0'};
	

	for( ;wordPtr != NULL ; wordPtr = wordPtr->_rlink )
	{
		wordIniPt = wordPtr->_pt;
		wordEndPt = wordIniPt;

		wordEndPt.x += this->GetCharX();
		wordEndPt.y += this->GetCharY();

		//�����~
		if( this->IsTheWordInTheWindowView( wordIniPt , wordEndPt ) == FALSE )
		{
			continue;
		}
		//������
		else
		{
			nCount = 0;
			wordStreamPosX = CommonVar::DRAW_MAP_WIDTH;

			//�ˬd�����������r
			while( wordIniPt.y == wordPtr->_pt.y )
			{
				//�ϥզr
				if( this->_parent->GetCTextOut()->IsTheWordInTheInverseWord(wordPtr->_pt,inverseWordPt1,inverseWordPt2) == TRUE )
				{
					
					if( wordStreamPosX >= wordPtr->_pt.x )
						wordStreamPosX = wordPtr->_pt.x;

					//�D����
					if( wordPtr->_cha != '\r' )
					{
						desTmp[nCount++] = wordPtr->_cha ;
					}
					//����
					else
					{
						break;  
					}
				}

				if( wordPtr->_rlink != NULL && wordPtr->_rlink->_pt.y == wordIniPt.y )
				{
					wordPtr = wordPtr->_rlink;
				}
				else
				{
					break;  
				}

			}

			//��r��X
			desTmp[nCount]   = '\0';


			if(  ::_tcslen(desTmp) > 0)
			{
				
				//::OutputDebugString(desTmp);
				this->_parent->GetDrawTool()->ScreenToWindowCoordinate(wordStreamPosX,wordIniPt.y);
				this->_parent->GetCTextOut()->TextWordToDC(copyMemHDC
					,wordStreamPosX
					,wordIniPt.y
					,desTmp
					,IDM_WHITE
					,IDM_BG_BLACK);
			}
		
		}
	}

}

/******************
* ��r��X
*******************/
void CTextOut::TextWordToDC(HDC& copyMemHDC,int posX,int posY, TCHAR cha[], int penColor, int bgColor)
{
	int nlen = _tcslen(cha);
	if( nlen <= 0 )
		return;

	int saveDC = SaveDC(copyMemHDC);

	HFONT hFont = CreateFont(this->GetCharY(),this->GetCharX()+1,0,0,0,0,0,0,dwCharSet,0,0,0,FIXED_PITCH,NULL);
	HRGN recRgn = ::CreateRectRgn( this->_clipRgnPt1.x , this->_clipRgnPt1.y , this->_clipRgnPt2.x , this->_clipRgnPt2.y );

	
	SelectObject(copyMemHDC,hFont);
	SetTextColor(copyMemHDC, PenSetting::TransferIDToPenColor(  penColor ));
    SelectClipRgn(copyMemHDC, recRgn );

	COLORREF bgFig = PenSetting::TransferIDToBg( bgColor );

	

	if( bgFig != CommonVar::BG_TRANSPARENT )
	{
		//::OutputDebugString(_T(" �ϥզr  \n"));
		SetBkColor(copyMemHDC,bgFig );
		TextOut(copyMemHDC,posX,posY,cha,nlen);
	    RestoreDC(copyMemHDC,saveDC);
	}
	else
	{
		//::SetBkColor(copyMemHDC,RGB(255,255,255) );
		//::OutputDebugString(_T(" �D�ϥզr  \n"));
		::SetBkMode(copyMemHDC,TRANSPARENT );
		TextOut(copyMemHDC,posX,posY,cha,nlen);
	    RestoreDC(copyMemHDC,saveDC);
		
	}

	
	DeleteObject( hFont );
	
	::DeleteObject(	recRgn );
}

/**********************
* �@����r��X
************************/
void CTextOut::TextLineWordToDcInWindowView(MyListNode *ptr,HDC &copyMemHDC,POINT clipRgnPt1, POINT clipRgnPt2)
{
	this->_clipRgnPt1 = clipRgnPt1;
	this->_clipRgnPt2 = clipRgnPt2;

	WordNodeInfo *wordPtr = ptr->GetHead();
	POINT ptStart, ptEnd;
	int chaIdx;
	TCHAR cha[CommonVar::MAX_WORD];
	int nlen;

	for( ; wordPtr != NULL;  wordPtr = wordPtr->_rlink )
	{
		ptStart   = wordPtr->_pt;

		ptEnd.x   = ptStart.x + this->GetCharX();
		ptEnd.y   = ptStart.y + this->GetCharY();

		
		chaIdx = 0;

		while( ptStart.y == wordPtr->_pt.y )
		{
			if( wordPtr->_cha != '\r' )
			{
				cha[chaIdx++] = wordPtr->_cha  ;

				//end
				if( wordPtr->_rlink != NULL && wordPtr->_rlink->_pt.y == ptStart.y  )
				{
					wordPtr = wordPtr->_rlink;
				}				
				else
				{
					break;
				}
			}
			else
			{
				break;  
			}
		}

		cha[chaIdx]   = '\0';

		nlen = ::_tcslen(cha);
		if( nlen > 0)
		{  
			this->_parent->GetCTextOut()->TextWordToDC(_parent->GetMemDC()
				,ptStart.x
				,ptStart.y
				,cha
				,ptr->GetPenColor()
				,ptr->GetFigBg());
		}

	}
	
}

/************************
* ����r�O�_���b�����d��
************************/
BOOL CTextOut::IsTheWordInTheWindowView(POINT ptStart,POINT ptEnd)
{
	// �����d��
	POINT windowStartPt, windowEndPt;
	windowStartPt.x = this->_parent->GetScrollBarHandle()->GetNowScrollBarPos().x;
	windowStartPt.y = this->_parent->GetScrollBarHandle()->GetNowScrollBarPos().y;
	windowEndPt.x   = windowStartPt.x + _parent->GetClientWidth();
	windowEndPt.y   = windowStartPt.y + _parent->GetClientHeight();

	//�����~�C
	if( this->_parent->GetCHitTest()->RegionHitRegion(ptStart,ptEnd,windowStartPt,windowEndPt) == FALSE )
	{   
		return FALSE;
	}
	else 
		return TRUE;

}
/************************
*  Private function
************************/
BOOL CTextOut::IsLastLine()
{
	int ID = _parent->GetHitID();
	MyListNode *ptr= gMyList.visitID_Node(ID);

	POINT pt = ptr->GetPtEnd();

	// �U��
	if(  pt.y + _cyChar >= CommonVar::DRAW_MAP_HEIGHT - CommonVar::WORD_REC_MARGIN )
	{
		return TRUE;
	}
	else 
		return FALSE;
}

/************************
*  x��V�O�_�w�F����
*************************/
BOOL CTextOut::IsXLineEnd()
{
	int ID = _parent->GetHitID();
	MyListNode *ptr = gMyList.visitID_Node(ID);

	POINT recPt2;
	recPt2 = gMyList.GetCurrentPtrPosEnd();

	POINT pt = ptr->FindLastWordPosInWordRec(pt);

	if( pt.x + this->_cxChar  > recPt2.x - CommonVar::WORD_REC_MARGIN )
	{
		//OutputDebugString(_T("--------------IsXLineEnd ok\n"));
		return TRUE;
	}
	else 
	{
		//OutputDebugString(_T("--------------IsXLineEnd no\n"));
		return FALSE;
	}
}

/******************
* y��V�O�_�w�F����
*******************/
BOOL CTextOut::IsYLineEnd()
{
	int ID = _parent->GetHitID();
	MyListNode *ptr = gMyList.visitID_Node(ID);

	POINT recPt2;
	recPt2 = gMyList.GetCurrentPtrPosEnd();

	POINT pt = ptr->FindLastWordPosInWordRec(pt);

	if( pt.y + 2*this->_cyChar >= recPt2.y - CommonVar::WORD_REC_MARGIN )
	{
		//OutputDebugString(_T("--------------IsYLineEnd ok\n"));
		return TRUE;
	}
	else 
	{
		//OutputDebugString(_T("--------------IsYLineEnd no\n"));
		return FALSE;
	}
}

/******************
* ���] clipRgn
*******************/
void CTextOut::ResetClipRgnPt()
{
	this->_clipRgnPt1.x = this->_clipRgnPt1.y = 0;
	this->_clipRgnPt2.x = CommonVar::DRAW_MAP_WIDTH;
	this->_clipRgnPt2.y = CommonVar::DRAW_MAP_HEIGHT;
}

/***********************************
* �w��ثe���r��Poys�A�i���r�ح�ø 
************************************/
void CTextOut::RenewNowCurScope()
{
	int ID = _parent->GetHitID();

	MyListNode* ptr = gMyList.visitID_Node( ID );


	POINT recPt1, recPt2;
	recPt1 = ptr->GetPtBeg();
	recPt2 = ptr->GetPtEnd();

	POINT caretPt;
	::GetCaretPos( &caretPt );

	this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos( caretPt.x ,caretPt.y);
	recPt1.y = caretPt.y;

	if( recPt1.y - this->GetCharY() > 0 )
		recPt1.y -= this->GetCharY();
	/**���*/
	_parent->GetDrawTool()->FillWhileRec( recPt1 ,recPt2  );
	_parent->GetDrawTool()->RePaintWithClipRgn( recPt1, recPt2);
}


//�B�z�R��
void CTextOut::BackSpaceHandle(BOOL bAction)
{
	MyListNode* ptr  = gMyList.visitID_Node(_parent->GetHitID());

	POINT pt;
	::GetCaretPos(&pt);
	this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos(pt.x,pt.y);

	POINT iniPt;
	ptr->FindIniWordPosInWordRec( iniPt );

	//�w�F���W���A����i��R���ʧ@
	if( pt.x <= iniPt.x && pt.y <= iniPt.y )
		return;

	POINT lastWordPt = ptr->BackSpaceEvent();
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate( lastWordPt.x,lastWordPt.y);
	
	this->CaretHide();
	
	this->DeleteWordTokenForWordReputation(lastWordPt);

	this->CaretSetPos(lastWordPt);  
	this->CaretShow();

	// ���صe��
    this->RenewNowCurScope();
	_parent->GetDrawTool()->ShowTarGetRecForTypingWord();

	int iLastCmd = gRedoUndoController->GetLastCmd();
	int ID       = _parent->GetHitID();
	int iLastID  = gRedoUndoController->GetFinalID();
	// �����ª���r�s��
	if( iLastCmd == COMMAND_EDIT_WORD && ID == iLastID && bAction == FALSE)
		gRedoUndoController->PopFinalWord();

	gAddRedoUndoPaintInfo.AddEditWordNodeInfo( ID);

}

/*****************
* �O�_�s�b�ϥսd��
*****************/
BOOL CTextOut::IsInverseWordScope()
{
	POINT caretPt1 = _parent->GetCInverseWordInfo().GetCaretPt1();
	POINT caretPt2 = _parent->GetCInverseWordInfo().GetCaretPt2();

	if( caretPt1.x == caretPt2.x && caretPt1.y == caretPt2.y)
		return FALSE;
	else
		return TRUE;
}

/******************
* ����r�O�_�O�ϥզr
*******************/
BOOL CTextOut::IsTheWordInTheInverseWord(POINT candicateWordPt,POINT inverseWordStartPt, POINT inverseWordEndPt)
{
	BOOL bIsInverseWord = FALSE;

	ToolKits::InverseWordNormalization( inverseWordStartPt, inverseWordEndPt);

	//���
	if( inverseWordStartPt.y == inverseWordEndPt.y)
	{
		if(  candicateWordPt.y == inverseWordStartPt.y  && candicateWordPt.x >= inverseWordStartPt.x  && candicateWordPt.x < inverseWordEndPt.x )
		{
			bIsInverseWord = TRUE;
		}
	}
	//�h��
	else
	{
		if(  candicateWordPt.y == inverseWordStartPt.y  && candicateWordPt.x >= inverseWordStartPt.x)
		{
			bIsInverseWord = TRUE;
		}
		else if( candicateWordPt.y == inverseWordEndPt.y && candicateWordPt.x < inverseWordEndPt.x)
		{
			bIsInverseWord = TRUE;
		}
		else if( candicateWordPt.y > inverseWordStartPt.y && candicateWordPt.y < inverseWordEndPt.y)
		{
			bIsInverseWord = TRUE;
		}	
	}
	return bIsInverseWord;
}
//�B�z�ϥ�
void CTextOut::InverseWordHandle(BOOL isAction)
{
	//�ϥդ�r���
	int ID = _parent->GetHitID();

	POINT wordPt;
	POINT inverseWordPt1 = _parent->GetCInverseWordInfo().GetCaretPt1();
	POINT inverseWordPt2 = _parent->GetCInverseWordInfo().GetCaretPt2();
	ToolKits::InverseWordNormalization( inverseWordPt1 , inverseWordPt2);
	

	MyListNode *ptr = gMyList.visitID_Node( ID );
	WordNodeInfo *wordPtr = ptr->GetHead();
	WordNodeInfo *delNode;
	bool bDeleted;
  
   
	//�䴡�J�I
	while( wordPtr != NULL ) 
	{
		wordPt = wordPtr->_pt;

		////�]�w�ثe���Ы��V
		if( wordPtr->_pt.x == inverseWordPt1.x && wordPtr->_pt.y == inverseWordPt1.y )
		{
			if( wordPtr->_llink != NULL )
				ptr->SetCurrWordPtr(wordPtr->_llink);
			else
			    ptr->SetCurrWordPtr(NULL);
        }
		if ( this->IsTheWordInTheInverseWord( wordPt,inverseWordPt1,inverseWordPt2 ) == TRUE )
		{
			delNode = wordPtr;
			wordPtr = wordPtr->_rlink;

			bDeleted = ptr->DeleteNode( delNode );
			assert( bDeleted == true );
		}
		else
		{
			wordPtr = wordPtr->_rlink;
		}

	}

	POINT caretPt1 = _parent->GetCInverseWordInfo().GetCaretPt1();
	POINT caretPt2 = _parent->GetCInverseWordInfo().GetCaretPt2();
	ToolKits::InverseWordNormalization( caretPt1 , caretPt2);

	this->_parent->GetDrawTool()->ScreenToWindowCoordinate( caretPt1.x,caretPt1.y);
	

	if( ptr->GetWordNum() > 0 )
	{
	    //�R���r������
	    this->DeleteWordTokenForWordReputation(caretPt1);
    }

	int iLastCmd = gRedoUndoController->GetLastCmd();
	int iLastID  = gRedoUndoController->GetFinalID();

	// �����ª���r�s��
	if( iLastCmd == COMMAND_EDIT_WORD && ID == iLastID && isAction == FALSE )
	{
		gRedoUndoController->PopFinalWord();
	}
	gAddRedoUndoPaintInfo.AddEditWordNodeInfo( ID);
    //�ƹ���m
	this->CaretSetPos(caretPt1);
	//�����ϥ�
	_parent->GetCInverseWordInfo().ResetInverseWordScope();

}


/**********************************************************
* ���r���ܰʤj�p�ɡA�P�_�O�_�i�H�]�t�ثe��r�ؤ��Ҧ���r
***********************************************************/
BOOL CTextOut::IsChangeThisWordRecScope(int ID, POINT pt1, POINT pt2)
{
	MyListNode *ptr =  gMyList.visitID_Node(ID);
   
    if( abs(pt1.x - pt2.x ) >= abs (ptr->GetPtBeg().x - ptr->GetPtEnd().x) 
     && abs(pt1.y - pt2.y ) >= abs (ptr->GetPtBeg().y - ptr->GetPtEnd().y)  )
     return TRUE;
     
	ToolKits::Normalization(pt1,pt2);

	
	POINT pt = pt1;

	int xChar;

	//�]�w�_�l�r��m
	WordNodeInfo *wordPtr = ptr->GetHead();

	while( wordPtr != NULL )
	{
		///�M�w�e��
		if( wordPtr->_cha == '\r' )
		{
			xChar = 0;
		}
		else
		{
			xChar = this->GetCharX();
		}

		if( pt.x + xChar  > pt2.x - CommonVar::WORD_REC_MARGIN  )
		{
			pt.x = pt1.x;
			pt.y += this->GetCharY();
		}

		//wordPtr->_pt = pt;

		//�p��U�@�Ӥ�r�_�l�I		
		if( wordPtr->_cha == '\r' )
		{
			pt.x = pt1.x;
			pt.y += this->GetCharY();
		}
		else
		{
			pt.x += xChar + CommonVar::WORD_GAP;
		}

		wordPtr = wordPtr->_rlink;
	}

	ptr = NULL;
	wordPtr = NULL;

	if( pt.y + 2*this->GetCharY() > pt2.y - CommonVar::WORD_REC_MARGIN )
		return FALSE;
	else
		return TRUE;
}

/**�W�[��r�ؤj�p*/
void CTextOut::IsAddWordRecForWordContent(int ID)
{
	MyListNode *ptr =  gMyList.visitID_Node(ID);

	POINT pt2;
	pt2.x = ptr->GetPtEnd().x;
	pt2.y = ptr->GetPtEnd().y;

	POINT pt = ptr->FindLastWordPosInWordRec(pt);

	

	if( pt2.y <  pt.y + this->_cyChar + CommonVar::WORD_REC_MARGIN + CommonVar::SMALL_RECT_BUFFER)
	{
		pt2.y =  pt.y + this->_cyChar + CommonVar::WORD_REC_MARGIN + CommonVar::SMALL_RECT_BUFFER;
		ptr->SetPtEnd(pt2.x, pt2.y);
		_parent->GetCSelectRecInfo().SetPosEnd(pt2);
	}

	// scroll
	POINT lastWordPt = ptr->FindLastWordPosInWordRec(lastWordPt);

	int diff_y = lastWordPt.y + this->GetCharY() + CommonVar::WORD_REC_MARGIN +  CommonVar::SMALL_RECT_BUFFER
		          - _parent->GetClientHeight() - _parent->GetScrollBarHandle()->GetVertPos();
	         
	if( diff_y > 0 )
	{
		_parent->GetScrollBarHandle()->VerticalScrollBarAutoEvent(_parent->GetMainHwnd(),diff_y);
	}

}

//�B�zdelete
void CTextOut::DeleteHandle(BOOL bAction)
{

	POINT pt;
	::GetCaretPos(&pt);

	int ID = _parent->GetHitID();
	MyListNode* ptr = gMyList.visitID_Node( ID );

	POINT pt1 = gMyList.GetCurrentPtrPosBeg();
	POINT pt2 = gMyList.GetCurrentPtrPosEnd();
	this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos(pt.x,pt.y);


	POINT finalCaretPt, finalWordPt;
	finalCaretPt = ptr->FindLastWordPosInWordRec(finalWordPt);

	if( finalCaretPt.x == pt.x && finalCaretPt.y == pt.y )
	{ 
		OutputDebugString(_T("---------------�F��r����---\n"));
	}
	else if( finalCaretPt.y < pt.y )
	{
		OutputDebugString(_T("---------------�F��r����---\n"));
	}
	else
	{

		POINT nextWordPt;
		nextWordPt = ptr->DelEvent();
		this->_parent->GetDrawTool()->ScreenToWindowCoordinate( nextWordPt.x,nextWordPt.y);
		
		this->CaretHide();
		this->DeleteWordTokenForWordReputation(nextWordPt);
		this->CaretSetPos(nextWordPt);
		this->CaretShow();
		
		// ���صe��
		this->RenewNowCurScope();

		_parent->GetDrawTool()->ShowTarGetRecForTypingWord();

		int iLastID = gRedoUndoController->GetFinalID();
		int iLastCmd = gRedoUndoController->GetLastCmd();
		// �����ª���r�s��
		if( iLastCmd == COMMAND_EDIT_WORD && ID == iLastID && bAction == FALSE)
			gRedoUndoController->PopFinalWord();

		gAddRedoUndoPaintInfo.AddEditWordNodeInfo( ID);
	}
}
//VK_RETURN 
void CTextOut::ReturnKeyHandle()
{
	int ID = _parent->GetHitID();
	MyListNode* ptr =  gMyList.visitID_Node( ID );

	POINT caretPosPt;
	::GetCaretPos(&caretPosPt);
	POINT wordPt;
	wordPt = caretPosPt;

	this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos( wordPt.x , wordPt.y );


	if( gMyList.IsLastTypeIsWordRec( ID ) == TRUE 
		&& ptr->IsArrChaIdxBelongEnd() == TRUE)  
	{
		ptr->AppendWordNode(wordPt,(TCHAR) '\r');
	}
	else
	{
		ptr->InsertWordNode(wordPt,(TCHAR) '\r');
		this->WordReputationForInsertingWord( ID );
	}

	
	this->IsAddWordRecForWordContent( ID );
	
	


	caretPosPt = ptr->FindCurrWordPosInWordRec( caretPosPt );
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate( caretPosPt.x , caretPosPt.y );

	this->CaretSetPos(caretPosPt);
	ptr = NULL;
}


//VK_LEFT
void CTextOut::VkLeftHandle()
{	
	MyListNode *ptr = gMyList.visitID_Node( _parent->GetHitID() );
	POINT caret;

	::GetCaretPos( &caret );
	this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos( caret.x, caret.y );

	caret = ptr->ShiftLeftArrChaIdx();
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate( caret.x ,caret.y);


	this->CaretSetPos( caret );
	ptr = NULL;
}

//VK_RIGHT
void CTextOut::VkRightHandle()
{
	MyListNode *ptr = gMyList.visitID_Node( _parent->GetHitID() );

	POINT caret;
	::GetCaretPos( &caret );
	this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos( caret.x, caret.y );

	caret = ptr->ShiftRightArrChaIdx();
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate( caret.x ,caret.y);

	this->CaretSetPos( caret );
	ptr = NULL;
}


//VK_DOWN
void CTextOut::VkDownHandle()
{
	MyListNode *ptr = gMyList.visitID_Node( _parent->GetHitID() );

	if( ptr != NULL && ptr->GetWordNum() == 0 )
		return ;

	POINT pt2 = gMyList.GetCurrentPtrPosEnd();


	POINT caretPosPt;
	::GetCaretPos(&caretPosPt);
	this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos(caretPosPt.x,caretPosPt.y);


	if( caretPosPt.y + 2*_cyChar > pt2.y )
		return;
	else
	{
		caretPosPt.y +=  _cyChar;
		caretPosPt = ptr->ShiftDownArrChaIdx( caretPosPt );
	} 
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate( caretPosPt.x ,caretPosPt.y);
	this->CaretSetPos( caretPosPt );
	ptr = NULL;
}


//VK_UP
void CTextOut::VkUpHandle()
{
	MyListNode *ptr = gMyList.visitID_Node( _parent->GetHitID() );
	POINT pt1 = gMyList.GetCurrentPtrPosBeg();


	POINT caretPosPt;
	::GetCaretPos(&caretPosPt);
	this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos(caretPosPt.x,caretPosPt.y);


	if( caretPosPt.y - _cyChar < pt1.y )
		return;
	else
	{
		caretPosPt.y -=  _cyChar;
		caretPosPt = ptr->ShiftUpArrChaIdx( caretPosPt );
	} 

	this->_parent->GetDrawTool()->ScreenToWindowCoordinate( caretPosPt.x ,caretPosPt.y);
	this->CaretSetPos( caretPosPt );
	ptr = NULL;
}

// home ��B�z
void CTextOut::VkHomeHandle()
{

	int ID = this->_parent->GetHitID();

	MyListNode *ptr = gMyList.visitID_Node( ID );

	POINT caretPosPt;
	if( this->_IS_CLICK_CONTROL == TRUE )
	{
		caretPosPt = ptr->ShiftCtrlHomeArrChaIdx();
	}
	else
	{
		//���o�ثe���Ц�m
		::GetCaretPos( & caretPosPt );
		this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos( caretPosPt.x, caretPosPt.y );
		caretPosPt = ptr->ShiftHomeArrChaIdx(caretPosPt);
	}

	this->_parent->GetDrawTool()->ScreenToWindowCoordinate( caretPosPt.x, caretPosPt.y );

	this->CaretSetPos( caretPosPt );

	ptr = NULL;
}

// end ��B�z
void CTextOut::VkEndHandle()
{
	int ID = this->_parent->GetHitID();

	MyListNode *ptr = gMyList.visitID_Node( ID );

	POINT caretPosPt;
	if( this->_IS_CLICK_CONTROL == TRUE )
	{
		caretPosPt = ptr->ShiftCtrlEndArrChaIdx();
	}
	else
	{

		//���o�ثe���Ц�m
		::GetCaretPos( & caretPosPt );
		this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos( caretPosPt.x, caretPosPt.y );

		caretPosPt = ptr->ShiftEndArrChaIdx(caretPosPt);
	}

	this->_parent->GetDrawTool()->ScreenToWindowCoordinate( caretPosPt.x, caretPosPt.y );

	this->CaretSetPos( caretPosPt );

	ptr = NULL;

}

/********************************
* �W�[�Ť�r�ئܦ�C
********************************/
void CTextOut::AddWordRecIntoQueue(POINT recPt1, POINT recPt2)
{
	ToolKits::Normalization(recPt1,recPt2);
    
	// ��r���}�Y
	gMyList.AddGraphNode(
		IDM_WORD_REC
		,recPt1
		,recPt2
		,_parent->GetColorSelection()
		,_parent->GetFigBg()
		,1);

	this->_parent->SetHitID( gMyList.GetCurrentNo() );
	
	// �]�w��������ئ�m
	_parent->GetCSelectRecInfo().SetIsShow(TRUE);
	_parent->GetCSelectRecInfo().SetPosStart(recPt1);
	_parent->GetCSelectRecInfo().SetPosEnd(recPt2);
	_parent->GetCSelectRecInfo().SetType(IDM_WORD_REC);
	
	
   // set caret
	_parent->GetCTextOut()->CaretHide();
    this->_parent->GetDrawTool()->ScreenToWindowCoordinate( recPt1.x , recPt1.y );
	recPt1.x += CommonVar::WORD_REC_MARGIN;
	recPt1.y += CommonVar::WORD_REC_MARGIN;
	this->CaretSetPos(recPt1);
	_parent->GetCTextOut()->CaretShow();


	//�s�W��r�ث᭫�]��l��rBuffer,�B�NACTION�i�J�������A
	//_parent->SetAction(FALSE);
    SgWordDataModelControler::Instance()->ClearTextBuffer();
} 