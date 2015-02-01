#pragma once

#include "ApiDefs.h"
#include "IPane.hpp"

#include "../nUtilities/d2d1.h"

/// <summary>
///
/// </summary>
class IPanePainter {
public:
  /// <summary>
  /// Called to initialize the painter for the given pane.
  /// </summary>
  /// <returns>A pointer which will be returned to the painter when this pane is painted.</returns>
  virtual LPVOID APICALL AddPane(const class IPane *pane) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="renderTarget"></param>
  virtual HRESULT APICALL CreateDeviceResources(ID2D1RenderTarget *renderTarget) = 0;


  /// <summary>
  /// When this is called, the ID2D1 render target is no longer valid, and the painter should
  /// discard all devices resources.
  /// </summary>
  virtual void APICALL DiscardDeviceResources() = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="renderTarget"></param>
  /// <returns>True if ...</returns>
  virtual bool APICALL DynamicColorChanged(ID2D1RenderTarget *renderTarget) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="renderTarget"></param>
  /// <param name="area"></param>
  virtual void APICALL Paint(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const class IPane *pane, LPVOID painterData) const = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="position"></param>
  virtual void APICALL PositionChanged(const class IPane *pane, LPVOID painterData,
    D2D1_RECT_F position) = 0;

  /// <summary>
  /// Called when the pane is no longer going to use this painter. The painter should free up any
  /// resources allocated for that particular pane.
  /// </summary>
  virtual void APICALL RemovePane(const class IPane *pane, LPVOID painterData) = 0;

  /// <summary>
  /// Called when the text of the given pane has changed.
  /// </summary>
  virtual void APICALL TextChanged(const class IPane *pane, LPVOID painterData, LPCWSTR text) = 0;
};
