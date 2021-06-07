#include <windows.h>
#include "resource.h"
#include <stdio.h>
#include <commctrl.h>
#include <string.h>
#include <iostream>
//#include <strsafe.h>
//#include <winuser.h>
#include <tchar.h>
//#include <windowsx.h>

const char g_szClassName[] = "textPad";
#define IDC_MAIN_EDIT       101
#define IDC_MAIN_TOOLBAR    102
#define IDC_MAIN_STATUS     103
#define WM_MOUSEHOVER       0x02A1
#define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))  //STATUS BAR INFO

HFONT g_hfFont = NULL;
COLORREF g_rgbText = RGB(0, 0, 0);

int valueStatusBar = 0;
int valueToolBar = 0;
int valueWordWrap = 0;
class mouseTrackEvents{
    bool m_bMouseTracking;
public:

    mouseTrackEvents() : m_bMouseTracking(false)
    {
    }

    void OnMouseMove(HWND hwnd)
    {
        if (!m_bMouseTracking)
        {
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(tme);
            tme.hwndTrack = hwnd;
            tme.dwFlags = TME_HOVER | TME_LEAVE;
            tme.dwHoverTime = HOVER_DEFAULT;
            TrackMouseEvent(&tme);
            m_bMouseTracking = true;
        }
    }
    void Reset(HWND hwnd)
    {
        m_bMouseTracking = false;
    }

    //void foo(HWND hwnd);
};
/*
void mouseTrackEvents::foo(HWND hwnd){
 UINT GetMouseHoverTime()
        {
            UINT msec;
            if (SystemParametersInfo(SPI_GETMOUSEHOVERTIME, 0, &msec, 0))
            {
                return msec;
            }
            else
            {
                return 0;
            }
        }
}*/

void Invalidate(HWND window) {
    RECT rect;
    GetClientRect(window, &rect);
    InvalidateRect(window, &rect, TRUE);
}

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

BOOL WordWrap(HWND hEdit, LPCTSTR pszFileName)
{
    HANDLE hFile;
    BOOL bSuccess = FALSE;

    hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if(hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dwFileSize;
        GetWindowTextLength(hEdit);

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
                            SetWindowText(hEdit, pszFileText);
                   }
                   GlobalFree(pszFileText);
            }
        }
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
    case WM_CLOSE:
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
    mouseTrackEvents mouseTrackEventsObject;

    //
    enum TimerId { TimerId_MouseHover = 1 };
    static const UINT HoverTimeoutInMs = 1000;
    static int PrevX = INT_MIN;
    static int PrevY = INT_MIN;
    static bool IsMouseOutside = true;
    static bool IsMouseHovered = false;
    //

    PAINTSTRUCT ps;
    HDC dc;
    RECT rect;
    int i;

    switch(msg){
    case WM_PAINT:
        dc = BeginPaint(hwnd, &ps);
        EndPaint(hwnd, &ps);
        return 0;
    case WM_CREATE: {
        //HFONT hfDefault;
        HWND hEdit;
        HFONT hFont;

        HWND hTool;
        TBBUTTON tbb[13];
        TBADDBITMAP tbab;

        HWND hStatus;
        int statwidhts[] = {100, -1};

        g_hfFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);


        if(valueWordWrap == 0)
        {
            hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL
            | WS_VSCROLL | ES_MULTILINE, 0, 0, 100, 100,
            hwnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
            //SendMessage(hwnd, EM_SETHANDLE, 0, 0);
            //SendMessage(hwnd, WM_SETTEXT, 0, 0);
            SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, WM_SETTEXT, 0, 0);
        }else
        if(valueWordWrap == 1)
        {
            hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL
            | WS_VSCROLL | ES_AUTOHSCROLL | WS_HSCROLL, 0, 0, 100, 100,
            hwnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
            //SendMessage(hwnd, EM_SETHANDLE, 0, 0);
            //SendMessage(hwnd, WM_SETTEXT, 0, 0);
            SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, WM_SETTEXT, 0, 0);
        }
        //SendMessage(hwnd, EM_SETWORDBREAKPROC, 0, 0);
        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);

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
        tbb[0].iString = IDS_TIPS_NEW;
        SendDlgItemMessage(hwnd, IDS_TIPS_NEW, TB_SETMAXTEXTROWS, 18, 0);
        //SendDlgItemMessage(hwnd, IDS_TIPS_NEW, TBSTYLE_TOOLTIPS, 0, (LPARAM)"Create a new file");
        SendDlgItemMessage(hwnd, ID_FILE_NEW, TBSTYLE_TOOLTIPS, 0, (LPARAM)"Create a new file");

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

        tbb[8].fsStyle = TBSTYLE_SEP;

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

        if(hStatus == NULL)
            MessageBox(hwnd, "Could not create Status Bar", "Error", MB_OK | MB_ICONERROR);

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

        if(valueToolBar == 0)
        {
            hTool = GetDlgItem(hwnd, IDC_MAIN_TOOLBAR);
            SendMessage(hTool, TB_AUTOSIZE, 0, 0);
            GetWindowRect(hTool, &rcTool);
            iToolHeight = rcTool.bottom - rcTool.top;
            UpdateWindow(hwnd);
        }
        if(valueStatusBar == 0)
        {
            hStatus = GetDlgItem(hwnd, IDC_MAIN_STATUS);
            SendMessage(hStatus, WM_SIZE, 0, 0);
            GetWindowRect(hStatus, &rcStatus);
            iStatusHeight = rcStatus.bottom - rcStatus.top;
            UpdateWindow(hwnd);
        }

        GetClientRect(hwnd, &rcClient);
        GetWindowRect(hwnd, &rcStatus);
        GetWindowRect(hwnd, &rcTool);
        if(valueStatusBar == 1 && valueToolBar == 0){
            GetWindowRect(hwnd, &rcTool);
            iEditHeight = rcClient.bottom - iToolHeight;
            UpdateWindow(hwnd);
        }if(valueToolBar == 1 && valueStatusBar == 0){
            GetWindowRect(hwnd, &rcStatus);
            iEditHeight = rcClient.bottom - iStatusHeight;
            UpdateWindow(hwnd);
        }if(valueToolBar == 0 && valueStatusBar == 0){
            iEditHeight = rcClient.bottom - iToolHeight - iStatusHeight;
            UpdateWindow(hwnd);
        }if(valueToolBar == 1 && valueStatusBar == 1){
            iEditHeight = rcClient.bottom - rcClient.right;
            UpdateWindow(hwnd);
        }

        InvalidateRect(hwnd, NULL, TRUE);
        UpdateWindow(hwnd);
        if(UpdateWindow(hwnd))
        {
            hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
            if(valueToolBar==0)
            {
                SetWindowPos(hEdit, NULL, 0, iToolHeight, rcClient.right, iEditHeight, NULL);
            }if(valueToolBar == 1){
                SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, NULL);
            }
            UpdateWindow(hwnd);
        }
        break;
    }
