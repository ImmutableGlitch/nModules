/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TaskButton.cpp                                                  July, 2012
 *  The nModules Project
 *
 *  Implementation of the TaskButton class. Represents a taskbar button.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "../nCoreCom/Core.h"
#include "TaskButton.hpp"

extern HINSTANCE g_hInstance;
extern LPCSTR g_szTaskButtonHandler;


/// <summary>
/// Constructor
/// </summary>
TaskButton::TaskButton(HWND parent, HWND window, LPCSTR prefix, Settings* parentSettings) {
    // 
    m_hWnd = window;
    m_hWndParent = parent;

    //
    LoadSettings();

    // Create the drawable window
    this->settings = parentSettings->CreateChild("Button");
    m_pWindow = new DrawableWindow(parent, g_szTaskButtonHandler, g_hInstance, this->settings, new DrawableSettings());
    SetWindowLongPtr(m_pWindow->GetWindow(), 0, (LONG_PTR)this);

    this->iconSettings = this->settings->CreateChild("Icon");

    // Add states to the window
    this->stateHover = this->m_pWindow->AddState("Hover", new DrawableSettings(), 100);
    this->stateActive = this->m_pWindow->AddState("Active", new DrawableSettings(), 80);
    this->stateFlashing = this->m_pWindow->AddState("Flashing", new DrawableSettings(), 50);

    // Configure the mouse tracking struct
    ZeroMemory(&m_TrackMouseStruct, sizeof(TRACKMOUSEEVENT));
    m_TrackMouseStruct.cbSize = sizeof(TRACKMOUSEEVENT);
    m_TrackMouseStruct.hwndTrack = m_pWindow->GetWindow();
    m_TrackMouseStruct.dwFlags = TME_LEAVE;
    m_TrackMouseStruct.dwHoverTime = 200;

    // Initalize variables
    m_bMouseIsOver = false;
    m_bIsActive = false;
    m_hIcon = NULL;
    m_bIsFlashing = false;
}


/// <summary>
/// Destructor
/// </summary>
TaskButton::~TaskButton() {
    if (m_pWindow) delete m_pWindow;
    if (this->settings) delete this->settings;
}


/// <summary>
/// Sets the icon of this button.
/// </summary>
void TaskButton::SetIcon(HICON hIcon) {
    m_pWindow->PurgeOverlays();
    if (hIcon != NULL) {
        D2D1_RECT_F f = { (float)this->iconSettings->GetInt("X", 0), (float)this->iconSettings->GetInt("Y", 0),
            (float)this->iconSettings->GetInt("Width", 32) + (float)this->iconSettings->GetInt("X", 0),
            (float)this->iconSettings->GetInt("Height", 32)+ (float)this->iconSettings->GetInt("Y", 0) };
        m_pWindow->AddOverlay(f, hIcon);
    }
    m_pWindow->Repaint();
}


/// <summary>
/// Sets the text of this button.
/// </summary>
void TaskButton::SetText(LPCWSTR pszTitle) {
    StringCchCopyW(this->m_pWindow->GetSettings()->text, MAX_LINE_LENGTH, pszTitle);
    m_pWindow->Repaint();
}


/// <summary>
/// Loads RC settings for task buttons.
/// </summary>
void TaskButton::LoadSettings(bool /* bIsRefresh */) {
    using namespace nCore::InputParsing;
}


/// <summary>
/// Moves and resizes the taaskbutton.
/// </summary>
void TaskButton::Reposition(UINT x, UINT y, UINT width, UINT height) {
    DrawableSettings* drawableSettings = this->m_pWindow->GetSettings();
    drawableSettings->x = x;
    drawableSettings->y = y;
    drawableSettings->width = width;
    drawableSettings->height = height;

    m_pWindow->UpdatePosition();
}


/// <summary>
/// Activates this button.
/// </summary>
void TaskButton::Activate() {
    m_bIsActive = true;
    this->m_pWindow->ActivateState(this->stateActive);
    this->m_pWindow->ClearState(this->stateFlashing);
}


