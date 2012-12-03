#include "StdAfx.h"
#include "RedoUndoController.h"
#include "DebugMsgOutputHandle.h"
#include "Resource.h"
#include "Memento.h"
#include <assert.h>


RedoUndoController::RedoUndoController(Originator* originator):_originator(originator)
{
	//_IsUndo = FALSE;
	_index = 0;
}


RedoUndoController::~RedoUndoController(void)
{
}

void RedoUndoController::Record()
{
	
	//Undo ��e���s��A�M��redo����
	Memento *ptr = NULL;

	int size = (int)_memoList.size();

	//undo��S�s�W�����A�}�l�R��
	while ( size > _index+1 )
	{
		ptr = _memoList[ _memoList.size() - 1];
		delete ptr;
		_memoList.pop_back();
        
        size = (int)_memoList.size();
	}
	
	_memoList.push_back( _originator->CreateMemento() ); 
	_index = _memoList.size()-1;
	ptr = NULL;
	//this->TraceAllRecord();
}

//����
void RedoUndoController::Redo()
{
	//OutputDebugString(_T("------------Redo\n"));


	for( int i = _memoList.size()-1 ; i >= 0 ; i -- )
	{
		if( i == _index+1 )
		{
	    	
			Memento* pMemo = _memoList[i]; 
			_originator->SetRedoUndoFlag(1);
			_originator->SetMementoState(pMemo);
	        _index ++;
			break;
		}
	}
	//this->TraceAllRecord();

}

//�_��
void RedoUndoController::Undo()
{
	//OutputDebugString(_T("------------Undo\n"));
	
	for( int i = _memoList.size()-1 ; i >= 0 ; i -- )
	{
		if( i == _index )
		{
			_index -- ;
			Memento* pMemo = _memoList[i]; 
			_originator->SetRedoUndoFlag(0);
			_originator->SetMementoState(pMemo);
			
			break;
		}
	}
	//this->TraceAllRecord();
}

//���s�]�w
void RedoUndoController::Reset()
{
	  vector<Memento*>::iterator iter = _memoList.begin(); 
      for (; iter != _memoList.end(); ++iter) 
      { 
            Memento* pMemo = *iter; 
            delete pMemo; 
      } 
	  _memoList.clear(); 

	  _index = 0;
}

//�����̤W�@�Ӥ�r��
void RedoUndoController::PopFinalWord()
{
	if( _memoList.size() <= 0 ) return ;

	RecordObj *pObj = _memoList[ _memoList.size()-1 ]->GetState();

	if( pObj->nObjType == IDM_WORD_REC )
	{
		_memoList.pop_back();
	}
	
}

int RedoUndoController::GetFinalID()
{
	if( _memoList.size() <= 0 ) 
	   return -1;
	RecordObj *pObj = _memoList[ _memoList.size()-1 ]->GetState();
	return pObj->ID;
}

int RedoUndoController::GetLastCmd()
{
	if( _memoList.size() <= 0 )
		return -1;
	RecordObj *pObj = _memoList[ _memoList.size()-1 ]->GetState();
	return pObj->Command;
}

// �O�_�୫�� (�N�_��L�{�٭�)
BOOL RedoUndoController::IsRedo()
{
	if( _memoList.size() == 0 )
		return FALSE;
	else if( _index == _memoList.size()  )
		return FALSE; 
	else
		return TRUE;
}

//�O�_��_��
BOOL RedoUndoController::IsUndo()
{
	if( _memoList.size() == 0 )
		return FALSE; 
	else if( _index < 0 )
		return FALSE;
	else
		return TRUE;
}


void RedoUndoController::TraceAllRecord()
{
	OutputDebugString(_T("------------TraceAllRecord\n"));
	  vector<Memento*>::iterator iter = _memoList.begin(); 

	  for( int i = 0 ; i < (int)_memoList.size() ; i++ )
	  {
		  if( i == _index )
			  OutputDebugString(_T("hit  *"));
		  Memento* pMemo = _memoList[i]; 
		  DebugMsgOutputHandle::Instance()->SetRedoUndoFormat(pMemo->GetState()->Command,pMemo->GetState()->ID,pMemo->GetState()->ptBeg, pMemo->GetState()->vecOriCharacater.size(), pMemo->GetState()->vecCharacter.size() );
		  OutputDebugString(DebugMsgOutputHandle::Instance()->GetSzDebugBuffer());
	  } 
}