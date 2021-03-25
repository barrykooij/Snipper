#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <thread>
#include <strsafe.h>
#include "Snippets.h"
#include "resource1.h"
#include "windowsx.h"

#define APPWM_ICONNOTIFY (WM_APP + 1)

const LPCWSTR g_szClassName = L"snipperClass";

NOTIFYICONDATA nid = {};

void DestroyApp()
{
    // clean up shell icon
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;

    TCHAR test[] = TEXT("Hello World!");

    switch (msg)
    {
    case APPWM_ICONNOTIFY:
    {
        switch (lParam)
        {
        case WM_LBUTTONUP:
            //...

            break;
        case WM_RBUTTONUP:            
            POINT pt;
            GetCursorPos(&pt);

            HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU3));
            hMenu = GetSubMenu(hMenu, 0);
            TrackPopupMenu(hMenu, TPM_RIGHTBUTTON | TPM_BOTTOMALIGN, pt.x, pt.y, 0, hwnd, NULL);

            break;
        }
        break;
    }
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        GetClientRect(hwnd, &rect);
        DrawText(hdc, test, -1, &rect, DT_SINGLELINE | DT_NOCLIP | DT_CENTER | DT_VCENTER);
        EndPaint(hwnd, &ps);
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        DestroyApp();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    Snippets snippets;

    std::thread snippetListenThread(&Snippets::ListenLoop, &snippets);

    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    //Step 1: Registering the Window Class
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, L"Window Registration Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        L"Snipper",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    //ShowWindow(hwnd, nCmdShow);
    //UpdateWindow(hwnd);

    // Shell icon start

    HICON hIcon = static_cast<HICON>(::LoadImage(hInstance,
        MAKEINTRESOURCE(ICON_SNIPPER),
        IMAGE_ICON,
        32, 32,    // or whatever size icon you want to load
        LR_DEFAULTCOLOR));

    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

    //Notification
    nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.uCallbackMessage = APPWM_ICONNOTIFY;
    nid.hIcon = hIcon;

    // This text will be shown as the icon's tooltip.
    StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), L"Snipper");

    // Show the notification.
    Shell_NotifyIcon(NIM_ADD, &nid);
    // shell icon stop

    // Step 3: The Message Loop
    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    // tell snippets to stop listening and wait for thread to catch up
    snippets.StopListenLoop();
    snippetListenThread.join();

    return Msg.wParam;
}