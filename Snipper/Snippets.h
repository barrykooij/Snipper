#include "pch.h"
#include <vector>
#include <map>

class Snippets
{
private:
	float timeToType = 200;
	float checkInterval = 20;
	float totalChecks = timeToType / checkInterval;
	int checksDone = 0;

	std::vector<char> chars;
	bool isListening;
	std::map<std::string, std::string> snippets;
	HKL currentKBL = GetKeyboardLayout(0);

	void PressKey(char letter);
	void PressEnter();
	void PressBackspace();

	
public:
	Snippets();
	std::string CatchChars();
	void HandleAbbr(std::string abbr);
	void PrintSnippet(std::string snippet);
};