#pragma once
#include <Arduino.h>
#include <BoardT5S3.h>

// HalDisplay now delegates to the FreeInk SDK's EInkDisplay (LgfxEpdDriver), so
// the app's display API is unchanged while the SDK drives the panel. The board's
// LgfxEpdConfig (pins + PCA9535/TPS65185 power hooks) is in FreeInkLgfxConfig.cpp.

class HalDisplay {
 public:
  // Constructor with pin configuration
  HalDisplay();

  // Destructor
  ~HalDisplay();

  // Refresh modes
  enum RefreshMode {
    FULL_REFRESH,  // Full refresh with complete waveform
    HALF_REFRESH,  // Half refresh (1720ms) - balanced quality and speed
    BALANCED_REFRESH,  // Reader-focused fast refresh using the cleaner mid waveform
    FAST_REFRESH   // Fast refresh using custom LUT
  };

  enum DisplayEffect {
    EFFECT_NONE,
    EFFECT_READER_TURN_FORWARD_STANDARD,
    EFFECT_READER_TURN_BACKWARD_STANDARD,
    EFFECT_READER_TURN_FORWARD_FAST,
    EFFECT_READER_TURN_BACKWARD_FAST
  };

  // Initialize the display hardware and driver
  void begin();

  // Display dimensions
  static constexpr uint16_t VISIBLE_WIDTH = BoardT5S3Pins::LogicalWidth;
  static constexpr uint16_t VISIBLE_HEIGHT = BoardT5S3Pins::LogicalHeight;
  // Keep the framebuffer in physical 960x540 scan orientation while exposing
  // portrait logical coordinates to the UI.
  static constexpr uint16_t DISPLAY_WIDTH = ((BoardT5S3Pins::DisplayWidth + 15) / 16) * 16;
  static constexpr uint16_t DISPLAY_HEIGHT = BoardT5S3Pins::DisplayHeight;
  static constexpr uint16_t DISPLAY_WIDTH_BYTES = DISPLAY_WIDTH / 8;
  static constexpr uint32_t BUFFER_SIZE = DISPLAY_WIDTH_BYTES * DISPLAY_HEIGHT;

  // Frame buffer operations
  void clearScreen(uint8_t color = 0xFF) const;
  void drawImage(const uint8_t* imageData, uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                 bool fromProgmem = false) const;
  void drawImageTransparent(const uint8_t* imageData, uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                            bool fromProgmem = false) const;

  void displayBuffer(RefreshMode mode = RefreshMode::FAST_REFRESH, bool turnOffScreen = false);
  void refreshDisplay(RefreshMode mode = RefreshMode::FAST_REFRESH, bool turnOffScreen = false);
  void requestNextRefresh(RefreshMode mode = RefreshMode::HALF_REFRESH);
  void requestNextDisplayEffect(DisplayEffect effect = DisplayEffect::EFFECT_NONE);
  void suppressInitialFullRefresh();

  // Power management
  void deepSleep();

  // Access to frame buffer
  uint8_t* getFrameBuffer() const;

  void copyGrayscaleBuffers(const uint8_t* lsbBuffer, const uint8_t* msbBuffer);
  void copyGrayscaleLsbBuffers(const uint8_t* lsbBuffer);
  void copyGrayscaleMsbBuffers(const uint8_t* msbBuffer);
  bool captureGrayscaleBaseBuffer(const uint8_t* bwBuffer);
  void cleanupGrayscaleBuffers(const uint8_t* bwBuffer);

  void displayGrayBuffer(RefreshMode mode = RefreshMode::HALF_REFRESH);

  // Runtime geometry passthrough
  uint16_t getDisplayWidth() const;
  uint16_t getDisplayHeight() const;
  uint16_t getVisibleWidth() const;
  uint16_t getVisibleHeight() const;
  uint16_t getDisplayWidthBytes() const;
  uint32_t getBufferSize() const;

 private:
  bool displayReady = false;
  bool forcedRefreshPending = false;
  RefreshMode forcedRefreshMode = RefreshMode::HALF_REFRESH;
};

extern HalDisplay display;
