#define NOMINMAX

#include "resource.h"
#include "Data.h"
#include "Payoff.h"
#include "Option.h"
#include <string>
#include <iostream>
#include <windows.h>
#include <CommCtrl.h>


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

LRESULT CALLBACK DlgPriceEU(HWND hWndPrice, UINT Msg, WPARAM wParam, LPARAM lParam)
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
        hWndDlgBox = NULL;
        break;

    default:
        return FALSE;
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


// PROC CALL EU ////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK DlgProcCallEu(HWND hWnDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
    case WM_INITDIALOG:
        return TRUE;
        break;
    //case WM_PAINT:
    //{
    //    PAINTSTRUCT ps;
    //    HDC hdc = BeginPaint(hWnDlg, &ps);
    //    Rectangle(hdc, 10, 270, 200, 275);
    //    //FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 10));
    //    EndPaint(hWnDlg, &ps);
    //    return 0;
    //}
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
                Prime = Prime + std::max(last_value - actif.K, 0.0);
            }
            Prime = Prime / 100000;

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
                Prime = Prime + std::max(actif.K - last_value, 0.0);
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
            HWND Result = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_RESULT), GetParent(hWnDlg), (DLGPROC)DlgPriceEU);

            // SPOT----------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_SPOT));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_SPOT, buf, len + 1);
            SetDlgItemText(Result, IDC_SPOT2, buf);
            DOUBLE Spot = atof(buf);
            GlobalFree((HANDLE)buf);

            // STRIKE--------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_STRIKE));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_STRIKE, buf, len + 1);
            SetDlgItemText(Result, IDC_STRIKE2, buf);
            DOUBLE Strike = atof(buf);
            GlobalFree((HANDLE)buf);

            // MATURITY-------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_MAT));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_MAT, buf, len + 1);
            SetDlgItemText(Result, IDC_MAT2, buf);
            DOUBLE Mat = atof(buf);
            GlobalFree((HANDLE)buf);

            // TAUX------------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_TAUX));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_TAUX, buf, len + 1);
            SetDlgItemText(Result, IDC_TAUX2, buf);
            DOUBLE Taux = atof(buf);
            GlobalFree((HANDLE)buf);

            // Vol------------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_VOL));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_VOL, buf, len + 1);
            SetDlgItemText(Result, IDC_VOL2, buf);
            DOUBLE Vol = atof(buf);
            GlobalFree((HANDLE)buf);

            // BARRIERE-------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_BAR));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_BAR, buf, len + 1);
            SetDlgItemText(Result, IDC_BAR2, buf);
            DOUBLE Bar = atof(buf);
            GlobalFree((HANDLE)buf);

            //-------------------------------------------------------------------

            Data actif;
            actif.S = Spot;
            actif.K = Strike;
            actif.r = pow(1+Taux,1/252) -1;
            actif.T = Mat;
            actif.v = Vol/sqrt(252);
            PayOff* pay_off_call = new PayOffCall();
            Asiatique call(pay_off_call);
            double Prime = call.prime(actif);
            TCHAR toStr[50];
            sprintf_s(toStr, "%f", Prime);
            SetDlgItemText(Result, IDC_RESULT, toStr);


            ShowWindow(Result, SW_SHOW);
            SendMessage(hWnDlg, WM_CLOSE, 0, 0);

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
            HWND Result = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_RESULT), GetParent(hWnDlg), (DLGPROC)DlgPriceEU);

            // SPOT----------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_SPOT));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_SPOT, buf, len + 1);
            SetDlgItemText(Result, IDC_SPOT2, buf);
            DOUBLE Spot = atof(buf);
            GlobalFree((HANDLE)buf);

            // STRIKE--------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_STRIKE));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_STRIKE, buf, len + 1);
            SetDlgItemText(Result, IDC_STRIKE2, buf);
            DOUBLE Strike = atof(buf);
            GlobalFree((HANDLE)buf);

            // MATURITY-------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_MAT));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_MAT, buf, len + 1);
            SetDlgItemText(Result, IDC_MAT2, buf);
            DOUBLE Mat = atof(buf);
            GlobalFree((HANDLE)buf);

            // TAUX------------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_TAUX));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_TAUX, buf, len + 1);
            SetDlgItemText(Result, IDC_TAUX2, buf);
            DOUBLE Taux = atof(buf);
            GlobalFree((HANDLE)buf);

            // Vol------------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_VOL));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_VOL, buf, len + 1);
            SetDlgItemText(Result, IDC_VOL2, buf);
            DOUBLE Vol = atof(buf);
            GlobalFree((HANDLE)buf);

            // BARRIERE-------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_BAR));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_BAR, buf, len + 1);
            SetDlgItemText(Result, IDC_BAR2, buf);
            DOUBLE Bar = atof(buf);
            GlobalFree((HANDLE)buf);

            //-------------------------------------------------------------------

            Data actif;
            actif.S = Spot;
            actif.K = Strike;
            actif.r = Taux;
            actif.T = Mat;
            actif.v = Vol;
            PayOff* pay_off_put = new PayOffPut();
            Asiatique put(pay_off_put);
            double Prime = put.prime(actif);
            TCHAR toStr[50];
            sprintf_s(toStr, "%f", Prime);
            SetDlgItemText(Result, IDC_RESULT, toStr);


            ShowWindow(Result, SW_SHOW);
            SendMessage(hWnDlg, WM_CLOSE, 0, 0);

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
            HWND Result = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_RESULTBAR), GetParent(hWnDlg), (DLGPROC)DlgPriceBA);

            // SPOT----------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_SPOT));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_SPOT, buf, len + 1);
            SetDlgItemText(Result, IDC_SPOT2, buf);
            DOUBLE Spot = atof(buf);
            GlobalFree((HANDLE)buf);

            // STRIKE--------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_STRIKE));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_STRIKE, buf, len + 1);
            SetDlgItemText(Result, IDC_STRIKE2, buf);
            DOUBLE Strike = atof(buf);
            GlobalFree((HANDLE)buf);

            // MATURITY-------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_MAT));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_MAT, buf, len + 1);
            SetDlgItemText(Result, IDC_MAT2, buf);
            DOUBLE Mat = atof(buf);
            GlobalFree((HANDLE)buf);

            // TAUX------------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_TAUX));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_TAUX, buf, len + 1);
            SetDlgItemText(Result, IDC_TAUX2, buf);
            DOUBLE Taux = atof(buf);
            GlobalFree((HANDLE)buf);

            // Vol------------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_VOL));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_VOL, buf, len + 1);
            SetDlgItemText(Result, IDC_VOL2, buf);
            DOUBLE Vol = atof(buf);
            GlobalFree((HANDLE)buf);

            Data actif;
            actif.S = Spot;
            actif.K = Strike;
            actif.r = Taux;
            actif.T = Mat;
            actif.v = Vol;
            PayOff* pay_off_call = new PayOffCall();
            KnockOut call(pay_off_call);
            double Prime = call.prime(actif);
            TCHAR toStr[50];
            sprintf_s(toStr, "%f", Prime);
            SetDlgItemText(Result, IDC_RESULT, toStr);


            ShowWindow(Result, SW_SHOW);
            SendMessage(hWnDlg, WM_CLOSE, 0, 0);

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
            HWND Result = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_RESULTBAR), GetParent(hWnDlg), (DLGPROC)DlgPriceBA);

            // SPOT----------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_SPOT));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_SPOT, buf, len + 1);
            SetDlgItemText(Result, IDC_SPOT2, buf);
            DOUBLE Spot = atof(buf);
            GlobalFree((HANDLE)buf);

            // STRIKE--------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_STRIKE));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_STRIKE, buf, len + 1);
            SetDlgItemText(Result, IDC_STRIKE2, buf);
            DOUBLE Strike = atof(buf);
            GlobalFree((HANDLE)buf);

            // MATURITY-------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_MAT));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_MAT, buf, len + 1);
            SetDlgItemText(Result, IDC_MAT2, buf);
            DOUBLE Mat = atof(buf);
            GlobalFree((HANDLE)buf);

            // TAUX------------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_TAUX));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_TAUX, buf, len + 1);
            SetDlgItemText(Result, IDC_TAUX2, buf);
            DOUBLE Taux = atof(buf);
            GlobalFree((HANDLE)buf);

            // Vol------------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_VOL));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_VOL, buf, len + 1);
            SetDlgItemText(Result, IDC_VOL2, buf);
            DOUBLE Vol = atof(buf);
            GlobalFree((HANDLE)buf);

            // BARRIERE------------------------------------------------------------
            len = GetWindowTextLength(GetDlgItem(hWnDlg, IDC_BAR));
            buf = (char*)GlobalAlloc(GPTR, len + 1);
            GetDlgItemText(hWnDlg, IDC_BAR, buf, len + 1);
            SetDlgItemText(Result, IDC_BAR2, buf);
            DOUBLE Bar = atof(buf);
            GlobalFree((HANDLE)buf);

            Data actif;
            actif.S = Spot;
            actif.K = Strike;
            actif.r = Taux;
            actif.T = Mat;
            actif.v = Vol;
            actif.L = Bar;
            PayOff* pay_off_put = new PayOffPut();
            KnockOut put(pay_off_put);
            double Prime = put.prime(actif);
            TCHAR toStr[50];
            sprintf_s(toStr, "%f", Prime);
            SetDlgItemText(Result, IDC_RESULT, toStr);


            ShowWindow(Result, SW_SHOW);
            SendMessage(hWnDlg, WM_CLOSE, 0, 0);

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
        WNDCLASSEX  wc;

        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = 0;
        wc.lpfnWndProc = (WNDPROC)DlgProcCallEu;
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
            750,
            30,
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
            30,
            100,
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
            750,
            100,
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
            30,
            170,
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
            750,
            170,
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
            30,
            240,
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
            750,
            240,
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
    //InitCommonControls();

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