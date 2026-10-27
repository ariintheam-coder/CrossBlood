#pragma once

#include <GfxRenderer.h>
#include <MappedInputManager.h>

#include "Activity.h"
#include "DungeonGame.h"

class DungeonActivity : public Activity {
 public:
  explicit DungeonActivity(GfxRenderer& renderer, MappedInputManager& mappedInput);

  void onEnter() override;
  void onExit() override;
  void loop() override;
  void render(RenderLock&&) override;

 private:
  DungeonGame game_;
  int selection_ = 0;
  bool requestedRefresh_ = true;

  void drawStats();
  void drawBattle();
  void drawMenu();
  void handleSelection();
};
