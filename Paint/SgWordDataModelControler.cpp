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

//�x�s����r�ظ�Ʀ� pBuffer
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

// �Ȧs���ɤ�r�ظ��
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

// �N�Ȧs��r�ظ�ƫ����� pBuffer */
void  SgWordDataModelControler::IniVectorCharacterToRecordObj(RecordObj *pBuffer)
{   
    int size = (int) this->_iniBuffer.size();
    

	for( int queueWordIdx = 0 ; queueWordIdx < size ; queueWordIdx++)
	{
		pBuffer->vecOriCharacater.push_back( this->_iniBuffer[queueWordIdx]);
	}
}

// �M���Ȧs��r���
void SgWordDataModelControler::ClearTextBuffer()
{
     SgWordDataModelControler::_iniBuffer.clear();
}