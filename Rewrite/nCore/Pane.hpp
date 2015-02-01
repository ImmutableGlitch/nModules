#pragma once

#include "../nCoreApi/IPane.hpp"

#include "../nUtilities/d2d1.h"

#include <unordered_set>

class Pane : public IPane {
private:
  struct Settings {
    // The position of the pane, relative to the parent.
    NRECT position;
    wchar_t parent[MAX_PREFIX];
    bool alwaysOnTop;
    // The monitor to use for computing relative sizes. Top-level pane only.
    UINT monitor;
  } mSettings;

public:
  static HRESULT CreateWindowClasses(HINSTANCE);
  static void DestroyWindowClasses(HINSTANCE);

public:
  Pane &operator=(Pane&) = delete;
  Pane(const Pane&) = delete;

public:
  Pane(const PaneInitData *initData, Pane *parent);
  ~Pane();

  // IMessageHandler;
public:
  LRESULT APICALL HandleMessage(HWND, UINT, WPARAM, LPARAM, NPARAM) override;

  // IPane (PublicApi)
public:
  IPane* APICALL CreateChild(const PaneInitData*) override;
  void APICALL Destroy() override;
  LPCWSTR APICALL GetRenderingText() const override;
  HWND APICALL GetWindow() const override;
  void APICALL Hide() override;
  void APICALL Lock() override;
  void APICALL PaintChildren(ID2D1RenderTarget*, const D2D1_RECT_F *area) const override;
  void APICALL Position(LPCNRECT) override;
  void APICALL Repaint(LPCNRECT) override;
  void APICALL SetText(LPCWSTR) override;
  void APICALL Show() override;
  void APICALL Unlock() override;

private:
  void DiscardDeviceResources();
  HRESULT ReCreateDeviceResources();

  float EvaluateLength(const NLENGTH &length, bool horizontal) const;
  inline bool IsChildPane() const;
  void Paint(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area) const;
  void Position(const RECT&);

  // Invalidates the entire pane.
  void Repaint(bool update);

  // Invalidates the given area of the pane.
  void Repaint(const D2D1_RECT_F &area, bool update);

private:
  static LRESULT WINAPI ExternWindowProc(HWND, UINT, WPARAM, LPARAM);
  static LRESULT WINAPI InitWindowProc(HWND, UINT, WPARAM, LPARAM);

  // Top-level panes only.
private:
  HWND mWindow;
  D2D1_RECT_U mWindowPosition;

  // Child specific.
private:
  Pane *mParent;

  // All panes.
private:
  IMessageHandler *const mMessageHandler;
  IPanePainter *const mPainter;
  std::unordered_set<Pane*> mChildren;
  // The absolute position of this element within the window.
  D2D1_RECT_F mRenderingPosition;
  // The size of the window, in pixels.
  D2D1_SIZE_F mSize;
  D2D1_POINT_2F mDpi;
  // When this is not 0, we won't repaint the pane.
  int mUpdateLock;
  bool mVisible;
  LPWSTR mText;
  LPVOID mPainterData;

  // Set on all panes, but created & destroyed by the top-level.
private:
  ID2D1HwndRenderTarget *mRenderTarget;
};
