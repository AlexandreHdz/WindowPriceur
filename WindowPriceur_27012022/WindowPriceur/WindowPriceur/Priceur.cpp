#define NOMINMAX

#include "resource.h"
#include "Data.h"
#include "Payoff.h"
#include "Option.h"
#include <string>
#include <cstring>
#include <iostream>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <atlstr.h>
#include "Yahoo.h"
#pragma comment(lib, "comctl32.lib")
#define CURL_STATICLIB



const char ClassName[] = "MainWindowClass";
const char DialogClassName[] = "DialogClassName";
const char GraphClass[] = "GraphClass";

HWND hWndDlgBox;
HWND hDlgButton;

// Main Screen --------------------------------------------
HWND hWndText;
HWND hWndMenu;
HWND hWndPrice;
HWND hWndButtonCallEU;
HWND hWndButtonPutEU;
HWND hWndButtonCallAS;
HWND hWndButtonPutAS;
HWND hWndButtonCallKO;
HWND hWndButtonCallRKO;
HWND hWndButtonPutKO;
HWND hWndButtonPutRKO;

// PROC BARRE DE CALCUL ///////////////////////////////////////
BOOL CALLBACK DlgPB(HWND hWnDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
    case WM_INITDIALOG:
        SendDlgItemMessage(hWnDlg, IDE_PB, PBM_SETRANGE, 0, MAKELONG(0, 100));
        SendDlgItemMessage(hWnDlg, IDE_PB, PBM_SETSTEP, 1, 0);
        return TRUE;
        break;
    case WM_CLOSE:
        DestroyWindow(hWnDlg);
        hWndDlgBox = NULL;
        break;

    default:
        return FALSE;
        break;
    }
}


LRESULT CALLBACK DlgPriceBA(HWND hWndPrice, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
    case WM_INITDIALOG:
    {
        return TRUE;
    }
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
        return FALSE;
    }
}



// Creation list view pour data Yahoo
HWND CreateYahooView(HWND hWndParent, Matrice Yahoo, std::vector<string> YahooDate)
{
    //Declaration
    HWND hWndList;
    RECT rcl;
    int index;
    LV_COLUMN lvC;
    char szText[260];
    LV_ITEM lvI;
    int iSubItem;
    InitCommonControls();
    GetClientRect(hWndParent, &rcl);

    //Fenetre
    hWndList = CreateWindowEx(0L,
        WC_LISTVIEW,
        "Historique",
        WS_VISIBLE | WS_BORDER | WS_CHILD | WS_VSCROLL | LVS_REPORT | LVS_EDITLABELS,
        rcl.left + 200*2 , rcl.top + 10, 390*2, 280*2,
        hWndParent,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWndDlgBox, GWLP_HINSTANCE),
        NULL);

    if (hWndList == NULL)
        return NULL;

    //Param Globale Des colonnes
    lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvC.fmt = LVCFMT_LEFT;
    lvC.cx = 100;
    lvC.pszText = szText;
    TCHAR toStr[50];
    //Insert colonne

    for (index = 0; index <= Yahoo.m; index++) {
        lvC.iSubItem = index;
        LoadString((HINSTANCE)GetWindowLongPtr(hWndDlgBox, GWLP_HINSTANCE),
            IDS_DATE + index,
            szText,
            sizeof(szText));
        if (ListView_InsertColumn(hWndList, index, &lvC) == -1)
            return NULL;
    }

    //Insertion des élements
    lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
    lvI.state = 0;
    lvI.stateMask = 0;


    for (int row = 0; row < Yahoo.n; row++)
    {
        lvI.iItem = row;
        lvI.iSubItem = 0;
        _tcscpy_s(toStr, CA2T(YahooDate[Yahoo.n - row-1].c_str()));
        lvI.pszText = toStr;
        lvI.cchTextMax = Yahoo.n;
        if (ListView_InsertItem(hWndList, &lvI) == -1)
            return NULL;
        for (int col = 1; col <= Yahoo.m; col++)
        {
            lvI.iSubItem = col;
            sprintf_s(toStr, "%f", Yahoo.valeurs[Yahoo.n-row-1][col]);
            lvI.pszText = toStr;
            ListView_SetItemText(hWndList,row,col,toStr);
        }
    }

    
    return (hWndList);
}

