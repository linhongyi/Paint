#include "StdAfx.h"
#include "ClipboardControl.h"
#include "PaintInfo.h"
#include "ToolKits.h"
#include "Resource.h"
#include "AddRedoUndoPaintInfo.h"
#include "MyListController.h"
#include "WordNodeInfo.h"

extern AddRedoUndoPaintInfo gAddRedoUndoPaintInfo;

extern MyListController gMyList;
CClipboardControl::CClipboardControl(void)
{
}


CClipboardControl::~CClipboardControl(void)
{
	_parent = NULL;
}

void CClipboardControl::SetParent(PaintInfo *ptr)
{
	_parent = ptr;
}

void CClipboardControl::SetCGraphClipboardControl(CGraphClipboardControl *ptr)
{
	_cGraphClipboardControl = ptr;
}

void CClipboardControl::SetCWordClipboardControl(CWordClipboardControl *ptr)
{
	_cWordClipboardControl = ptr;
}

/********************************
* �ŶKï ���K����( �ϧ� or ��r)
*********************************/
void CClipboardControl::CopyControl()
{
	int ID = _parent->GetHitID();

	//���ɨS���ؼйϡA��copy�L��
	if( ID < 0 )
		return;
	else
	{
		MyListNode *ptr = gMyList.visitID_Node(ID);
		int type = ptr->GetnObjType();
		
		POINT ptStart, ptEnd;
		ptStart = ptr->GetPtBeg();
		ptEnd   = ptr->GetPtEnd();
		
		int currNodePenColor  = ptr->GetPenColor();
		int currNodeFigBg     = ptr->GetFigBg();


		if( type != IDM_WORD_REC)
		{    
		    int currNodePenWidth  = ptr->GetPenWidth();
		    
		    // to do copy 
		    this->_cGraphClipboardControl->Copy(_parent->GetMainHwnd(), type, ptStart , ptEnd, currNodePenColor, currNodeFigBg,currNodePenWidth);
		}
		else
		{
				//�S����r���e
			if( gMyList.IsWordStart() == TRUE )
				return ;

			
        
			int nCount = 0;
			
			POINT inverseWordPt1; 
		    POINT inverseWordPt2;
			BOOL bIsSegmentWord = this->_parent->GetCTextOut()->IsInverseWordScope();
			// �P�_�O������Ӥ�r�� �٬O ��r�϶�
			if( bIsSegmentWord == TRUE )
			{
				inverseWordPt1 = _parent->GetCInverseWordInfo().GetCaretPt1();
				inverseWordPt2 = _parent->GetCInverseWordInfo().GetCaretPt2();
			}
			else
			{
				inverseWordPt1 = ptStart;
	            inverseWordPt2 = ptEnd;

				inverseWordPt1.x += CommonVar::WORD_REC_MARGIN;
				inverseWordPt1.y += CommonVar::WORD_REC_MARGIN;
				inverseWordPt2.x += CommonVar::WORD_REC_MARGIN;
				inverseWordPt2.y += CommonVar::WORD_REC_MARGIN;
			}


			// �������Ϭq���Ҧ���r
			POINT wordPt;
			int wordColor    = ptr->GetPenColor();
			int wordBgColor  = ptr->GetFigBg();

			WordNodeInfo *wordPtr = ptr->GetHead();
			
	
		    vector<char> myCha;

			while( wordPtr != NULL )
			{

				wordPt = wordPtr->_pt;
			
				if( this->_parent->GetCTextOut()->IsTheWordInTheInverseWord(wordPt,inverseWordPt1,inverseWordPt2) == TRUE )
				{
					myCha.push_back( (char)  wordPtr->_cha );

					//����줸�A�ŦX�e�r�� + '\n'
					if( wordPtr->_cha == '\r')
					{
						myCha.push_back( '\n' );
					}
				}

				wordPtr = wordPtr->_rlink;
					
			}
				
			if( (int) myCha.size() < 0 )
				 return;
	
		
			this->_cWordClipboardControl->CopyWord(_parent->GetMainHwnd()
				,_parent->GetHitID()
				,IDM_WORD_REC
				,ptStart
				,ptEnd
				,inverseWordPt1
				,wordColor
				,wordBgColor
				,myCha
				,bIsSegmentWord);

			myCha.clear();
		}
		
	}
}


/*********************************
* �ŶK�ϡA�ϧΩΤ�r���ŤU����
**********************************/
void CClipboardControl::CutControl()
{
	int ID = _parent->GetHitID();
	//�ثe�S��������`�I
	if( ID < 0 )
		return;
	
	
	MyListNode* ptr = gMyList.visitID_Node( ID );

	this->CopyControl();
	if( ptr->GetnObjType() != IDM_WORD_REC )
	{
		gAddRedoUndoPaintInfo.AddDeleteNodeInfo(ID);
		this->_cGraphClipboardControl->CutGraph(); 
	
	}
	else
	{
		if( ptr->GetWordNum() <= 0 )
		{
			this->_cWordClipboardControl->CutWordRec();
		}
		//�ŤU�@�q��r
		else if( _parent->GetCTextOut()->IsInverseWordScope() == TRUE )
		{
			this->_cWordClipboardControl->CutSegmentWord();
		}
		//�ŤU�@�����r
		else
		{
			//log
			gAddRedoUndoPaintInfo.AddDeleteNodeInfo( ID );
			this->_cWordClipboardControl->CutWordRec();
		}

	}
	ptr = NULL;

	//�٭�ƹ��]�w
	this->_parent->SetDirectionMouse(CursorDirection_ARROW);
	::SetCursor( LoadCursor(NULL,IDC_ARROW)); 
	this->_parent->GetDrawTool()->MapDCToMonitor();
}

