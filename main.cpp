
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdio.h>      
#include <time.h>
#include <ctime>

std::wstring openfilename(WCHAR* filter, HWND owner = NULL) {
	OPENFILENAME ofn;
	WCHAR fileName[MAX_PATH] = L"";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = owner;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = L"";
	std::wstring fileNameStr;
	if (GetOpenFileName(&ofn))
		fileNameStr = fileName;
	return fileNameStr;
}

bool valid_tm(const std::tm& tm)
{
	auto cpy = tm;
	const auto as_time_t = std::mktime(std::addressof(cpy));
	if (as_time_t) {
		if (std::addressof(as_time_t)) {
			if (std::localtime(std::addressof(as_time_t))) {
				try {
					cpy = *std::localtime(std::addressof(as_time_t));

					return tm.tm_mday == cpy.tm_mday && /* Day */
						tm.tm_mon == cpy.tm_mon && /* Month */
						tm.tm_year == cpy.tm_year && /* Year */
						tm.tm_wday == cpy.tm_wday; /* Week */
				}
				catch (EXCEPINFO) {

				}
			}
		}
	}

	return false;
}

std::string getCompileDateString(std::string filePath) {
	const int MAX_FILEPATH = 255;
	char fileName[MAX_FILEPATH] = { 0 };

	memcpy_s(&fileName, MAX_FILEPATH, filePath.c_str(), MAX_FILEPATH);

	HANDLE file = NULL;
	DWORD fileSize = NULL;
	DWORD bytesRead = NULL;
	LPVOID fileData = NULL;
	PIMAGE_DOS_HEADER dosHeader = {};
	PIMAGE_NT_HEADERS imageNTHeaders = {};
	PIMAGE_SECTION_HEADER sectionHeader = {};
	PIMAGE_SECTION_HEADER importSection = {};
	IMAGE_IMPORT_DESCRIPTOR* importDescriptor = {};
	PIMAGE_THUNK_DATA thunkData = {};
	DWORD thunk = NULL;
	DWORD rawOffset = NULL;

	file = CreateFileA(fileName, GENERIC_ALL, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE) return 0;

	fileSize = GetFileSize(file, NULL);
	fileData = HeapAlloc(GetProcessHeap(), 0, fileSize);

	if (fileData) {

		if (ReadFile(file, fileData, fileSize, &bytesRead, NULL)) {

			dosHeader = (PIMAGE_DOS_HEADER)fileData;
			imageNTHeaders = (PIMAGE_NT_HEADERS)((DWORD)fileData + dosHeader->e_lfanew);

			time_t epch;
			try {
				epch = imageNTHeaders->FileHeader.TimeDateStamp;
			}
			catch (EXCEPINFO) {
				return "none";
			}

			char buffer2[32];

			try {
				std::tm* ptm = std::localtime(&epch);
				if (ptm->tm_hour >= 1)
					ptm->tm_hour -= 1;
				else if (ptm->tm_hour == 0) {
					ptm->tm_mday -= 1;
					ptm->tm_hour += 23;
				}
				else {

				}

				if (valid_tm(*ptm)) {
					std::strftime(buffer2, 32, "%Y/%m/%d:%H:%M:%S", ptm); /* Format */

					std::stringstream ss;
					ss << buffer2;

					std::cout << "Sucessfully grabbed compiliation date! " << std::endl;
					return ss.str();
				}
			}
			catch (EXCEPINFO) {
			}
		}
	}

	return "none";
}

int main(int argc, char* argv[]) {

	const int MAX_FILEPATH = 255;
	char fileName[MAX_FILEPATH] = { 0 };
	std::wstring ws = openfilename((WCHAR*)L"Cheatie wheaties (*.exe)\0*.exe\0"); /* In order to find the compilation date of a .jar, .dll, etc. you'll need to change this */
	std::string s(ws.begin(), ws.end());

	std::cout << getCompileDateString(s); /* Outputs the compiliation date string */

	getchar();

	return 0;
}
