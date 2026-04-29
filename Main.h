#pragma once
#include <windows.h>
#include <gdiplus.h>
#include <string>

// --- Global Window Handles ---
extern HWND hEdit;
extern HWND hResult;
extern std::string firstLink;
extern bool hasImage;

// --- Callback Functions ---
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
size_t WriteFileData(void* ptr, size_t size, size_t nmemb, FILE* stream);

// --- Core Functions ---
void DownloadImage(std::string url);
void PerformSearch(HWND hwnd);

// --- Window Procedure ---
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// --- Entry Point ---
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
