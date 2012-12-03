#include "StdAfx.h"
#include "CMouseControl.h"
#include "CFileControl.h"
#include "PaintInfo.h"
#include "ToolKits.h"
#include "MyListController.h"
#include "AddRedoUndoPaintInfo.h"
#include "MyListNode.h"
#include "DebugMsgOutputHandle.h"

extern MyListController gMyList;
extern AddRedoUndoPaintInfo gAddRedoUndoPaintInfo;
CMouseControl::CMouseControl(void)
{
	_posBeg.x = 0;
	_posBeg.y = 0;
	_posEnd.x = 0;
	_posEnd.y = 0;
}


CMouseControl::~CMouseControl(void)
{
}

void CMouseControl::SetParent(PaintInfo *ptr)
{
	_parent = ptr;
}

/**�ƹ������I���B�z*/
void CMouseControl::WmLbuttonDownEvent(HWND hWnd,int mouseX,int mouseY)
{	
	_mouseIsMove = FALSE;
	//�O���ƹ��I�U�h���y��
	_iWindowMouseDownPosX = mouseX;
	_iWindowMouseDownPosY = mouseY;

	//�����ثex�By��V���y�Ц�m
	int x =_parent->GetScrollBarHandle()->GetHorzPos();
	int y =_parent->GetScrollBarHandle()->GetVertPos(); 
	_parent->GetScrollBarHandle()->SetBeforeHorzPos (x);
	_parent->GetScrollBarHandle()->SetBeforeVertPos (y);

	//�ϥը���
	_parent->GetCInverseWordInfo().ResetInverseWordScope();

	//���������ƹ���ЧΪ��A�P�_�ƹ��I���N�q
	int directionMouse = _parent->GetDirectionMouse();
	// �@��e��
	if( directionMouse == CursorDirection_ARROW )
	{
		if( _parent->GetSelection() == IDM_WORD )
		{
			_parent->GetCTextOut()->CaretHide();

			//�Y�������@�q��r���_�Y�A�h�����ˬd�W�Ӹ`�I�O�_����r���_�Y (TCHAR) -1 �Acha ==65535
			if ( gMyList.CheckIsRemoveNullWordNode(this->_parent->GetHitID()) == TRUE )
			{
				this->_parent->GetCSelectRecInfo().SetIsShow(false);
				this->_parent->SetHitID(-1);
			}
		}
		else
		{
			//�����إΤ��� = �b�K�@���e��DC
			_parent->GetCSelectRecInfo().SetIsShow(FALSE);
			_parent->SetHitID( -1 );
			_parent->GetDrawTool()->MapDCToMonitor();

			_parent->GetCTextOut()->CaretHide();
		}
	}
	//�ק��r
	else if( directionMouse == CursorDirection_IBEAM )
	{
	
		int ID =  gMyList.GetCurrentNo();
		this->_parent->SetHitID( ID );


		//�]�w�ثe�ƹ����Ц�m
		this->_parent->GetCTextOut()->CaretHide();
		this->_parent->GetCTextOut()->SetNearCaretPosInClickingMouse( mouseX, mouseY );

		POINT pt; 
		pt.x = mouseX;
		pt.y = mouseY;

		this->_parent->GetCTextOut()->CaretSetPos(pt);
		this->_parent->GetCTextOut()->CaretShow();

		//�]�w�ϥդ�r�_�l��r�d��
		this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos(pt.x,pt.y);
		this->_parent->GetCInverseWordInfo().SetCaretPt1(pt);
		this->_parent->GetCInverseWordInfo().SetCaretPt2(pt);

		//�]�w����ؤj�p�d��
		POINT ptStart, ptEnd;

		ptStart = gMyList.GetCurrentPtrPosBeg();
		ptEnd   = gMyList.GetCurrentPtrPosEnd();
		_parent->GetCSelectRecInfo().SetType(IDM_WORD_REC);
		_parent->GetCSelectRecInfo().SetIsShow(TRUE);
		_parent->GetCSelectRecInfo().SetPosStart(ptStart);
		_parent->GetCSelectRecInfo().SetPosEnd(ptEnd);
	}
	//�D�@��e��
	else
	{
		this->_parent->GetCTextOut()->CaretHide();
		
		int nType = gMyList.GetCurrentPtrType();

		//�Y�����I������r�ءA�B��r�ج��Ť�r��
		if( nType = IDM_WORD_REC )
		{
			//�p�G�O�Ť�r�ثh����
			if ( gMyList.CheckIsRemoveNullWordNode(this->_parent->GetHitID()) == TRUE )
			{
				this->_parent->GetCTextOut()->CaretHide();
				this->_parent->GetCSelectRecInfo().SetIsShow(false);
				this->_parent->SetHitID(-1);
				_parent->SetDirectionMouse(CursorDirection_ARROW);
				_parent->GetDrawTool()->MapDCToMonitor();
				return;
			}
		}
		
		/**********************
		**�]�w�T�w����ظ�T
		**********************/
		POINT ptStart, ptEnd;

		ptStart = gMyList.GetCurrentPtrPosBeg();
		ptEnd   = gMyList.GetCurrentPtrPosEnd();


		_parent->GetCSelectRecInfo().SetType(nType);
		_parent->GetCSelectRecInfo().SetIsShow(TRUE);
		_parent->GetCSelectRecInfo().SetPosStart(ptStart);
		_parent->GetCSelectRecInfo().SetPosEnd(ptEnd);

		this->_parent->SetHitID( gMyList.GetCurrentNo() );
		//�]�w���б���
		this->_parent->GetDrawTool()->SetMouseCurrsorInMouseEvent(directionMouse);
		//���ק�Ϯװ_�l�I�ץ��A��K�ƹ����ʮɡA�첾���T
		this->InitialPosFixForDragOrScale(mouseX,mouseY);
	}
}
/** �ƹ�����ø�Ͽ�� */
void CMouseControl::MouseMoveEventForDrawSomething(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	_mouseIsMove = TRUE;
	this->_iWindowMouseMovePosX = LOWORD(lParam);
	this->_iWindowMouseMovePosY = HIWORD(lParam);

	//��l��DC
	HDC hdc = GetDC(hWnd);	
	RECT rectClient;
	::GetClientRect(hWnd,&rectClient);

	HDC hdcMem = CreateCompatibleDC(hdc); 
	HBITMAP bmpMem = CreateCompatibleBitmap(hdc,rectClient.right-rectClient.left , rectClient.bottom-rectClient.top);
	SelectObject(hdcMem, bmpMem); 

	if( !hdcMem )
		MessageBox(hWnd,_T("fatal error"),NULL,NULL);

	//���������ƹ���ЧΪ��A�P�_�ƹ��첾���N�q
	int directionMouse = _parent->GetDirectionMouse();


	/************************************
	* �i��ƹ����ʮɡA�ƹ��y����ɭץ�
	**************************************/

	// ���`�e�ϱ���
	if( directionMouse == CursorDirection_ARROW )
	{
		//�y�в��X�����~�ץ��C
		this->GraphBoundaryPosFix( hWnd, this->_iWindowMouseMovePosX, this->_iWindowMouseMovePosY,TRUE,TRUE,CommonVar::SHIFT_PIXEL);
		//��ø, �N�e��DC �K�� ����DC�W
		this->_parent->GetDrawTool()->MapDcPasterToWindowDC(hdcMem);
	}
	else
	{
		//�ݥثe�ק�Ϫ�����
		int type = gMyList.GetCurrentPtrType();

		// �Y�O�������ܡA�ƹ��y�Хi�ण�O�b��ɤW�A�Ҧ��n��ƹ��y���I�@�ץ�
		if ( this->IsGraphShiftForMouseMoving() == TRUE )
		{
			this->InputPosFixForShiftGraph();
		}
		// �Y��V����
		else if( this->IsGraphShiftSingleDirectionForMouseMoving() == TRUE ) 
		{
			// to do... ���ʶb����b�Y�Ǥ�V���ʡC 
			BOOL Is_ScrollX = FALSE;
			BOOL Is_ScrollY = FALSE;

			//�W�U���ʡA�u�঳Y��Scroll
			if( directionMouse == CursorDirection_N || directionMouse == CursorDirection_S )
				Is_ScrollY = TRUE;
			else
				Is_ScrollX = TRUE;

			//�y�в��X�����~�ץ��C
			this->GraphBoundaryPosFix( hWnd, this->_iWindowMouseMovePosX, this->_iWindowMouseMovePosY,Is_ScrollX,Is_ScrollY,CommonVar::SHIFT_PIXEL);
		}
		// �D���������Ϊ�����ƹ��y�СA�]���y�г��b�ؤW
		else
		{
			//�y�в��X�����~�ץ��C
			this->GraphBoundaryPosFix( hWnd, this->_iWindowMouseMovePosX, this->_iWindowMouseMovePosY,TRUE,TRUE,CommonVar::SHIFT_PIXEL);
		}
	}

	//�ƹ����ʮɡA�ҵe�Ϫ��}�l�y�� = �ƹ����U���y�� -  �۾F�������ʶb�y�Ь۴� �]���b���k�A�Ϯש���)�@ 
	_posBeg.x   = this->_iWindowMouseDownPosX - (_parent->GetScrollBarHandle()->GetHorzPos() -_parent->GetScrollBarHandle()->GetBeforeHorzPos() );
	_posBeg.y   = this->_iWindowMouseDownPosY - (_parent->GetScrollBarHandle()->GetVertPos() -_parent->GetScrollBarHandle()->GetBeforeVertPos() );

	//�M�wø�Ϫ�����
	if(  directionMouse != CursorDirection_ARROW )
	{
		_posEnd.x   = this->_iWindowMouseMovePosX;
		_posEnd.y   = this->_iWindowMouseMovePosY;

		//�ݥثe�ק�Ϫ�����
		int type = gMyList.GetCurrentPtrType();


		// �Y�������Y��V�A�ϧζ}�l�ץ�
		if( this->IsGraphShiftSingleDirectionForMouseMoving() == TRUE )
		{	
			this->TuneGraphForShiftSingleDirectionGraph( type );


			if( type == IDM_WORD_REC )
			{
				this->WordRecBoundaryPosFixForSingleDirectoin();
			}
		}
		//�Y�����N��V�����A�ƹ���m�}�l�ץ�
		else if ( this->IsGraphShiftForMouseMoving() == TRUE )
		{
			this->TuneGraphForShiftiGraph();
		}
		// �I�b�׹﨤�A�i��ƹ���ܮy���ܴ�
		else
		{
			if( type == IDM_WORD_REC )
			{
				this->WordRecBoundaryPosFixForScaling();
			}
			else
				this->ChangeCursorShapeInScaleGraph();   
		}

		// �N�����y���ন�e���y�СA�~��K�ק�e��
		this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos( _posBeg.x,_posBeg.y);
		this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos( _posEnd.x,_posEnd.y);
		
		
		//��X�ثe�e���ϧΩM�s�b��C���ϧΨ⦸�ϧΩҦ��̤p����ΡA�i�� ClipRgn��ø
		POINT recPt1, recPt2, recPt3, recPt4 ;
		recPt1 = _posBeg;
		recPt2 = _posEnd;
		recPt3 = gMyList.GetCurrentPtrPosBeg();
		recPt4 = gMyList.GetCurrentPtrPosEnd();

		ToolKits::FindMaxScopeWithFourPoint( recPt1, recPt2 , recPt3, recPt4);



		//�ק��C�����ؼйϪ���T�A��K���᭫��
		gMyList.ModifyCurrNodeInfo(
			gMyList.GetCurrentPtrType()
			,_posBeg
			,_posEnd
			,gMyList.GetCurrentPenColor()
			,gMyList.GetCurrentFigBg()
			,gMyList.GetCurrentPenWidth());

	


		// �Y�����קאּ��r��ܮءA�h�����ק��r
		if( type == IDM_WORD_REC )
		{
			_parent->GetCTextOut()->CaretHide();
			_parent->GetCTextOut()->WordReputationForInsertingWord(_parent->GetHitID());
		}

	    // �e�����
		this->_parent->GetDrawTool()->FillWhileRec( recPt1, recPt4);
		// ���صe��DC
		this->_parent->GetDrawTool()->RePaintWithClipRgn(recPt1, recPt4);
		//��ø, �N�e��DC �K�� ����DC�W
		this->_parent->GetDrawTool()->MapDcPasterToWindowDC(hdcMem);

		// �N�e���y���ন�����y�СA�~��b����DC�W�e�e
		this->_parent->GetDrawTool()->ScreenToWindowCoordinate( _posBeg.x,_posBeg.y);
		this->_parent->GetDrawTool()->ScreenToWindowCoordinate( _posEnd.x,_posEnd.y);


		// ����DC�W�e�W�K���I
		_parent->GetDrawTool()->FigSenSorDraw( 
			hdcMem
			,gMyList.GetCurrentPtrType()
			,_posBeg.x, _posBeg.y,_posEnd.x, _posEnd.y
			,CommonVar::COLOR_ORANGE);
	}
	else
	{
		_posEnd.x   = this->_iWindowMouseMovePosX;
		_posEnd.y   = this->_iWindowMouseMovePosY;


		_parent->GetDrawTool()->FigSenSorDraw(
			hdcMem
			,_parent->GetSelection()
			, _posBeg.x, _posBeg.y,_posEnd.x, _posEnd.y 
			,CommonVar::COLOR_ORANGE);

		//���e�s�ϡA�ҥH�ݥثe�\���W�I�諸����
		_parent->GetDrawTool()->DrawFig( hdcMem,_parent->GetSelection(),  _posBeg.x, _posBeg.y,_posEnd.x, _posEnd.y,_parent->GetColorSelection(),_parent->GetFigBg(),_parent->GetPenWidth() );

		
	}

	BitBlt(hdc, 0, 0, rectClient.right-rectClient.left, rectClient.bottom-rectClient.top, hdcMem,0,0, SRCCOPY);

	DeleteObject(bmpMem);
	DeleteDC(hdcMem);
	ReleaseDC(hWnd,hdc);

}
/** �ϥդ�r���*/
void CMouseControl::MouseMoveEventForInverseWord(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	this->_parent->GetCTextOut()->CaretHide();

	this->_iWindowMouseMovePosX = LOWORD(lParam);
	this->_iWindowMouseMovePosY = HIWORD(lParam);
	

	//����r�ؤW�U�y��
	MyListNode *ptr = gMyList.visitID_Node( _parent->GetHitID() );
	POINT recPt1 = ptr->GetPtBeg();
	POINT recPt2 = ptr->GetPtEnd();
	ToolKits::Normalization(recPt1,recPt2);
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate(recPt1.x,recPt1.y);
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate(recPt2.x,recPt2.y);

	//�����~�y�Эץ�
	this->GraphBoundaryPosFix(hWnd,this->_iWindowMouseMovePosX,this->_iWindowMouseMovePosY,TRUE,TRUE,CommonVar::SHIFT_PIXEL);

	POINT lastWordPt, iniWordPt;
	ptr->FindIniWordPosInWordRec( iniWordPt );
	ptr->FindLastWordPosInWordRec( lastWordPt );

	POINT pt; 

	//�ˬd���ЬO�_�b��ؤ�

	//��ؤ�
	if( this->_iWindowMouseMovePosX >= recPt1.x && this->_iWindowMouseMovePosX <= recPt2.x 
		&& this->_iWindowMouseMovePosY >= recPt1.y && this->_iWindowMouseMovePosY <= recPt2.y )
	{
		this->_parent->GetCTextOut()->SetNearCaretPosInClickingMouse(this->_iWindowMouseMovePosX,this->_iWindowMouseMovePosY);
		
		pt.x = this->_iWindowMouseMovePosX;
		pt.y = this->_iWindowMouseMovePosY;
		this->_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos( pt.x, pt.y );

		this->_parent->GetCInverseWordInfo().SetCaretPt2(pt);
	}
	//���жW�L��r�W��
	else if( this->_iWindowMouseMovePosY < iniWordPt.y )
	{
		pt.x = iniWordPt.x;
		pt.y = iniWordPt.y;
	
		this->_parent->GetCInverseWordInfo().SetCaretPt2(pt);
	}
	//���жW�L��r�U��
	else if( this->_iWindowMouseMovePosY > lastWordPt.y )
	{
		pt.x = lastWordPt.x;
		pt.y = lastWordPt.y;
	
		this->_parent->GetCInverseWordInfo().SetCaretPt2(pt);
	}

	//��l��DC
	HDC hdc = GetDC(hWnd);	
	RECT rectClient;
	::GetClientRect(hWnd,&rectClient);

	HDC hdcMem = CreateCompatibleDC(hdc); 
	HBITMAP bmpMem = CreateCompatibleBitmap(hdc,rectClient.right-rectClient.left , rectClient.bottom-rectClient.top);
	SelectObject(hdcMem, bmpMem); 

	//��ø, �N�e��DC �K�� ����DC�W
	this->_parent->GetDrawTool()->MapDcPasterToWindowDC(hdcMem);

	this->_parent->GetCTextOut()->InverseTextWordToDC(hdcMem);

	// ����DC�W�e�W�K���I
	_parent->GetDrawTool()->FigSenSorDraw( 
		hdcMem
		,IDM_WORD_REC
		,recPt1.x, recPt1.y,recPt2.x, recPt2.y
		,CommonVar::COLOR_ORANGE);

	BitBlt(hdc, 0, 0, rectClient.right-rectClient.left, rectClient.bottom-rectClient.top, hdcMem,0,0, SRCCOPY);

	DeleteObject(bmpMem);
	DeleteDC(hdcMem);
	ReleaseDC(hWnd,hdc);
}


