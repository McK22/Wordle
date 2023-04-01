#include "framework.h"
#include "wordle.h"
#include "GameWindow.h"
#include "KeyboardWindow.h"
#include <Windowsx.h>
#include <fstream>
#include <ctime>
#include <chrono>

#define MAX_LOADSTRING 100
#define STEP 1
#define APP_NAME L"Wordle"
#define KEY_NAME L"Difficulty"
#define INI_FILENAME L"Wordle.ini"

using namespace std;

void setFileSize();

// Global Variables:
KeyboardWindow* keyboardWindowPointer;
string fileName = "Wordle.txt";
string easyString = "Easy";
string mediumString = "Medium";
string hardString = "Hard";
int fileSize = 0;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    setFileSize();
    std::srand(time(NULL));

    int screenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYFULLSCREEN);

    KeyboardWindow::registerClass(hInstance);
    GameWindow::registerClass(hInstance);

    KeyboardWindow keyboardWindow(hInstance, nCmdShow, screenWidth, screenHeight);
    keyboardWindowPointer = &keyboardWindow;
    keyboardWindow.create();
    loadDifficulty();
    keyboardWindow.startGame();

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GAME));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

void setFileSize()
{
    ifstream f(fileName);
    string s;
    while (!f.eof())
    {
        fileSize++;
        f >> s;
    }
    f.close();
}

bool isWordValid(string s)
{
    for (int i = 0; i < s.size(); i++)
        s[i] = tolower(s[i]);
    ifstream f(fileName);
    string t;
    while (!f.eof())
    {
        f >> t;
        if (t == s)
        {
            f.close();
            return true;
        }
    }

    f.close();
    return false;
}

string getRandomWord()
{
    ifstream f(fileName);
    int i = rand() % fileSize;
    string s;
    while (i--)
        f >> s;
    f.close();
    return s;
}

bool areStringsEqual(string s, TCHAR c[MAX_LOADSTRING])
{
    for (int i = 0; i < MAX_LOADSTRING && s[i] != '\0'; i++)
        if (s[i] != c[i])
            return false;
    return true;
}

void loadDifficulty()
{
    TCHAR s[MAX_LOADSTRING];
    TCHAR path[MAX_LOADSTRING];
    GetFullPathName(INI_FILENAME, MAX_LOADSTRING, path, NULL);
    BOOL success = GetPrivateProfileString(APP_NAME, KEY_NAME, NULL, s, MAX_LOADSTRING, path);
    if (!success)
    {
        int difficulty = rand() % 3;
        if (difficulty == 0)
            keyboardWindowPointer->setDifficulty(IDM_EASY);
        else if (difficulty == 1)
            keyboardWindowPointer->setDifficulty(IDM_MEDIUM);
        else
            keyboardWindowPointer->setDifficulty(IDM_HARD);
        return;
    }

    if (areStringsEqual(easyString, s))
        keyboardWindowPointer->setDifficulty(IDM_EASY);
    else if (areStringsEqual(mediumString, s))
        keyboardWindowPointer->setDifficulty(IDM_MEDIUM);
    else if (areStringsEqual(hardString, s))
        keyboardWindowPointer->setDifficulty(IDM_HARD);
}

void saveDifficulty()
{
    TCHAR s[MAX_LOADSTRING];
    TCHAR path[MAX_LOADSTRING];
    GetFullPathName(INI_FILENAME, MAX_LOADSTRING, path, NULL);
    int difficulty = keyboardWindowPointer->getDifficulty();
    int i;
    if (difficulty == IDM_EASY)
        for (i = 0; i < easyString.size(); i++)
            s[i] = easyString[i];
    else if (difficulty == IDM_MEDIUM)
        for (i = 0; i < mediumString.size(); i++)
            s[i] = mediumString[i];
    else
        for (i = 0; i < hardString.size(); i++)
            s[i] = hardString[i];
    s[i] = '\0';

    WritePrivateProfileString(APP_NAME, KEY_NAME, s, path);
}

long long getTime()
{
    return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
}

LRESULT CALLBACK KeyboardWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_TIMER:
        keyboardWindowPointer->updateAnimation();
        break;
    case WM_KEYDOWN:
        if (wParam == VK_BACK)
            keyboardWindowPointer->erase();
        else if (wParam == VK_ACCEPT)
            keyboardWindowPointer->validateWord();
        else
        {
            TCHAR text[MAX_LOADSTRING];
            GetKeyNameText(lParam, text, MAX_LOADSTRING);
            if (text[1] == '\0' && text[0] <= 'Z' && text[0] >= 'A')
                keyboardWindowPointer->type(text[0]);
        }
        break;
    case WM_COMMAND:
    {
        if (wParam == 98311)
        {
            keyboardWindowPointer->validateWord();
            break;
        }

        int wmId = LOWORD(wParam);
        keyboardWindowPointer->setDifficulty(wmId);
        keyboardWindowPointer->startGame();
    }
    break;
    case WM_PAINT:
    {
        keyboardWindowPointer->paint();
    }
    break;
    case WM_DESTROY:
        saveDifficulty();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK GameWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
        keyboardWindowPointer->paintGameWindow(hWnd);
        break;
    case WM_ERASEBKGND:
        return 1;
    case WM_NCHITTEST: //https://stackoverflow.com/questions/7773771/how-do-i-implement-dragging-a-window-using-its-client-area
    {
        LRESULT hit = DefWindowProc(hWnd, message, wParam, lParam);
        if (hit == HTCLIENT) hit = HTCAPTION;
        return hit;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
