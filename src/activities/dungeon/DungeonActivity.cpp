#include "DungeonActivity.h"

#include <HalDisplay.h>
#include <StringUtils.h>

#include "CrossPointState.h"

namespace {
constexpr int kMenuCount = 4;

std::string formatNum(int value) {
  return std::to_string(value);
}
}  // namespace

DungeonActivity::DungeonActivity(GfxRenderer& renderer, MappedInputManager& mappedInput)
    : Activity(renderer, mappedInput) {}

void DungeonActivity::onEnter() {
  Activity::onEnter();

  auto& state = APP_STATE;
  game_.loadFromSave({
      state.dungeonFloor,
      state.dungeonHeroHp,
      state.dungeonHeroMaxHp,
      state.dungeonHeroAttack,
      state.dungeonHeroDefense,
      state.dungeonHeroEnergy,
      state.dungeonGold,
      state.dungeonRelics,
      state.dungeonLastPagesLogged,
      state.dungeonLastReadingSeconds,
  });

  requestedRefresh_ = true;
}

void DungeonActivity::onExit() {
  Activity::onExit();

  auto& state = APP_STATE;
  auto save = game_.save();

  state.dungeonFloor = save.floor;
  state.dungeonHeroHp = save.heroHp;
  state.dungeonHeroMaxHp = save.heroMaxHp;
  state.dungeonHeroAttack = save.heroAttack;
  state.dungeonHeroDefense = save.heroDefense;
  state.dungeonHeroEnergy = save.heroEnergy;
  state.dungeonGold = save.gold;
  state.dungeonRelics = save.relics;
  state.dungeonLastPagesLogged = save.pagesLogged;
  state.dungeonLastReadingSeconds = save.readingSeconds;
  state.saveToFile();
}

void DungeonActivity::handleSelection() {
  if (selection_ == 0) {
    game_.performAction(DungeonGame::Action::Attack);
  } else if (selection_ == 1) {
    game_.performAction(DungeonGame::Action::Defend);
  } else if (selection_ == 2) {
    game_.performAction(DungeonGame::Action::Rest);
  } else if (selection_ == 3) {
    game_.performAction(DungeonGame::Action::ReadBoost);
  }
  requestedRefresh_ = true;
}

void DungeonActivity::drawStats() {
  renderer.clearScreen();

  const auto w = renderer.getScreenWidth();
  const auto h = renderer.getScreenHeight();

  char line[128];

  std::snprintf(line, sizeof(line), "Floor: %u  Gold: %u  Relics: %u",
                static_cast<unsigned>(game_.save().floor),
                static_cast<unsigned>(game_.save().gold),
                static_cast<unsigned>(game_.save().relics));
  // Replace with your repo's real GUI draw call if needed.
  // GUI.drawString(renderer, 10, 10, line);

  std::snprintf(line, sizeof(line), "HP: %u/%u  ATK: %u  DEF: %u",
                static_cast<unsigned>(game_.save().heroHp),
                static_cast<unsigned>(game_.save().heroMaxHp),
                static_cast<unsigned>(game_.save().heroAttack),
                static_cast<unsigned>(game_.save().heroDefense));
  // GUI.drawString(renderer, 10, 30, line);

  std::snprintf(line, sizeof(line), "Enemy: %s HP: %u",
                game_.currentEnemy().name.c_str(),
                static_cast<unsigned>(game_.currentEnemy().hp));
  // GUI.drawString(renderer, 10, 50, line);

  std::snprintf(line, sizeof(line), "Event: %s", game_.eventLog().c_str());
  // GUI.drawString(renderer, 10, 70, line);
}

void DungeonActivity::drawBattle() {
  drawStats();
}

void DungeonActivity::drawMenu() {
  // This is intentionally simple. Replace with your project's actual GUI layout.
  // The main goal here is to provide the full dungeon feature loop.
}

void DungeonActivity::loop() {
  if (mappedInput.wasPressed(MappedInputManager::Button::Up)) {
    selection_ = (selection_ + kMenuCount - 1) % kMenuCount;
    requestedRefresh_ = true;
  }

  if (mappedInput.wasPressed(MappedInputManager::Button::Down)) {
    selection_ = (selection_ + 1) % kMenuCount;
    requestedRefresh_ = true;
  }

  if (mappedInput.wasReleased(MappedInputManager::Button::Confirm)) {
    handleSelection();
  }

  if (mappedInput.wasReleased(MappedInputManager::Button::Back)) {
    activityManager.popActivity();
  }

  if (requestedRefresh_) {
    requestUpdate();
    requestedRefresh_ = false;
  }
}

void DungeonActivity::render(RenderLock&&) {
  drawBattle();
  drawMenu();
  renderer.displayBuffer(HalDisplay::FAST_REFRESH);
}