LRESULT CALLBACK DlgGraph(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
    case WM_CREATE:
        return TRUE;
        break;
    case WM_PAINT:
    {
        char* buf;
        int len;

        len = GetWindowTextLength(GetDlgItem(GetParent(hWnd), IDC_TICKER));
        buf = (char*)GlobalAlloc(GPTR, len + 1);
        GetDlgItemText(GetParent(hWnd), IDC_TICKER, buf, len + 1);
        string Ticker = buf;
        GlobalFree((HANDLE)buf);

        len = GetWindowTextLength(GetDlgItem(GetParent(hWnd), IDC_DD));
        buf = (char*)GlobalAlloc(GPTR, len + 1);
        GetDlgItemText(GetParent(hWnd), IDC_DD, buf, len + 1);
        string DD = buf;
        GlobalFree((HANDLE)buf);

        len = GetWindowTextLength(GetDlgItem(GetParent(hWnd), IDC_DF));
        buf = (char*)GlobalAlloc(GPTR, len + 1);
        GetDlgItemText(GetParent(hWnd), IDC_DF, buf, len + 1);
        string DF = buf;
        GlobalFree((HANDLE)buf);

        YahooFinanceAPI api;
        Matrice Yahoo = api.get_ticker_data(Ticker, DD, DF);
        std::vector<string> YahooDate = api.get_ticker_date(Ticker, DD, DF);

        PAINTSTRUCT ps;
        HDC hdc;
        hdc = BeginPaint(hWnd, &ps);

        for (int i = 0; i < Yahoo.n; i++)
        {
            if (i == 0)
            {
                MoveToEx(hdc, 0, 0, NULL);
                LineTo(hdc, 0 + 5, Yahoo.valeurs[i][4]);
            }
            else
            {
                MoveToEx(hdc, 360 / i, 290 / Yahoo.valeurs[i][4],NULL);
                LineTo(hdc, 360 / i, 290 / Yahoo.valeurs[i][4]);
            }
        }
        EndPaint(hWnd, &ps);


    }
    break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    default:
        return (DefWindowProc(hWnd, Msg, wParam, lParam));
    }
    return 0;
}

