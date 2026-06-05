#include "HalDisplay.h"

#include <BoardConfig.h>
#include <EInkDisplay.h>  // FreeInk SDK facade (LgfxEpdDriver for LilyGo)

HalDisplay display;

namespace {
EInkDisplay* eink = nullptr;

EInkDisplay::RefreshMode toEink(HalDisplay::RefreshMode m) {
  switch (m) {
    case HalDisplay::FULL_REFRESH:
      return EInkDisplay::FULL_REFRESH;
    case HalDisplay::HALF_REFRESH:
    case HalDisplay::BALANCED_REFRESH:
      return EInkDisplay::HALF_REFRESH;
    case HalDisplay::FAST_REFRESH:
    default:
      return EInkDisplay::FAST_REFRESH;
  }
}
}  // namespace

HalDisplay::HalDisplay() = default;
HalDisplay::~HalDisplay() = default;

void HalDisplay::begin() {
  const auto& d = BoardConfig::ACTIVE.display;
  static EInkDisplay instance(d.sclk, d.mosi, d.cs, d.dc, d.rst, d.busy);
  eink = &instance;
  eink->begin();
  displayReady = true;
}

void HalDisplay::clearScreen(uint8_t color) const {
  if (eink) eink->clearScreen(color);
}

void HalDisplay::drawImage(const uint8_t* imageData, uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                           bool fromProgmem) const {
  if (eink) eink->drawImage(imageData, x, y, w, h, fromProgmem);
}

void HalDisplay::drawImageTransparent(const uint8_t* imageData, uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                                      bool fromProgmem) const {
  if (eink) eink->drawImageTransparent(imageData, x, y, w, h, fromProgmem);
}

void HalDisplay::displayBuffer(RefreshMode mode, bool turnOffScreen) {
  if (forcedRefreshPending) {
    mode = forcedRefreshMode;
    forcedRefreshPending = false;
  }
  if (eink) eink->displayBuffer(toEink(mode), turnOffScreen);
}

void HalDisplay::refreshDisplay(RefreshMode mode, bool turnOffScreen) {
  if (eink) eink->refreshDisplay(toEink(mode), turnOffScreen);
}

void HalDisplay::requestNextRefresh(RefreshMode mode) {
  forcedRefreshMode = mode;
  forcedRefreshPending = true;
}

void HalDisplay::requestNextDisplayEffect(DisplayEffect /*effect*/) {
  // Page-turn slice effects are not modeled by the SDK; refresh runs normally.
}

void HalDisplay::suppressInitialFullRefresh() {
  if (eink) eink->skipInitialResync();
}

void HalDisplay::deepSleep() {
  if (eink) eink->deepSleep();
}

uint8_t* HalDisplay::getFrameBuffer() const {
  return eink ? eink->getFrameBuffer() : nullptr;
}

void HalDisplay::copyGrayscaleBuffers(const uint8_t* lsbBuffer, const uint8_t* msbBuffer) {
  if (eink) eink->copyGrayscaleBuffers(lsbBuffer, msbBuffer);
}

void HalDisplay::copyGrayscaleLsbBuffers(const uint8_t* lsbBuffer) {
  if (eink) eink->copyGrayscaleLsbBuffers(lsbBuffer);
}

void HalDisplay::copyGrayscaleMsbBuffers(const uint8_t* msbBuffer) {
  if (eink) eink->copyGrayscaleMsbBuffers(msbBuffer);
}

bool HalDisplay::captureGrayscaleBaseBuffer(const uint8_t* /*bwBuffer*/) {
  // The SDK uses the live framebuffer as the grayscale base at displayGray time,
  // so no separate capture is needed.
  return true;
}

void HalDisplay::cleanupGrayscaleBuffers(const uint8_t* bwBuffer) {
  if (eink) eink->cleanupGrayscaleBuffers(bwBuffer);
}

void HalDisplay::displayGrayBuffer(RefreshMode /*mode*/) {
  if (eink) eink->displayGrayBuffer();  // SDK grayscale runs the quality waveform
}

uint16_t HalDisplay::getDisplayWidth() const { return eink ? eink->getDisplayWidth() : DISPLAY_WIDTH; }
uint16_t HalDisplay::getDisplayHeight() const { return eink ? eink->getDisplayHeight() : DISPLAY_HEIGHT; }
uint16_t HalDisplay::getVisibleWidth() const { return VISIBLE_WIDTH; }
uint16_t HalDisplay::getVisibleHeight() const { return VISIBLE_HEIGHT; }
uint16_t HalDisplay::getDisplayWidthBytes() const { return eink ? eink->getDisplayWidthBytes() : DISPLAY_WIDTH_BYTES; }
uint32_t HalDisplay::getBufferSize() const {
  return eink ? static_cast<uint32_t>(eink->getDisplayWidthBytes()) * eink->getDisplayHeight() : BUFFER_SIZE;
}