// �R�� = �ŤU ( ��������ŶKï )
void CClipboardControl::DeleteControl()
{
	int ID = _parent->GetHitID();
	//�ثe�S��������`�I
	if( ID < 0 ) return;

	MyListNode* ptr = gMyList.visitID_Node( ID );
	
	if( ptr->GetnObjType() != IDM_WORD_REC )
	{
		gAddRedoUndoPaintInfo.AddDeleteNodeInfo( ID );
		this->_cGraphClipboardControl->CutGraph();

	}
	else
	{
		if( ptr->GetWordNum() <= 0 )
		{
			this->_cWordClipboardControl->CutWordRec();
		}
		//�ŤU�@�q��r
		else if( _parent->GetCTextOut()->IsInverseWordScope() == TRUE )
		{
			this->_cWordClipboardControl->CutSegmentWord();
		}
		//�ŤU�@�����r
		else
		{
			//log
			gAddRedoUndoPaintInfo.AddDeleteNodeInfo( ID );
			this->_cWordClipboardControl->CutWordRec();
		}

	}
	ptr = NULL;
	//�٭�ƹ��]�w
	this->_parent->SetDirectionMouse(CursorDirection_ARROW);
	::SetCursor( LoadCursor(NULL,IDC_ARROW)); 
	this->_parent->GetDrawTool()->MapDCToMonitor();
}

/***********************************
* �ŶKï �ϧΩΤ�r�K�W����
*************************************/
void CClipboardControl::PasteControl()
{
	UINT format = RegisterClipboardFormat(_T("MY_CUSTOM_FORMAT"));

	if ( ::IsClipboardFormatAvailable( format ) == TRUE )
	{

		::OpenClipboard(_parent->GetMainHwnd());

		//Get the buffer
		HANDLE hData = GetClipboardData(format);
		clipObj * buffer = (clipObj *)GlobalLock( hData );

		// �ק� buffer ��m�A�令��ثe��m
		int ID = _parent->GetHitID();


		// ���ؼЦ�m�A�H�ؼЦ�m������I
		if( ID >= 0 )
		{
			this->FindTargetGraphLocation(ID,buffer);
		}
		// �S���ؼЦ�m
		else
		{
			// to do nothing ,�ΤW���ŶK����m
		}


		if( buffer->nObjType != IDM_WORD_REC )
		{
			this->GetPaintInfo()->GetCTextOut()->CaretHide();
			//�K��
			this->_cGraphClipboardControl->Paste(buffer);
			gAddRedoUndoPaintInfo.AddNewNodeInfo(_parent->GetHitID());

			GlobalUnlock( hData );
			/*GlobalUnlock(buffer);*/
			::CloseClipboard();

		}
		else
		{

			//�S����r 
			if ( ::IsClipboardFormatAvailable( CF_TEXT ) == FALSE )
			{
				//Put it on the clipboard
		    	GlobalUnlock( hData );
				::CloseClipboard();
				return ;
			}

			//Get the buffer
			HANDLE hText = GetClipboardData(CF_TEXT);
			char * pText = (char *)GlobalLock( hText );
			this->_cWordClipboardControl->Paste(buffer,pText);

			GlobalUnlock( hText );
			//Put it on the clipboard
			GlobalUnlock( hData );
			/*GlobalUnlock(buffer);*/
			::CloseClipboard();

		}
		
		_parent->GetDrawTool()->MapDCToMonitor();
	}
	else if( ::IsClipboardFormatAvailable( CF_TEXT ) == TRUE )
	{
		if( _parent->GetHitID() >= 0 && gMyList.GetCurrentPtrType() == IDM_WORD_REC )
		{	int ID = this->_parent->GetHitID();
  
	    	//���}�ŶKï
		    ::OpenClipboard(_parent->GetMainHwnd());
		    //Get the buffer
		    HANDLE hText = GetClipboardData(CF_TEXT);

	    	char* pText = NULL ;
	    	pText = (char* )GlobalLock( hText );

	    	this->_cWordClipboardControl->SetPtext( pText);
	    	this->_cWordClipboardControl->Paste_SegmentWord();

	    	GlobalUnlock( hText );
	    	::CloseClipboard();

	    	pText  = NULL;
			_parent->GetDrawTool()->MapDCToMonitor();
		}
	/*	else
			MessageBox(_parent->GetMainHwnd(), _T("�J�I���s�b�s���r���\n"), NULL ,NULL );*/


	}
	else
		return;
}


