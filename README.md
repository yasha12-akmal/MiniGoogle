# Mini Google Pro v2.0

A lightweight Windows desktop search application that queries the **Google Custom Search API** and displays the top result — including a preview image — inside a native Win32 window styled with GDI+.

---

## Features

- Colorful Google-style logo rendered with GDI+
- Real-time search via Google Custom Search JSON API
- Thumbnail image download and display for the top result
- "Visit Site" button to open the result in the default browser
- Gradient background + glowing result card UI

---

## Project Structure

```
MiniGooglePro/
├── MiniGooglePro.slnx          ← Visual Studio solution
├── MiniGooglePro.vcxproj       ← Project file
├── MiniGooglePro.vcxproj.filters
├── MiniGooglePro.vcxproj.user
├── MiniGooglePro.rc            ← Win32 resource script (icon, version info, strings)
├── Main.cpp                    ← WinMain, WndProc, UI rendering
├── Main.h                      ← Global declarations
├── SearchEngine.cpp            ← Google Custom Search API logic
├── SearchEngine.h              ← SearchEngine class declaration
├── Resource.h                  ← Resource ID constants
├── json.hpp                    ← nlohmann/json single-header (add manually)
└── README.md
```

---

## Dependencies

| Library | Purpose | How to get |
|---------|---------|------------|
| **libcurl** | HTTP requests | https://curl.se/windows/ |
| **nlohmann/json** | JSON parsing | https://github.com/nlohmann/json (single header) |
| **GDI+** | 2-D graphics | Ships with Windows SDK |

Place `libcurl.lib` + `curl.dll` in `lib\x64\` and `curl\curl.h` in `include\curl\`.  
Download `json.hpp` and drop it in the project root.

---

## Setup

1. **Clone / download** this repository.
2. **Add dependencies** (see table above).
3. **Open** `MiniGooglePro.slnx` in Visual Studio 2022.
4. **Replace API credentials** in `SearchEngine.cpp`:
   ```cpp
   m_apiKey = "YOUR_GOOGLE_API_KEY";
   m_cx     = "YOUR_CUSTOM_SEARCH_ENGINE_ID";
   ```
5. **Build** → `Release | x64`.
6. Copy `curl.dll` next to the `.exe` before running.

---

## How to Get API Credentials

1. Go to [Google Cloud Console](https://console.cloud.google.com/).
2. Enable the **Custom Search API**.
3. Create an API key under **Credentials**.
4. Go to [Programmable Search Engine](https://programmablesearchengine.google.com/) and create an engine to get your **CX ID**.

---

## License

This project is for educational purposes.
