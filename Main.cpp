//----- INCLUDES ---------------------------------------------------------------
#include "clFunctions.h"
//------------------------------------------------------------------------------
#pragma hdrstop
#pragma argsused
//----- MAIN -------------------------------------------------------------------
int _tmain(int _argc, _TCHAR* _argv[])
{
	clFunctions myFunc;
	std::vector<AnsiString> vecFunctions;

	vecFunctions = myFunc.GetFunctions(L"E:\\test64.dll");

	for (int i = 0; i < vecFunctions.size(); i++)
		std::cout << vecFunctions[i].c_str() << "\n";

	system("pause");
	return 0;
}
//------------------------------------------------------------------------------

