#include "CrossPointState.h"

#include <ArduinoJson.h>
#include <Logging.h>
#include <PersistableStore.h>
#include <Serialization.h>

#include <algorithm>
#include <cstring>

void CrossPointState::toJson(JsonDocument& doc) const {
  doc["openEpubPath"] = openEpubPath;
  doc["favoriteSleepImagePath"] = favoriteSleepImagePath;
  doc["preferredSleepFolderPath"] = preferredSleepFolderPath;

  JsonArray recentSleepImagesJson = doc["recentSleepImages"].to<JsonArray>();
  for (uint8_t i = 0; i < SLEEP_RECENT_COUNT; ++i) {
    recentSleepImagesJson.add(recentSleepImages[i]);
  }
  doc["recentSleepPos"] = recentSleepPos;
  doc["recentSleepFill"] = recentSleepFill;

  doc["favoriteBootImagePath"] = favoriteBootImagePath;
  JsonArray recentBootImagesJson = doc["recentBootImages"].to<JsonArray>();
  for (uint8_t i = 0; i < BOOT_RECENT_COUNT; ++i) {
    recentBootImagesJson.add(recentBootImages[i]);
  }
  doc["recentBootPos"] = recentBootPos;
  doc["recentBootFill"] = recentBootFill;

  doc["readerActivityLoadCount"] = readerActivityLoadCount;
  doc["lastSleepFromReader"] = lastSleepFromReader;
  doc["showBootScreen"] = showBootScreen;
  doc["quickLockResumePending"] = quickLockResumePending;
  doc["quickLockRestoreFrontlight"] = quickLockRestoreFrontlight;

  doc["pendingBookmarkSpine"] = pendingBookmarkSpine;
  doc["pendingBookmarkProgress"] = pendingBookmarkProgress;
  doc["pendingBookmarkParagraphIndex"] = pendingBookmarkParagraphIndex;
  doc["pendingClippingIndex"] = pendingClippingIndex;

  // Dungeon state
  doc["dungeonFloor"] = dungeonFloor;
  doc["dungeonHeroHp"] = dungeonHeroHp;
  doc["dungeonHeroMaxHp"] = dungeonHeroMaxHp;
  doc["dungeonHeroAttack"] = dungeonHeroAttack;
  doc["dungeonHeroDefense"] = dungeonHeroDefense;
  doc["dungeonHeroEnergy"] = dungeonHeroEnergy;
  doc["dungeonGold"] = dungeonGold;
  doc["dungeonRelics"] = dungeonRelics;
  doc["dungeonLastPagesLogged"] = dungeonLastPagesLogged;
  doc["dungeonLastReadingSeconds"] = dungeonLastReadingSeconds;
}

