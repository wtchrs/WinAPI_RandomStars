#include <Windows.h>
#include <tchar.h>
#include <cstdio>
#include <algorithm>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "star.h"

static const TCHAR* M_CLASSNAME = _T("Main");
static const TCHAR* S_CLASSNAME = _T("Sub");

static HINSTANCE g_hInstance;

static RECT subWndRect = { 0, 0, 500, 500 };

static std::vector<std::vector<std::unique_ptr<Star>>> wnds(20);

LRESULT CALLBACK mainProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK subProc(HWND, UINT, WPARAM, LPARAM);
void CALLBACK timerProc(HWND, UINT, UINT, DWORD);

INT64 getWndNumber(HWND hWnd);

INT APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, TCHAR* lpszArg, INT nCmdShow) {
    g_hInstance = hInstance;

    MSG msg;
    
    WNDCLASS mWndClass;
    HWND mHWnd;

    mWndClass.style         = CS_VREDRAW | CS_HREDRAW;
    mWndClass.lpfnWndProc   = mainProc;
    mWndClass.cbClsExtra    = 0;
    mWndClass.cbWndExtra    = 0;
    mWndClass.hInstance     = hInstance;
    mWndClass.hIcon         = LoadIcon(NULL, IDC_ICON);
    mWndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    mWndClass.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    mWndClass.lpszMenuName  = nullptr;
    mWndClass.lpszClassName = M_CLASSNAME;

    RegisterClass(&mWndClass);

    WNDCLASS sWndClass;

    sWndClass.style         = NULL;
    sWndClass.lpfnWndProc   = subProc;
    sWndClass.cbClsExtra    = 0;
    sWndClass.cbWndExtra    = 0;
    sWndClass.hInstance     = hInstance;
    sWndClass.hIcon         = LoadIcon(NULL, IDC_ICON);
    sWndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    sWndClass.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    sWndClass.lpszMenuName  = nullptr;
    sWndClass.lpszClassName = S_CLASSNAME;

    RegisterClass(&sWndClass);

    mHWnd = CreateWindow(
        M_CLASSNAME, _T("Stars"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    ShowWindow(mHWnd, nCmdShow);
    UpdateWindow(mHWnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<INT>(msg.wParam);
}

LRESULT mainProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
    static std::list<HWND> subHWnds = { };
    static RECT rect = { 10, 10, 90, 35 };
    static bool is_clicked = false;
    static INT64 count = 0;

    switch (iMsg) {
    case WM_CREATE:
    {
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 250, 80, SWP_NOMOVE);
        break;
    }
    case WM_GETMINMAXINFO:
    {
        auto info = reinterpret_cast<MINMAXINFO*>(lParam);
        info->ptMinTrackSize.x = 250;
        info->ptMinTrackSize.y = 85;
        info->ptMaxTrackSize.x = 250;
        info->ptMaxTrackSize.y = 85;
        break;
    }
    case WM_LBUTTONDOWN:
    {
        INT nx = LOWORD(lParam);
        INT ny = HIWORD(lParam);

        if ((rect.left < nx) && (nx < rect.right) && (rect.top < ny) && (ny < rect.bottom)) {
            is_clicked = true;
            InvalidateRect(hWnd, nullptr, false);
        }

        break;
    }
    case WM_MOUSEMOVE:
    {
        INT nx = LOWORD(lParam);
        INT ny = HIWORD(lParam);

        if (((nx < rect.left) || (rect.right < nx) || (ny < rect.top) || (rect.bottom < ny)) && is_clicked) {
            is_clicked = false;
            InvalidateRect(hWnd, nullptr, false);
        }

        break;
    }
    case WM_LBUTTONUP:
    {
        if (is_clicked) {
            if (count < 20) {
                std::basic_string<TCHAR> wndName;
                wndName.resize(15);
                _stprintf_s(const_cast<TCHAR*>(wndName.c_str()), wndName.size(), _T("Stars %I64d"), ++count);

                HWND subHWnd = CreateWindow(
                    S_CLASSNAME, wndName.c_str(),
                    WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                    NULL, NULL, g_hInstance, NULL);

                ShowWindow(subHWnd, SW_SHOWNORMAL);

                subHWnds.push_back(subHWnd);
            }

            is_clicked = false;
            InvalidateRect(hWnd, nullptr, false);
        }

        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps = { };
        HDC hDC = BeginPaint(hWnd, &ps);
        HBRUSH hBr = nullptr;
        HBRUSH hOldBr = nullptr;

        if (is_clicked) {
            hBr = CreateSolidBrush(RGB(195, 195, 195));
            hOldBr = reinterpret_cast<HBRUSH>(SelectObject(hDC, hBr));
        }

        Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);
        SetBkMode(hDC, TRANSPARENT);
        TextOut(hDC, 15, 15, _T("New"), 3);

        if (hOldBr) {
            SelectObject(hDC, hOldBr);
            DeleteObject(hBr);
        }

        EndPaint(hWnd, &ps);

        break;
    }
    case WM_DESTROY:
    {
        std::for_each(
            subHWnds.begin(), subHWnds.end(),
            [](HWND hWnd) {
                if (IsWindow(hWnd))
                    DestroyWindow(hWnd);
            });

        PostQuitMessage(0);
    }
    }

    return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

LRESULT subProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
    switch (iMsg) {
    case WM_CREATE:
    {
        INT64 idx = getWndNumber(hWnd);
        for (INT64 i = 0; i < 20; ++i) {
            wnds[idx - 1].emplace_back(
                new Star((DOUBLE)(std::rand() % subWndRect.right), (DOUBLE)(std::rand() % subWndRect.bottom),
                    (DOUBLE)((INT64)std::rand() % 200 - 100), (DOUBLE)((INT64)std::rand() % 200 - 100),
                    (DOUBLE)(std::rand()), (DOUBLE)(std::rand() % 360),
                    (DOUBLE)(10 + (INT64)std::rand() % 30)));
        }

        SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, subWndRect.right, subWndRect.bottom, SWP_NOMOVE);
        SetTimer(hWnd, 1, 10, reinterpret_cast<TIMERPROC>(timerProc));
        break;
    }
    case WM_GETMINMAXINFO:
    {
        auto info = reinterpret_cast<MINMAXINFO*>(lParam);
        info->ptMinTrackSize.x = 500;
        info->ptMinTrackSize.y = 500;
        info->ptMaxTrackSize.x = 500;
        info->ptMaxTrackSize.y = 500;
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps = { };
        HDC hDC = BeginPaint(hWnd, &ps);
        SetPolyFillMode(hDC, WINDING);

        INT64 idx = getWndNumber(hWnd);

        std::for_each(wnds[idx - 1].cbegin(), wnds[idx - 1].cend(),
            [&hDC](const std::unique_ptr<Star>& star) {
                Polygon(hDC, star->getPoints(), 10);
            });

        EndPaint(hWnd, &ps);

        break;
    }
    case WM_DESTROY:
    {
        KillTimer(hWnd, 1);
        wnds[getWndNumber(hWnd)].clear();
    }
    }

    return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

void timerProc(HWND hWnd, UINT nID, UINT nEl, DWORD time) {
    INT64 idx = getWndNumber(hWnd);

    std::for_each(wnds[idx-1].cbegin(), wnds[idx-1].cend(),
        [](const std::unique_ptr<Star>& star) {
            star->move(0.01);
            if (subWndRect.left > star->getCenterX() || star->getCenterX() > (INT64)subWndRect.right - 15) {
                star->negVelocityX();
            }
            if (subWndRect.top > star->getCenterY() || star->getCenterY() > (INT64)subWndRect.bottom - 40) {
                star->negVelocityY();
            }
        });

    InvalidateRect(hWnd, nullptr, false);
}

inline INT64 getWndNumber(HWND hWnd) {
    std::basic_string<TCHAR> title;
    title.resize(15);

    GetWindowText(hWnd, const_cast<TCHAR*>(title.c_str()), sizeof(title) / sizeof(TCHAR));

    std::string number;
    number.assign(title.begin() + 5, title.end());

    return std::stoi(number);
}