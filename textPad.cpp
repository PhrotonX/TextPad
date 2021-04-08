#include <windows.h>
#include "resource.h"
#include <stdio.h>
#include <commctrl.h>
#include <string.h>
#include <iostream>
//#include <winuser.h>

const char g_szClassName[] = "textPad";
#define IDC_MAIN_EDIT       101
#define IDC_MAIN_TOOLBAR    102
#define IDC_MAIN_STATUS     103

HFONT g_hfFont = NULL;
COLORREF g_rgbText = RGB(0, 0, 0);

BOOL LoadTextFileToEdit(HWND hEdit, LPCTSTR pszFileName)
{
    HANDLE hFile;
    BOOL bSuccess = FALSE;

    hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if(hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dwFileSize;

        dwFileSize = GetFileSize(hFile, NULL);
        if(dwFileSize != 0xFFFFFFFF)
        {
            LPSTR pszFileText;
            pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);

            if(pszFileText != NULL)
            {
                DWORD dwRead;

                if(ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
                   {
                       pszFileText[dwFileSize] = 0;
                       if(SetWindowText(hEdit, pszFileText))
                            bSuccess = TRUE;
                   }
                   GlobalFree(pszFileText);
            }
        }
    }
    return bSuccess;
}

BOOL SaveTextFileFromEdit(HWND hEdit, LPCTSTR pszFileName)
{
    HANDLE hFile;
    BOOL bSuccess = FALSE;

    hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dwTextLenght;
        dwTextLenght = GetWindowTextLength(hEdit); //unicode

        if(dwTextLenght > 0)
        {
            LPSTR pszText;
            DWORD dwBufferSize = dwTextLenght + 1;

            pszText = (LPSTR)GlobalAlloc(GPTR, dwBufferSize);
            if(pszText != NULL)
            {
                if(GetWindowText(hEdit, pszText, dwBufferSize)) //unicode
                {
                    DWORD dwWritten;

                    if(WriteFile(hFile, pszText, dwTextLenght, &dwWritten, NULL))
                        bSuccess = TRUE;
                }
                GlobalFree(pszText);
            }
        }
        CloseHandle(hFile);
    }
    return bSuccess;
}

INT_PTR CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_INITDIALOG:
        return TRUE;
        break;
    case WM_DESTROY:
        EndDialog(hwnd, WM_CLOSE);
        break;
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case ID_ABOUTDIALOG_OK:
            EndDialog(hwnd, ID_ABOUTDIALOG_OK);
            break;
        case ID_ABOUTDIALOG_VIEWONGITHUB:
        {
            char linkGithub[36] = "https://github.com/PhrotonX/TextPad";
            ShellExecute(NULL, "open", linkGithub, NULL, NULL, SW_SHOWNORMAL);
            break;
        }

        case ID_ABOUTDIALOG_CHECKFORUPDATES:
        {
            char linkUpdatesTemp[45] = "https://github.com/PhrotonX/TextPad/releases";
            ShellExecute(NULL, "open", linkUpdatesTemp, NULL, NULL, SW_SHOWNORMAL);
            break;
        }

        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

void DoFileOpen(HWND hwnd)
{
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "txt";

    if(GetOpenFileName(&ofn)) //unicode
    {
        HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
        if(LoadTextFileToEdit(hEdit, szFileName))
        {
            SendDlgItemMessage(hwnd, IDC_MAIN_STATUS, SB_SETTEXT, 0, (LPARAM)"Opened");
            SendDlgItemMessage(hwnd, IDC_MAIN_STATUS, SB_SETTEXT, 1, (LPARAM)szFileName);

            SetWindowText(hwnd, szFileName);
        }
    }
}

void DoFileSave(HWND hwnd)
{
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = "txt";

    if(GetSaveFileName(&ofn)) //unicode
    {
        HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
        if(SaveTextFileFromEdit(hEdit, szFileName))
        {
            SendDlgItemMessage(hwnd, IDC_MAIN_STATUS, SB_SETTEXT, 0, (LPARAM)"Saved");
            SendDlgItemMessage(hwnd, IDC_MAIN_STATUS, SB_SETTEXT, 1, (LPARAM)szFileName);

            SetWindowText(hwnd, szFileName);
        }
    }
}

