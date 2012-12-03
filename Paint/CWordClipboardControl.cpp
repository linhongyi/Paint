#include "StdAfx.h"
#include "CWordClipboardControl.h"
#include "CommonVar.h"
#include "PaintInfo.h"
#include <assert.h>
#include "SgWordDataModelControler.h"
#include "ToolKits.h"
#include "AddRedoUndoPaintInfo.h"
#include "MyListController.h"
#include <assert.h>
#include "RedoUndoController.h"

extern RedoUndoController *gRedoUndoController;
extern AddRedoUndoPaintInfo gAddRedoUndoPaintInfo;

extern MyListController gMyList;
CWordClipboardControl::CWordClipboardControl(void)
{
}


CWordClipboardControl::~CWordClipboardControl(void)
{
		_parent = NULL;
}

void CWordClipboardControl::CopyWord(HWND hWnd,int ID, int type, POINT ptBeg, POINT ptEnd, POINT wordIniPt,int iPenColor, int iBgColor, const vector<char>& tmp,BOOL IsSegment)
{
	UINT format = RegisterClipboardFormat(_T("MY_CUSTOM_FORMAT"));

	::OpenClipboard(hWnd);
	::EmptyClipboard();

	clipObj data;
	data.nObjType   = type;
	data.ptBeg      = ptBeg;
	data.ptEnd      = ptEnd;
	data.iPenWidth  = 1;
	data.iPenColor  = iPenColor;
	data.iFigBg     = iBgColor;

	data.IsSegmentWord = IsSegment;
	data.wordIniPt  = wordIniPt;


	//allocate some global memory
	HGLOBAL clipbuffer;
	clipbuffer = GlobalAlloc(GMEM_DDESHARE, sizeof(clipObj));
	clipObj * buffer = (clipObj*)GlobalLock(clipbuffer);

	//put the data into that memory
	*buffer = data;

	//Put it on the clipboard
	GlobalUnlock(clipbuffer);
	SetClipboardData(format,clipbuffer);


	//allocate some global memory
	int nlen =  (int ) tmp.size();

	HGLOBAL wordClipbuffer;
	wordClipbuffer = GlobalAlloc(GHND | GMEM_SHARE, (nlen+1) );
	char*  wordbuffer = (char*) GlobalLock(wordClipbuffer);
	
	
	for( int i = 0 ; i < nlen ; i ++ )
		*(wordbuffer+i) = tmp[i];
	wordbuffer[nlen] = '\0';

	//Put it on the clipboard
	GlobalUnlock(wordClipbuffer);

	#ifndef UNICODE
	     SetClipboardData(CF_UNICODETEXT,wordClipbuffer);
    #else 
	     SetClipboardData(CF_TEXT ,wordClipbuffer);
    #endif
	
	CloseClipboard();

	//tmp = NULL;
	//reset direction
	_xOffset = -CommonVar::SHIFT_PIXEL;
	_yOffset = -CommonVar::SHIFT_PIXEL;
}

void CWordClipboardControl::Paste(clipObj *pBuffer,char* pText)
{
	MyListNode* ptr = NULL;
	_parent->GetPaintInfo()->GetCTextOut()->CaretHide();

	
	_buffer = pBuffer;
	_pText = pText;

	int ID = _parent->GetPaintInfo()->GetHitID();
	
	if( ID >=0 )
		ptr = gMyList.visitID_Node(ID);

	//檢查是否可以貼上此段文字

	if( _buffer->IsSegmentWord == TRUE && ID >= 0 && ptr->GetnObjType() == IDM_WORD_REC)
	{
		this->Paste_SegmentWord();
	}
	else
	{
		//貼文字框
		this->_parent->GetCGraphClipboardControl()->Paste(pBuffer);
	    //加文字
		this->Paste_NewWord();
		
		//重貼文字框後，佇列ID++
		int newID = _parent->GetPaintInfo()->GetHitID();
		ptr = gMyList.visitID_Node(newID);
		
		// set pos
		POINT caretPt;
		caretPt =  ptr->FindLastWordPosInWordRec(caretPt);
		this->_parent->GetPaintInfo()->GetDrawTool()->ScreenToWindowCoordinate( caretPt.x, caretPt.y);
		this->_parent->GetPaintInfo()->GetCTextOut()->CaretSetPos(caretPt);

	    //清除文字暫存，確定此次加入為一完整動作。
		SgWordDataModelControler::Instance()->ClearTextBuffer();
		gAddRedoUndoPaintInfo.AddEditWordNodeInfo(newID);
	}

	_parent->GetPaintInfo()->GetCTextOut()->CaretShow();



}



