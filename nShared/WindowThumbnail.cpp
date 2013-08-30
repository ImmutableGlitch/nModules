/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WindowThumbnail.cpp
 *  The nModules Project
 *
 *  Draws a thumbnail of some window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "WindowThumbnail.hpp"
#include <dwmapi.h>
#include <algorithm>
#include "../Utilities/Math.h"

using std::min;
using std::max;


WindowThumbnail::WindowThumbnail(LPCTSTR prefix, Settings* parentSettings) : Drawable(prefix, parentSettings)
{
    this->thumbnailHandle = nullptr;

    LoadSettings();
}


WindowThumbnail::~WindowThumbnail()
{
}


void WindowThumbnail::Show(HWND hwnd, LPRECT position)
{
    HRESULT hr = S_OK;
    SIZE sourceSize = {0};

    this->hwnd = hwnd;

    if (this->thumbnailHandle != nullptr)
    {
        DwmUnregisterThumbnail(this->thumbnailHandle);
    }

    hr = DwmRegisterThumbnail(mWindow->GetWindowHandle(), hwnd, &this->thumbnailHandle);
    
    if (SUCCEEDED(hr))
    {
        hr = DwmQueryThumbnailSourceSize(this->thumbnailHandle, &sourceSize);
    }

    if (SUCCEEDED(hr))
    {
        int x, y, width, height, maxWidth, maxHeight;
        double scale;
        switch (this->position)
        {
        default:
        case TOP:
        case BOTTOM:
            {
                maxWidth = (this->sizeToButton ? position->right - position->left : this->maxWidth) - this->offset.left - this->offset.right;
                maxHeight = (this->maxHeight - this->offset.top - this->offset.bottom);
                scale = min(maxWidth/(double)sourceSize.cx, maxHeight/(double)sourceSize.cy);
                width = int(scale*sourceSize.cx) + this->offset.left + this->offset.right;
                height = int(scale*sourceSize.cy) + this->offset.top + this->offset.bottom;
                x = position->left + (position->right - position->left - width)/2;
                y = this->position == BOTTOM ? position->bottom + this->distanceFromButton : position->top - height - this->distanceFromButton;

                // Ensure that the entire preview is on the same monitor as the button.
                MonitorInfo::Monitor monitor = mWindow->GetMonitorInformation()->m_monitors[mWindow->GetMonitorInformation()->MonitorFromRECT(position)];
                x = CLAMP(monitor.rect.left, (long)x, monitor.rect.right - width);
            }
            break;

        case LEFT:
        case RIGHT:
            {
                maxWidth = this->maxWidth - this->offset.left - this->offset.right;
                maxHeight = (this->sizeToButton ? position->bottom - position->top : this->maxHeight) - this->offset.top - this->offset.bottom;
                scale = min(maxWidth/(double)sourceSize.cx, maxHeight/(double)sourceSize.cy);
                width = int(scale*sourceSize.cx) + this->offset.left + this->offset.right;
                height = int(scale*sourceSize.cy) + this->offset.top + this->offset.bottom;
                y = position->top;
                x = this->position == LEFT ? position->left - width - this->distanceFromButton : position->right + this->distanceFromButton;

                // Ensure that the entire preview is on the same monitor as the button.
                MonitorInfo::Monitor monitor = mWindow->GetMonitorInformation()->m_monitors[mWindow->GetMonitorInformation()->MonitorFromRECT(position)];
                y = CLAMP(monitor.rect.top, (long)y, monitor.rect.bottom - height);
            }
            break;
        }

        if (mAnimate)
        {
            switch (this->position)
            {
                case TOP:
                    mWindow->SetPosition((float)position->left, (float)position->top - 1, (float)position->right - (float)position->left, 1);
                    break;

                case BOTTOM:
                    mWindow->SetPosition((float)position->left, (float)position->bottom, (float)position->right - (float)position->left, 1);
                    break;

                case LEFT:
                    mWindow->SetPosition((float)position->left, (float)position->bottom, 1, (float)position->bottom - (float)position->top);
                    break;

                case RIGHT:
                    mWindow->SetPosition((float)position->right, (float)position->bottom, 1, (float)position->bottom - (float)position->top);
                    break;
            }
            mWindow->SetAnimation((float)x, (float)y, (float)width, (float)height, mAnimationDuration, Easing::Type::Sine);
        }
        else
        {
            mWindow->SetPosition((float)x, (float)y, (float)width, (float)height);
        }

        DWM_THUMBNAIL_PROPERTIES properties;
        properties.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY | DWM_TNP_OPACITY;
        properties.fSourceClientAreaOnly = FALSE;
        properties.fVisible = TRUE;
        properties.opacity = (BYTE)this->thumbnailOpacity;

        hr = DwmUpdateThumbnailProperties(this->thumbnailHandle, &properties);
    }

    if (SUCCEEDED(hr))
    {
        mWindow->Show();
    }
}


