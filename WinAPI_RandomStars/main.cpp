#include <Windows.h>
#include <tchar.h>
#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "star.h"

#define tstring std::basic_string<TCHAR>

#ifdef UNICODE
#define to_tstring std::to_wstring
#else
#define to_tstring std::to_string
#endif

static constexpr UINT TIME_INTERVAL = 30;

static const TCHAR* M_CLASSNAME = _T("Main");
static const TCHAR* S_CLASSNAME = _T("Sub");

static const RECT sub_wnd_rect = { 0, 0, 500, 500 };

static HINSTANCE g_hInstance;

static std::vector<std::unique_ptr<std::vector<std::unique_ptr<Star>>>> g_star_vecs;
static std::vector<HWND> g_sub_hwnds_vec;

static INT64 count = 0;

LRESULT CALLBACK main_proc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK sub_proc(HWND, UINT, WPARAM, LPARAM);
void CALLBACK timer_proc(HWND, UINT, UINT_PTR, DWORD);

INT64 get_wnd_number(HWND hWnd);

INT APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, TCHAR* lpszArg, INT nCmdShow) {
    g_hInstance = hInstance;

    g_star_vecs.reserve(20);
    g_sub_hwnds_vec.reserve(20);

    MSG msg;
    
    WNDCLASS m_wnd_class;
    HWND m_hwnd;

    m_wnd_class.style         = CS_VREDRAW | CS_HREDRAW;
    m_wnd_class.lpfnWndProc   = main_proc;
    m_wnd_class.cbClsExtra    = 0;
    m_wnd_class.cbWndExtra    = 0;
    m_wnd_class.hInstance     = hInstance;
    m_wnd_class.hIcon         = LoadIcon(NULL, IDC_ICON);
    m_wnd_class.hCursor       = LoadCursor(NULL, IDC_ARROW);
    m_wnd_class.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    m_wnd_class.lpszMenuName  = nullptr;
    m_wnd_class.lpszClassName = M_CLASSNAME;

    RegisterClass(&m_wnd_class);

    WNDCLASS s_wnd_class;

    s_wnd_class.style         = NULL;
    s_wnd_class.lpfnWndProc   = sub_proc;
    s_wnd_class.cbClsExtra    = 0;
    s_wnd_class.cbWndExtra    = 0;
    s_wnd_class.hInstance     = hInstance;
    s_wnd_class.hIcon         = LoadIcon(NULL, IDC_ICON);
    s_wnd_class.hCursor       = LoadCursor(NULL, IDC_ARROW);
    s_wnd_class.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    s_wnd_class.lpszMenuName  = nullptr;
    s_wnd_class.lpszClassName = S_CLASSNAME;

    RegisterClass(&s_wnd_class);

    m_hwnd = CreateWindow(
        M_CLASSNAME, _T("Stars"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    ShowWindow(m_hwnd, nCmdShow);
    UpdateWindow(m_hwnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<INT>(msg.wParam);
}

LRESULT main_proc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
    static const RECT rect = { 10, 10, 90, 35 };
    static bool is_clicked = false;

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
                tstring wnd_text(_T("Stars "));
                wnd_text += to_tstring(++count);

                HWND sub_hwnd = CreateWindow(
                    S_CLASSNAME, wnd_text.c_str(),
                    WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                    NULL, NULL, g_hInstance, NULL);

                ShowWindow(sub_hwnd, SW_SHOWNORMAL);

                g_sub_hwnds_vec.push_back(sub_hwnd);
            }

            is_clicked = false;
            InvalidateRect(hWnd, nullptr, false);
        }

        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps = { };
        HDC hDC        = BeginPaint(hWnd, &ps);
        HBRUSH hBr     = nullptr;
        HBRUSH old_hBr = nullptr;

        if (is_clicked) {
            hBr = CreateSolidBrush(RGB(195, 195, 195));
            old_hBr = reinterpret_cast<HBRUSH>(SelectObject(hDC, hBr));
        }

        Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);
        SetBkMode(hDC, TRANSPARENT);
        TextOut(hDC, 15, 15, _T("New"), 3);

        if (old_hBr) {
            SelectObject(hDC, old_hBr);
            DeleteObject(hBr);
        }

        EndPaint(hWnd, &ps);

        break;
    }
    case WM_DESTROY:
    {
        std::for_each(
            g_sub_hwnds_vec.begin(), g_sub_hwnds_vec.end(),
            [](HWND hwnd) {
                if (IsWindow(hwnd))
                    DestroyWindow(hwnd);
            });

        PostQuitMessage(0);
    }
    }

    return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

