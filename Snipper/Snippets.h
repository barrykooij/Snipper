#include "pch.h"
#include <vector>
#include <map>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <shlobj.h>//for knownFolder
#include <winerror.h> //for HRESULT
#include <comutil.h> //for _bstr_t (used in the string conversion)
#include <string>
#pragma comment(lib, "comsuppw")

namespace fs = std::filesystem;

class Snippets
{
private:
	float timeToType = 500;
	float checkInterval = 5;
	float totalChecks = timeToType / checkInterval;
	int checksDone = 0;

	std::vector<char> chars;
	bool isListening;
	std::map<std::string, std::string> snippets;
	HKL currentKBL = GetKeyboardLayout(0);

	void PressKey(char letter);
	void PressEnter();
	void PressBackspace();

	std::string directoryPath;

	bool SetDirectoryPath();
	void CreateSnippetDir();
	void LoadSnippetFiles();

	std::string GetAbbreviation(std::filesystem::path p);
public:
	Snippets();
	void Listen();
	bool HandleAbbr(std::string abbr);
	void PrintSnippet(std::string snippet);
};