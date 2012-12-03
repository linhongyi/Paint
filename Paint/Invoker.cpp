#include "StdAfx.h"
#include "Invoker.h"
#include <assert.h>

CInvoker::CInvoker(void)
{

}


CInvoker::~CInvoker(void)
{
}


Command& CInvoker::GetRedoUndoCmd(RedoUndoCmd cmd)
{
	switch ( cmd )
	{
	case COMMAND_DRAW_NEW_GRAPH:
		return this->_addNewGarphCmd;
		break;
	case COMMAND_FIX_GRAPH:
		return this->_modifyGarphCmd;
		break;
	case COMMAND_DELETE_GRAPH:
		return this->_deleteGraphCmd;
		break;
	case COMMAND_FIX_COLOR_WORD:
		return this->_changeWordColorCommand;
		break;
	case COMMAND_FIX_BGCOLOR_WORD:
		return this->_changeWordBgColorCommand;
		break;
	case COMMAND_CHANGE_WORD_REC:
		return this->_changeWordRecScopeCommand;
		break;
	case COMMAND_NEW_TRAGET_GRAPH_SEL:
		return this->_newTarGetGraphSelectCmd;
		break;
	case COMMAND_ADD_WORD:
		return this->_addWordCmd;
		break;
	case COMMAND_DELETE_WORD:
		return this->_deleteWordCmd;
		break;
	case COMMAND_EDIT_WORD:
	    return this->_modifyEditWordCmd;
	    break;
	default:
		return this->_warningCmd;
		break;
	}
}
