//#include <windows.h>
//#include "resource.h" 
//
//
//const char g_szClassName[] = "myWindowClass";
//
//TCHAR Produits[6][20] =
//{
//	TEXT("Call Europeen"),TEXT("Put Europeen"),TEXT("Call Asiatique"),TEXT("Put Asiatique"),TEXT("Call KO"),TEXT("Put KO")
//};
//HWND window_var = NULL;
//
//BOOL CALLBACK DlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
//{
//	switch (Message)
//	{
//	case WM_INITDIALOG:
//
//		return TRUE;
//	case WM_COMMAND:
//		switch (LOWORD(wParam))
//		{
//		case IDC_CALLEU:
//			EndDialog(hwnd, IDC_CALLEU);
//			break;
//		case IDC_PUTEU:
//			EndDialog(hwnd, IDC_PUTEU);
//			break;
//		}
//		break;
//	default:
//		return FALSE;
//	}
//	return TRUE;
//}
//
//
//LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
//{
//	switch (Message)
//	{
//		case WM_CREATE:
//		{
//			HMENU hMenu, hSubMenu;
//			HICON hIcon, hIconSm;
//
//			hMenu = CreateMenu();
//
//			hSubMenu = CreatePopupMenu();
//			AppendMenu(hSubMenu, MF_STRING, ID_STARTPRICE, "Pricing");
//			AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&Pricing");
//
//
//			SetMenu(hwnd, hMenu);
//		}
//
//
//		//int i;
//		//for (i = 0; i < 6; i++) {
//		// int index = SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0, (LPARAM)Produits[i]);
//		// SendDlgItemMessage(hwnd, IDC_LIST, LB_SETITEMDATA, (WPARAM)index, (LPARAM)2);
//		//}
//		break;
//		case WM_COMMAND:
//			switch (LOWORD(wParam))
//			{
//				case ID_STARTPRICE:
//				{
//					int ret = DialogBox(GetModuleHandle(NULL),
//						MAKEINTRESOURCE(IDD_ACC), hwnd, (DLGPROC)DlgProc);
//					if (ret == IDC_CALLEU) {
//						MessageBox(hwnd, "Dialog exited with IDOK.", "Notice",
//							MB_OK | MB_ICONINFORMATION);
//					}
//					else if (ret == IDC_PUTEU) {
//						MessageBox(hwnd, "Dialog exited with IDCANCEL.", "Notice",
//							MB_OK | MB_ICONINFORMATION);
//					}
//					else if (ret == -1) {
//						MessageBox(hwnd, "Dialog failed!", "Error",
//							MB_OK | MB_ICONINFORMATION);
//					}
//					break;
//				}
//			case ID_EXIT:
//				PostMessage(hwnd, WM_CLOSE,0,0);
//				break;
//			}
//		break;
//	case WM_CLOSE:
//		DestroyWindow(hwnd);
//	break;
//	case WM_DESTROY:
//		PostQuitMessage(0);
//	break;
//	default:
//		return DefWindowProc(hwnd, Message, wParam, lParam);
//	}
//	return TRUE;
//}
//
//
////LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
////{
////	switch (Message)
////	{
////	case WM_CLOSE:
////		DestroyWindow(hwnd);
////		break;
////	case WM_DESTROY:
////		PostQuitMessage(0);
////		break;
////	default:
////		return DefWindowProc(hwnd, Message, wParam, lParam);
////	}
////	return 0;
////}
//
//
//int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
//	_In_ LPSTR lpCmdLine, _In_ int nCmdShow)
//{
//	WNDCLASSEX wc;
//	HWND hwnd;
//	MSG Msg;
//
//	wc.cbSize = sizeof(WNDCLASSEX);
//	wc.style = 0;
//	wc.lpfnWndProc = WndProc;
//	wc.cbClsExtra = 0;
//	wc.cbWndExtra = 0;
//	wc.hInstance = hInstance;
//	wc.hIcon = NULL;
//	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
//	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
//	wc.lpszMenuName = NULL;
//	wc.lpszClassName = g_szClassName;
//	wc.hIconSm = NULL;
//
//	if (!RegisterClassEx(&wc))
//	{
//		MessageBox(NULL, "Window Registration Failed!", "Error!",
//			MB_ICONEXCLAMATION | MB_OK);
//		return 0;
//	}
//
//	hwnd = CreateWindowEx(
//		WS_EX_CLIENTEDGE,
//		g_szClassName,
//		"Priceur",
//		WS_OVERLAPPEDWINDOW,
//		CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
//		NULL, NULL, hInstance, NULL);
//
//	if (hwnd == NULL)
//	{
//		MessageBox(NULL, "Window Creation Failed!", "Error!",
//			MB_ICONEXCLAMATION | MB_OK);
//		return 0;
//	}
//
//	ShowWindow(hwnd, nCmdShow);
//	UpdateWindow(hwnd);
//
//	while (GetMessage(&Msg, NULL, 0, 0) > 0)
//	{
//		TranslateMessage(&Msg);
//		DispatchMessage(&Msg);
//	}
//	return Msg.wParam;
//
//	// return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, (DLGPROC)DlgProc);
//}
