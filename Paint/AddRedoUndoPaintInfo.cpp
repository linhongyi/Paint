#include "StdAfx.h"
#include "AddRedoUndoPaintInfo.h"
#include "MyListController.h"
#include "SgWordDataModelControler.h"
#include "Resource.h"
#include "Originator.h"
#include "RedoUndoController.h"
#include <assert.h>
#include "PaintInfo.h"

extern PaintInfo        gPaintInfo;
extern RedoUndoController *gRedoUndoController;
extern Originator       *gOriginator;
extern MyListController gMyList;


AddRedoUndoPaintInfo::AddRedoUndoPaintInfo(void)
{
}


AddRedoUndoPaintInfo::~AddRedoUndoPaintInfo(void)
{
}


/*************
* input 還未變形前的位置
**************/
void AddRedoUndoPaintInfo::AddModifySizeInfo(int ID,POINT posBeg,POINT posEnd)
{
	RecordObj *data = new RecordObj();
	::SecureZeroMemory(data, sizeof(RecordObj));

	gMyList.visitID_Node(ID);
	gMyList.CurrDataToRecordData( data );

	if( data->nObjType != IDM_WORD_REC )
	{
		data->Command = COMMAND_FIX_GRAPH;

	}
	else
	{

		data->Command = COMMAND_CHANGE_WORD_REC;
		SgWordDataModelControler::Instance()->SaveAllWordnThisWordRecToRecordObj( gMyList.GetCurrentNo() , data); 
		gPaintInfo.SetAction(TRUE);
	}


	data->ptBeg = posBeg;
	data->ptEnd = posEnd;
	gOriginator->SetRecordObj(data);
	gRedoUndoController->Record();
	data = NULL;


}

void AddRedoUndoPaintInfo::AddModifyPenColorInfo(int penColor)
{
	RecordObj *data = new RecordObj();
	::SecureZeroMemory(data, sizeof(RecordObj));

	gMyList.CurrDataToRecordData( data );

if( data->nObjType != IDM_WORD_REC )
	{
		data->Command = COMMAND_FIX_GRAPH;
		gOriginator->SetRecordObj(data);
	}
	else
	{
		data->Command = COMMAND_FIX_COLOR_WORD;
	
	}

	data->iNewPenColor = penColor;
	gOriginator->SetRecordObj(data);
	gRedoUndoController->Record();
	data = NULL;

	gPaintInfo.SetAction(TRUE);
}

void AddRedoUndoPaintInfo::AddModifyPenWidthInfo(int penWidth)
{
	RecordObj *data = new RecordObj();
	::SecureZeroMemory(data, sizeof(RecordObj));

	gMyList.CurrDataToRecordData( data );

	data->Command = COMMAND_FIX_GRAPH;

	data->iNewPenWidth = penWidth;

	gOriginator->SetRecordObj(data);
	gRedoUndoController->Record();
	data = NULL;

	gPaintInfo.SetAction(TRUE);
}

void AddRedoUndoPaintInfo::AddModifyBgColorInfo(int bgColor)
{
	RecordObj *data = new RecordObj();
	::SecureZeroMemory(data, sizeof(RecordObj));

	gMyList.CurrDataToRecordData( data );

	if( data->nObjType != IDM_WORD_REC )
	{
		data->Command = COMMAND_FIX_GRAPH;
	}
	else
	{
		data->Command = COMMAND_FIX_BGCOLOR_WORD;
	}

	data->iNewFigBg = bgColor;
	gOriginator->SetRecordObj(data);
	gRedoUndoController->Record();
	data = NULL;

	gPaintInfo.SetAction(TRUE);
}

void AddRedoUndoPaintInfo::AddDeleteNodeInfo(int ID)
{
	//儲存Log
	RecordObj *data = new RecordObj();
	::SecureZeroMemory(data, sizeof(RecordObj));

	MyListNode* ptr = gMyList.visitID_Node(ID);
	gMyList.CurrDataToRecordData( data );
	
	if( data->nObjType != IDM_WORD_REC )
	{
		data->Command = COMMAND_DELETE_GRAPH;
	
	}
	else 
	{
		data->Command = COMMAND_DELETE_WORD;
		SgWordDataModelControler::Instance()->SaveAllWordnThisWordRecToRecordObj( gMyList.GetCurrentNo() , data); 
	}
		
	gOriginator->SetRecordObj(data);
	gRedoUndoController->Record();

	gPaintInfo.SetAction(TRUE);
	data = NULL;
	ptr = NULL;
}

void AddRedoUndoPaintInfo::AddNewNodeInfo(int ID)
{
	//儲存Log
	RecordObj *data = new RecordObj();
	::SecureZeroMemory(data, sizeof(RecordObj));

	gMyList.visitID_Node(ID);
	gMyList.CurrDataToRecordData( data );

	if( data->nObjType != IDM_WORD_REC )
	{
		data->Command = COMMAND_DRAW_NEW_GRAPH;
	}
	else
	{
		data->Command = COMMAND_ADD_WORD;
		
		SgWordDataModelControler::Instance()->SaveAllWordnThisWordRecToRecordObj( gMyList.GetCurrentNo() , data); 
	}
	gOriginator->SetRecordObj(data);
	gRedoUndoController->Record();
	data = NULL;

	gPaintInfo.SetAction(TRUE);
}



void AddRedoUndoPaintInfo::AddEditWordNodeInfo(int ID)
{
	//儲存Log
	RecordObj *data = new RecordObj();
	::SecureZeroMemory(data, sizeof(RecordObj));

	gMyList.visitID_Node(ID);
	gMyList.CurrDataToRecordData( data );
	
	data->Command = COMMAND_EDIT_WORD;
	SgWordDataModelControler::Instance()->SaveAllWordnThisWordRecToRecordObj( ID , data); 
    SgWordDataModelControler::Instance()->IniVectorCharacterToRecordObj( data );
    
  
	gOriginator->SetRecordObj(data);
	gRedoUndoController->Record();
	data = NULL;
}