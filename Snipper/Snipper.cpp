#include "pch.h"
#include <iostream>
#include <Windows.h>
#include "Snippets.h"

int main()
{
    std::cout << "--------------------------------------------------------------------------" << std::endl;
    std::cout << "                          Snipper" << std::endl;
    std::cout << "--------------------------------------------------------------------------" << std::endl;

    Snippets snippets;

    //std::thread catchThread;

    while (true)
    {
        // key input
        if (GetAsyncKeyState(VK_DELETE) & 1)
        {
            break;
        }

        if (GetAsyncKeyState(VK_OEM_1) & 1)
        {
            snippets.Listen();
        }

        Sleep(5);
    }

    // clean
    std::cout << "Closing Snipper, bye bye" << std::endl;
    
}