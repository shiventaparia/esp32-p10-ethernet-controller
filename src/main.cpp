/*
 * ESP32 P10 controller — redacted public source showcase
 *
 * This file is intentionally non-buildable and is not production firmware.
 * It preserves selected display, persistence, concurrency, and lifecycle
 * patterns for portfolio review while omitting deployment-specific networking,
 * operational command handling, and LAN OTA implementation.
 *
 * The original production project remains in its separate workspace. Do not
 * add .pio output, firmware images, local IDE files, credentials, addresses,
 * MAC values, ports, customer data, or production logs to this showcase.
 */

// Prevent accidental compilation or upload of this source showcase.
#error "Redacted portfolio source only: use the independent host-side example for runnable code."

#include <DMD32.h>
#include "fonts/Arial_black_16.h"
#include "fonts/SystemFont5x7.h"
#include <Preferences.h>

// -----------------------------------------------------------------------------
// Display configuration retained as a public, generic example.
// -----------------------------------------------------------------------------
constexpr int kDefaultDisplaysAcross = 3;
constexpr int kDefaultDisplaysDown = 1;
constexpr int kMaxRows = 16;
constexpr uint16_t kScrollIntervalMs = 40;

int displaysAcross = kDefaultDisplaysAcross;
int displaysDown = kDefaultDisplaysDown;
DMD* dmd = nullptr;
Preferences prefs;

// Legacy single-message state. The production command schema is intentionally
// omitted; these fields only show the display-state boundary.
String displayMessage = "READY";
int messageXPos = 0;
int messageYPos = 0;
bool scrolling = false;
int fontSize = 0;

unsigned long lastScroll = 0;
unsigned long lastRowScroll = 0;
constexpr int kInitialScrollX = 0;
int scrollX = kInitialScrollX;
int scrollY = 0;

struct RowSlot {
  bool active = false;
  String text;
  int x = 0;
  int size = 0;
  bool scrolling = false;
  int scrollX = 0;
};

RowSlot rows[kMaxRows];

// One mutex protects shared display state while the network-facing task and
// display task are scheduled independently in the original architecture.
SemaphoreHandle_t dataMutex = nullptr;
volatile bool displayUpdateNeeded = true;
hw_timer_t* timer = nullptr;

// -----------------------------------------------------------------------------
// Deliberately omitted deployment boundary
// -----------------------------------------------------------------------------
// The production source contains Ethernet initialization, TCP server objects,
// network persistence, configurable service settings, and LAN OTA handling.
// Those blocks are intentionally not reproduced here. No production addresses,
// MAC values, ports, upload routes, or network-management command schema belong
// in this public source showcase.
//
// The public design is documented in docs/networking.md and docs/ota-design.md.
// The decision rationale is documented in docs/design-decisions.md.

// -----------------------------------------------------------------------------
// Preferences/NVS: display and row state only
// -----------------------------------------------------------------------------
void saveDisplayData() {
  if (timer != nullptr) {
    timerAlarmDisable(timer);
  }

  prefs.begin("display", false);
  prefs.putString("msg", displayMessage);
  prefs.putInt("x", messageXPos);
  prefs.putInt("y", messageYPos);
  prefs.putBool("scroll", scrolling);
  prefs.putInt("size", fontSize);
  prefs.putInt("across", displaysAcross);
  prefs.putInt("down", displaysDown);
  prefs.end();

  if (timer != nullptr) {
    timerAlarmEnable(timer);
  }
}

void loadDisplayData() {
  prefs.begin("display", true);
  displayMessage = prefs.getString("msg", "READY");
  messageXPos = prefs.getInt("x", 0);
  messageYPos = prefs.getInt("y", 0);
  scrolling = prefs.getBool("scroll", false);
  fontSize = prefs.getInt("size", 0);
  displaysAcross = prefs.getInt("across", kDefaultDisplaysAcross);
  displaysDown = prefs.getInt("down", kDefaultDisplaysDown);
  prefs.end();

  if (displaysAcross < 1) {
    displaysAcross = kDefaultDisplaysAcross;
  }
  if (displaysDown < 1 || displaysDown > kMaxRows) {
    displaysDown = kDefaultDisplaysDown;
  }
}