void CWordClipboardControl::CutWordRec()
{
	int ID = _parent->GetPaintInfo()->GetHitID();

	MyListNode *ptr = gMyList.visitID_Node( ID );

	POINT fillWhitePt1 = ptr->GetPtBeg();
	POINT fillWhitePt2 = ptr->GetPtEnd();

	/**塗白*/
	_parent->GetPaintInfo()->GetDrawTool()->FillWhileRec(fillWhitePt1,fillWhitePt2);

	
	bool bdeleted = gMyList.DeleteID_Node(ID);
	
	assert( bdeleted == true );

	this->MoveSelectRecIntoLastTarGetRecByDrawMap();
	

	//若刪除是最後一個節點則不用調整視窗
	if( _parent->GetPaintInfo()->GetHitID() >= 0 )
		this->_parent->GetPaintInfo()->GetScrollBarHandle()->TuneWindowView();

	// setPos
	int newID = _parent->GetPaintInfo()->GetHitID();

	if( newID >= 0 )
	{

		this->_parent->GetPaintInfo()->GetCTextOut()->CaretHide();

		MyListNode *ptr = gMyList.visitID_Node(  newID );
		POINT caretPt = ptr->FindLastWordPosInWordRec(caretPt);
		this->_parent->GetPaintInfo()->GetDrawTool()->ScreenToWindowCoordinate( caretPt.x, caretPt.y);
		this->_parent->GetPaintInfo()->GetCTextOut()->CaretSetPos( caretPt);
		ptr = NULL;
	}
	else
	{
		this->_parent->GetPaintInfo()->GetCTextOut()->CaretHide();
		this->_parent->GetPaintInfo()->GetCSelectRecInfo().SetIsShow(false);
	}

	_parent->GetPaintInfo()->GetDrawTool()->RePaintWithClipRgn( fillWhitePt1, fillWhitePt2);

}

void CWordClipboardControl::CutSegmentWord()
{
	int ID = _parent->GetPaintInfo()->GetHitID();
	int iLastCmd = gRedoUndoController->GetLastCmd();
	int iLastID  = gRedoUndoController->GetFinalID();
	int bAction  = FALSE;

	//上個動作為一個Action，所以要將此時的文字資訊做記錄
	if( _parent->GetPaintInfo()->GetAction() == TRUE )
	{
		bAction = TRUE;
		//設定不為一個ACTION，進行ACTION收集狀態
		 _parent->GetPaintInfo()->SetAction( FALSE );
		SgWordDataModelControler::Instance()->SaveAllWordIniInThisWordRec(ID);
	}
	//目前編輯ID，不為LOG紀錄中最後一個ID，則視為一個新的動作
	else if( iLastID != ID )
	{
		bAction = TRUE;
		SgWordDataModelControler::Instance()->SaveAllWordIniInThisWordRec(ID);
	}

	MyListNode *ptr = gMyList.visitID_Node(ID);


	//刪除節點
	this->_parent->GetPaintInfo()->GetCTextOut()->InverseWordHandle(bAction);


	POINT recPt1, recPt2;
	recPt1 = ptr->GetPtBeg();
	recPt2 = ptr->GetPtEnd();


	/**塗白*/
	_parent->GetPaintInfo()->GetDrawTool()->FillWhileRec( recPt1 ,recPt2 );
	_parent->GetPaintInfo()->GetDrawTool()->RePaintWithClipRgn( recPt1, recPt2);

}

void CWordClipboardControl::Paste_NewWord()
{
	// 反白選取 取消
	_parent->GetPaintInfo()->GetCInverseWordInfo().ResetInverseWordScope();

	int nlen =  ::strlen(_pText);
	int ID   =  _parent->GetPaintInfo()->GetHitID();

	POINT pt1;
	pt1.x = _buffer->ptBeg.x + CommonVar::WORD_REC_MARGIN;
	pt1.y = _buffer->ptBeg.y + CommonVar::WORD_REC_MARGIN;


	MyListNode *listPtr = gMyList.visitID_Node(ID);
	POINT charPt = pt1;

	//插入文字
	for( int j = 0 ; j < nlen; j++)
	{
		// unicode 換行鍵 ignore
		if( _pText[j] != '\n' )
			listPtr->AppendWordNode(charPt,_pText[j]);

	}

	//計算文字位置
	this->_parent->GetPaintInfo()->GetCTextOut()->WordReputationForInsertingWord(ID);
	_parent->GetPaintInfo()->GetCTextOut()->IsAddWordRecForWordContent(ID);


	POINT pt2;
	pt2.x = listPtr->GetPtEnd().x + CommonVar::WORD_REC_MARGIN;
	pt2.y = listPtr->GetPtEnd().y + CommonVar::WORD_REC_MARGIN;


	 //輸出文字
	this->_parent->GetPaintInfo()->GetCTextOut()->TextLineWordToDcInWindowView( 
		listPtr
		,_parent->GetPaintInfo()->GetMemDC()
		,pt1
		,pt2);
	listPtr = NULL;
}

