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

	// clear current list
	this->snippets.clear();

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

void Snippets::OpenSnippetDirectory()
{
	std::wstring stemp = std::wstring(this->directoryPath.begin(), this->directoryPath.end());
	LPCWSTR sw = stemp.c_str();

	ShellExecute(NULL, L"open", sw, NULL, NULL, SW_SHOWDEFAULT);
}

std::string Snippets::GetAbbreviation(std::filesystem::path p)
{
	std::string abbr = p.stem().string();

	std::transform(abbr.begin(), abbr.end(), abbr.begin(), ::toupper);

	return abbr;
}

void Snippets::ListenLoop()
{
	this->listenLoopActive = true;
	while (this->listenLoopActive)
	{
		if (GetAsyncKeyState(VK_OEM_1) & 1)
		{
			Listen();
		}

		Sleep(1);
	}
}

void Snippets::StopListenLoop()
{
	this->listenLoopActive = false;
}

void Snippets::ResetListen()
{
	this->checksDone = 0;

	// set chars typed to empty
	this->chars = {};
}

void Snippets::Listen()
{
	this->isListening = true;

	ResetListen();

	while (this->isListening && checksDone < totalChecks)
	{

		// if you press the ; key while listening for a abbr -> reset and start over
		if (GetAsyncKeyState(VK_OEM_1) & 1)
		{
			ResetListen();
		}

		// on backspace remove a char from this->chars
		if (GetAsyncKeyState(VK_BACK) & 1)
		{
			if (this->chars.size() > 0)
			{
				this->chars.pop_back();
			}
		}

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

		Sleep(500);

		this->PrintSnippet(it->second);

		return true;
	}

	return false;
}

void Snippets::PrintSnippet(std::string snippet)
{
	std::vector<char> chars(snippet.begin(), snippet.end());
	chars.push_back('\0'); // Make sure we are null-terminated

	int i = 0;
	for (auto& letter : chars) {

		if ((i % 20) == 0)
			Sleep(1);

		PressKey(letter);
		i++;
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
	Sleep(1);
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