void WindowThumbnail::Hide()
{
    DwmUnregisterThumbnail(this->thumbnailHandle);
    this->thumbnailHandle = NULL;
    this->hwnd = NULL;
    mWindow->Hide();
}


void WindowThumbnail::LoadSettings(bool /*bIsRefresh*/)
{
    // Load Window settings
    WindowSettings windowSettings;
    WindowSettings windowDefaults;
    windowDefaults.width = 150;
    windowDefaults.height = 40;
    windowDefaults.alwaysOnTop = true;
    windowSettings.Load(mSettings, &windowDefaults);

    // Load state Settings
    StateRender<States>::InitData stateDefaults;
    stateDefaults[States::Base].defaults.brushSettings[State::BrushType::Background].color = Color::Create(0xAA009900);
    stateDefaults[States::Base].defaults.brushSettings[State::BrushType::Text].color = Color::Create(0xFF000000);
    stateDefaults[States::Base].defaults.textOffsetTop = 2;
    stateDefaults[States::Base].defaults.textOffsetBottom = 2;
    stateDefaults[States::Base].defaults.textOffsetRight = 2;
    stateDefaults[States::Base].defaults.textOffsetLeft = 2;
    stateDefaults[States::Base].defaults.brushSettings[State::BrushType::Outline].color = Color::Create(0xAAFFFFFF);
    stateDefaults[States::Base].defaults.outlineWidth = 2.0f;
    mStateRender.Load(stateDefaults, mSettings);

    mWindow->Initialize(windowSettings, &mStateRender);

    TCHAR szBuf[32];

    this->distanceFromButton = mSettings->GetInt(_T("DistanceFromButton"), 2);
    this->maxHeight = mSettings->GetInt(_T("MaxHeight"), 300);
    this->maxWidth = mSettings->GetInt(_T("MaxWidth"), 300);
    this->offset = mSettings->GetOffsetRect(_T("Offset"), 5, 5, 5, 5);
    mSettings->GetString(_T("Position"), szBuf, sizeof(szBuf), _T("Top")); 
    if (_tcsicmp(szBuf, _T("Left")) == 0)
    {
        this->position = LEFT;
    }
    else if (_tcsicmp(szBuf, _T("Right")) == 0)
    {
        this->position = RIGHT;
    }
    else if (_tcsicmp(szBuf, _T("Bottom")) == 0)
    {
        this->position = BOTTOM;
    }
    else
    {
        this->position = TOP;
    }
    mAnimate = mSettings->GetBool(_T("Animate"), false);
    mAnimationDuration = mSettings->GetInt(_T("AnimationDuration"), 200);
    this->sizeToButton = mSettings->GetBool(_T("SizeToButton"), true);
    this->thumbnailOpacity = mSettings->GetInt(_T("ThumbnailOpacity"), 255);
}


LRESULT WINAPI WindowThumbnail::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID)
{
    switch (message)
    {
    case WM_SIZE:
        {
            DWM_THUMBNAIL_PROPERTIES properties;
            properties.dwFlags = DWM_TNP_RECTDESTINATION;
            properties.rcDestination.left = this->offset.left;
            properties.rcDestination.right = LOWORD(lParam) - this->offset.right;
            properties.rcDestination.top = this->offset.top;
            properties.rcDestination.bottom = HIWORD(lParam) - this->offset.bottom;
            DwmUpdateThumbnailProperties(this->thumbnailHandle, &properties);
        }
        return 0;
    }
    return DefWindowProc(window, message, wParam, lParam);
}