/**�ƹ������}�I���B�z*/
void CMouseControl::WmLbuttonUpEvent(HWND hWnd,int mouseX,int mouseY)
{	
	// ���u�B��ءB���B��r
	int iType = _parent->GetSelection();
     //���������ƹ���ЧΪ��A�P�_�ƹ��I���N�q
	int directionMouse = _parent->GetDirectionMouse();
	
	// �s�W�ϧ�
    if( directionMouse == CursorDirection_ARROW )
    {
        //������ܫD��r
	    if( iType != IDM_WORD)
	    {
	         //�Ӥp, �]���̤p���j�p
	        if( abs( mouseX - this->_iWindowMouseDownPosX ) <=4 && abs( mouseY - this->_iWindowMouseDownPosY) <=4  )
	        {
	            _posBeg.x = this->_iWindowMouseDownPosX;
			    _posBeg.y = this->_iWindowMouseDownPosY;
			    _posEnd.x = _posBeg.x + 4;
			    _posEnd.y = _posBeg.y + 4;
    		
	        }
	    }
		//��r
	    else
	    {
			//�ˬd��r�جO�_���ŦX�̤p��r�ؤj�p( 5�Ӥ�r �A���k�d�աA �r�P�r������)
	        int minWidth  =  5*_parent->GetCTextOut()->GetCharX() + 2* CommonVar::WORD_REC_MARGIN + 4* CommonVar::WORD_GAP;
	        int minHeight =  _parent->GetCTextOut()->GetCharY()   + 2* CommonVar::WORD_REC_MARGIN;
    	    
	        if( abs(mouseX - this->_iWindowMouseDownPosX) < minWidth || abs(mouseY - this->_iWindowMouseDownPosY) < minHeight)
	        {
	              _posBeg.x = this->_iWindowMouseDownPosX;
                  _posBeg.y = this->_iWindowMouseDownPosY;
                  _posEnd.x =  _posBeg.x + minWidth;
                  _posEnd.y =  _posBeg.y + minHeight;
            		
             
	        }
	    }
    }
    else
    {
          //�I�U�S��} 
          //if( abs( mouseX - this->_iWindowMouseDownPosX ) <=0 && abs( mouseY - this->_iWindowMouseDownPosY) <=0  )
		  if( this->_mouseIsMove == FALSE )
		  {
			  _parent->GetCSelectRecInfo().SetIsShow( true );
			  _parent->GetCSelectRecInfo().SetType( gMyList.GetCurrentPtrType() );
			  return;
	      }
    }
	        

	// �P�w�e���Q�ק�L
	_parent->GetCFileControl()->newChangeWithFile( TRUE );

	// ��������
	if ( this->IsGraphShiftForMouseMoving() == TRUE )
	{	
		//�N�����y�СA�ন�e���y��
		_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos(_posBeg.x,_posBeg.y);
		//�N�����y�СA�ন�e���y��
		_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos(_posEnd.x,_posEnd.y);
	}
	else
	{
		_posBeg.x = this->_iWindowMouseDownPosX;
		_posBeg.y = this->_iWindowMouseDownPosY;


		//�N�����y�СA�ন�e���y��
		_parent->GetDrawTool()->WindowToScreenCoordinateForStartPos(_posBeg.x,_posBeg.y);
		//�N�����y�СA�ন�e���y��
		_parent->GetDrawTool()->WindowToScreenCoordinateForEndPos(_posEnd.x,_posEnd.y);
	}

	// �@��e��
	if( directionMouse == CursorDirection_ARROW )
	{
		if( iType == IDM_WORD  )
		{
		     ////�Y�������@�q��r���_�Y�A�h�����ˬd�W�Ӹ`�I�O�_����r���_�Y (TCHAR) -1 �Acha ==65535
	        if ( gMyList.CheckIsRemoveNullWordNode(this->_parent->GetHitID()) == TRUE )
	        {
		         this->_parent->SetHitID(-1);
	        }
	        
			_parent->GetCTextOut()->AddWordRecIntoQueue(_posBeg,_posEnd);
			_parent->GetDrawTool()->ShowTarGetRecForTypingWord();

		}
		else if( iType != IDM_SELECT)
		{
			//// �x�s�e����T
			gMyList.AddGraphNode( iType
				,_posBeg
				,_posEnd
				,_parent->GetColorSelection()
				,_parent->GetFigBg()
				,_parent->GetPenWidth());


			gAddRedoUndoPaintInfo.AddNewNodeInfo(  gMyList.GetCurrentNo() );
			_parent->SetHitID( gMyList.GetCurrentNo() );

			// �]�w��������ئ�m
			_parent->GetCSelectRecInfo().SetIsShow(TRUE);
			_parent->GetCSelectRecInfo().SetPosStart(_posBeg);
			_parent->GetCSelectRecInfo().SetPosEnd(_posEnd);
			_parent->GetCSelectRecInfo().SetType(iType);
			_parent->GetDrawTool()->DrawFig( _parent->GetMemDC(),iType,_posBeg.x, _posBeg.y, _posEnd.x , _posEnd.y, _parent->GetColorSelection(),_parent->GetFigBg(),_parent->GetPenWidth() );


		}

	}
	//�D�@��e��
	else
	{
		MyListNode *ptr = gMyList.visitID_Node( _parent->GetHitID() );
		
		iType = gMyList.GetCurrentPtrType();
		_parent->GetCSelectRecInfo().SetType(iType);

        // �[�J���ܹϹ��d��LOG
		POINT oldGraphSrcPt1, oldGraphSrcPt2;
		oldGraphSrcPt1 = _parent->GetCSelectRecInfo().GetPosStart();
		oldGraphSrcPt2 = _parent->GetCSelectRecInfo().GetPosEnd();
	
		if( iType == IDM_WORD_REC )
		{
			ToolKits::Normalization( _posBeg, _posEnd );
			//�ק��C�����ؼйϪ���T�A��K���᭫��
			gMyList.ModifyCurrNodeInfo(
				gMyList.GetCurrentPtrType()
				,_posBeg
				,_posEnd
				,gMyList.GetCurrentPenColor()
				,gMyList.GetCurrentFigBg()
				,gMyList.GetCurrentPenWidth());


			// set pos
			POINT caretPt = ptr->FindCurrWordPosInWordRec(caretPt);
			this->_parent->GetDrawTool()->ScreenToWindowCoordinate( caretPt.x, caretPt.y);

			_parent->GetCTextOut()->CaretSetPos(caretPt);
			_parent->GetCTextOut()->CaretShow();

			
			//�Y����r�~�����ܰʤ�r����ܰʪ���T
			if( ptr->GetWordNum() > 0 )
			   gAddRedoUndoPaintInfo.AddModifySizeInfo( _parent->GetHitID(), oldGraphSrcPt1 , oldGraphSrcPt2);
		}
        else
        {
            gAddRedoUndoPaintInfo.AddModifySizeInfo( _parent->GetHitID(), oldGraphSrcPt1 , oldGraphSrcPt2);
        }
		

		// �]�w��������ئ�m
		_parent->GetCSelectRecInfo().SetIsShow(TRUE);
		_parent->GetCSelectRecInfo().SetPosStart(_posBeg);
		_parent->GetCSelectRecInfo().SetPosEnd(_posEnd);
		_parent->GetCSelectRecInfo().SetType(iType);
		
		ptr = NULL;
	}
}

