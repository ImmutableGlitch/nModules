/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Balloon.hpp
 *  The nModules Project
 *
 *  A general balloon popup window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Drawable.hpp"
#include "DrawableWindow.hpp"
#include "Tooltip.hpp"

class Balloon : public Drawable {
public:
    explicit Balloon(LPCSTR prefix, Settings* parentSettings);
    virtual ~Balloon();

    void Show(LPCWSTR text, int x, int y, int duration);
    void Hide();

    void LoadSettings(bool = false);
    LRESULT WINAPI HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
};
