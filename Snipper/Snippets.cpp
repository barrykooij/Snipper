#include "Snippets.h"

Snippets::Snippets()
{
	std::cout << "constructor called" << std::endl;

	this->snippets = {
		{"AE", "Please let me know if there's anything else I can help you with"},
		{"REFUND", "I just did a full refund of your payment. Please note that your license key will no longer be valid and can't be used to receive updates and support anymore.\n\nI'm sorry to see you go but hope to welcome you as a customer again one day. Please let me know if you've got any remaining questions about the payment or the order.\n\nPlease find the credit invoice attached."},
	};
}

std::string Snippets::CatchChars()
{
	this->isListening = true;

	this->checksDone = 0;

	// set chars typed to empty
	this->chars = {};

	while (this->isListening && checksDone < totalChecks)
	{
		for (int i = 'A'; i <= 'Z'; i++) 
		{
			if (GetAsyncKeyState(i) & 1)
			{
				char c = i;
				this->chars.push_back(c);
				this->checksDone = 0;
			}
		}

		checksDone++;
		Sleep(checkInterval);
	}

	std::string abbr(chars.begin(), chars.end());

	this->isListening = false;

	return abbr;
}

void Snippets::HandleAbbr(std::string abbr)
{
	// print snippet
	std::map<std::string, std::string>::iterator it = snippets.find(abbr);
	if (it != snippets.end())
	{
		// remove abbr (+1 for delimiter)
		for (int i = 0; i < abbr.length() + 1; ++i)
		{
			PressBackspace();
		}

		Sleep(10);

		this->PrintSnippet(it->second);
	}
}

void Snippets::PrintSnippet(std::string snippet)
{
	std::vector<char> chars(snippet.begin(), snippet.end());
	chars.push_back('\0'); // Make sure we are null-terminated

	for (auto& letter : chars) {
		PressKey(letter);
	}

	std::cout << std::endl;
}

void Snippets::PressKey(char letter)
{
	if (letter == 0)
		return;

	INPUT ip;
	ip.type = INPUT_KEYBOARD;

	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;
	ip.ki.dwFlags = KEYEVENTF_UNICODE;

	if ((int)letter < 65 && (int)letter>90) //for lowercase
	{
		ip.ki.wScan = 0;
		ip.ki.wVk = VkKeyScanEx(letter, currentKBL);
	}
	else //for uppercase
	{
		ip.ki.wScan = letter;
		ip.ki.wVk = 0;

	}

	SendInput(1, &ip, sizeof(INPUT));
}

void Snippets::PressEnter()
{
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.time = 0;
	ip.ki.dwFlags = KEYEVENTF_UNICODE;
	ip.ki.wScan = VK_RETURN; //VK_RETURN is the code of Return key
	ip.ki.wVk = 0;

	ip.ki.dwExtraInfo = 0;
	SendInput(1, &ip, sizeof(INPUT));
}

void Snippets::PressBackspace()
{
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.time = 0;
	ip.ki.dwFlags = KEYEVENTF_UNICODE;
	//77ip.ki.wScan = VK_BACK;
	ip.ki.wVk = VK_BACK;

	ip.ki.dwExtraInfo = 0;
	SendInput(1, &ip, sizeof(INPUT));

	Sleep(1);
}