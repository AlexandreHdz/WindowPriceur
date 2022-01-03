#define NOMINMAX
#include <windows.h>
#include "resource.h"
//#include "Payoff.cpp"
//#include "Option.cpp"
#include "Data.h"
#include "Payoff.h"
#include "Option.h"
#include <string>
#include <iostream>

const char ClassName[] = "MainWindowClass";
const char DialogClassName[] = "DialogClassName";

HWND hWndDlgBox;
HWND hDlgButton;
// Main Screen --------------------------------------------
HWND hWndText;
HWND hWndPrice;
HWND hWndButtonCallEU;
HWND hWndButtonPutEU;
HWND hWndButtonCallAS;
HWND hWndButtonPutAS;
HWND hWndButtonCallKO;
HWND hWndButtonCallRKO;
HWND hWndButtonPutKO;
HWND hWndButtonPutRKO;

//Input Screen --------------------------------------------
HWND hWndSpot;
HWND hWndStrike;
HWND hWndMaturity;
HWND hWndTaux;
HWND hWndVol;
HWND hWndBar;

LRESULT CALLBACK DlgPrice(HWND hWndPrice, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
    case WM_INITDIALOG:
        //SetDlgItemText(hWndPrice, IDC_RESULT, (char*)&Prix);
        //char* buf;
        //int len;
        //len = GetWindowTextLength(GetDlgItem(GetParent(hWndPrice), IDC_VOL));
        //buf = (char*)GlobalAlloc(GPTR, len + 1);
        //GetDlgItemText(GetParent(hWndPrice), IDC_VOL, buf, len + 1);
        //DOUBLE Vol = 0.0;
        //Vol = atof(buf);
        //GlobalFree((HANDLE)buf);
        //SendMessage(GetParent(hWndPrice), WM_CLOSE, 0, 0);
        //SetDlgItemText(hWndPrice, IDC_RESULT,(char*)&Vol);
        return TRUE;
    break;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_OK:
        {
            SendMessage(hWndPrice, WM_CLOSE, 0, 0);
        }
        break;
        }
        return 0;
    }
    break;
    case WM_CLOSE:
        DestroyWindow(hWndPrice);
        hWndPrice = NULL;
        break;

    default:
        return FALSE; // (DefWindowProc(hWndPrice, Msg, wParam, lParam));
    }
}