HWND CreateGraph(HWND hWnd, Matrice Yahoo, std::vector<string> YahooDate, RECT rc)
{
    WNDCLASSEX  wcg;

    wcg.cbSize = sizeof(WNDCLASSEX);
    wcg.style = 0;
    wcg.lpfnWndProc = (WNDPROC)DlgGraph;
    wcg.cbClsExtra = 0;
    wcg.cbWndExtra = 0;
    wcg.hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
    wcg.hIcon = NULL;
    wcg.hIconSm = NULL;
    wcg.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcg.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcg.lpszMenuName = NULL;
    wcg.lpszClassName = GraphClass;

    if (!RegisterClassEx(&wcg))
    {
        MessageBox(NULL, "Failed To Register The Window Class.", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    /*HWND    hWndGraph;
    hWndGraph = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        GraphClass,
        "Historique",
        WS_CHILD,
        rc.left +20,
        rc.top + 140*2,
        360,
        290,
        hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL);*/
    HWND hWndGraph;
    hWndGraph = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        GraphClass,
        "Pricer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1000,
        1000,
        NULL,
        NULL,
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL);
    
    SendMessage(hWndGraph, WM_PAINT, 0, 0);
    return (hWndGraph);

}

//Proc Get Data Yahoo Finance
LRESULT CALLBACK DlgGetData(HWND hWndYahoo, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
    case WM_INITDIALOG:
    {
        ShowWindow(GetDlgItem(hWndYahoo, IDC_REND), SW_HIDE);
        ShowWindow(GetDlgItem(hWndYahoo, IDC_VARIANCE), SW_HIDE);
        ShowWindow(GetDlgItem(hWndYahoo, IDC_STATICREND), SW_HIDE);
        ShowWindow(GetDlgItem(hWndYahoo, IDC_STATICVAR), SW_HIDE);
        TCHAR toStr[50];
        std::string DateAsof = GetDateAsof(FALSE);
        _tcscpy_s(toStr, CA2T(DateAsof.c_str()));
        SetDlgItemText(hWndYahoo, IDC_DF, toStr);
        std::string DateDD = GetDateAsof(TRUE);
        _tcscpy_s(toStr, CA2T(DateDD.c_str()));
        SetDlgItemText(hWndYahoo, IDC_DD, toStr);
        return TRUE;
    }
    break;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_VALID:
        {
            char* buf;
            int len;

            len = GetWindowTextLength(GetDlgItem(hWndDlgBox, IDC_TICKER));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWndDlgBox, IDC_TICKER, buf, len + 1);
            string Ticker = buf;
            GlobalFree((HANDLE)buf);
            
            len = GetWindowTextLength(GetDlgItem(hWndDlgBox, IDC_DD));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWndDlgBox, IDC_DD, buf, len + 1);
            string DD = buf;
            GlobalFree((HANDLE)buf);

            len = GetWindowTextLength(GetDlgItem(hWndDlgBox, IDC_DF));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWndDlgBox, IDC_DF, buf, len + 1);
            string DF = buf;
            GlobalFree((HANDLE)buf);

            YahooFinanceAPI api;
            Matrice Yahoo = api.get_ticker_data(Ticker,DD,DF);
            std::vector<string> YahooDate = api.get_ticker_date(Ticker, DD, DF);
            HWND hWndListview = CreateYahooView(hWndYahoo, Yahoo,YahooDate);

            TCHAR toStr[50];
            std::vector<double> DailyRdt = Dailyreturn(Yahoo);
            double Rdt =Moyenne(DailyRdt);
            sprintf_s(toStr, "%f", Rdt);
            SetDlgItemText(hWndDlgBox, IDC_REND, toStr);
            double Var = EcartType(DailyRdt);
            sprintf_s(toStr, "%f", Var);
            SetDlgItemText(hWndDlgBox, IDC_VARIANCE, toStr);
            ShowWindow(GetDlgItem(hWndYahoo, IDC_REND), SW_SHOW);
            ShowWindow(GetDlgItem(hWndYahoo, IDC_VARIANCE), SW_SHOW);
            ShowWindow(GetDlgItem(hWndYahoo, IDC_STATICREND), SW_SHOW);
            ShowWindow(GetDlgItem(hWndYahoo, IDC_STATICVAR), SW_SHOW);

            RECT rc;
            GetClientRect(hWndYahoo, &rc);
            /*HWND hWndGraph = CreateGraph(hWndYahoo, Yahoo, YahooDate,rc);
            ShowWindow(hWndGraph, SW_SHOW);*/
            
        }
        break;
        case IDC_RET:
        {

            SendMessage(hWndYahoo, WM_CLOSE, 0, 0);
        }
        break;
        }
        return 0;
    }
    break;

    case WM_CLOSE:
        
        DestroyWindow(hWndDlgBox);
        hWndDlgBox = NULL;
        break;

    default:
        return FALSE;
    }
}

