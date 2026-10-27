#pragma once

#include <ArduinoJson.h>
#include <PersistableStore.h>

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>

#include "PendingOverlayResume.h"

class CrossPointState : public PersistableStore<CrossPointState> {
  mutable std::mutex _mutex;
  CrossPointState() = default;
  friend class PersistableStore<CrossPointState>;

 public:
  std::mutex& getMutex() const { return _mutex; }

  static constexpr uint8_t SLEEP_RECENT_COUNT = 16;
  static constexpr uint8_t BOOT_RECENT_COUNT = 16;

  std::string openEpubPath;
  std::string favoriteSleepImagePath;
  std::string preferredSleepFolderPath;
  uint16_t recentSleepImages[SLEEP_RECENT_COUNT] = {};
  uint8_t recentSleepPos = 0;
  uint8_t recentSleepFill = 0;
  std::string favoriteBootImagePath;
  uint16_t recentBootImages[BOOT_RECENT_COUNT] = {};
  uint8_t recentBootPos = 0;
  uint8_t recentBootFill = 0;
  uint8_t readerActivityLoadCount = 0;
  bool lastSleepFromReader = false;
  bool showBootScreen = true;
  bool quickLockResumePending = false;
  bool quickLockRestoreFrontlight = false;
  PendingOverlayResume pendingOverlayResume{};

  // Dungeon Crawler Carl-inspired state
  uint16_t dungeonFloor = 1;
  uint16_t dungeonHeroHp = 20;
  uint16_t dungeonHeroMaxHp = 20;
  uint16_t dungeonHeroAttack = 5;
  uint16_t dungeonHeroDefense = 2;
  uint16_t dungeonHeroEnergy = 3;
  uint16_t dungeonGold = 0;
  uint16_t dungeonRelics = 0;
  uint32_t dungeonLastPagesLogged = 0;
  uint32_t dungeonLastReadingSeconds = 0;

  void setPendingOverlayResume(PendingOverlayResume value);
  bool consumePendingOverlayResume(PendingOverlayResume& value);

  bool isRecentSleep(uint16_t idx, uint8_t checkCount) const;
  void pushRecentSleep(uint16_t idx);
  void clearRecentSleepHistory();
  void pushRecentBoot(uint16_t idx);
  ~CrossPointState() = default;

  bool saveToFile() const;
  bool loadFromFile();
  static const char* getFilePath() { return "/.crosspoint/state.json"; }
  void toJson(JsonDocument& doc) const;
  bool fromJson(JsonVariantConst doc);

  uint16_t pendingBookmarkSpine = UINT16_MAX;
  float pendingBookmarkProgress = -1.0f;
  uint16_t pendingBookmarkParagraphIndex = UINT16_MAX;
  uint16_t pendingClippingIndex = UINT16_MAX;

  std::atomic<bool> hasPendingAlert{false};
  std::atomic<bool> pendingAlertGoHomeOnBack{false};
  char pendingAlertTitle[64] = {};
  char pendingAlertBody[256] = {};

 private:
  bool loadFromBinaryFile();
  mutable uint32_t lastSavedCrc = 0;
  mutable bool lastSavedCrcValid = false;
};

#define APP_STATE CrossPointState::getInstance()
