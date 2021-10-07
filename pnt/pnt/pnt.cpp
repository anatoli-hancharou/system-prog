// pnt.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "pnt.h"

#define MAX_LOADSTRING 100
#define IDM_CLEARBUTTON 10001
#define IDM_RUBBERBUTTON 5
#define ID_LIST 101
#define ID_COLORS 6
#define ID_WIDTH 7
#define ID_BRUSHCOLOR 8

wchar_t listBoxStr[15];
HWND hListBox;
HWND ColorBox;
HWND WidthBox;
HWND BrushColorBox;


// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL Line(HDC hdc, int x1, int y1, int x2, int y2);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PNT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PNT));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PNT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PNT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


HPEN hPen = NULL;
HPEN pen = NULL;
HBRUSH hb = NULL;

BOOL fDraw = FALSE;
POINT ptPrevious = { 0 };
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HDC hdc;
    static int x1 = -1, y1 = -1, x2 = -1, y2 = -1;
    static bool aa = true;
    static int uSelectedItem = 0;
    static int colorIndex = 0;
    static int brushIndex = 4;
    static int width = 1;
    static int colors[5][3] = {{0, 0 ,0},
                               {255, 0, 0},
                               {0, 255, 0},
                               {0, 0, 255},
                               {255,255,255}};

    switch (message)
    {
    case WM_CREATE:
        {
            hPen = CreatePen(PS_SOLID, 25, RGB(255,255,255));

            hListBox = CreateWindow(L"listbox", NULL,
            WS_CHILD | WS_VISIBLE | LBS_STANDARD |
            LBS_WANTKEYBOARDINPUT,
            201, 1, 100, 40,
            hWnd, (HMENU)ID_LIST, hInst, NULL);
            SendMessage(hListBox, LB_ADDSTRING, 0,
                (LPARAM)L"Line");
            SendMessage(hListBox, LB_ADDSTRING, 0,
                (LPARAM)L"Circle");
            SendMessage(hListBox, LB_ADDSTRING, 0,
                (LPARAM)L"Rectangle");

            ColorBox = CreateWindow(L"listbox", NULL,
                WS_CHILD | WS_VISIBLE | LBS_NOTIFY |
                LBS_WANTKEYBOARDINPUT | WS_VSCROLL | WS_BORDER,
                301, 1, 100, 40,
                hWnd, (HMENU)ID_COLORS, hInst, NULL);
            SendMessage(ColorBox, LB_ADDSTRING, 0,
                (LPARAM)L"Black");
            SendMessage(ColorBox, LB_ADDSTRING, 0,
                (LPARAM)L"Red");
            SendMessage(ColorBox, LB_ADDSTRING, 0,
                (LPARAM)L"Green");
            SendMessage(ColorBox, LB_ADDSTRING, 0,
                (LPARAM)L"Blue");

            WidthBox = CreateWindow(L"listbox", NULL,
                WS_CHILD | WS_VISIBLE | LBS_NOTIFY |
                LBS_WANTKEYBOARDINPUT | WS_VSCROLL | WS_BORDER,
                401, 1, 100, 40,
                hWnd, (HMENU)ID_WIDTH, hInst, NULL);
            SendMessage(WidthBox, LB_ADDSTRING, 0,
                (LPARAM)L"1px");
            SendMessage(WidthBox, LB_ADDSTRING, 0,
                (LPARAM)L"5px");
            SendMessage(WidthBox, LB_ADDSTRING, 0,
                (LPARAM)L"10px");
            SendMessage(WidthBox, LB_ADDSTRING, 0,
                (LPARAM)L"15px");

            BrushColorBox = CreateWindow(L"listbox", NULL,
                WS_CHILD | WS_VISIBLE | LBS_NOTIFY |
                LBS_WANTKEYBOARDINPUT | WS_VSCROLL | WS_BORDER,
                501, 1, 100, 40,
                hWnd, (HMENU)ID_BRUSHCOLOR, hInst, NULL);
            SendMessage(BrushColorBox, LB_ADDSTRING, 0,
                (LPARAM)L"Black brush");
            SendMessage(BrushColorBox, LB_ADDSTRING, 0,
                (LPARAM)L"Red brush");
            SendMessage(BrushColorBox, LB_ADDSTRING, 0,
                (LPARAM)L"Green brush");
            SendMessage(BrushColorBox, LB_ADDSTRING, 0,
                (LPARAM)L"Blue brush");
            SendMessage(BrushColorBox, LB_ADDSTRING, 0,
                (LPARAM)L"None");

            SendMessage(WidthBox, LB_SETCURSEL, 0, 0);
            SendMessage(BrushColorBox, LB_SETCURSEL, 4, 0);
            SendMessage(ColorBox, LB_SETCURSEL, 0, 0);
            SendMessage(hListBox, LB_SETCURSEL, 0, 0);

            HWND hButtonClearWnd = CreateWindow(_T("BUTTON"), _T("Clear"),
            WS_CHILD | WS_VISIBLE, 1, 1, 100, 30, hWnd,
            (HMENU)IDM_CLEARBUTTON, NULL, NULL);

            HWND hButtonRubberWnd = CreateWindow(_T("BUTTON"), _T("Rubber"),
                WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 101, 1, 100, 30, hWnd,
                (HMENU)IDM_RUBBERBUTTON, NULL, NULL);

            break;
        }
    case WM_LBUTTONDOWN:
        {
            if (uSelectedItem == -1)
            {
                fDraw = TRUE;
                ptPrevious.x = LOWORD(lParam);
                ptPrevious.y = HIWORD(lParam);               
            }

            x1 = LOWORD(lParam);
            y1 = HIWORD(lParam);
            break;
        }
    case WM_MOUSEMOVE:
        {
            if (fDraw && uSelectedItem == -1)
            {
                hdc = GetDC(hWnd);
                SelectObject(hdc, hPen);
                MoveToEx(hdc, ptPrevious.x, ptPrevious.y, NULL);
                LineTo
                (
                    hdc,
                    ptPrevious.x = LOWORD(lParam),
                    ptPrevious.y = HIWORD(lParam)
                );
                ReleaseDC(hWnd, hdc);
            }
            break;
        }
    case WM_LBUTTONUP:
        {
            if (fDraw && uSelectedItem == -1)
            {
                hdc = GetDC(hWnd);
                MoveToEx(hdc, ptPrevious.x, ptPrevious.y, NULL);
                LineTo(hdc, LOWORD(lParam), HIWORD(lParam));
                ReleaseDC(hWnd, hdc);
                fDraw = FALSE;
            }
            x2 = LOWORD(lParam);
            y2 = HIWORD(lParam);
            hdc = GetDC(hWnd);

            auto colorPen = colors[colorIndex];
            pen = CreatePen(PS_SOLID, width, RGB(colorPen[0], colorPen[1], colorPen[2]));
            auto colorBrush = colors[brushIndex];
            hb = CreateSolidBrush(RGB(colorBrush[0], colorBrush[1], colorBrush[2]));

            SelectObject(hdc, pen);
            SelectObject(hdc, hb);

            if (uSelectedItem == 0) {
                Ellipse(hdc, x1, y1, x2, y2);
            }
            else if (uSelectedItem == 1) {
                Line(hdc, x1, y1, x2, y2);
            }
            else if (uSelectedItem == 2) {
                Rectangle(hdc, x1, y1, x2, y2);
            }
         //   ReleaseDC(hWnd, hdc);
            break;
        }
    case WM_COMMAND:
        {
            BOOL rubberChecked = IsDlgButtonChecked(hWnd, IDM_RUBBERBUTTON);
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case ID_LIST:
                CheckDlgButton(hWnd, IDM_RUBBERBUTTON, BST_UNCHECKED);
                // Если пользователь сделал двойной щелчок
                if (HIWORD(wParam) == LBN_DBLCLK || HIWORD(wParam) == LBN_SELCHANGE)
                {
                    uSelectedItem = (int)SendMessage(hListBox,
                        LB_GETCURSEL, 0, 0);
                }
                break;
            case ID_COLORS:
                if (HIWORD(wParam) == LBN_DBLCLK || HIWORD(wParam) == LBN_SELCHANGE)
                {
                    colorIndex = (int)SendMessage(ColorBox,
                        LB_GETCURSEL, 0, 0);
                }
                break;
            case ID_BRUSHCOLOR:
                if (HIWORD(wParam) == LBN_DBLCLK || HIWORD(wParam) == LBN_SELCHANGE)
                {
                    brushIndex = (int)SendMessage(BrushColorBox,
                        LB_GETCURSEL, 0, 0);
                }
                break;
            case ID_WIDTH:
                if (HIWORD(wParam) == LBN_DBLCLK || HIWORD(wParam) == LBN_SELCHANGE)
                {
                    int index = (int)SendMessage(WidthBox,
                        LB_GETCURSEL, 0, 0);
                    switch (index)
                    {
                    case 0:
                        width = 1;
                        break;
                    case 1:
                        width = 5;
                        break;
                    case 2:
                        width = 10;
                        break;
                    case 3:
                        width = 15;
                        break;
                    default:
                        width = 1;
                        break;
                    }
                }
                break;
            case IDM_CLEARBUTTON:
                RECT r;
                GetClientRect(hWnd, &r);
                InvalidateRect(hWnd, &r, TRUE);
                UpdateWindow(hWnd);
                break;
            case IDM_RUBBERBUTTON:
                uSelectedItem = -1;
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

BOOL Line(HDC hdc, int x1, int y1, int x2, int y2)
{
    MoveToEx(hdc, x1, y1, NULL); //сделать текущими координаты x1, y1
    return LineTo(hdc, x2, y2);
}