void CWordClipboardControl::Paste_SegmentWord()
{
	int ID = _parent->GetPaintInfo()->GetHitID();

	MyListNode* ptr = gMyList.visitID_Node(ID);

	POINT pt = ptr->FindLastWordPosInWordRec(pt);
	// 每行寬度
	int rec_diff_x = abs(ptr->GetPtEnd().x - ptr->GetPtBeg().x - 2*CommonVar::WORD_REC_MARGIN);
	// 要幾行
	int need_row   = ::strlen(_pText) * _parent->GetPaintInfo()->GetCTextOut()->GetCharX () / rec_diff_x;

	// 檢查否過超過下界
	if( (need_row +1 )* _parent->GetPaintInfo()->GetCTextOut()->GetCharY() +  pt.y > CommonVar::DRAW_MAP_HEIGHT )
	{
		::MessageBox( _parent->GetPaintInfo()->GetMainHwnd(), _T("-----------已達畫布下界"),NULL,NULL);
	}
	else
	{
		
		POINT inverseWordPt1 = this->_parent->GetPaintInfo()->GetCInverseWordInfo().GetCaretPt1();
		POINT inverseWordPt2 = this->_parent->GetPaintInfo()->GetCInverseWordInfo().GetCaretPt2();
		ToolKits::InverseWordNormalization( inverseWordPt1 , inverseWordPt2);

		if ( this->_parent->GetPaintInfo()->GetCTextOut()->IsInverseWordScope() == TRUE )
		{
			WordNodeInfo *wordPtr = ptr->GetCurrWordPtr();

			//將目前指標移至目前反白字最後一個位置
			for( wordPtr ; wordPtr != NULL ; wordPtr = wordPtr->_rlink )
			{
				if( wordPtr->_pt.x < inverseWordPt2.x && wordPtr->_pt.y == inverseWordPt2.y )
				{
					ptr->SetCurrWordPtr( wordPtr );
					//break;
				}
				else if( wordPtr->_pt.y >= inverseWordPt2.y )
				{
					break;
				}
			}
			wordPtr = NULL;
			
			//// set pos
			POINT caretPt = ptr->FindCurrWordPosInWordRec( caretPt );
			this->_parent->GetPaintInfo()->GetDrawTool()->ScreenToWindowCoordinate(caretPt.x, caretPt.y );
			this->_parent->GetPaintInfo()->GetCTextOut()->CaretSetPos(caretPt);

			_parent->GetPaintInfo()->GetCInverseWordInfo().ResetInverseWordScope();
	
		}
		else
		{
	
			int iLastCmd = gRedoUndoController->GetLastCmd();
			int iLastID  = gRedoUndoController->GetFinalID();

			//上個動作為一個Action，所以要將此時的文字資訊做記錄
			if( _parent->GetPaintInfo()->GetAction() == TRUE )
			{
				//設定不為一個ACTION，進行ACTION收集狀態
				_parent->GetPaintInfo()->SetAction( FALSE );
				SgWordDataModelControler::Instance()->SaveAllWordIniInThisWordRec(ID);
			}
			//目前編輯ID，不為LOG紀錄中最後一個ID，則視為一個新的動作
			else if( iLastID != ID )
			{
				SgWordDataModelControler::Instance()->SaveAllWordIniInThisWordRec(ID);
			}

			POINT caretPt;
			::GetCaretPos(&caretPt);
			_parent->GetPaintInfo()->GetDrawTool()->WindowToScreenCoordinateForEndPos( caretPt.x, caretPt.y );

			int size = ::strlen( _pText );

			for( int i = 0; i < size ; i ++ )
			{
				// unicode 換行鍵 ignore
				if( _pText[i] != '\n' )
					ptr->InsertWordNode(caretPt,_pText[i]);
			}

		
			_parent->GetPaintInfo()->GetCTextOut()->WordReputationForInsertingWord(	ID);
			_parent->GetPaintInfo()->GetCTextOut()->IsAddWordRecForWordContent(ID);

			
			POINT recPt1, recPt2;
			recPt1 = ptr->GetPtBeg();
			recPt2 = ptr->GetPtEnd();
			

			/**塗白*/
			_parent->GetPaintInfo()->GetDrawTool()->FillWhileRec( recPt1 ,recPt2 );
			_parent->GetPaintInfo()->GetDrawTool()->RePaintWithClipRgn( recPt1, recPt2);
		

			//// set pos
			caretPt = ptr->FindCurrWordPosInWordRec( caretPt );
			this->_parent->GetPaintInfo()->GetDrawTool()->ScreenToWindowCoordinate(caretPt.x, caretPt.y );
			this->_parent->GetPaintInfo()->GetCTextOut()->CaretSetPos(caretPt);


			// 移除舊的文字編輯
			if( iLastCmd == COMMAND_EDIT_WORD && ID == iLastID )
			{
				gRedoUndoController->PopFinalWord();
			}
			gAddRedoUndoPaintInfo.AddEditWordNodeInfo( ID);

			ptr = NULL;
		
		}
	}
}

void CWordClipboardControl::SetPtext(char *pText)
{
	_pText = pText;
}