void saveRows() {
  if (timer != nullptr) {
    timerAlarmDisable(timer);
  }

  prefs.begin("rows", false);
  for (int index = 0; index < kMaxRows; ++index) {
    String base = "r" + String(index);
    prefs.putBool((base + "a").c_str(), rows[index].active);
    if (rows[index].active) {
      prefs.putString((base + "t").c_str(), rows[index].text);
      prefs.putInt((base + "x").c_str(), rows[index].x);
      prefs.putInt((base + "s").c_str(), rows[index].size);
      prefs.putBool((base + "sc").c_str(), rows[index].scrolling);
    }
  }
  prefs.end();

  if (timer != nullptr) {
    timerAlarmEnable(timer);
  }
}

void loadRows() {
  prefs.begin("rows", true);
  for (int index = 0; index < kMaxRows; ++index) {
    String base = "r" + String(index);
    rows[index].active = prefs.getBool((base + "a").c_str(), false);
    if (rows[index].active) {
      rows[index].text = prefs.getString((base + "t").c_str(), "");
      rows[index].x = prefs.getInt((base + "x").c_str(), 0);
      rows[index].size = prefs.getInt((base + "s").c_str(), 0);
      rows[index].scrolling = prefs.getBool((base + "sc").c_str(), false);
      rows[index].scrollX = rows[index].x;
    }
  }
  prefs.end();
}

void clearDisplayState() {
  prefs.begin("display", false);
  prefs.clear();
  prefs.end();

  prefs.begin("rows", false);
  prefs.clear();
  prefs.end();

  for (int index = 0; index < kMaxRows; ++index) {
    rows[index] = RowSlot{};
  }
  displayMessage = "";
  scrolling = false;
  displayUpdateNeeded = true;
}

// -----------------------------------------------------------------------------
// Display rendering and refresh
// -----------------------------------------------------------------------------
void clearRowPixels(int row) {
  if (dmd == nullptr || row < 0 || row >= kMaxRows) {
    return;
  }

  const int yStart = row * 16;
  const int width = displaysAcross * 32;
  for (int x = 0; x < width; ++x) {
    for (int y = yStart; y < yStart + 16; ++y) {
      dmd->writePixel(x, y, GRAPHICS_NORMAL, 0);
    }
  }
}

void drawRow(int row) {
  if (dmd == nullptr || row < 0 || row >= kMaxRows) {
    return;
  }

  if (timer != nullptr) {
    timerAlarmDisable(timer);
  }

  clearRowPixels(row);
  if (rows[row].active) {
    dmd->selectFont(rows[row].size ? Arial_Black_16 : SystemFont5x7);
    const int drawX = rows[row].scrolling ? rows[row].scrollX : rows[row].x;
    dmd->drawString(drawX, row * 16, rows[row].text.c_str(),
                   rows[row].text.length(), GRAPHICS_NORMAL);
  }

  if (timer != nullptr) {
    timerAlarmEnable(timer);
  }
}

// The ISR does only the timing-sensitive scan operation. It does not allocate,
// parse commands, access Preferences, or wait on a mutex.
void IRAM_ATTR triggerScan() {
  if (dmd != nullptr) {
    dmd->scanDisplayBySPI();
  }
}

void updateDisplay() {
  if (dmd == nullptr) {
    return;
  }

  if (timer != nullptr) {
    timerAlarmDisable(timer);
  }

  dmd->clearScreen(true);
  bool anyRowActive = false;

  for (int row = 0; row < displaysDown && row < kMaxRows; ++row) {
    if (!rows[row].active) {
      continue;
    }
    anyRowActive = true;
    dmd->selectFont(rows[row].size ? Arial_Black_16 : SystemFont5x7);
    const int drawX = rows[row].scrolling ? rows[row].scrollX : rows[row].x;
    dmd->drawString(drawX, row * 16, rows[row].text.c_str(),
                   rows[row].text.length(), GRAPHICS_NORMAL);
  }

  if (!anyRowActive) {
    dmd->selectFont(fontSize ? Arial_Black_16 : SystemFont5x7);
    const int drawX = scrolling ? scrollX : messageXPos;
    const int drawY = scrolling ? scrollY : messageYPos;
    dmd->drawString(drawX, drawY, displayMessage.c_str(),
                   displayMessage.length(), GRAPHICS_NORMAL);
  }

  displayUpdateNeeded = false;
  if (timer != nullptr) {
    timerAlarmEnable(timer);
  }
}