/*****************************************
* �ŶKï�A�K�ϧΩΤ�r�خɡA�򥻦�m����w�C
* �Y���ؼйϡA�h�Φ��ؼйϷ��Ѧ��I; 
* �Y�S���ؼйϡA�h�ΤW���Kï����m���Ѧ��I
******************************************/
void CClipboardControl::FindTargetGraphLocation(int ID,clipObj *buffer)
{
	MyListNode* ptr = gMyList.visitID_Node( ID );

	int diff_x =  buffer->ptEnd.x - buffer->ptBeg.x;
	int diff_y =  buffer->ptEnd.y - buffer->ptBeg.y;

	POINT baseIniPt = ptr->GetPtBeg();
	POINT baseEndPt = ptr->GetPtEnd();

	ToolKits::TunePtsWithPosY( &baseIniPt, &baseEndPt);

	// ptEnd �b ptStart�k�U
	if( diff_x >=0 && diff_y >=0 )
	{
		buffer->ptBeg = baseIniPt;
		buffer->ptEnd = buffer->ptBeg;

		buffer->ptEnd.x += diff_x;
		buffer->ptEnd.y += diff_y;

		if( buffer->ptEnd.y > CommonVar::DRAW_MAP_HEIGHT )
		{
			diff_y =  buffer->ptEnd.y - CommonVar::DRAW_MAP_HEIGHT;
			buffer->ptEnd.y =  CommonVar::DRAW_MAP_HEIGHT;
			buffer->ptBeg.y -= diff_y;
		}
		else if( buffer->ptEnd.x > CommonVar::DRAW_MAP_WIDTH )
		{
			diff_x =  buffer->ptEnd.x - CommonVar::DRAW_MAP_HEIGHT;
			buffer->ptEnd.x =  CommonVar::DRAW_MAP_HEIGHT;
			buffer->ptBeg.x-= diff_x;
		}
	}
	// ptEnd �b ptStart�k�W
	else if( diff_x >=0 && diff_y <=0 )
	{
		buffer->ptEnd.x = baseEndPt.x;
		buffer->ptEnd.y = baseIniPt.y;

		buffer->ptBeg.x = buffer->ptEnd.x - diff_x;
		buffer->ptBeg.y = buffer->ptEnd.y - diff_y;


		if( buffer->ptBeg.y > CommonVar::DRAW_MAP_HEIGHT )
		{
			diff_y =  buffer->ptBeg.y - CommonVar::DRAW_MAP_HEIGHT;
			buffer->ptBeg.y =  CommonVar::DRAW_MAP_HEIGHT;
			buffer->ptEnd.y -= diff_y;
		}
		else if( buffer->ptEnd.x > CommonVar::DRAW_MAP_WIDTH )
		{
			diff_x =  buffer->ptEnd.x - CommonVar::DRAW_MAP_HEIGHT;
			buffer->ptEnd.x =  CommonVar::DRAW_MAP_HEIGHT;
			buffer->ptBeg.x-= diff_x;
		}

	}
	// ptEnd �b ptStart���W
	else if( diff_x <=0 && diff_y <=0 )
	{
		buffer->ptBeg = baseIniPt;
		buffer->ptEnd = buffer->ptBeg;
		buffer->ptEnd.x -= diff_x;
		buffer->ptEnd.y -= diff_y;

		if( buffer->ptBeg.y > CommonVar::DRAW_MAP_HEIGHT )
		{
			diff_y =  buffer->ptBeg.y - CommonVar::DRAW_MAP_HEIGHT;
			buffer->ptBeg.y =  CommonVar::DRAW_MAP_HEIGHT;
			buffer->ptEnd.y -= diff_y;
		}
		else if( buffer->ptBeg.x > CommonVar::DRAW_MAP_WIDTH )
		{
			diff_x =  buffer->ptBeg.x - CommonVar::DRAW_MAP_HEIGHT;
			buffer->ptBeg.x =  CommonVar::DRAW_MAP_HEIGHT;
			buffer->ptEnd.x-= diff_x;
		}
	}
	// ptEnd �b ptStart���U
	else
	{
		buffer->ptBeg.x = baseEndPt.x;
		buffer->ptBeg.y = baseIniPt.y;

		buffer->ptEnd.x = buffer->ptBeg.x + diff_x;
		buffer->ptEnd.y = buffer->ptBeg.y + diff_y;


		if( buffer->ptEnd.y > CommonVar::DRAW_MAP_HEIGHT )
		{
			diff_y =  buffer->ptEnd.y - CommonVar::DRAW_MAP_HEIGHT;
			buffer->ptEnd.y =  CommonVar::DRAW_MAP_HEIGHT;
			buffer->ptBeg.y -= diff_y;
		}
		else if( buffer->ptBeg.x > CommonVar::DRAW_MAP_WIDTH )
		{
			diff_x =  buffer->ptBeg.x - CommonVar::DRAW_MAP_HEIGHT;
			buffer->ptBeg.x =  CommonVar::DRAW_MAP_HEIGHT;
			buffer->ptEnd.x-= diff_x;
		}
	}
	buffer = NULL;
	ptr = NULL;
}