// PROC CALL EU ////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK DlgProcCallEu(HWND hWnDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
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

            HWND Calcul = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PB), hWnDlg, (DLGPROC)DlgPB);

            // SPOT----------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_SPOT));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_SPOT, buf, len + 1);
            double Spot = atof(buf);
            GlobalFree((HANDLE)buf);

            // STRIKE--------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_STRIKE));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_STRIKE, buf, len + 1);
            double Strike = atof(buf);
            GlobalFree((HANDLE)buf);

            // MATURITY-------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_MAT));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_MAT, buf, len + 1);
            double Mat = atof(buf);
            GlobalFree((HANDLE)buf);

            // TAUX------------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_TAUX));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_TAUX, buf, len + 1);
            double Taux = atof(buf);
            GlobalFree((HANDLE)buf);

            // Vol------------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_VOL));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_VOL, buf, len + 1);
            double Vol = atof(buf);

            GlobalFree((HANDLE)buf);
            Data actif;
            actif.S = Spot;
            actif.K = Strike;
            actif.r = Taux;
            actif.T = Mat;
            actif.v = Vol;
            double Prime = 0.0;

            ShowWindow(Calcul, SW_SHOW);
            std::vector<double> test;
            for (int i = 0; i < 100000; i++)
            {
                if ((i % 1000) == 0)
                {
                    SendDlgItemMessage(Calcul, IDE_PB, PBM_STEPIT, 0, 0);
                }
                std::vector<double> proc_prix = MBG(actif.S, actif.r, actif.v, actif.T);
                double last_value = proc_prix[proc_prix.size() - 1];
                Prime = Prime + std::max(last_value - actif.K, (double)0.0);
            }
            Prime = Prime /100000.0;

            SendMessage(Calcul, WM_CLOSE, 0, 0);
            TCHAR toStr[50];
            sprintf_s(toStr, "%f", Prime);
            SetDlgItemText(hWnDlg, IDC_RESULT, toStr);
            
            double Price = call_price(actif.S, actif.K, actif.r, actif.v, actif.T);
            sprintf_s(toStr, "%f", Price);
            SetDlgItemText(hWnDlg, IDC_RESULT2, toStr);

            double Delta = call_delta(actif.S, actif.K, actif.r, actif.v, actif.T);
            sprintf_s(toStr, "%f", Delta);
            SetDlgItemText(hWnDlg, IDC_DELTA, toStr);

            double Gamma = call_gamma(actif.S, actif.K, actif.r, actif.v, actif.T);
            sprintf_s(toStr, "%f", Gamma);
            SetDlgItemText(hWnDlg, IDC_GAMMA, toStr);

            double Vega = call_vega(actif.S, actif.K, actif.r, actif.v, actif.T);
            sprintf_s(toStr, "%f", Vega);
            SetDlgItemText(hWnDlg, IDC_VEGA, toStr);

            double Theta = call_theta(actif.S, actif.K, actif.r, actif.v, actif.T);
            sprintf_s(toStr, "%f", Theta);
            SetDlgItemText(hWnDlg, IDC_THETA, toStr);

            double Rho = call_rho(actif.S, actif.K, actif.r, actif.v, actif.T);
            sprintf_s(toStr, "%f", Rho);
            SetDlgItemText(hWnDlg, IDC_RHO, toStr);

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
        hWndDlgBox = NULL;
        break;

    default:
        return FALSE;
    }
}