/**��ɮy�Эץ�*/
void CMouseControl::GraphBoundaryPosFix(HWND hWnd, int &iEndPosX, int &iEndPosY,BOOL IS_SCROOLX, BOOL IS_SCROOLY, int offset)
{
	direction x =_parent->GetScrollBarHandle()->IsXScroll(iEndPosX);
	direction y =_parent->GetScrollBarHandle()->IsYScroll(iEndPosY);

	//x ��ɧP�w�ץ�
	if( x == Left )
	{
		iEndPosX = 0;

		if( IS_SCROOLX )
			_parent->GetScrollBarHandle()->HorizontalScrollBarAutoEvent(hWnd, -1* offset);
	}
	else if( x == LeftBoundary )
	{
		iEndPosX = 0;
	}
	else if( x == Right )
	{
		if( IS_SCROOLX )
			_parent->GetScrollBarHandle()->HorizontalScrollBarAutoEvent(hWnd, offset );

		if( iEndPosX >  _parent->GetClientWidth() )
			iEndPosX =  _parent->GetClientWidth();
	}
	else if( x == RightBoundary )
	{

		iEndPosX = _parent->GetClientWidth();
	}

	// y��ɧP�w�ץ�
	if( y == Up )
	{
		iEndPosY = 0;

		if( IS_SCROOLY )
			_parent->GetScrollBarHandle()->VerticalScrollBarAutoEvent(hWnd, -1* offset  );
	}
	else if( y == UpBoundary )
	{
		iEndPosY = 0;
	}
	else if ( y == Down )
	{
		if( IS_SCROOLY )
			_parent->GetScrollBarHandle()->VerticalScrollBarAutoEvent(hWnd, offset );

		if( iEndPosY >  _parent->GetClientHeight() )
			iEndPosY =  _parent->GetClientHeight();

	}
	else if( y == DownBoundary )
	{
		iEndPosY =  _parent->GetClientHeight();
	}
}

