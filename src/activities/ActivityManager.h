#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <atomic>
#include <cassert>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "CrossPointSettings.h"
#include "GfxRenderer.h"
#include "MappedInputManager.h"
#include "util/QuickLockTrigger.h"
#include "util/ScreenshotInfo.h"

#ifndef portMUX_INITIALIZER_UNLOCKED
struct portMUX_TYPE {};
#define portMUX_INITIALIZER_UNLOCKED \
  {                                  \
  }
#endif

class Activity;
class RenderLock;

#if CROSSINK_APP_CAP_TOUCH
struct LiveLightSwipeState {
  Activity* owner = nullptr;
  bool tracking = false;
  bool active = false;
  bool blocked = false;
  bool changed = false;
  bool initialOn = false;
  uint8_t action = 0;
  int startX = 0;
  int startY = 0;
  int firstX = 0;
  int firstY = 0;
  int secondX = 0;
  int secondY = 0;
  int direction = 0;
  int movementSign = 0;
  bool vertical = false;
  uint8_t initialValue = 0;
};
#endif

enum class RequestUpdateResult { Rendered, Rejected };
enum class HomeMenuItem { NONE, FILE_BROWSER, LIBRARY, OPDS_BROWSER, FILE_TRANSFER, SETTINGS_MENU, DUNGEON };

class ActivityManager {
  friend class RenderLock;

 protected:
  GfxRenderer& renderer;
  MappedInputManager& mappedInput;
  std::vector<std::unique_ptr<Activity>> stackActivities;
  std::unique_ptr<Activity> currentActivity;

#if CROSSINK_APP_CAP_TOUCH
  LiveLightSwipeState edgeLightSwipe;
  LiveLightSwipeState twoFingerLightSwipe;
#endif

  void exitActivity(const RenderLock& lock);

  std::unique_ptr<Activity> pendingActivity;
  enum class PendingAction { None, Push, Pop, Replace };
  PendingAction pendingAction = PendingAction::None;
  int16_t pendingReaderMenuAction = -1;
  Activity* pendingHomeReaderTarget = nullptr;

  std::string preferredHomeBookPath;
  bool returningHomeThroughSettings = false;

  TaskHandle_t renderTaskHandle = nullptr;
  static void renderTaskTrampoline(void* param);
  [[noreturn]] virtual void renderTaskLoop();

  TaskHandle_t waitingTaskHandle = nullptr;
  portMUX_TYPE renderStateMux = portMUX_INITIALIZER_UNLOCKED;
  SemaphoreHandle_t renderingMutex = nullptr;

  std::atomic<bool> requestedUpdate{false};
  std::atomic<bool> restoredActivityNeedsRender{false};

  Activity* findEpubReader() const;
  bool handleHomeReaderShortcut();
  bool continueHomeReaderUnwind();
  bool handleGlobalHomeGesture();
  bool restoreBackdropBehindCurrentOverlay();

 public:
  explicit ActivityManager(GfxRenderer& renderer, MappedInputManager& mappedInput)
      : renderer(renderer), mappedInput(mappedInput), renderingMutex(xSemaphoreCreateMutex()) {
    assert(renderingMutex != nullptr && "Failed to create rendering mutex");
    stackActivities.reserve(10);
  }

  ~ActivityManager() { assert(false); };

  void begin(uint32_t renderTaskStackBytes = 16384);
  void loop();

  void replaceActivity(std::unique_ptr<Activity>&& newActivity);

  void goToFileTransfer(std::string returnBookPath = {});
  void goToCalibreWireless(const std::string& returnBookPath = {});
  void goToJoinNetworkFileTransfer(const std::string& returnBookPath = {});
  void goToHotspotFileTransfer(const std::string& returnBookPath = {});
  void goToUsbDrive();
  bool resumeFileTransferFromNetworkBoot(uint32_t payload);
  void goToNearbyStatsSync();
  bool goToNearbyBookSend(std::string path, bool returnToReader);
  void goToNearbyBookReceive();
  void goToSettings(bool dismissOnUpSwipe = false);
  void goToFileBrowser(std::string path = {});
  void goToLibrary();
  void goToBrowser();
  void goToDungeon();
  bool goToOpdsServer(uint32_t serverIndex, bool networkBootReady = false);
  void goToReader(std::string path, bool suppressBackRelease = false, bool allowFastInitialRefresh = false,
                  bool cleanImageBaseOnEntry = false);
  void goToReaderAndRunMenuAction(std::string path, uint8_t action);
  void goToSleep(bool fromTimeout = false);
  void goToBoot();
  void goToFullScreenMessage(std::string message, EpdFontFamily::Style style = EpdFontFamily::REGULAR);
  void goToCrashReport();
  void goHome(HomeMenuItem initialMenuItem = HomeMenuItem::NONE,
              HalDisplay::RefreshMode initialRefreshMode = HalDisplay::FAST_REFRESH);

  void pushActivity(std::unique_ptr<Activity>&& activity);
  void popActivity();

  bool preventAutoSleep() const;
  bool requiresExclusiveStorageLoop() const;
  bool blocksGlobalInput() const;
  bool isHomeActivity() const;
  bool isReaderActivity() const;
  bool openReaderSettingsForTouchscreenEscapeHatch();
  bool handleHomeButtonBackOrHome();
  bool openReaderMenuFromShortcut();
  bool handleShortcutAction(uint8_t action);
  bool hasActivityNamed(const char* activityName) const;
#ifdef SIMULATOR
  bool isCurrentActivityNamed(const char* activityName) const;
  Activity* simulatorCurrentActivity() const { return currentActivity.get(); }
#endif
  bool canSnapshotForSleepOverlay() const;
  bool requestManualReaderRefresh();
  bool handleShortcutAction(CrossPointSettings::SHORT_PWRBTN action);
  bool handleQuickLockUnlock(QuickLockTrigger trigger);
  void persistGlobalSettings();
  bool beginGlobalSettingsEdit();
  void endGlobalSettingsEdit();
  void notifyInputLockChanged(bool locked);
  void notifyUserInput();
  bool skipLoopDelay() const;
  uint8_t inputPollDelayMs() const;
  std::string getCurrentBookPath() const;
  ScreenshotInfo getScreenshotInfo() const;

  void requestUpdate(bool immediate = false);
  RequestUpdateResult requestUpdateAndWait();
};

extern ActivityManager activityManager;
