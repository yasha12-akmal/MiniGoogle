#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(linker,"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='' publicKeyToken='6595b64144ccf1df' language=''")

#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <curl/curl.h>
#include "json.hpp"
#include "SearchEngine.h"

using json = nlohmann::json;
using namespace Gdiplus;

// --- Global Variables ---
HWND hEdit, hResult;
std::string firstLink = "";
bool hasImage = false;

// --- Networking: Write Callbacks ---
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

size_t WriteFileData(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    return fwrite(ptr, size, nmemb, stream);
}

// --- Image Download Logic ---
void DownloadImage(std::string url) {
    CURL* curl = curl_easy_init();
    if (curl) {
        FILE* fp = fopen("result.jpg", "wb");
        if (!fp) return;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFileData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_perform(curl);
        fclose(fp);
        curl_easy_cleanup(curl);
        hasImage = true;
    }
}

// --- Core Search Function ---
void PerformSearch(HWND hwnd) {
    char q[256];
    GetWindowText(hEdit, q, 256);
    SetWindowText(hResult, "Scanning secure servers...");

    SearchEngine engine;
    SearchResult result = engine.Search(std::string(q));

    if (result.success) {
        firstLink = result.link;
        if (!result.imageUrl.empty()) {
            DownloadImage(result.imageUrl);
        }
        std::string resStr = "TOP DISCOVERY\r\n\r\n";
        resStr += result.title + "\r\n\r\n";
        resStr += result.snippet;
        SetWindowText(hResult, resStr.c_str());
    }
    else {
        SetWindowText(hResult, result.errorMessage.c_str());
    }

    InvalidateRect(hwnd, NULL, TRUE);
}

// --- Windows Procedure ---
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        HFONT hFont = CreateFont(20, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
        hEdit = CreateWindowEx(0, "EDIT", "Tesla", WS_VISIBLE | WS_CHILD |
            ES_AUTOHSCROLL | ES_CENTER, 250, 115, 400, 30, hwnd, NULL, NULL, NULL);
        SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        CreateWindow("BUTTON", "Search", WS_VISIBLE | WS_CHILD, 330, 165, 120, 35,
            hwnd, (HMENU)1, NULL, NULL);
        CreateWindow("BUTTON", "Visit Site", WS_VISIBLE | WS_CHILD, 460, 165, 100, 35,
            hwnd, (HMENU)2, NULL, NULL);
        hResult = CreateWindowEx(0, "EDIT", "", WS_VISIBLE | WS_CHILD | ES_MULTILINE |
            ES_READONLY | WS_VSCROLL, 75, 250, 360, 240, hwnd, NULL, NULL, NULL);
        SendMessage(hResult, WM_SETFONT, (WPARAM)hFont, TRUE);
        break;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == 1) PerformSearch(hwnd);
        if (LOWORD(wParam) == 2 && !firstLink.empty())
            ShellExecuteA(NULL, "open", firstLink.c_str(), NULL, NULL, SW_SHOWNORMAL);
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        Graphics g(BeginPaint(hwnd, &ps));
        g.SetSmoothingMode(SmoothingModeHighQuality);

        LinearGradientBrush bgBrush(Point(0, 0), Point(0, 600),
            Color(255, 240, 248, 255), Color(255, 255, 255, 255));
        g.FillRectangle(&bgBrush, 0, 0, 900, 600);

        FontFamily ff(L"Segoe UI");
        Gdiplus::Font logoFont(&ff, 65, FontStyleBold, UnitPixel);
        SolidBrush b(Color(255, 66, 133, 244)), r(Color(255, 234, 67, 53)),
            y(Color(255, 251, 188, 5)), gr(Color(255, 52, 168, 83));
        g.DrawString(L"G", 1, &logoFont, PointF(305, 15), &b);
        g.DrawString(L"o", 1, &logoFont, PointF(365, 15), &r);
        g.DrawString(L"o", 1, &logoFont, PointF(410, 15), &y);
        g.DrawString(L"g", 1, &logoFont, PointF(455, 15), &b);
        g.DrawString(L"l", 1, &logoFont, PointF(505, 15), &gr);
        g.DrawString(L"e", 1, &logoFont, PointF(525, 15), &r);

        if (!firstLink.empty()) {
            SolidBrush cardBg(Color(255, 255, 255, 255));
            Pen glowPen(Color(100, 66, 133, 244), 3);
            g.FillRectangle(&cardBg, 50, 220, 800, 310);
            g.DrawRectangle(&glowPen, 50, 220, 800, 310);
            if (hasImage) {
                Image image(L"result.jpg");
                if (image.GetLastStatus() == Ok) {
                    g.DrawImage(&image, 450, 240, 380, 270);
                }
            }
        }
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_DESTROY: PostQuitMessage(0); break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR, int) {
    GdiplusStartupInput gsi;
    ULONG_PTR token;
    GdiplusStartup(&token, &gsi, NULL);

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hI;
    wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(245, 250, 255));
    wc.lpszClassName = "GoogleApp";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);
    CreateWindow("GoogleApp", "Mini Google Pro v2.0", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 915, 600, 0, 0, hI, 0);

    MSG msg;
    while (GetMessage(&msg, 0, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    GdiplusShutdown(token);
    return 0;
}