void DoFileSaveOnly(HWND hwnd)
{

    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER;
    ofn.lpstrDefExt = "txt";

    if(GetSaveFileName(&ofn)) //unicode
    {
        HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
        if(SaveTextFileFromEdit(hEdit, szFileName))
        {
            SendDlgItemMessage(hwnd, IDC_MAIN_STATUS, SB_SETTEXT, 0, (LPARAM)"Saved");
            SendDlgItemMessage(hwnd, IDC_MAIN_STATUS, SB_SETTEXT, 1, (LPARAM)szFileName);

            SetWindowText(hwnd, szFileName);
        }
    }
}

void DoSelectFont(HWND hwnd)
{
    CHOOSEFONT cf = { sizeof(CHOOSEFONT) };
    LOGFONT lf;

    GetObject(g_hfFont, sizeof(LOGFONT), &lf);

    cf.Flags = CF_EFFECTS | CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
    cf.hwndOwner = hwnd;
    cf.lpLogFont = &lf;
    cf.rgbColors = g_rgbText;
    cf.nFontType = SCREEN_FONTTYPE;

    if (ChooseFont(&cf))
    {
        HFONT hf = CreateFontIndirect(&lf);
        if (hf)
        {
            g_hfFont = hf;
            SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, WM_SETFONT, (WPARAM)g_hfFont, TRUE);
        }
        else {
            MessageBox(hwnd, "Failed to open ChooseFont()", "Error", MB_OK | MB_ICONERROR);
        }
        g_rgbText = cf.rgbColors;
    }

}

HINSTANCE g_hinst;