// PROC PUT EU //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK DlgProcPutEu(HWND hWnDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
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

            HWND Calcul = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PB), hWnDlg, (DLGPROC)DlgPB);

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
            double Prime = 0.0;

            ShowWindow(Calcul, SW_SHOW);
            for (int i = 0; i < 100000; i++)
            {
                if ((i % 1000) == 0)
                {
                    SendDlgItemMessage(Calcul, IDE_PB, PBM_STEPIT, 0, 0);
                }
                vector<double> proc_prix = MBG(actif.S, actif.r, actif.v, actif.T);
                double last_value = proc_prix[proc_prix.size() - 1];
                Prime = Prime + std::max(actif.K - last_value, (double)0.0);
            }
            Prime = Prime / 100000;

            SendMessage(Calcul, WM_CLOSE, 0, 0);
            TCHAR toStr[50];
            sprintf_s(toStr, "%f", Prime);
            SetDlgItemText(hWnDlg, IDC_RESULT, toStr);

            double Price = put_price(actif.S, actif.K, actif.r, actif.v, actif.T);
            sprintf_s(toStr, "%f", Price);
            SetDlgItemText(hWnDlg, IDC_RESULT2, toStr);

            double Delta = put_delta(actif.S, actif.K, actif.r, actif.v, actif.T);
            sprintf_s(toStr, "%f", Delta);
            SetDlgItemText(hWnDlg, IDC_DELTA, toStr);

            double Gamma = put_gamma(actif.S, actif.K, actif.r, actif.v, actif.T);
            sprintf_s(toStr, "%f", Gamma);
            SetDlgItemText(hWnDlg, IDC_GAMMA, toStr);

            double Vega = put_vega(actif.S, actif.K, actif.r, actif.v, actif.T);
            sprintf_s(toStr, "%f", Vega);
            SetDlgItemText(hWnDlg, IDC_VEGA, toStr);

            double Theta = put_theta(actif.S, actif.K, actif.r, actif.v, actif.T);
            sprintf_s(toStr, "%f", Theta);
            SetDlgItemText(hWnDlg, IDC_THETA, toStr);

            double Rho = put_rho(actif.S, actif.K, actif.r, actif.v, actif.T);
            sprintf_s(toStr, "%f", Rho);
            SetDlgItemText(hWnDlg, IDC_RHO, toStr);

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
        hWndDlgBox = NULL;
        break;

    default:
        return FALSE; // (DefWindowProc(hWnDlg, Msg, wParam, lParam));
    }
}


// PROC CALL ASIATIQUE////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK DlgProcCallAs(HWND hWnDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
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
            HWND Calcul = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PB), hWnDlg, (DLGPROC)DlgPB);

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

            // BARRIERE-------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_BAR));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_BAR, buf, len + 1);
            DOUBLE Bar = atof(buf);
            GlobalFree((HANDLE)buf);

            //-------------------------------------------------------------------

            Data actif;
            actif.S = Spot;
            actif.K = Strike;
            actif.r = Taux;
            actif.T = Mat;
            actif.v = Vol;

            double Prime = 0.0;
            double Somme1 = 0.0;
            double Somme2 = 0.0;
            double w = LeftTerme(actif.S, actif.r, actif.v, actif.T, actif.K);
            double EspZ = Ez(actif.S, actif.r, actif.v, actif.T, actif.K, w);

            ShowWindow(Calcul, SW_SHOW);
            for (int i = 0; i < 100000; i++)
            {
                if ((i % 1000) == 0)
                {
                    SendDlgItemMessage(Calcul, IDE_PB, PBM_STEPIT, 0, 0);
                }

                vector<double> proc_prix = Euler(actif.S, actif.r, actif.v, actif.T);
                double SumTrajLog = somme_trajLog(proc_prix);
                double SumTraj = somme_traj(proc_prix);
                Somme1 = std::max(exp(SumTrajLog / (nb_pas)) - actif.K, 0.0);
                Somme2 = std::max(SumTraj/ (nb_pas) - actif.K, 0.0);
                Prime = Prime + Somme2 - Somme1;
            }
            Prime = Prime / 100000 + EspZ;


            SendMessage(Calcul, WM_CLOSE, 0, 0);
            TCHAR toStr[50];
            sprintf_s(toStr, "%f", Prime);
            SetDlgItemText(hWnDlg, IDC_RESULT, toStr);


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
        hWndDlgBox = NULL;
        break;

    default:
        return (DefWindowProc(hWnDlg, Msg, wParam, lParam));
    }
}