// -----------------------------------------------------------------------------
// Publicly documented display-command boundary
// -----------------------------------------------------------------------------
// This small handler demonstrates the display-only state transition. The
// production transport, command vocabulary, validation rules, and response
// format are intentionally omitted from this source showcase.
void applyIllustrativeDisplayCommand(const String& message, int row, int x,
                                     int requestedSize, bool requestScroll,
                                     bool persist) {
  if (row < 0 || row >= kMaxRows) {
    return;
  }

  xSemaphoreTake(dataMutex, portMAX_DELAY);
  rows[row].active = true;
  rows[row].text = message;
  rows[row].x = max(0, min(x, displaysAcross * 32 - 1));
  rows[row].size = requestedSize ? 1 : 0;
  rows[row].scrolling = requestScroll;
  rows[row].scrollX = rows[row].x;
  drawRow(row);
  if (persist) {
    saveRows();
  }
  xSemaphoreGive(dataMutex);
}

void eraseIllustrativeRow(int row) {
  if (row < 0 || row >= kMaxRows) {
    return;
  }

  xSemaphoreTake(dataMutex, portMAX_DELAY);
  rows[row] = RowSlot{};
  clearRowPixels(row);
  xSemaphoreGive(dataMutex);
}

// -----------------------------------------------------------------------------
// RTOS display task
// -----------------------------------------------------------------------------
void displayTask(void* /*parameters*/) {
  for (;;) {
    xSemaphoreTake(dataMutex, portMAX_DELAY);

    if (displayUpdateNeeded) {
      updateDisplay();
    }

    if (scrolling && millis() - lastScroll >= kScrollIntervalMs) {
      lastScroll = millis();
      --scrollX;
      const int textWidth = displayMessage.length() * 16;
      if (scrollX < -textWidth) {
        scrollX = displaysAcross * 32;
      }
      displayUpdateNeeded = true;
    }

    if (millis() - lastRowScroll >= kScrollIntervalMs) {
      lastRowScroll = millis();
      for (int row = 0; row < displaysDown && row < kMaxRows; ++row) {
        if (!rows[row].active || !rows[row].scrolling) {
          continue;
        }
        --rows[row].scrollX;
        const int glyphWidth = rows[row].size ? 16 : 6;
        const int textWidth = rows[row].text.length() * glyphWidth;
        if (rows[row].scrollX < -textWidth) {
          rows[row].scrollX = displaysAcross * 32;
        }
        drawRow(row);
      }
    }

    xSemaphoreGive(dataMutex);
    vTaskDelay(5 / portTICK_PERIOD_MS);
  }
}

// -----------------------------------------------------------------------------
// Lifecycle sketch
// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  dataMutex = xSemaphoreCreateMutex();
  loadDisplayData();
  loadRows();

  dmd = new DMD(displaysAcross, displaysDown);

  // The production project initializes the W5500, chooses normal/maintenance
  // mode, creates its network task, and configures the display timer here.
  // Those deployment and OTA operations are intentionally omitted.
  timer = nullptr;
  displayUpdateNeeded = true;

  xTaskCreatePinnedToCore(displayTask, "Display_Task", 4096, nullptr, 1,
                          nullptr, 1);
}

void loop() {
  // The production loop also monitored the reset/factory-reset input. The
  // hardware-specific input and operational reset behavior are omitted here.
  vTaskDelay(10 / portTICK_PERIOD_MS);
}

// END OF SHOWCASE FILE