void CMouseControl::InverseWordBoundaryFix(HWND hWnd, POINT ptStart, POINT ptEnd, int &iEndPosX, int &iEndPosY, int offset)
{
	direction x =_parent->GetScrollBarHandle()->IsXScroll(iEndPosX);
	direction y =_parent->GetScrollBarHandle()->IsYScroll(iEndPosY);


	//x ��ɧP�w�ץ�
	if( x == Left )
	{
		iEndPosX = 0;

		if( _parent->GetScrollBarHandle()->GetNowScrollBarPos().x - offset >= ptStart.x )
			_parent->GetScrollBarHandle()->HorizontalScrollBarAutoEvent(hWnd, -1* offset);
	}
	else if( x == LeftBoundary )
	{
		iEndPosX = 0;
	}
	else if( x == Right )
	{
		if( _parent->GetScrollBarHandle()->GetNowScrollBarPos().x + offset >= ptEnd.x  )
			_parent->GetScrollBarHandle()->HorizontalScrollBarAutoEvent(hWnd, offset );

		if( iEndPosX >  _parent->GetClientWidth() )
			iEndPosX =  _parent->GetClientWidth();
	}
	else if( x == RightBoundary )
	{

		iEndPosX = _parent->GetClientWidth();
	}

	// y��ɧP�w�ץ�
	if( y == Up )
	{
		iEndPosY = 0;

		if( _parent->GetScrollBarHandle()->GetNowScrollBarPos().y - offset >= ptStart.y )
			_parent->GetScrollBarHandle()->VerticalScrollBarAutoEvent(hWnd, -1* offset  );
	}
	else if( y == UpBoundary )
	{
		iEndPosY = 0;
	}
	else if ( y == Down )
	{
		if( _parent->GetScrollBarHandle()->GetNowScrollBarPos().y + offset <= ptEnd.y  )
			_parent->GetScrollBarHandle()->VerticalScrollBarAutoEvent(hWnd, offset );

		if( iEndPosY >  _parent->GetClientHeight() )
			iEndPosY =  _parent->GetClientHeight();

	}
	else if( y == DownBoundary )
	{
		iEndPosY =  _parent->GetClientHeight();
	}
}