// PROC PUT ASIA ////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK DlgProcPutAs(HWND hWnDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
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
            HWND Calcul = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PB), hWnDlg, (DLGPROC)DlgPB);

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

            double Prime = 0.0;
            double Somme1 = 0.0;
            double Somme2 = 0.0;
            double w = LeftTerme(actif.S, actif.r, actif.v, actif.T, actif.K);
            double EspZ = Ez(actif.S, actif.r, actif.v, actif.T, actif.K, w);

            ShowWindow(Calcul, SW_SHOW);
            for (int i = 0; i < 100000; i++)
            {
                if ((i % 1000) == 0)
                {
                    SendDlgItemMessage(Calcul, IDE_PB, PBM_STEPIT, 0, 0);
                }

                vector<double> proc_prix = Euler(actif.S, actif.r, actif.v, actif.T);
                double SumTrajLog = somme_trajLog(proc_prix);
                double SumTraj = somme_traj(proc_prix);
                Somme1 = std::max(actif.K - exp(SumTrajLog / (nb_pas)), 0.0);
                Somme2 = std::max(actif.K - SumTraj / (nb_pas), 0.0);
                Prime = Prime + Somme2 - Somme1;
            }
            Prime = Prime / 100000 + EspZ;


            SendMessage(Calcul, WM_CLOSE, 0, 0);
            TCHAR toStr[50];
            sprintf_s(toStr, "%f", Prime);
            SetDlgItemText(hWnDlg, IDC_RESULT, toStr);


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
        hWndDlgBox = NULL;
        break;

    default:
        return (DefWindowProc(hWnDlg, Msg, wParam, lParam));
    }
}

// PROC CALL KO /////////////////////////////////////////////////////////////
LRESULT CALLBACK DlgProcCallKO(HWND hWnDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
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
            HWND Calcul = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PB), hWnDlg, (DLGPROC)DlgPB);

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

            // BARRIERE-------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_BAR));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_BAR, buf, len + 1);
            DOUBLE Bar = atof(buf);
            GlobalFree((HANDLE)buf);

            //-------------------------------------------------------------------

            Data actif;
            actif.S = Spot;
            actif.K = Strike;
            actif.r = Taux;
            actif.T = Mat;
            actif.v = Vol;
            actif.L = Bar;

            double Prime = 0.0;

            ShowWindow(Calcul, SW_SHOW);
            for (int i = 0; i < 100000; i++)
            {
                if ((i % 1000) == 0)
                {
                    SendDlgItemMessage(Calcul, IDE_PB, PBM_STEPIT, 0, 0);
                }

                vector<double> proc_prix = MBG(actif.S, actif.r, actif.v, actif.T);

                if (valeur_max(proc_prix) < actif.L) {
                    Prime = Prime + std::max(proc_prix[proc_prix.size() - 1] - actif.K,0.0);
                }
                else {
                    Prime = Prime + 0;
                }
            }
            Prime = Prime / 100000;


            SendMessage(Calcul, WM_CLOSE, 0, 0);
            TCHAR toStr[50];
            sprintf_s(toStr, "%f", Prime);
            SetDlgItemText(hWnDlg, IDC_RESULT, toStr);


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
        hWndDlgBox = NULL;
        break;

    default:
        return (DefWindowProc(hWnDlg, Msg, wParam, lParam));
    }
}
//PROC PUT KO ///////////////////////////////////////////////////////////////////////
LRESULT CALLBACK DlgProcPutKO(HWND hWnDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
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
            HWND Calcul = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PB), hWnDlg, (DLGPROC)DlgPB);

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

            // BARRIERE-------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_BAR));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_BAR, buf, len + 1);
            DOUBLE Bar = atof(buf);
            GlobalFree((HANDLE)buf);

            //-------------------------------------------------------------------

            Data actif;
            actif.S = Spot;
            actif.K = Strike;
            actif.r = Taux;
            actif.T = Mat;
            actif.v = Vol;
            actif.L = Bar;

            double Prime = 0.0;

            ShowWindow(Calcul, SW_SHOW);
            for (int i = 0; i < 100000; i++)
            {
                if ((i % 1000) == 0)
                {
                    SendDlgItemMessage(Calcul, IDE_PB, PBM_STEPIT, 0, 0);
                }

                vector<double> proc_prix = MBG(actif.S, actif.r, actif.v, actif.T);

                if (valeur_max(proc_prix) > actif.L) {
                    Prime = Prime + std::max(actif.K - proc_prix[proc_prix.size() - 1] , 0.0);
                }
                else {
                    Prime = Prime + 0;
                }
            }
            Prime = Prime / 100000;


            SendMessage(Calcul, WM_CLOSE, 0, 0);
            TCHAR toStr[50];
            sprintf_s(toStr, "%f", Prime);
            SetDlgItemText(hWnDlg, IDC_RESULT, toStr);

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
        hWndDlgBox = NULL;
        break;

    default:
        return (DefWindowProc(hWnDlg, Msg, wParam, lParam));
    }
}