/// <summary>
/// Deactivates this button.
/// </summary>
void TaskButton::Deactivate() {
    m_bIsActive = false;
    this->m_pWindow->ClearState(this->stateActive);
}


/// <summary>
/// Tells this button to start flashing.
/// </summary>
void TaskButton::Flash() {
    m_bIsFlashing = true;
    this->m_pWindow->ActivateState(this->stateFlashing);
}


/// <summary>
/// Shows this button.
/// </summary>
void TaskButton::Show() {
    m_pWindow->Show();
}


/// <summary>
/// Shows the context menu for this task button.
/// </summary>
void TaskButton::Menu() {
    WINDOWPLACEMENT wp;

    hMenu = GetSystemMenu(m_hWnd, FALSE);

    ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
    wp.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(m_hWnd, &wp);

    // restore is enabled only if minimized or maximized (not normal)
    EnableMenuItem(hMenu, SC_RESTORE, MF_BYCOMMAND | (wp.showCmd != SW_SHOWNORMAL ? MF_ENABLED : MF_GRAYED));

    // move is enabled only if normal
    EnableMenuItem(hMenu, SC_MOVE, MF_BYCOMMAND | (wp.showCmd == SW_SHOWNORMAL ? MF_ENABLED : MF_GRAYED));

    // size is enabled only if normal
    EnableMenuItem(hMenu, SC_SIZE, MF_BYCOMMAND | (wp.showCmd == SW_SHOWNORMAL ? MF_ENABLED : MF_GRAYED));

    // minimize is enabled only if not minimized
    EnableMenuItem(hMenu, SC_MOVE, MF_BYCOMMAND | (wp.showCmd != SW_SHOWMINIMIZED ? MF_ENABLED : MF_GRAYED));

    // maximize is enabled only if not maximized
    EnableMenuItem(hMenu, SC_MOVE, MF_BYCOMMAND | (wp.showCmd != SW_SHOWMAXIMIZED ? MF_ENABLED : MF_GRAYED));

    // let application modify menu
    PostMessage(m_hWnd, WM_INITMENUPOPUP, (WPARAM)hMenu, MAKELPARAM(0, TRUE));
    PostMessage(m_hWnd, WM_INITMENU, (WPARAM)hMenu, 0);
    
    POINT pt;
    GetCursorPos(&pt);
    
    int command = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, m_pWindow->GetWindow(), NULL);
    if (command != 0) {
        PostMessage(m_hWnd, WM_SYSCOMMAND, (WPARAM)command, MAKELPARAM(pt.x, pt.y));
    }
}


/// <summary>
/// Places the rect of this button into the 2 POINTS structures pointed to by lpPoints.
/// </summary>
void TaskButton::GetMinRect(LPPOINTS lpPoints) {
    RECT r;
    GetWindowRect(m_pWindow->GetWindow(), &r);
    lpPoints[0].x = (short)r.left;
    lpPoints[0].y = (short)r.top;
    lpPoints[1].x = (short)r.right;
    lpPoints[1].y = (short)r.bottom;
}


/// <summary>
/// Handles window messages for this button.
/// </summary>
LRESULT WINAPI TaskButton::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_LBUTTONUP:
        if (GetForegroundWindow() == m_hWnd) {
            ShowWindow(m_hWnd, SW_MINIMIZE);
        }
        else if (IsIconic(m_hWnd)) {
            SetForegroundWindow(m_hWnd);
            ShowWindow(m_hWnd, SW_RESTORE);
        }
        else {
            SetForegroundWindow(m_hWnd);
        }
        return 0;

    case WM_RBUTTONUP:
        SetWindowPos(m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
        Menu();
        return 0;

    case WM_MOUSEMOVE:
        if (!m_bMouseIsOver) {
            m_bMouseIsOver = true;
            TrackMouseEvent(&m_TrackMouseStruct);
            this->m_pWindow->ActivateState(this->stateHover);
        }
        return 0;

    case WM_MOUSELEAVE:
        m_bMouseIsOver = false;
        this->m_pWindow->ClearState(this->stateHover);
        return 0;

    default:
        return m_pWindow->HandleMessage(uMsg, wParam, lParam);
    }
}