HWND WINAPI InitializeHotkey(HWND hwndDlg)
{
    HWND hwndHot = NULL;

    // Ensure that the common control DLL is loaded.
    INITCOMMONCONTROLSEX icex;  //declare an INITCOMMONCONTROLSEX Structure
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_HOTKEY_CLASS;   //set dwICC member to ICC_HOTKEY_CLASS
                                     // this loads the Hot Key control class.
    InitCommonControlsEx(&icex);

    hwndHot = CreateWindowEx(0,                        // no extended styles
                             HOTKEY_CLASS,             // class name
                             TEXT(""),                 // no title (caption)
                             WS_CHILD | WS_VISIBLE,    // style
                             15, 10,                   // position
                             200, 20,                  // size
                             hwndDlg,                  // parent window
                             NULL,                     // uses class menu
                             g_hinst,                  // instance
                             NULL);                    // no WM_CREATE parameter

    SetFocus(hwndHot);

    // Set rules for invalid key combinations. If the user does not supply a
    // modifier key, use ALT as a modifier. If the user supplies SHIFT as a
    // modifier key, use SHIFT + ALT instead.
    SendMessage(hwndHot,
                HKM_SETRULES,
                (WPARAM) HKCOMB_NONE | HKCOMB_S,   // invalid key combinations
                MAKELPARAM(HOTKEYF_ALT, 0));       // add ALT to invalid entries

    // Set CTRL + ALT + A as the default hot key for this window.
    // 0x41 is the virtual key code for 'A'.
    SendMessage(hwndHot,
                HKM_SETHOTKEY,
                MAKEWORD(0x41, HOTKEYF_CONTROL | HOTKEYF_ALT),
                0);

    return hwndHot;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    switch(msg){
    case WM_CREATE: {
        HFONT hfDefault;
        HWND hEdit;
        HFONT hFont;

        HWND hTool;
        TBBUTTON tbb[13];
        TBADDBITMAP tbab;

        HWND hStatus;
        int statwidhts[] = {100, -1};

        g_hfFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

        hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL |
                               ES_AUTOHSCROLL | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | WS_EX_ACCEPTFILES, 0, 0, 100, 100,
                               hwnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);

        if(hEdit == NULL)
            MessageBox(hwnd, "Could not create IDC_MAIN_EDIT", "Error", MB_RETRYCANCEL | MB_ICONSTOP);

        //hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        //GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
        //SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

        hFont = CreateFont(0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("Consolas"));
        SendMessage (hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

        hTool = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)IDC_MAIN_TOOLBAR,
                               GetModuleHandle(NULL), NULL);
        if(hTool == NULL)
            MessageBox(hwnd, "Could not create IDC_MAIN_TOOLBAR", "Error", MB_RETRYCANCEL | MB_OK);

        SendMessage(hTool, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

        tbab.hInst = HINST_COMMCTRL;
        tbab.nID = IDB_STD_SMALL_COLOR;
        SendMessage(hTool, TB_ADDBITMAP, 0, (LPARAM)&tbab);

        ZeroMemory(tbb, sizeof(tbb));

        tbb[0].iBitmap = STD_FILENEW;
        tbb[0].fsState = TBSTATE_ENABLED;
        tbb[0].fsStyle = TBSTYLE_BUTTON;
        tbb[0].idCommand = ID_FILE_NEW;

        tbb[1].iBitmap = STD_FILEOPEN;
        tbb[1].fsState = TBSTATE_ENABLED;
        tbb[1].fsStyle = TBSTYLE_BUTTON;
        tbb[1].idCommand = ID_FILE_OPEN;

        tbb[2].iBitmap = STD_FILESAVE;
        tbb[2].fsState = TBSTATE_ENABLED;
        tbb[2].fsStyle = TBSTYLE_BUTTON;
        tbb[2].idCommand = ID_FILE_SAVE;

        tbb[3].fsStyle = TBSTYLE_SEP;

        tbb[4].iBitmap = STD_CUT;
        tbb[4].fsState = TBSTATE_ENABLED;
        tbb[4].fsStyle = TBSTYLE_BUTTON;
        tbb[4].idCommand = ID_EDIT_CUT;

        tbb[5].iBitmap = STD_COPY;
        tbb[5].fsState = TBSTATE_ENABLED;
        tbb[5].fsStyle = TBSTYLE_BUTTON;
        tbb[5].idCommand = ID_EDIT_COPY;

        tbb[6].iBitmap = STD_PASTE;
        tbb[6].fsState = TBSTATE_ENABLED;
        tbb[6].fsStyle = TBSTYLE_BUTTON;
        tbb[6].idCommand = ID_EDIT_PASTE;

        tbb[7].iBitmap = STD_DELETE;
        tbb[7].fsState = TBSTATE_ENABLED;
        tbb[7].fsStyle = TBSTYLE_BUTTON;
        tbb[7].idCommand = ID_EDIT_CLEAR;

        //tbb[1].iBitmap = STD_FILEOPEN;
        //tbb[1].fsState = TBSTATE_ENABLED;
        tbb[8].fsStyle = TBSTYLE_SEP;
        //tbb[1].idCommand = ID_FILE_OPEN;

        tbb[9].iBitmap = STD_UNDO;
        tbb[9].fsState = TBSTATE_ENABLED;
        tbb[9].fsStyle = TBSTYLE_BUTTON;
        tbb[9].idCommand = ID_EDIT_UNDO;

        tbb[10].iBitmap = STD_REDOW;
        tbb[10].fsState = TBSTATE_ENABLED;
        tbb[10].fsStyle = TBSTYLE_BUTTON;
        tbb[10].idCommand = ID_EDIT_REDO;

        tbb[11].fsStyle = TBSTYLE_SEP;

        tbb[12].iBitmap = STD_HELP;
        tbb[12].fsState = TBSTATE_ENABLED;
        tbb[12].fsStyle = TBSTYLE_BUTTON;
        tbb[12].idCommand = ID_HELP_GETHELP;

        SendMessage(hTool, TB_ADDBUTTONS, sizeof(tbb)/sizeof(TBBUTTON), (LPARAM)&tbb);

        hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, hwnd, (HMENU)IDC_MAIN_STATUS, GetModuleHandle(NULL), NULL);

        SendMessage(hStatus, SB_SETPARTS, sizeof(statwidhts)/sizeof(int), (LPARAM)statwidhts);
        SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)"TextPad");

        EnableWindow(hTool, ID_EDIT_CLEAR);

        break;
    }
    case WM_SIZE: {
        HWND hEdit;
        RECT rcClient;
        int iEditHeight;

        HWND hTool;
        RECT rcTool;
        int iToolHeight;

        HWND hStatus;
        RECT rcStatus;
        int iStatusHeight;


        hTool = GetDlgItem(hwnd, IDC_MAIN_TOOLBAR);
        SendMessage(hTool, TB_AUTOSIZE, 0, 0);
        GetWindowRect(hTool, &rcTool);
        iToolHeight = rcTool.bottom - rcTool.top;

        hStatus = GetDlgItem(hwnd, IDC_MAIN_STATUS);
        SendMessage(hStatus, WM_SIZE, 0, 0);
        GetWindowRect(hStatus, &rcStatus);
        iStatusHeight = rcStatus.bottom - rcStatus.top;

        GetClientRect(hwnd, &rcClient);
        iEditHeight = rcClient.bottom - iToolHeight - iStatusHeight;
        hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
        SetWindowPos(hEdit, NULL, 0, iToolHeight, rcClient.right, iEditHeight, SWP_NOZORDER);

        break;
    }
    case WM_CLOSE: {
        int ret = MessageBox(hwnd, "Are you sure do you want to quit?", "Warning", MB_ICONWARNING | MB_YESNO);
        if(ret == IDYES)
        {
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            ShowWindow( GetConsoleWindow(), SW_SHOW);
            DestroyWindow(hwnd);
        }
        break;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }
    case WM_COMMAND: {
        switch(LOWORD(wParam))
        {
            //FILE
            case ID_FILE_NEW:
                SetDlgItemText(hwnd, IDC_MAIN_EDIT, "");
                break;
            case ID_FILE_OPEN:
                DoFileOpen(hwnd);
                break;
            case ID_FILE_SAVE:
                //RegisterHotKey(hwnd, ID_FILE_SAVE, MOD_CONTROL, 0x53);
                DoFileSaveOnly(hwnd);
                break;
            case ID_FILE_SAVEAS:
                DoFileSave(hwnd);
                break;
            case ID_FILE_EXIT:
                {
                    int ret = MessageBox(hwnd, "Are you sure do you want to quit?", "Warning", MB_ICONWARNING | MB_YESNO);
                    if(ret == IDYES)
                    {
                        PostMessage(hwnd, WM_CLOSE, 0, 0);
                    }
                    break;
                }

            //EDIT
            case ID_EDIT_CUT:
                //RegisterHotKey(hwnd, ID_EDIT_CUT, MOD_CONTROL, 0x43);
                SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, WM_CUT, 0, 0);
                break;
            case ID_EDIT_COPY:
                SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, WM_COPY, 0, 0);
                break;
            case ID_EDIT_PASTE:
                SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, WM_PASTE, 0, 0);
                break;
            case ID_EDIT_SELECTALL:
                //SendDlgItemMessage(hwnd, IDC_MAIN_EDIT,)
                break;
            case ID_EDIT_UNDO:
                SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, WM_UNDO, 0, 0);
                break;
            case ID_EDIT_REDO:
                SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, WM_UNDO, 0, 0);
                break;
            case ID_EDIT_CLEAR:
                SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, WM_CLEAR, 0, 0);
                break;
            //VIEW
            case ID_VIEW_STATUSBAR:
                {
                HMENU hMenu = (HMENU)MAKEINTRESOURCE(IDR_MENU1);
                CheckMenuItem(hMenu, MF_BYCOMMAND | MF_UNCHECKED, ID_VIEW_STATUSBAR);
                UpdateWindow(hwnd);
                }
            case ID_VIEW_WORDWRAP:
                {
                    int valueWordWrap;
                    if(valueWordWrap == 0){
                        //SendDlgItemMessage(hwnd, IDC_MAIN_EDIT,);
                        valueWordWrap = 1;
                    }else{
                        valueWordWrap = 0;
                    }

                }
                UpdateWindow(hwnd);
                break;
            case ID_VIEW_DEBUGWINDOW:
                {
                    int valueDebugWindow;
                    if(valueDebugWindow == 0)
                    {
                        ShowWindow(GetConsoleWindow(), SW_SHOW);
                        valueDebugWindow = 1;
                    }if(valueDebugWindow == 1){
                        ShowWindow(GetConsoleWindow(), SW_HIDE);
                        valueDebugWindow = 0;
                    }
                }
                ShowWindow(GetConsoleWindow(), SW_SHOW);
                break;
            //FORMAT
            case ID_FORMAT_FONT:
                DoSelectFont(hwnd);

                InvalidateRect(hwnd, NULL, TRUE);
                UpdateWindow(hwnd);
                break;
            case ID_FORMAT_DEFAULTFONT:
                DeleteObject(g_hfFont);
                g_hfFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

                InvalidateRect(hwnd, NULL, TRUE);
                UpdateWindow(hwnd);
                break;
            //HELP
            case ID_HELP_GETHELP:
                {
                    char dirHelp[0xff] = "textpadHelp.chm";
                    ShellExecute(NULL, "open", dirHelp, NULL, NULL, SW_SHOWNORMAL);
                    break;
                }
            case ID_HELP_ABOUT:
                {
                    int ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTDIALOG), hwnd, /*(DLGPROC)AboutDlgProc(hwnd, msg, wParam, lParam)*/ AboutDlgProc);
                }
                break;
            case ID_HELP_CHECKFORUPDATES: {
                char linkUpdatesTemp[45] = "https://github.com/PhrotonX/TextPad/releases";
                ShellExecute(NULL, "open", linkUpdatesTemp, NULL, NULL, SW_SHOWNORMAL);
                break;
            }
            case ID_HELP_VIEWONGITHUB: {
                char linkGithub[36] = "https://github.com/PhrotonX/TextPad";
                ShellExecute(NULL, "open", linkGithub, NULL, NULL, SW_SHOWNORMAL);
                break;
            }
        }

        break;
    }
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;
    bool ret;

    wc.cbSize           =   sizeof(WNDCLASSEX);
    wc.style            =   0;
    wc.lpfnWndProc      =   WndProc;
    wc.cbClsExtra       =   0;
    wc.cbWndExtra       =   0;
    wc.hInstance        =   hInstance;
    wc.hIcon            =   LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor          =   LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    =   (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName     =   MAKEINTRESOURCE(IDR_MENU1);
    wc.lpszClassName    =   g_szClassName;
    wc.hIconSm          =   LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(WS_EX_APPWINDOW, g_szClassName, "TextPad", WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, 1020, 520, NULL, NULL, hInstance, NULL);

    if(hwnd == NULL){
        MessageBox(NULL, "Error", "Window Creation Failed", MB_ICONHAND | MB_OK);
        return 0;
    }

    HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
    if(hAccel == NULL)
    {
        MessageBox(hwnd, "Could not create accelerators", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    ShowWindow( GetConsoleWindow(), SW_HIDE);
    UpdateWindow(hwnd);

    while(GetMessage(&Msg, NULL, 0, 0) > 0){
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    while((ret = GetMessage(&Msg, NULL, 0, 0)) > 0)
    {
        if(ret <= 0)
        {
            MessageBox(hwnd, "Message failed!", "Error", MB_ICONERROR | MB_OK);
            return 0;
        }else{
            if(TranslateAccelerator(hwnd, hAccel, &Msg))
            {
                while(GetMessage(&Msg, NULL, 0, 0) > 0){
                    TranslateMessage(&Msg);
                    DispatchMessage(&Msg);
                }
            }
        }
    }

    return Msg.wParam;
}
