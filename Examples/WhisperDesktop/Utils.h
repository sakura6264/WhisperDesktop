#pragma once
#include <io.h>
#include <vector>
#include <fcntl.h>
#include <cstdio>
void getAllFiles(CString path, std::vector<CString>& files);
CString GetModuleDir();
bool WriteTo(CString path, CString data);


