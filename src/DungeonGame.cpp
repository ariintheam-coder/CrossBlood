#include "DungeonGame.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <string>

namespace {
constexpr uint16_t kStartHp = 20;
constexpr uint16_t kStartAttack = 5;
constexpr uint16_t kStartDefense = 2;
constexpr uint16_t kStartEnergy = 3;

const char* enemyNames[] = {
    "Slime", "Goblin", "Skeleton", "Wisp", "Mimic", "Wolf", "Bat", "Crawler"
};

int randomRange(int minInclusive, int maxInclusive) {
  return minInclusive + (std::rand() % (maxInclusive - minInclusive + 1));
}
}  // namespace

DungeonGame::DungeonGame() {
  reset();
}

void DungeonGame::reset() {
  state_ = {};
  state_.floor = 1;
  state_.heroHp = kStartHp;
  state_.heroMaxHp = kStartHp;
  state_.heroAttack = kStartAttack;
  state_.heroDefense = kStartDefense;
  state_.heroEnergy = kStartEnergy;

  battleActive_ = false;
  playerDead_ = false;
  statusText_ = "A quiet hall waits...";
  eventLog_ = "You enter the dungeon.";
  beginFloor();
}

void DungeonGame::loadFromSave(const SaveState& state) {
  state_ = state;
  if (state_.heroHp == 0) {
    state_.heroHp = state_.heroMaxHp;
  }
  beginFloor();
}

DungeonGame::SaveState DungeonGame::save() const {
  return state_;
}

void DungeonGame::beginFloor() {
  spawnEnemy();
  battleActive_ = true;
  playerDead_ = false;
  statusText_ = "Encounter! " + enemy_.name + " blocks the path.";
}

void DungeonGame::spawnEnemy() {
  const int index = randomRange(0, 7);
  enemy_.name = enemyNames[index];
  enemy_.maxHp = 6 + state_.floor * 3 + randomRange(0, 6);
  enemy_.hp = enemy_.maxHp;
  enemy_.attack = 2 + state_.floor + randomRange(0, 2);
  enemy_.goldReward = 4 + state_.floor * 2 + randomRange(0, 5);
  enemy_.relicChance = 5 + state_.floor;
}

bool DungeonGame::isBattleActive() const {
  return battleActive_;
}

bool DungeonGame::isDead() const {
  return playerDead_;
}

const DungeonGame::Enemy& DungeonGame::currentEnemy() const {
  return enemy_;
}

const std::string& DungeonGame::eventLog() const {
  return eventLog_;
}

const std::string& DungeonGame::statusText() const {
  return statusText_;
}

void DungeonGame::resolveEnemyTurn() {
  if (!battleActive_ || playerDead_) {
    return;
  }

  const int damage = std::max(1, static_cast<int>(enemy_.attack) - static_cast<int>(state_.heroDefense));
  state_.heroHp = static_cast<uint16_t>(std::max(0, static_cast<int>(state_.heroHp) - damage));

  char buffer[128];
  std::snprintf(buffer, sizeof(buffer), "%s hits for %d damage.", enemy_.name.c_str(), damage);
  eventLog_ = buffer;

  if (state_.heroHp == 0) {
    playerDead_ = true;
    statusText_ = "You fall in battle. The dungeon claims another run.";
    state_.heroHp = 1;
    battleActive_ = false;
  } else {
    statusText_ = "You stand your ground.";
  }
}

void DungeonGame::maybeDropRelic() {
  if (randomRange(1, 100) <= std::max(1, static_cast<int>(enemy_.relicChance))) {
    state_.relics++;
    eventLog_ = "A relic glints from the fallen foe.";
    statusText_ = "Relic recovered.";
  } else {
    eventLog_ = "The enemy falls, but nothing else remains.";
  }
}

bool DungeonGame::performAction(Action action) {
  if (playerDead_) {
    return false;
  }

  if (!battleActive_) {
    beginFloor();
    return true;
  }

  switch (action) {
    case Action::Attack: {
      int damage = static_cast<int>(state_.heroAttack) + randomRange(0, 3);
      enemy_.hp = static_cast<uint16_t>(std::max(0, static_cast<int>(enemy_.hp) - damage));
      eventLog_ = "You strike for " + std::to_string(damage) + " damage.";
      if (enemy_.hp == 0) {
        state_.gold += enemy_.goldReward;
        maybeDropRelic();
        statusText_ = "Enemy defeated.";
        battleActive_ = false;
        state_.floor++;
        beginFloor();
      } else {
        statusText_ = "The enemy survives.";
        resolveEnemyTurn();
      }
      break;
    }

    case Action::Defend: {
      state_.heroDefense += 1;
      eventLog_ = "You brace for impact.";
      statusText_ = "Block and breathe.";
      resolveEnemyTurn();
      break;
    }

    case Action::Rest: {
      const uint16_t heal = std::min<uint16_t>(6, state_.heroMaxHp - state_.heroHp);
      state_.heroHp += heal;
      state_.heroEnergy = std::min<uint16_t>(state_.heroEnergy + 1, 9);
      eventLog_ = "You rest and recover " + std::to_string(heal) + " HP.";
      statusText_ = "The dungeon holds for a moment.";
      break;
    }

    case Action::Loot: {
      if (state_.gold > 0) {
        state_.gold--;
        state_.heroHp = std::min<uint16_t>(state_.heroMaxHp, static_cast<uint16_t>(state_.heroHp + 3));
        eventLog_ = "You pocket a scrap of loot and recover 3 HP.";
        statusText_ = "A little luck goes a long way.";
      } else {
        eventLog_ = "You have no spare gold to spend.";
        statusText_ = "A bare stash is all you have.";
      }
      break;
    }

    case Action::ReadBoost: {
      state_.heroEnergy = std::min<uint16_t>(state_.heroEnergy + 1, 9);
      state_.heroAttack = static_cast<uint16_t>(state_.heroAttack + 1);
      state_.heroDefense = static_cast<uint16_t>(state_.heroDefense + 1);
      eventLog_ = "Reading grants courage and power.";
      statusText_ = "You feel stronger from the pages you just read.";
      break;
    }

    default:
      return false;
  }

  return true;
}

void DungeonGame::applyReadingReward(uint32_t pagesRead, uint32_t secondsRead) {
  if (pagesRead == 0 && secondsRead == 0) {
    return;
  }

  const uint32_t totalBoost = pagesRead + (secondsRead / 30U);
  const uint32_t bonus = std::min<uint32_t>(8U, totalBoost / 4U);

  state_.pagesLogged += pagesRead;
  state_.readingSeconds += secondsRead;

  state_.gold += static_cast<uint16_t>(std::min<uint32_t>(20U, bonus));
  state_.heroHp = std::min<uint16_t>(state_.heroMaxHp, static_cast<uint16_t>(state_.heroHp + bonus));
  state_.heroEnergy = std::min<uint16_t>(state_.heroEnergy + 1, 9);

  char buffer[128];
  std::snprintf(buffer, sizeof(buffer),
                "Reading reward: +%lu pages, +%lu sec, +%lu gold, +%lu HP.",
                static_cast<unsigned long>(pagesRead),
                static_cast<unsigned long>(secondsRead),
                static_cast<unsigned long>(bonus),
                static_cast<unsigned long>(bonus));
  eventLog_ = buffer;
  statusText_ = "The dungeon rewards your reading streak.";
}