// MAINSCREEN //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{

    switch (Msg)

    {
    case WM_CREATE:
    {
        RECT rc;
        GetClientRect(hWnd, &rc);

        //MENU-----------------------------------------------------------
        HMENU menu;
        menu = CreateMenu();
        AppendMenu(menu, MF_ENABLED, ID_DONNEE, "Get Yahoo Data");
        SetMenu(hWnd, menu);


        const char MainScreen[] = "Choissisez l'option à pricer.";
        hWndText = CreateWindowEx(0, TEXT("STATIC"), NULL,
            WS_CHILD | WS_VISIBLE, rc.left, rc.top, rc.right,
            40, hWnd, NULL, NULL, NULL);

        hWndText = CreateWindowEx(0, TEXT("STATIC"), TEXT(MainScreen),
            WS_CHILD | WS_VISIBLE, (rc.right - rc.left - 200)/2, rc.top +10, 200,
            20, hWnd, NULL, NULL, NULL);

        //Call Européen -----------------------------------------------
        hWndButtonCallEU = CreateWindowEx(
            0,
            "BUTTON",
            "Call Europeen",
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            rc.left + 30,
            rc.top + 60,
            200,
            40,
            hWnd,
            (HMENU)IDB_CALLEU,
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
            rc.right - 230,
            rc.top + 60,
            200,
            40,
            hWnd,
            (HMENU)IDB_PUTEU,
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
            rc.left + 30,
            rc.top + 130,
            200,
            40,
            hWnd,
            (HMENU)IDB_CALLAS,
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
            rc.right - 230,
            rc.top + 130,
            200,
            40,
            hWnd,
            (HMENU)IDB_PUTAS,
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
            rc.left + 30,
            rc.top + 200,
            200,
            40,
            hWnd,
            (HMENU)IDB_CALLKO,
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
            rc.right - 230,
            rc.top + 200,
            200,
            40,
            hWnd,
            (HMENU)IDB_PUTKO,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        if (!hWndButtonPutKO)
            MessageBox(NULL, "Main Window Button Failed.", "Error", MB_OK | MB_ICONERROR);


        // Call RKO ----------------------------------------------------------
        hWndButtonCallRKO = CreateWindowEx(
            0,
            "BUTTON",
            "Call RKO",
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_DISABLED,
            rc.left + 30,
            rc.top + 270,
            200,
            40,
            hWnd,
            (HMENU)IDB_CALLRKO,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        if (!hWndButtonCallRKO)
            MessageBox(NULL, "Main Window Button Failed.", "Error", MB_OK | MB_ICONERROR);

        // Put RKO ----------------------------------------------------------------
        hWndButtonPutRKO = CreateWindowEx(
            0,
            "BUTTON",
            "Put RKO",
            WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_DISABLED,
            rc.right - 230,
            rc.top + 270,
            200,
            40,
            hWnd,
            (HMENU)IDB_PUTRKO,
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
        case ID_DONNEE :
        {
            switch (HIWORD(wParam))
            {
            case BN_CLICKED:
            {
                if (!hWndDlgBox)
                {
                    hWndDlgBox = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_YAHOO), hWnd, (DLGPROC)DlgGetData);
                    ShowWindow(hWndDlgBox, SW_SHOW);

                    if (!hWndDlgBox)
                        MessageBox(NULL, "Dialog Box Failed.", "Error", MB_OK | MB_ICONERROR);
                }
            }
            break;
            }
        }
        case IDB_CALLEU:
        {
            switch (HIWORD(wParam))
            {
            case BN_CLICKED:
            {
                if (!hWndDlgBox)
                {
                    hWndDlgBox = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PRICE), hWnd, (DLGPROC)DlgProcCallEu);
                    ShowWindow(hWndDlgBox, SW_SHOW);

                    if (!hWndDlgBox)
                        MessageBox(NULL, "Dialog Box Failed.", "Error", MB_OK | MB_ICONERROR);
                }
            }
            break;
            }
        }
        break;

        case IDB_PUTEU:
        {
            switch (HIWORD(wParam))
            {
            case BN_CLICKED:
            {
                if (!hWndDlgBox)
                {
                    hWndDlgBox = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PRICE), hWnd, (DLGPROC)DlgProcPutEu);
                    ShowWindow(hWndDlgBox, SW_SHOW);

                    if (!hWndDlgBox)
                        MessageBox(NULL, "Dialog Box Failed.", "Error", MB_OK | MB_ICONERROR);
                }
            }
            break;
            }
        }
        break;
        case IDB_CALLAS:
        {
            switch (HIWORD(wParam))
            {
            case BN_CLICKED:
            {
                if (!hWndDlgBox)
                {
                    hWndDlgBox = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PRICE), hWnd, (DLGPROC)DlgProcCallAs);
                    ShowWindow(hWndDlgBox, SW_SHOW);

                    if (!hWndDlgBox)
                        MessageBox(NULL, "Dialog Box Failed.", "Error", MB_OK | MB_ICONERROR);
                }
            }
            break;

            }

        }
        break;

        case IDB_PUTAS:
        {
            switch (HIWORD(wParam))
            {
            case BN_CLICKED:
            {
                if (!hWndDlgBox)
                {
                    hWndDlgBox = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PRICE), hWnd, (DLGPROC)DlgProcPutAs);
                    ShowWindow(hWndDlgBox, SW_SHOW);

                    if (!hWndDlgBox)
                        MessageBox(NULL, "Dialog Box Failed.", "Error", MB_OK | MB_ICONERROR);
                }
            }
            break;

            }

        }
        break;

        case IDB_CALLKO:
        {
            switch (HIWORD(wParam))
            {
            case BN_CLICKED:
            {
                if (!hWndDlgBox)
                {
                    hWndDlgBox = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PRICEBAR), hWnd, (DLGPROC)DlgProcCallKO);
                    ShowWindow(hWndDlgBox, SW_SHOW);

                    if (!hWndDlgBox)
                        MessageBox(NULL, "Dialog Box Failed.", "Error", MB_OK | MB_ICONERROR);
                }
            }
            break;

            }

        }
        break;

        case IDB_PUTKO:
        {
            switch (HIWORD(wParam))
            {
            case BN_CLICKED:
            {
                if (!hWndDlgBox)
                {
                    hWndDlgBox = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PRICEBAR), hWnd, (DLGPROC)DlgProcPutKO);
                    ShowWindow(hWndDlgBox, SW_SHOW);

                    if (!hWndDlgBox)
                        MessageBox(NULL, "Dialog Box Failed.", "Error", MB_OK | MB_ICONERROR);
                }
            }
            break;

            }

        }
        break;
        case IDB_CALLRKO:
        {
            switch (HIWORD(wParam))
            {
            case BN_CLICKED:
            {
                if (!hWndDlgBox)
                {
                    hWndDlgBox = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PRICEBAR), hWnd, (DLGPROC)DlgProcCallKO);
                    ShowWindow(hWndDlgBox, SW_SHOW);

                    if (!hWndDlgBox)
                        MessageBox(NULL, "Dialog Box Failed.", "Error", MB_OK | MB_ICONERROR);
                }
            }
            break;

            }

        }
        break;

        case IDB_PUTRKO:
        {
            switch (HIWORD(wParam))
            {
            case BN_CLICKED:
            {
                if (!hWndDlgBox)
                {
                    hWndDlgBox = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PRICEBAR), hWnd, (DLGPROC)DlgProcPutKO);
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