bool CrossPointState::fromJson(JsonVariantConst doc) {
  openEpubPath = doc["openEpubPath"] | "";
  favoriteSleepImagePath = doc["favoriteSleepImagePath"] | "";
  preferredSleepFolderPath = doc["preferredSleepFolderPath"] | "";

  std::fill_n(recentSleepImages, SLEEP_RECENT_COUNT, 0);
  JsonArrayConst recentSleepImagesJson = doc["recentSleepImages"];
  for (uint8_t i = 0; i < SLEEP_RECENT_COUNT && i < recentSleepImagesJson.size(); ++i) {
    recentSleepImages[i] = recentSleepImagesJson[i] | 0;
  }
  recentSleepPos = doc["recentSleepPos"] | 0;
  recentSleepFill = doc["recentSleepFill"] | 0;

  std::fill_n(recentBootImages, BOOT_RECENT_COUNT, 0);
  JsonArrayConst recentBootImagesJson = doc["recentBootImages"];
  for (uint8_t i = 0; i < BOOT_RECENT_COUNT && i < recentBootImagesJson.size(); ++i) {
    recentBootImages[i] = recentBootImagesJson[i] | 0;
  }
  recentBootPos = doc["recentBootPos"] | 0;
  recentBootFill = doc["recentBootFill"] | 0;

  readerActivityLoadCount = doc["readerActivityLoadCount"] | 0;
  lastSleepFromReader = doc["lastSleepFromReader"] | false;
  showBootScreen = doc["showBootScreen"] | true;
  quickLockResumePending = doc["quickLockResumePending"] | false;
  quickLockRestoreFrontlight = doc["quickLockRestoreFrontlight"] | false;

  pendingBookmarkSpine = doc["pendingBookmarkSpine"] | UINT16_MAX;
  pendingBookmarkProgress = doc["pendingBookmarkProgress"] | -1.0f;
  pendingBookmarkParagraphIndex = doc["pendingBookmarkParagraphIndex"] | UINT16_MAX;
  pendingClippingIndex = doc["pendingClippingIndex"] | UINT16_MAX;

  dungeonFloor = doc["dungeonFloor"] | 1;
  dungeonHeroHp = doc["dungeonHeroHp"] | 20;
  dungeonHeroMaxHp = doc["dungeonHeroMaxHp"] | 20;
  dungeonHeroAttack = doc["dungeonHeroAttack"] | 5;
  dungeonHeroDefense = doc["dungeonHeroDefense"] | 2;
  dungeonHeroEnergy = doc["dungeonHeroEnergy"] | 3;
  dungeonGold = doc["dungeonGold"] | 0;
  dungeonRelics = doc["dungeonRelics"] | 0;
  dungeonLastPagesLogged = doc["dungeonLastPagesLogged"] | 0;
  dungeonLastReadingSeconds = doc["dungeonLastReadingSeconds"] | 0;

  return true;
}

bool CrossPointState::saveToFile() const {
  JsonDocument doc;
  toJson(doc);
  return PersistableStoreBase::writeDocToFile(getFilePath(), doc);
}

bool CrossPointState::loadFromFile() {
  JsonDocument doc;
  if (!PersistableStoreBase::readDocFromFile(getFilePath(), doc)) {
    return false;
  }
  return fromJson(doc.as<JsonVariantConst>());
}

void CrossPointState::pushRecentSleep(uint16_t idx) {
  recentSleepImages[recentSleepPos] = idx;
  recentSleepPos = (recentSleepPos + 1) % SLEEP_RECENT_COUNT;
  if (recentSleepFill < SLEEP_RECENT_COUNT) {
    recentSleepFill++;
  }
}

void CrossPointState::clearRecentSleepHistory() {
  std::fill_n(recentSleepImages, SLEEP_RECENT_COUNT, 0);
  recentSleepPos = 0;
  recentSleepFill = 0;
}

void CrossPointState::pushRecentBoot(uint16_t idx) {
  recentBootImages[recentBootPos] = idx;
  recentBootPos = (recentBootPos + 1) % BOOT_RECENT_COUNT;
  if (recentBootFill < BOOT_RECENT_COUNT) {
    recentBootFill++;
  }
}

bool CrossPointState::consumePendingOverlayResume(PendingOverlayResume& value) {
  if (pendingOverlayResume.isEmpty()) {
    return false;
  }
  value = pendingOverlayResume;
  pendingOverlayResume = {};
  return true;
}

void CrossPointState::setPendingOverlayResume(PendingOverlayResume value) {
  pendingOverlayResume = value;
}

bool CrossPointState::isRecentSleep(uint16_t idx, uint8_t checkCount) const {
  const uint8_t effective = std::min(checkCount, recentSleepFill);
  for (uint8_t i = 0; i < effective; ++i) {
    const uint8_t slot = (recentSleepPos + SLEEP_RECENT_COUNT - 1 - i) % SLEEP_RECENT_COUNT;
    if (recentSleepImages[slot] == idx) {
      return true;
    }
  }
  return false;
}

bool CrossPointState::loadFromBinaryFile() {
  return false;
}
