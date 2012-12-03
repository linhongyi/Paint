#include "StdAfx.h"
#include "SgWordDataModelControler.h"

#include "Resource.h"
#include "MyListController.h"
#include "MyListNode.h"

#include <assert.h>

extern MyListController gMyList;

SgWordDataModelControler* SgWordDataModelControler::_instance = NULL;
SgWordDataModelControler::SgWordDataModelControler(void)
{
}


SgWordDataModelControler::~SgWordDataModelControler(void)
{
}

SgWordDataModelControler* SgWordDataModelControler::Instance()
{
    if( !_instance)
    {
        _instance = new SgWordDataModelControler();
    }

    return _instance;
}

//儲存此文字框資料至 pBuffer
void SgWordDataModelControler::SaveAllWordnThisWordRecToRecordObj(int ID, RecordObj *pBuffer)
{

	int chaIdx = 0;
	
	MyListNode *ptr = gMyList.visitID_Node(ID);
	WordNodeInfo *wordPtr = ptr->GetHead();

	pBuffer->ptNewBeg = ptr->GetPtBeg();
	pBuffer->ptNewEnd = ptr->GetPtEnd();
	
	int size = ptr->GetWordNum();
    TCHAR cha;
	for( int queueWordIdx = 0 ; queueWordIdx < size ; queueWordIdx++, wordPtr = wordPtr->_rlink )
	{
	    cha = wordPtr->_cha;
		pBuffer->vecCharacter.push_back(cha);
	}
}

// 暫存此時文字框資料
void SgWordDataModelControler::SaveAllWordIniInThisWordRec(int ID)
{

	int chaIdx = 0;
	
	MyListNode *ptr = gMyList.visitID_Node(ID);
	WordNodeInfo *wordPtr = ptr->GetHead();
	

	int size = ptr->GetWordNum();
	this->_iniBuffer.clear();


	TCHAR cha;
	for( int queueWordIdx = 0 ; queueWordIdx < size ; queueWordIdx++, wordPtr = wordPtr->_rlink )
	{
	    cha =  wordPtr->_cha;
		this->_iniBuffer.push_back(cha);
	}
}

// 將暫存文字框資料拷貝至 pBuffer */
void  SgWordDataModelControler::IniVectorCharacterToRecordObj(RecordObj *pBuffer)
{   
    int size = (int) this->_iniBuffer.size();
    

	for( int queueWordIdx = 0 ; queueWordIdx < size ; queueWordIdx++)
	{
		pBuffer->vecOriCharacater.push_back( this->_iniBuffer[queueWordIdx]);
	}
}

// 清除暫存文字資料
void SgWordDataModelControler::ClearTextBuffer()
{
     SgWordDataModelControler::_iniBuffer.clear();
}