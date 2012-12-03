#include "StdAfx.h"
#include "Memento.h"


Memento::Memento(RecordObj *recordObj):_recordObj(recordObj)
{
} 


Memento::~Memento(void)
{
}


 RecordObj* Memento::GetState() const
{ 
	 return _recordObj; 
} 
