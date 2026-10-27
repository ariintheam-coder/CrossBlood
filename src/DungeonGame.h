#pragma once

#include <stdint.h>
#include <string>

class DungeonGame {
 public:
  enum class Action {
    Attack,
    Defend,
    Rest,
    Loot,
    ReadBoost
  };

  struct Enemy {
    std::string name;
    uint16_t hp = 0;
    uint16_t maxHp = 0;
    uint16_t attack = 0;
    uint16_t goldReward = 0;
    uint16_t relicChance = 0;
  };

  struct SaveState {
    uint16_t floor = 1;
    uint16_t heroHp = 20;
    uint16_t heroMaxHp = 20;
    uint16_t heroAttack = 5;
    uint16_t heroDefense = 2;
    uint16_t heroEnergy = 3;
    uint16_t gold = 0;
    uint16_t relics = 0;
    uint32_t pagesLogged = 0;
    uint32_t readingSeconds = 0;
  };

  DungeonGame();
  void reset();
  void loadFromSave(const SaveState& state);
  SaveState save() const;

  void beginFloor();
  void applyReadingReward(uint32_t pagesRead, uint32_t secondsRead);
  bool performAction(Action action);

  bool isBattleActive() const;
  bool isDead() const;
  const Enemy& currentEnemy() const;
  const std::string& eventLog() const;
  const std::string& statusText() const;

 private:
  SaveState state_;
  Enemy enemy_;
  bool battleActive_ = false;
  bool playerDead_ = false;
  std::string statusText_;
  std::string eventLog_;
  uint32_t floorProgress_ = 0;

  void spawnEnemy();
  void resolveEnemyTurn();
  void maybeDropRelic();
};
