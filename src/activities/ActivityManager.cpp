#include "ActivityManager.h"

#include "activities/dungeon/DungeonActivity.h"

void ActivityManager::goToDungeon() {
  replaceActivity(std::make_unique<DungeonActivity>(renderer, mappedInput));
}
