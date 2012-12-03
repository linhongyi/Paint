#include "StdAfx.h"
#include "AddNewGraphCmd.h"


AddNewGraphCmd::AddNewGraphCmd(void)
{
}


AddNewGraphCmd::~AddNewGraphCmd(void)
{
}

void AddNewGraphCmd::Execute(RecordObj * recordObj)
{
	InsertAction insertAction  = gMyList.InsertGraphIDNode(
		recordObj->ID
		,recordObj->nObjType
		,recordObj->ptBeg
		,recordObj->ptEnd
		,recordObj->iPenColor
		,recordObj->iFigBg
		,recordObj->iPenWidth);


	if( insertAction == InsertNone )

	{
		::MessageBox(gPaintInfo.GetMainHwnd(),_T(" redo Fail "),NULL,NULL);
	}
	else 
	{
		gPaintInfo.SetHitID( recordObj->ID );

		if( insertAction == InsertTail) 
		{
			gPaintInfo.GetDrawTool()->DrawFig( gPaintInfo.GetMemDC()
				,recordObj->nObjType
				,recordObj->ptBeg.x
				,recordObj->ptBeg.y
				,recordObj->ptEnd.x 
				,recordObj->ptEnd.y
				,recordObj->iPenColor
				,recordObj->iFigBg
				,recordObj->iPenWidth );
			gPaintInfo.GetScrollBarHandle()->TuneWindowView(); 
		}
		else
		{
			gPaintInfo.GetScrollBarHandle()->TuneWindowView();
			gPaintInfo.GetDrawTool()->RePaintWithClipRgn(recordObj->ptBeg,recordObj->ptEnd);
		}

		gPaintInfo.GetDrawTool()->MapDCToMonitor();
	}
	recordObj = NULL;

			
}


void AddNewGraphCmd::UnExecute(RecordObj * recordObj)
{
	if( gMyList.IsID_NodeExit( recordObj->ID ) == FALSE )
	{
		MessageBox(gPaintInfo.GetMainHwnd(),_T(" 要undo 對象不存在於視窗中 " ), NULL ,NULL );
	}

	gPaintInfo.SetHitID( recordObj->ID );

	gPaintInfo.GetClipboardControl()->GetCGraphClipboardControl()->CutGraph();

	gPaintInfo.GetDrawTool()->MapDCToMonitor();


	recordObj = NULL;

}