/*
    case WM_MOUSEMOVE:
        mouseTrackEventsObject.OnMouseMove(hwnd);
        return 0;
    case WM_MOUSELEAVE:
        mouseTrackEventsObject.Reset(hwnd);
        return 0;
    case WM_MOUSEHOVER:
        SendDlgItemMessage(hwnd, IDC_MAIN_STATUS, SB_SETTEXT, 1, (LPARAM)"Create a new file");
        mouseTrackEventsObject.Reset(hwnd);
        return 0;
        */
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
                //SendDlgItemMessage(hwnd, ID_FILE_NEW, TBSTYLE_TOOLTIPS, 0, (LPARAM)"Create a new file");
                /*
                switch(msg)
                {
                    case WM_MOUSEMOVE:
                        mouseTrackEventsObject.OnMouseMove(hwnd);
                        return 0;
                    case WM_MOUSELEAVE:
                        mouseTrackEventsObject.Reset(hwnd);
                        return 0;
                    case WM_MOUSEHOVER:
                        SendDlgItemMessage(hwnd, IDC_MAIN_STATUS, SB_SETTEXT, 1, (LPARAM)"Create a new file");
                        mouseTrackEventsObject.OnMouseMove(hwnd);
                        return 0;
                }
                */
                break;
            case ID_FILE_OPEN:
                DoFileOpen(hwnd);
                break;
            case ID_FILE_SAVE:
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
                SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, EM_SETSEL, 0, -1);
                return 0;
                break;
            case ID_EDIT_UNDO:
                if(SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, EM_CANUNDO, 0, 0))
                    SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, WM_UNDO, 0, 0);
                break;
            case ID_EDIT_REDO:
                SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, WM_UNDO, 0, 0);
                break;
            case ID_EDIT_CLEAR:
                SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, WM_CLEAR, 0, 0);
                break;
            //VIEW
            case ID_VIEW_TOOLBAR:
                {
                    if(valueToolBar == 0)
                    {
                        //SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, EM_GETHANDLE, 0, 0);
                        //SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, EM_SETHANDLE, 0, 0);
                        valueToolBar = 1;
                    }else if(valueToolBar == 1)
                    {
                        //SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, EM_GETHANDLE, 0, 0);
                        //SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, EM_SETHANDLE, 0, 0);
                        valueToolBar = 0;
                    }
                    InvalidateRect(hwnd, NULL, TRUE);
                    UpdateWindow(hwnd);
                    SendMessage(hwnd, WM_SIZE, 0, 0);
                }
                break;
            case ID_VIEW_STATUSBAR:
                {
                    //HMENU hMenu = (HMENU)MAKEINTRESOURCE(IDR_MENU1);
                    HMENU hMenu = (HMENU)IDR_MENU1;
                /*

                int valueStatusBar;

                if(valueStatusBar == 0)
                {
                    SetMenuItemInfo(hMenu, ID_VIEW_STATUSBAR, FALSE, MF_UNCHECKED);
                }
                //CheckMenuItem(hMenu, MF_BYCOMMAND | MF_UNCHECKED, ID_VIEW_STATUSBAR);
                UpdateWindow(hwnd);
                */
                if(valueStatusBar == 0)
                {
                    CheckMenuItem(hMenu, ID_VIEW_STATUSBAR, MF_BYPOSITION | MF_CHECKED);
                    //SendDlgItemMessage(hwnd, ID_VIEW_STATUSBAR, MF_UNCHECKED | MF_BYPOSITION, 0, 0);
                    SendMessage(hwnd, WS_MAXIMIZE, 0, 0);
                    SendMessage(hwnd, WS_DLGFRAME, 0, 0);
                    valueStatusBar = 1;
                }else{
                    CheckMenuItem(hMenu, ID_VIEW_STATUSBAR, MF_BYPOSITION | MF_UNCHECKED);
                    //SendDlgItemMessage(hwnd, ID_VIEW_STATUSBAR, MF_CHECKED | MF_BYPOSITION, 0, 0);
                    SendMessage(hwnd, WS_MAXIMIZE, 0, 0);
                    SendMessage(hwnd, WS_DLGFRAME, 0, 0);
                    valueStatusBar = 0;
                }

                    InvalidateRect(hwnd, NULL, TRUE);
                    UpdateWindow(hwnd);
                    SendMessage(hwnd, WM_SIZE, 0, 0);
                }
                break;
            case ID_VIEW_WORDWRAP:
                {
                    if(valueWordWrap == 0){
                        valueWordWrap = 1;
                    }if(valueWordWrap == 1){
                        valueWordWrap = 0;
                    }
                    //SendMessage(hwnd, EM_GETHANDLE, 0, 0);
                    //SendMessage(hwnd, WM_GETTEXT, 0, 0);
                    SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, WM_GETTEXT, 0, 0);
                    InvalidateRect(hwnd, NULL, TRUE);
                    UpdateWindow(hwnd);
                    //SendMessage(hwnd, WM_SETTEXT, 0, 0);
                    SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, WM_SETTEXT, 0, 0);
                    SendMessage(hwnd, WM_SIZE, 0, 0);
                }
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
            case ID_VIEW_REFRESHWINDOW:
                InvalidateRect(hwnd, NULL, TRUE);
                UpdateWindow(hwnd);
                SendMessage(hwnd, WM_SIZE, 0, 0);
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
    case WM_NOTIFY:{
        switch(((LPNMHDR)lParam)->code)
        {
        case TTN_GETDISPINFO:
            {
                LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;

                lpttt->hinst = g_hinst;

                UINT_PTR idButton = lpttt->hdr.idFrom;

                switch(idButton){
                case ID_FILE_NEW:
                    lpttt->lpszText = MAKEINTRESOURCE(IDS_TIPS_NEW);
                    break;
                }
            }
        }
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
    //bool ret;

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
// PATHETIC COPY-PASTED CODE FROM THE INTERNET
/*
    NOTIFYICONDATA IconData = {0};

    IconData.cbSize = sizeof(IconData);
    IconData.hWnd   = hwnd;
    IconData.uFlags = NIF_INFO;

    HRESULT hr = StringCchCopy(IconData.szInfo,
                               ARRAYSIZE(IconData.szInfo),
                               TEXT("Your message text goes here."));

    if(FAILED(hr))
    {
    }
    IconData.uTimeout = 15000; // in milliseconds

    Shell_NotifyIcon(NIM_MODIFY, &IconData);
*/

    HANDLE hIcon = LoadImage(0, _T("img/textPad_icon.ico"), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
    if(hIcon){
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

        SendMessage(GetWindow(hwnd, GW_OWNER), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        SendMessage(GetWindow(hwnd, GW_OWNER), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    }

    ShowWindow(hwnd, nCmdShow);
    ShowWindow( GetConsoleWindow(), SW_HIDE);
    UpdateWindow(hwnd);

    while (GetMessage(&Msg, NULL, 0, 0))
        {
            if (!TranslateAccelerator(hwnd, hAccel, &Msg))
            {
                SendDlgItemMessage(hwnd, IDC_MAIN_EDIT, EM_GETHANDLE, 0, 0);
                TranslateMessage(&Msg);
                DispatchMessage(&Msg);
            }
        }
    return Msg.wParam;
}
