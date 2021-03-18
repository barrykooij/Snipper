#include "Snippets.h"

Snippets::Snippets()
{

	// set correct path
	if (!SetDirectoryPath())
	{
		std::cout << "ERROR: Failed to load snippet directory" << std::endl;
		return;
	}

	// create dir
	CreateSnippetDir();

	// Load Snippets
	LoadSnippetFiles();

	/*
	this->snippets = {
		{"AE", "Please let me know if there's anything else I can help you with"},
		{"REFUND", "I just did a full refund of your payment. Please note that your license key will no longer be valid and can't be used to receive updates and support anymore.\n\nI'm sorry to see you go but hope to welcome you as a customer again one day. Please let me know if you've got any remaining questions about the payment or the order.\n\nPlease find the credit invoice attached."},
	};
	*/
}

bool Snippets::SetDirectoryPath()
{

	LPWSTR wszPath = NULL;
	HRESULT hr;

	hr = SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_CREATE, NULL, &wszPath);
	if (SUCCEEDED(hr))
	{
		_bstr_t bstrPath(wszPath);
		std::string strPath((char*)bstrPath);

		strPath += "\\Snipper\\";

		this->directoryPath = strPath;

		return true;
	}

	return false;
}

void Snippets::CreateSnippetDir()
{

	std::wstring stemp = std::wstring(this->directoryPath.begin(), this->directoryPath.end());
	LPCWSTR sw = stemp.c_str();

	if (CreateDirectory(sw, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
	{
	}
}

void Snippets::LoadSnippetFiles()
{
	int snippetsLoaded = 0;

	LPWSTR wszPath = NULL;
	HRESULT hr;

	hr = SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_CREATE, NULL, &wszPath);
	if (SUCCEEDED(hr))
	{
	
		std::ifstream infile;
		std::string line, snippet;

		// convert to string
		_bstr_t bstrPath(wszPath);
		std::string strPath((char*)bstrPath);

		strPath += "\\Snipper\\";

		for (auto& file : fs::directory_iterator{ strPath })  //loop through the current folder
		{
			snippet = "";

			infile.open(file.path());
			while (std::getline(infile, line))
			{
				if (snippet != "")
					snippet += "\n";

				//snippet += line.c_str();
				snippet.append(line.c_str());
			}

			// close file
			infile.close();

			// insert snippet
			this->snippets.insert(std::pair<std::string, std::string>(GetAbbreviation(file.path()), snippet));

			// incr counter
			snippetsLoaded++;
		}
	}

	std::cout << "Snippets loaded: " << snippetsLoaded << std::endl;
}

std::string Snippets::GetAbbreviation(std::filesystem::path p)
{
	std::string abbr = p.stem().string();

	std::transform(abbr.begin(), abbr.end(), abbr.begin(), ::toupper);

	return abbr;
}

void Snippets::Listen()
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
				// push key in chars list
				char c = i;
				this->chars.push_back(c);
				this->checksDone = 0;

				// convert the chars to string
				std::string abbr(chars.begin(), chars.end());

				// try to handle the abbr
				if (this->HandleAbbr(abbr))
				{
					this->isListening = false;
				}
			}
		}

		checksDone++;
		Sleep(checkInterval);
	}

	this->isListening = false;
}

bool Snippets::HandleAbbr(std::string abbr)
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

		Sleep(5);

		this->PrintSnippet(it->second);

		return true;
	}

	return false;
}

void Snippets::PrintSnippet(std::string snippet)
{
	std::vector<char> chars(snippet.begin(), snippet.end());
	chars.push_back('\0'); // Make sure we are null-terminated

	for (auto& letter : chars) {
		PressKey(letter);
	}
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