//�P�w�����ƹ����ʨƥ�O�_���ϧΥ���
BOOL CMouseControl::IsGraphShiftForMouseMoving()
{
	int direction = _parent->GetDirectionMouse();

	if( direction == CursorDirection_SIZEALL  )
		return TRUE;
	else
	{
		return FALSE;
	}
}

//�P�w�����ƹ����ʨƥ�O�_���ϧγ��V����
BOOL CMouseControl::IsGraphShiftSingleDirectionForMouseMoving()
{
	int direction = _parent->GetDirectionMouse();

	if( direction == CursorDirection_N 
		|| direction == CursorDirection_W 
		|| direction == CursorDirection_E 
		|| direction == CursorDirection_S )
		return TRUE;
	else
	{
		return FALSE;
	}
}

/**�ץ��Ϯ׮ɡA�_�l�I�y�Эץ�*/
void CMouseControl::InitialPosFixForDragOrScale(int mouseX,int mouseY)
{
	gMyList.visitID_Node( _parent->GetHitID());
	//���o�ثe�ҧ��n�ק諸�ϮסA�b��C����m�����A(���u�B���B���)
	int type = gMyList.GetCurrentPtrType();
	//������Ц������I�W
	int directionMouse = _parent->GetDirectionMouse();

	POINT currNodeStartPt, currNodeEndPt;

	currNodeStartPt.x = gMyList.GetCurrentPtrPosBeg().x;
	currNodeStartPt.y = gMyList.GetCurrentPtrPosBeg().y;
	currNodeEndPt.x   = gMyList.GetCurrentPtrPosEnd().x;
	currNodeEndPt.y   = gMyList.GetCurrentPtrPosEnd().y;

	//�e���y��������y��
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate(currNodeStartPt.x,currNodeStartPt.y);
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate(currNodeEndPt.x,currNodeEndPt.y);

	POINT tarGetPt;  tarGetPt.x = mouseX; tarGetPt.y = mouseY;

	POINT arrTarGetPt[4]; RECT tarGetRect;


	if( directionMouse == CursorDirection_NW ||  directionMouse == CursorDirection_SE || directionMouse == CursorDirection_NE ||  directionMouse == CursorDirection_SW)
	{
		//��ؼХ|���I�A��X��
		arrTarGetPt[0].x = currNodeStartPt.x;   
		arrTarGetPt[0].y = currNodeStartPt.y;

		arrTarGetPt[1].x = currNodeEndPt.x;
		arrTarGetPt[1].y = currNodeEndPt.y;

		arrTarGetPt[2].x = currNodeEndPt.x;
		arrTarGetPt[2].y = currNodeStartPt.y;

		arrTarGetPt[3].x = currNodeStartPt.x;
		arrTarGetPt[3].y = currNodeEndPt.y;


		//���I�ƹ��O�_�R�����I
		for( int iNumPt = 0 ; iNumPt < 4 ; iNumPt++ )
		{
			tarGetRect.left   = arrTarGetPt[iNumPt].x - CommonVar::SMALL_RECT_BUFFER;
			tarGetRect.right  = arrTarGetPt[iNumPt].x + CommonVar::SMALL_RECT_BUFFER;
			tarGetRect.top    = arrTarGetPt[iNumPt].y - CommonVar::SMALL_RECT_BUFFER;
			tarGetRect.bottom = arrTarGetPt[iNumPt].y + CommonVar::SMALL_RECT_BUFFER;

			//�ƹ���ЩR���I����C�����Ϯת��_�I�A���﨤�u���( �_�l�I ��� )
			if( ::PtInRect(&tarGetRect, tarGetPt ) == TRUE )
			{
				switch( iNumPt )
				{
				case 0:
					this->_iWindowMouseDownPosX = currNodeEndPt.x;
					this->_iWindowMouseDownPosY = currNodeEndPt.y;
					break;
				case 1:
					this->_iWindowMouseDownPosX = currNodeStartPt.x;
					this->_iWindowMouseDownPosY = currNodeStartPt.y;
					break;
				case 2:
					this->_iWindowMouseDownPosX = currNodeStartPt.x;
					this->_iWindowMouseDownPosY = currNodeEndPt.y;
					break;
				case 3:
					this->_iWindowMouseDownPosX = currNodeEndPt.x;
					this->_iWindowMouseDownPosY = currNodeStartPt.y;
					break;
				}
				break;
			}
		}
	}
	//�w��ƹ��C�Ь��W�U
	else if ( directionMouse == CursorDirection_N ||  directionMouse == CursorDirection_S)
	{
		//�ƹ��y�Ф���a�ؼйϮת��_�I�A�W�U�_�l�I�ﴫ
		if( abs(tarGetPt.y - currNodeEndPt.y) >  abs(tarGetPt.y - currNodeStartPt.y) )
		{
			this->_iWindowMouseDownPosX = currNodeEndPt.x;
			this->_iWindowMouseDownPosY = currNodeEndPt.y;

			if( type == IDM_CIR )
				this->_parent->GetDrawTool()->GetDrawCircular()->SetEndPosForFixingPoint(currNodeStartPt);
			else if ( type == IDM_LINE )
				this->_parent->GetDrawTool()->GetDrawLine()->SetEndPosForFixingPoint(currNodeStartPt);
			else if ( type == IDM_REC)
				this->_parent->GetDrawTool()->GetDrawRec()->SetEndPosForFixingPoint(currNodeStartPt);
			else if( type == IDM_WORD_REC )
				this->_parent->GetDrawTool()->GetDrawRec()->SetEndPosForFixingPoint(currNodeStartPt);
		}
		else
		{
			this->_iWindowMouseDownPosX = currNodeStartPt.x;
			this->_iWindowMouseDownPosY = currNodeStartPt.y;

			if( type == IDM_CIR )
				this->_parent->GetDrawTool()->GetDrawCircular()->SetEndPosForFixingPoint(currNodeEndPt);
			else if ( type == IDM_LINE )
				this->_parent->GetDrawTool()->GetDrawLine()->SetEndPosForFixingPoint(currNodeEndPt);
			else if ( type == IDM_REC)
				this->_parent->GetDrawTool()->GetDrawRec()->SetEndPosForFixingPoint(currNodeEndPt);
			else if( type == IDM_WORD_REC )
				this->_parent->GetDrawTool()->GetDrawRec()->SetEndPosForFixingPoint(currNodeEndPt);

		}
	}
	//�w��ƹ���Х��k
	else if ( directionMouse == CursorDirection_W || directionMouse == CursorDirection_E)
	{
	
		//�ƹ��y�Ф���a�ؼйϮת��_�I�A���k�_�l�I�ﴫ
		if( abs(tarGetPt.x - currNodeEndPt.x) >  abs(tarGetPt.x - currNodeStartPt.x) )
		{
			this->_iWindowMouseDownPosX =  currNodeEndPt.x ;
			this->_iWindowMouseDownPosY =  currNodeEndPt.y ;

			if( type == IDM_CIR )
				this->_parent->GetDrawTool()->GetDrawCircular()->SetEndPosForFixingPoint(currNodeStartPt);
			else if ( type == IDM_LINE )
				this->_parent->GetDrawTool()->GetDrawLine()->SetEndPosForFixingPoint(currNodeStartPt);
			else if ( type == IDM_REC)
				this->_parent->GetDrawTool()->GetDrawRec()->SetEndPosForFixingPoint(currNodeStartPt);
			else if( type == IDM_WORD_REC )
				this->_parent->GetDrawTool()->GetDrawRec()->SetEndPosForFixingPoint(currNodeStartPt);
		}
		else
		{
			this->_iWindowMouseDownPosX = currNodeStartPt.x;
			this->_iWindowMouseDownPosY = currNodeStartPt.y;


			if( type == IDM_CIR )
				this->_parent->GetDrawTool()->GetDrawCircular()->SetEndPosForFixingPoint(currNodeEndPt);
			else if ( type == IDM_LINE )
				this->_parent->GetDrawTool()->GetDrawLine()->SetEndPosForFixingPoint(currNodeEndPt);
			else if ( type == IDM_REC)
				this->_parent->GetDrawTool()->GetDrawRec()->SetEndPosForFixingPoint(currNodeEndPt);
			else if( type == IDM_WORD_REC )
				this->_parent->GetDrawTool()->GetDrawRec()->SetEndPosForFixingPoint(currNodeEndPt);
		}


	}

	//�����A��t�q�A�ƹ��y���I�M���I���Z���t
	else
	{  
	 

		_iDisX_Cursor_PosStart = abs(tarGetPt.x - currNodeStartPt.x);
		_iDisY_Cursor_PosStart = abs(tarGetPt.y - currNodeStartPt.y);
		_iDisX_Cursor_PosEnd = abs(currNodeEndPt.x - tarGetPt.x);
		_iDisY_Cursor_PosEnd = abs(currNodeEndPt.y - tarGetPt.y);
	}
}

