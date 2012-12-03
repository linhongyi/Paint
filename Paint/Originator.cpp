#include "StdAfx.h"
#include "Originator.h"
#include "CommonVar.h"

#include "PaintInfo.h"
#include "Memento.h"
#include "SgWordDataModelControler.h"
#include "Invoker.h"

extern PaintInfo        gPaintInfo;
extern CInvoker         gCInvoer;
Originator::Originator(void)
{
	this->_iRedoUndoFlag = 0;
}


Originator::~Originator(void)
{
}

Memento* Originator::CreateMemento()
{
	return new Memento(_recordObj); 
}

void Originator::SetMementoState(Memento *pMem)
{
	this->_recordObj = pMem->GetState();

    gPaintInfo.SetDirectionMouse(CursorDirection_ARROW);
	::SetCursor( LoadCursor(NULL,IDC_ARROW)); 

	//²¾°£ªÅ¤å¦r®Ø
	if ( gMyList.CheckIsRemoveNullWordNode(gPaintInfo.GetHitID()) == TRUE )
	{
		gPaintInfo.GetCSelectRecInfo().SetIsShow(false);
		gPaintInfo.SetHitID(-1);
		gPaintInfo.GetCTextOut()->CaretHide();
	}

	// undo
	if( this->_iRedoUndoFlag == 0 )
	{
		if(  _recordObj->Command != COMMAND_FIX_GRAPH )
			gCInvoer.GetRedoUndoCmd(COMMAND_NEW_TRAGET_GRAPH_SEL).UnExecute( _recordObj );
	    gCInvoer.GetRedoUndoCmd(_recordObj->Command ).UnExecute( _recordObj );
	}
	//redo
	else
	{
		if(   _recordObj->Command != COMMAND_FIX_GRAPH )
			gCInvoer.GetRedoUndoCmd(COMMAND_NEW_TRAGET_GRAPH_SEL).Execute( _recordObj );
	    gCInvoer.GetRedoUndoCmd(_recordObj->Command ).Execute( _recordObj );
	}
	
}

/* ¬ö¿ýlog*/
void Originator::SetRecordObj(RecordObj *recordObj)
{
	_recordObj = recordObj;
}

/** 0 Undo 1 Redo*/
void Originator::SetRedoUndoFlag( int val )
{
	this->_iRedoUndoFlag = val;
}