LRESULT CALLBACK DlgProc(HWND hWnDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
    case WM_INITDIALOG:
        return TRUE;
    break;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_PRICER:
        {
            
            char* buf;
            int len;
            // SPOT----------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_SPOT));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_SPOT, buf, len + 1);
            DOUBLE Spot = atof(buf);
            GlobalFree((HANDLE)buf);

            // STRIKE--------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_STRIKE));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_STRIKE, buf, len + 1);
            DOUBLE Strike = atof(buf);
            GlobalFree((HANDLE)buf);

            // MATURITY-------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_MAT));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_MAT, buf, len + 1);
            DOUBLE Mat = atof(buf);
            GlobalFree((HANDLE)buf);

            // TAUX------------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_TAUX));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_TAUX, buf, len + 1);
            DOUBLE Taux = atof(buf);
            GlobalFree((HANDLE)buf);

            // Vol------------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_VOL));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_VOL, buf, len + 1);
            DOUBLE Vol = atof(buf);
            GlobalFree((HANDLE)buf);
            
            Data actif;
            actif.S = Spot;
            actif.K = Strike;
            actif.r = Taux;
            actif.T = Mat;
            actif.v = Vol;
            PayOff* pay_off_call = new PayOffCall();
            Europeene call(pay_off_call);
            double Prime = call.prime(actif);
            TCHAR toStr[50];
            sprintf_s(toStr, "%f", Prime);
            

            //hwndText = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("STATIC"), TEXT(MainScreen),
            //    WS_CHILD | WS_VISIBLE, 350, 20, 300,
            //    20, hWnDlg, NULL, NULL, NULL);
            //HWND Result = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_RESULT), GetParent(hWnDlg), (DLGPROC)DlgPrice);




            MessageBox(GetParent(hWnDlg),toStr, "", MB_OK);

            SendMessage(hWnDlg, WM_CLOSE,0,0);

        }
        break;
        case IDC_BACK:
        {
            SendMessage(hWnDlg, WM_CLOSE, 0, 0);
        }
        break;
        }
        return 0;
    }
    break;

    case WM_CLOSE:
        DestroyWindow(hWnDlg);
        hWnDlg = NULL;
        break;

    default:
        return FALSE; // (DefWindowProc(hWnDlg, Msg, wParam, lParam));
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{

    switch (Msg)

    {
    case WM_CREATE:
    {
        WNDCLASSEX  wc;

        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = 0;
        wc.lpfnWndProc = (WNDPROC)DlgProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
        wc.hIcon = NULL;
        wc.hIconSm = NULL;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = DialogClassName;

        if (!RegisterClassEx(&wc))
        {
            MessageBox(NULL, "Failed To Register The Dialog Class.", "Error", MB_OK | MB_ICONERROR);
            return FALSE;
        }

        const char MainScreen[] = "Quelle option voulez vous pricer ?";
        hWndText = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("STATIC"), TEXT(MainScreen),
            WS_CHILD | WS_VISIBLE, 350, 20, 300,
            20, hWnd, NULL, NULL, NULL);

        //Call Européen -----------------------------------------------
        hWndButtonCallEU = CreateWindowEx(
            0,
            "BUTTON",
            "Call Europeen",
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            30,
            30,
            200,
            40,
            hWnd,
            (HMENU)IDB_WNDBUTTON,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        if (!hWndButtonCallEU)
            MessageBox(NULL, "Main Window Button Failed.", "Error", MB_OK | MB_ICONERROR);

        // Put Européen -------------------------------------------
        hWndButtonPutEU = CreateWindowEx(
            0,
            "BUTTON",
            "Put Europeen",
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            750,
            30,
            200,
            40,
            hWnd,
            (HMENU)IDB_WNDBUTTON,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        if (!hWndButtonPutEU)
            MessageBox(NULL, "Main Window Button Failed.", "Error", MB_OK | MB_ICONERROR);

        // Call Asiatique ------------------------------------------
        hWndButtonCallAS = CreateWindowEx(
            0,
            "BUTTON",
            "Call Asiatique",
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            30,
            100,
            200,
            40,
            hWnd,
            (HMENU)IDB_WNDBUTTON,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        if (!hWndButtonCallAS)
            MessageBox(NULL, "Main Window Button Failed.", "Error", MB_OK | MB_ICONERROR);

        //Put Asiatique ---------------------------------------------------
        hWndButtonPutAS = CreateWindowEx(
            0,
            "BUTTON",
            "Put Asiatique",
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            750,
            100,
            200,
            40,
            hWnd,
            (HMENU)IDB_WNDBUTTON,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        if (!hWndButtonPutAS)
            MessageBox(NULL, "Main Window Button Failed.", "Error", MB_OK | MB_ICONERROR);

        // Call KO--------------------------------------------------------------
        hWndButtonCallKO = CreateWindowEx(
            0,
            "BUTTON",
            "Call KO",
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            30,
            170,
            200,
            40,
            hWnd,
            (HMENU)IDB_WNDBUTTON,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        if (!hWndButtonCallKO)
            MessageBox(NULL, "Main Window Button Failed.", "Error", MB_OK | MB_ICONERROR);

        // Put KO-------------------------------------------------------------------------------

        hWndButtonPutKO = CreateWindowEx(
            0,
            "BUTTON",
            "Put KO",
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            750,
            170,
            200,
            40,
            hWnd,
            (HMENU)IDB_WNDBUTTON,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        if (!hWndButtonPutKO)
            MessageBox(NULL, "Main Window Button Failed.", "Error", MB_OK | MB_ICONERROR);


        // Call RKO ----------------------------------------------------------
        hWndButtonCallRKO = CreateWindowEx(
            0,
            "BUTTON",
            "Call RKO",
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            30,
            240,
            200,
            40,
            hWnd,
            (HMENU)IDB_WNDBUTTON,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        if (!hWndButtonCallRKO)
            MessageBox(NULL, "Main Window Button Failed.", "Error", MB_OK | MB_ICONERROR);

        // Put RKO ----------------------------------------------------------------
        hWndButtonPutRKO = CreateWindowEx(
            0,
            "BUTTON",
            "Put RKO",
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            750,
            240,
            200,
            40,
            hWnd,
            (HMENU)IDB_WNDBUTTON,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        if (!hWndButtonPutRKO)
            MessageBox(NULL, "Main Window Button Failed.", "Error", MB_OK | MB_ICONERROR);
    }
    break;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDB_WNDBUTTON:
        {
            switch (HIWORD(wParam))
            {
            case BN_CLICKED:
            {
                if (!hWndDlgBox)
                {
                    hWndDlgBox = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PRICE), hWnd, (DLGPROC)DlgProc);
                    ShowWindow(hWndDlgBox, SW_SHOW);

                    if (!hWndDlgBox)
                        MessageBox(NULL, "Dialog Box Failed.", "Error", MB_OK | MB_ICONERROR);
                }
            }
            break;
            }
        }
        break;
        }
        return 0;
    }
    break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return (DefWindowProc(hWnd, Msg, wParam, lParam));
    }
    return 0;
}



INT WINAPI WinMain(HINSTANCE  hInstance,
    HINSTANCE  hPrevInstance,
    LPSTR      lpCmdLine,
    INT        nCmdShow)
{
    WNDCLASSEX    wc;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = (WNDPROC)WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hIconSm = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = ClassName;

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Failed To Register The Window Class.", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    HWND    hWnd;
    hWnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        ClassName,
        "Pricer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1000,
        1000,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!hWnd)
    {
        MessageBox(NULL, "Window Creation Failed.", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    MSG    Msg;
    while (GetMessage(&Msg, NULL, 0, 0))
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}