/** �I������*/
void CMouseControl::IsHitGraphAndChangeCursorShape(int mouseX, int mouseY)
{
	POINT tarGetPt; 
	tarGetPt.x = mouseX;
	tarGetPt.y = mouseY;

	//��C����
	if( gMyList.GetTotalNum() < 0 )
		return;

	//����ܿ��
	if( _parent->GetSelection() == IDM_SELECT )
	{
		//�s�b�ؼйϧΪ���
		if( _parent->GetHitID() >= 0 )
		{
			int ID = _parent->GetHitID();

			MyListNode* ptr = gMyList.visitID_Node( _parent->GetHitID() );

			if( !ptr )
			{
				::OutputDebugString(_T("  �e�������s�b����H \n"));
			}
			else if( _parent->GetCHitTest()->PtHitTestRegion(ID, tarGetPt, ptr->GetPtBeg() , ptr->GetPtEnd()  ) == TRUE )
			{
				this->_parent->GetDrawTool()->CurrentSelectRecInfoShow(_parent->GetHitID());
				return ;
			}

		}
	
		int ID;
		MyListNode* last = gMyList.GetLast();

		//�q��C�᭱�}�l��
		for( ; last != NULL ; last = last->_llink )
		{
			ID = last->GetNo();

			if( _parent->GetCHitTest()->PtHitTestRegion(ID,tarGetPt, last->GetPtBeg(), last->GetPtEnd() ) == TRUE )
			{
				this->_parent->GetDrawTool()->CurrentSelectRecInfoShow(ID);
				break;
			}
			//�̫ᤴ�M�S���
			else if( last->_llink == NULL )
			{
				this->_parent->GetDrawTool()->MapDCToMonitor();
			}
		}
	}
	// �@�뱡�p�u�ˬd��C�̫�@��( �e���s����A���ʷƹ�)
	else if( _parent->GetHitID() >= 0 )
	{
		int ID = _parent->GetHitID();

		MyListNode* ptr = gMyList.visitID_Node( _parent->GetHitID() );

        if( !ptr )
        {
             ::OutputDebugString(_T("  �e�������s�b����H \n"));
        }
		else if( _parent->GetCHitTest()->PtHitTestRegion(ID, tarGetPt, ptr->GetPtBeg() , ptr->GetPtEnd()  ) == TRUE )
		{
			this->_parent->GetDrawTool()->CurrentSelectRecInfoShow(_parent->GetHitID());
		}

	}
}

