#include "stdafx.h"
#include "Utils.h"
using namespace std;
void getAllFiles(CString path, vector<CString>& files)
{
	HANDLE hFile = 0;
	WIN32_FIND_DATA fileinfo;
	CString p = path;
	p.Append(L"\\*.bin");

	if ((hFile = FindFirstFile(p.GetString(), &fileinfo)) != INVALID_HANDLE_VALUE) {
		do {
			// 保存文件的名称
			files.push_back(fileinfo.cFileName);

		} while (FindNextFile(hFile, &fileinfo));

		FindClose(hFile);
	}
}

CString GetModuleDir() {
	HMODULE module = GetModuleHandle(0);
	TCHAR pFileName[MAX_PATH] = { 0 };
	GetModuleFileName(module, pFileName, MAX_PATH);
	CString csFullName(pFileName);
	int nPos = csFullName.ReverseFind('\\');
	if (nPos < 0)
		return CString(L"");
	else
		return csFullName.Left(nPos);
}

bool WriteTo(CString path, CString data)
{
	FILE* f = nullptr;
	if (_tfopen_s(&f, path.GetString(), L"w")==0) {
		_setmode(_fileno(f), _O_U8TEXT);
		_ftprintf_s(f, data.GetString());
		fclose(f);
		return true;
	}
	return false;
}
