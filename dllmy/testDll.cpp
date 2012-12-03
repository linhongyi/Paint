#include <Windows.h>

extern "C" void  WINAPI WelcomeUseThisDLL()
{
	::OutputDebugString( TEXT(" --------------------------------dllmy loaded successful\n") );
	//::MessageBox(NULL,TEXT(" load dll successful---\n"),NULL,NULL );
}

extern "C" void  WINAPI LongSwap(long &x,long &y )
{
	LONG tmp = 0;
	tmp = x;
	x   = y;
	y   = tmp;
}

extern "C" void  WINAPI IntSwap(int &x,int &y )
{
	int tmp = 0;
	tmp = x;
	x   = y;
	y   = tmp;
}

/**���W�ƨ��I�Ҧ������*/
extern "C" void  WINAPI Normalization(POINT &iniPt, POINT &endPos)
{
    if( iniPt.x >= endPos.x )
	{
		
		if( iniPt.y >= endPos.y )
		{
			LongSwap(iniPt.x,endPos.x);
			LongSwap(iniPt.y,endPos.y);
		}
		else
		{
			LongSwap(iniPt.x,endPos.x);
		}
	}
	else
	{
	   
		if( iniPt.y >= endPos.y )
		{
			LongSwap(iniPt.y,endPos.y);
		}
		else
		{
			//���δ�
		}
	}
}

/**�|���I�A�D�Ҧ��̤j���*/
extern "C" void  WINAPI FindMaxScopeWithFourPoint( POINT &recPt1, POINT &recPt2, POINT &recPt3, POINT &recPt4)
{
	Normalization( recPt1, recPt2 );
	Normalization( recPt3, recPt4 );
	Normalization( recPt1, recPt3 );
	Normalization( recPt2, recPt4 );
}

/**���I�ھ�Y��V�ȡA�ƦC*/
extern "C" void  WINAPI TunePtsWithPosY(POINT *ptrPt1, POINT *ptrPt2)
{
	if( ptrPt1->x > ptrPt2->x )
	{
		LongSwap(  ptrPt1->x , ptrPt2->x);
	}

	if( ptrPt1->y > ptrPt2->y )
	{
	    LongSwap(  ptrPt1->y , ptrPt2->y);
	}
}

/** �ϥզr���I�Ҧ��d��A���W��*/
extern "C" void  WINAPI InverseWordNormalization( POINT &iniPt, POINT &endPt)
{
	if( iniPt.y == endPt.y && iniPt.x > endPt.x )
	{
		LongSwap( iniPt.x , endPt.x);
	}
	else if( endPt.y < iniPt.y )
	{
	    LongSwap( iniPt.x , endPt.x);
		LongSwap( iniPt.y , endPt.y);
	}
}