/**�Ϲ������ɡA�ƹ��y�Ъ��վ�*/
void CMouseControl::InputPosFixForShiftGraph()
{

	POINT currNodeStartPt, currNodeEndPt;
	gMyList.visitID_Node( _parent->GetHitID());
	currNodeStartPt.x = gMyList.GetCurrentPtrPosBeg().x;
	currNodeStartPt.y = gMyList.GetCurrentPtrPosBeg().y;
	currNodeEndPt.x   = gMyList.GetCurrentPtrPosEnd().x;
	currNodeEndPt.y   = gMyList.GetCurrentPtrPosEnd().y;



	//�e���y��������y��
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate(currNodeStartPt.x,currNodeStartPt.y);
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate(currNodeEndPt.x,currNodeEndPt.y);

	//y��V���U
	if( this->_iWindowMouseMovePosY >=  this->_iWindowMouseDownPosY )
	{   
		/*OutputDebugString(_T("---detecion y��V���U\n"));*/
		if( currNodeEndPt.y >= currNodeStartPt.y )
			this->_iWindowMouseMovePosY += this->_iDisY_Cursor_PosEnd;
		else
			this->_iWindowMouseMovePosY += this->_iDisY_Cursor_PosStart;
	}
	//y��V���W
	else
	{   
		//OutputDebugString(_T("---detecion y��V���W\n"));
		if( currNodeEndPt.y >= currNodeStartPt.y )
			this->_iWindowMouseMovePosY -= this->_iDisY_Cursor_PosStart;
		else
			this->_iWindowMouseMovePosY -= this->_iDisY_Cursor_PosEnd;
	}

	//x��V���k
	if( this->_iWindowMouseMovePosX >=  this->_iWindowMouseDownPosX )
	{
		//OutputDebugString(_T("---detecion x��V���k\n"));
		if( currNodeEndPt.x >= currNodeStartPt.x )
			this->_iWindowMouseMovePosX += this->_iDisX_Cursor_PosEnd;
		else
			this->_iWindowMouseMovePosX += this->_iDisX_Cursor_PosStart;
	}
	else
	{
		//OutputDebugString(_T("---detecion x��V����\n"));
		if( currNodeEndPt.x >= currNodeStartPt.x )
			this->_iWindowMouseMovePosX -= this->_iDisX_Cursor_PosStart;
		else
			this->_iWindowMouseMovePosX -= this->_iDisX_Cursor_PosEnd;
	}

	if(   this->_iWindowMouseMovePosX < 0 )
		this->_iWindowMouseMovePosX =  CommonVar::OUT_WINDOW_CONSTANT+1;
	if (  this->_iWindowMouseMovePosY < 0 )
		this->_iWindowMouseMovePosY =  CommonVar::OUT_WINDOW_CONSTANT+1 ;
	//�y�в��X�����~�ץ��C
	this->GraphBoundaryPosFix( _parent->GetMainHwnd(), this->_iWindowMouseMovePosX, this->_iWindowMouseMovePosY,TRUE,TRUE,CommonVar::SHIFT_PIXEL);
}

/**�Ϲ������ɡA�Ϲ����վ�*/
void CMouseControl::TuneGraphForShiftiGraph()
{
	// OutputDebugString(_T("/�Y�����N��V�����A�ƹ���m�}�l�ץ�\n"));

	gMyList.visitID_Node( _parent->GetHitID());

	POINT posEndPt = gMyList.GetCurrentPtrPosEnd();
	_parent->GetDrawTool()->ScreenToWindowCoordinate(posEndPt.x,posEndPt.y);

	POINT posStartPt = gMyList.GetCurrentPtrPosBeg();
	_parent->GetDrawTool()->ScreenToWindowCoordinate(posStartPt.x,posStartPt.y);

	


	//���l�ϧΰ_�l�I�y�Юt
	int _disX = abs(posEndPt.x - posStartPt.x);
	int _disY = abs(posEndPt.y - posStartPt.y);


	//y��V���U
	if( this->_iWindowMouseMovePosY >=  this->_iWindowMouseDownPosY )
	{   

		if( posEndPt.y >= posStartPt.y )
		{
			//   OutputDebugString(_T("y��V���U�A�_�I�b���I�W��\n"));
			_posEnd.y = this->_iWindowMouseMovePosY;
			_posBeg.y = _posEnd.y - _disY;
		}
		else
		{
			//  OutputDebugString(_T("y��V���U�A�_�I�b���I�U��\n"));
			_posBeg.y = this->_iWindowMouseMovePosY;
			_posEnd.y = _posBeg.y - _disY;
		}
	}
	//y��V���W
	else
	{   
		if( posEndPt.y >= posStartPt.y )
		{
			//  OutputDebugString(_T("y��V���W�A�_�I�b���I�W��\n"));
			_posBeg.y = this->_iWindowMouseMovePosY;
			_posEnd.y = _posBeg.y + _disY;
		}
		else
		{
			//  OutputDebugString(_T("y��V���W�A�_�I�b���I�U��\n"));
			_posEnd.y = this->_iWindowMouseMovePosY;
			_posBeg.y = _posEnd.y + _disY;
		}
	}

	//x��V���k
	if( this->_iWindowMouseMovePosX >=  this->_iWindowMouseDownPosX )
	{

		if( posEndPt.x >= posStartPt.x )
		{

			_posEnd.x = this->_iWindowMouseMovePosX;
			_posBeg.x = _posEnd.x - _disX;
		}
		else
		{

			_posBeg.x = this->_iWindowMouseMovePosX;
			_posEnd.x = _posBeg.x - _disX;
		}
	}
	else
	{
		if( posEndPt.x >= posStartPt.x )
		{
			//   OutputDebugString(_T("x��V�����A�_�I�b���I�k��\n"));
			_posBeg.x = this->_iWindowMouseMovePosX;
			_posEnd.x = _posBeg.x + _disX;
		}
		else
		{
			// OutputDebugString(_T("x��V�����A�_�I�b���I����\n"));
			_posEnd.x = this->_iWindowMouseMovePosX;
			_posBeg.x = _posEnd.x + _disX;
		}
	}
	
}

/** �Ϲ���j�Y�p�ɡA���ЧΪ��ܴ�*/
void CMouseControl::ChangeCursorShapeInScaleGraph()
{
	//�ثe�ƹ��y�Цb�_�I�y�Ъ��k��
	if( this->_posEnd.x > _posBeg.x )
	{
		//��4�H��
		if( this->_posEnd.y < _posBeg.y )
		{
			_parent->GetDrawTool()->SetMouseCurrsorInMouseEvent(CursorDirection_NE);
		}
		//��1�H��
		else
		{

			_parent->GetDrawTool()->SetMouseCurrsorInMouseEvent(CursorDirection_SE);
		}
	}
	//.. ����
	else
	{
		//��3�H��
		if( this->_posEnd.y < _posBeg.y )
		{
			_parent->GetDrawTool()->SetMouseCurrsorInMouseEvent(CursorDirection_NW);
		}
		//��2�H��
		else
		{
			_parent->GetDrawTool()->SetMouseCurrsorInMouseEvent(CursorDirection_SW);
		}
	}
}

