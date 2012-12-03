#pragma once
#include "CommonVar.h"
class SgWordDataModelControler
{
public:
    static SgWordDataModelControler* Instance();

	void SaveAllWordnThisWordRecToRecordObj(int ID, RecordObj *pBuffer);
    void SaveAllWordIniInThisWordRec(int ID);
    void IniVectorCharacterToRecordObj(RecordObj *pBuffer);
    void ClearTextBuffer();
private:
    vector<TCHAR> _iniBuffer;

	SgWordDataModelControler(void);
	~SgWordDataModelControler(void);
    static SgWordDataModelControler* _instance;

};

