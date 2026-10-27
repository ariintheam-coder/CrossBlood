#include "CrossPointState.h"
#include "activities/ActivityManager.h"

void ActivityManager::goToDungeon() {
  replaceActivity(std::make_unique<DungeonActivity>(renderer, mappedInput));
}