/**�Ϲ����V���ʮɡA�Ϲ����վ�*/
void CMouseControl::TuneGraphForShiftSingleDirectionGraph(int type)
{
	//OutputDebugString(_T("/�Y�������Y��V�A�ƹ���m�}�l�ץ�\n"));

	switch( type )
	{
	case IDM_LINE:
		_parent->GetDrawTool()->GetDrawLine()->FixPosForSingleMovingGraph( _parent->GetDirectionMouse());
	
		break;
	case IDM_REC:
		_parent->GetDrawTool()->GetDrawRec()->FixPosForSingleMovingGraph( _parent->GetDirectionMouse());
	
		break;
	case IDM_CIR:
		_parent->GetDrawTool()->GetDrawCircular()->FixPosForSingleMovingGraph( _parent->GetDirectionMouse());

		break;
	case IDM_WORD_REC:
		_parent->GetDrawTool()->GetDrawRec()->FixPosForSingleMovingGraph( _parent->GetDirectionMouse());
		break;
	}
}

/**��r����س��V�첾�ɡA�ƹ��y�Эץ�*/
void CMouseControl::WordRecBoundaryPosFixForSingleDirectoin()
{

	int xBuffer = 2*CommonVar::WORD_REC_MARGIN + _parent->GetCTextOut()->GetCharX() * 5;

	MyListNode* ptr = gMyList.visitID_Node(_parent->GetHitID());
	POINT queuePtBeg = ptr->GetPtBeg();
	POINT queuePtEnd = ptr->GetPtEnd();
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate( queuePtBeg.x, queuePtBeg.y );
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate( queuePtEnd.x, queuePtEnd.y );

	int direction = _parent->GetDirectionMouse();
	if( direction == CursorDirection_E  || direction == CursorDirection_W)
	{

		if( direction == CursorDirection_E )
		{
			if( _posEnd.x < _posBeg.x + xBuffer )
				_posEnd.x = _posBeg.x + xBuffer;
		}
		else if( direction == CursorDirection_W )
		{
			if( _posEnd.x > _posBeg.x - xBuffer )
				_posEnd.x = _posBeg.x - xBuffer;
		}
		
	}

	else if( direction == CursorDirection_N )
	{
		int yBuffer = _parent->GetCTextOut()->GetCharY();

		if( _posEnd.y > _posBeg.y - yBuffer )
			_posEnd.y = _posBeg.y - yBuffer;
	}
	else 
	{
		int yBuffer = _parent->GetCTextOut()->GetCharY();
		if( _posEnd.y < _posBeg.y + yBuffer )
			_posEnd.y = _posBeg.y + yBuffer;
	}

	BOOL answer = this->_parent->GetCTextOut()->IsChangeThisWordRecScope( _parent->GetHitID() , _posBeg,_posEnd);
	
	if( answer == FALSE )
	{
		if( direction == CursorDirection_N )
		{
			_posEnd.y = queuePtBeg.y;
		}
		else if( direction == CursorDirection_W )
		{
			_posEnd.x = queuePtBeg.x;
		
		}
		else if( direction == CursorDirection_S )
		{
			_posEnd.y = queuePtEnd.y;
		}
		else
		{
			_posEnd.x = queuePtEnd.x;
		}
	}
	ptr = NULL;
}


/** ��r�خy�Эץ��A�bscale���p�U*/
void CMouseControl::WordRecBoundaryPosFixForScaling()
{

	MyListNode* ptr =  gMyList.visitID_Node(_parent->GetHitID());
	POINT queuePtBeg = ptr->GetPtBeg();
	POINT queuePtEnd = ptr->GetPtEnd();
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate( queuePtBeg.x, queuePtBeg.y );
	this->_parent->GetDrawTool()->ScreenToWindowCoordinate( queuePtEnd.x, queuePtEnd.y );

	int xBuffer = 2*CommonVar::WORD_REC_MARGIN + 8 * 5;
	int yBuffer = _parent->GetCTextOut()->GetCharY();

	
	int direction = _parent->GetDirectionMouse();

	// �|���I��ɧP�_
	if( direction == CursorDirection_SE  )
	{
		if( _posEnd.x < _posBeg.x + xBuffer )
			_posEnd.x = _posBeg.x + xBuffer;

		if( _posEnd.y < _posBeg.y +yBuffer)
		{
			_posEnd.y = _posBeg.y +yBuffer;
		}

	}
	else if( direction == CursorDirection_SW )
	{
		if( _posEnd.x > _posBeg.x - xBuffer)
			_posEnd.x = _posBeg.x - xBuffer;

		if( _posEnd.y < _posBeg.y + yBuffer)
			_posEnd.y = _posBeg.y + yBuffer;
	}
	else if( direction == CursorDirection_NW )
	{
		if( _posEnd.x > _posBeg.x - xBuffer )
			_posEnd.x = _posBeg.x - xBuffer ;

		if( _posEnd.y > _posBeg.y -yBuffer )
			_posEnd.y = _posBeg.y -yBuffer ;
	}
	else
	{
		if( _posEnd.x < _posBeg.x + xBuffer )
			_posEnd.x = _posBeg.x + xBuffer ;

		if( _posEnd.y > _posBeg.y - yBuffer )
			_posEnd.y = _posBeg.y - yBuffer ;
	}

	int diff_x = abs( _posEnd.x - _posBeg.x );
	int diff_y = abs( _posEnd.y - _posBeg.y );

	BOOL answer = this->_parent->GetCTextOut()->IsChangeThisWordRecScope( _parent->GetHitID() , _posBeg,_posEnd);
	
	if( answer == FALSE )
	{
		if( direction == CursorDirection_NE )
		{
			_posBeg.x = queuePtBeg.x;
			_posBeg.y = queuePtEnd.y;

			_posEnd.x = queuePtEnd.x;
			_posEnd.y = queuePtBeg.y;
		}
		else if( direction == CursorDirection_NW )
		{
			_posBeg = queuePtEnd;
			_posEnd = queuePtBeg;
		}
		else if( direction == CursorDirection_SW )
		{
		    _posEnd.x = queuePtBeg.x;
			_posEnd.y = queuePtEnd.y;

			_posBeg.x = queuePtEnd.x;
			_posBeg.y = queuePtBeg.y;
		}
		else
		{
			_posBeg = queuePtBeg;
			_posEnd = queuePtEnd;
		}
	}
		
	ptr = NULL;
}
/************************
*  Getter // setter 
***************************/

void CMouseControl::SetThisTurnPosEndX(LONG x)
{
	_posEnd.x = x;
}

void CMouseControl::SetThisTurnPosEndY(LONG y)
{
	_posEnd.y = y;
}