LRESULT sub_proc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
    switch (iMsg) {
    case WM_CREATE:
    {
        g_star_vecs.emplace_back(std::make_unique<std::vector<std::unique_ptr<Star>>>())
            ->reserve(20);

        for (INT64 i = 0; i < 20; ++i) {
            g_star_vecs.back()->emplace_back(
                std::make_unique<Star>(
                    RGB(std::rand() % 256, std::rand() % 256, std::rand() % 256),
                    (DOUBLE)(std::rand() % ((INT64)sub_wnd_rect.right - 15)), (DOUBLE)(std::rand() % ((INT64)sub_wnd_rect.bottom - 40)),
                    (DOUBLE)((INT64)std::rand() % 200 - 100), (DOUBLE)((INT64)std::rand() % 200 - 100),
                    (DOUBLE)(std::rand()), (DOUBLE)(std::rand() % 360),
                    (DOUBLE)(10 + (INT64)std::rand() % 30)));
        }

        SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, sub_wnd_rect.right, sub_wnd_rect.bottom, SWP_NOMOVE);
        SetTimer(hWnd, 1, TIME_INTERVAL, timer_proc);

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
        HDC hDC        = BeginPaint(hWnd, &ps);
        HBRUSH hbr     = nullptr;
        HBRUSH old_hbr = nullptr;
        HPEN hpen      = CreatePen(PS_NULL, 1, RGB(0, 0, 0));
        HPEN old_hpen  = reinterpret_cast<HPEN>(SelectObject(hDC, hpen));

        SetPolyFillMode(hDC, WINDING);

        INT64 idx = get_wnd_number(hWnd) - 1;

        std::for_each(g_star_vecs[idx]->cbegin(), g_star_vecs[idx]->cend(),
            [&hDC, &hbr, &old_hbr](const std::unique_ptr<Star>& star) {
                hbr = CreateSolidBrush(star->get_color());
                old_hbr = reinterpret_cast<HBRUSH>(SelectObject(hDC, hbr));

                Polygon(hDC, star->get_points(), 10);

                SelectObject(hDC, old_hbr);
                DeleteObject(hbr);
            });

        SelectObject(hDC, old_hpen);
        DeleteObject(hpen);

        EndPaint(hWnd, &ps);

        break;
    }
    case WM_DESTROY:
    {
        KillTimer(hWnd, 1);

        INT64 idx = get_wnd_number(hWnd) - 1;

        g_star_vecs.erase(g_star_vecs.begin() + idx);
        g_sub_hwnds_vec.erase(g_sub_hwnds_vec.begin() + idx);

        for (auto First = g_sub_hwnds_vec.begin() + idx,
                  Last  = g_sub_hwnds_vec.end(); First != Last; ++First){
            INT64 wnd_number = get_wnd_number(*First);

            tstring wnd_text(_T("Stars "));
            wnd_text += to_tstring(wnd_number - 1);

            SetWindowText(*First, wnd_text.c_str());
        }

        --count;

        break;
    }
    }

    return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

void timer_proc(HWND hWnd, UINT nID, UINT_PTR nEl, DWORD time) {
    INT64 idx = get_wnd_number(hWnd) - 1;

    std::for_each(g_star_vecs[idx]->cbegin(), g_star_vecs[idx]->cend(),
        [](const std::unique_ptr<Star>& star) {
            star->move((DOUBLE)TIME_INTERVAL / 1000.0);

            if (sub_wnd_rect.left > star->get_center_x() || star->get_center_x() > (INT64)sub_wnd_rect.right - 15) {
                star->neg_velocity_x();
            }
            if (sub_wnd_rect.top > star->get_center_y() || star->get_center_y() > (INT64)sub_wnd_rect.bottom - 40) {
                star->neg_velocity_y();
            }
        });

    InvalidateRect(hWnd, nullptr, true);
}

inline INT64 get_wnd_number(HWND hWnd) {
    tstring title;
    title.resize(15);

    GetWindowText(hWnd, const_cast<TCHAR*>(title.c_str()), sizeof(title) / sizeof(TCHAR));

    return std::stoi(title.substr(5));
}