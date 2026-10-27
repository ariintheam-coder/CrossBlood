namespace {
enum class HomeMenuAction {
  BrowseFiles,
  ContinueReading,
  Library,
  OpdsBrowser,
  ReadingStats,
  Bookmarks,
  FileTransfer,
  Dungeon,
  Settings,
};

struct HomeMenuEntry {
  const char* label;
  UIIcon icon;
  HomeMenuAction action;
};

struct HomeMenuEntries {
  static constexpr int kCapacity = 8;
  std::array<HomeMenuEntry, kCapacity> entries{};
  int count = 0;

  void push(const HomeMenuEntry& entry) {
    if (count >= kCapacity) return;
    entries[count++] = entry;
  }

  int size() const { return count; }

  const HomeMenuEntry& operator[](int index) const { return entries[index]; }
};

...
void appendHomeMenuItems(HomeMenuEntries& items, bool hasOpdsServers, bool hasReadingStats, bool hasBookmarks,
                         bool hasClippings) {
  items.push({tr(STR_BROWSE_FILES), Folder, HomeMenuAction::BrowseFiles});
  items.push({tr(STR_LIBRARY), Library, HomeMenuAction::Library});

  if (hasOpdsServers) {
    items.push({tr(STR_OPDS_BROWSER), Opds, HomeMenuAction::OpdsBrowser});
  }
  if (hasReadingStats) {
    items.push({tr(STR_READING_STATS), Chart, HomeMenuAction::ReadingStats});
  }
  if (hasBookmarks || hasClippings) {
    items.push({savedItemsLabel(hasBookmarks, hasClippings), BookmarkIcon, HomeMenuAction::Bookmarks});
  }

  items.push({tr(STR_FILE_TRANSFER), Transfer, HomeMenuAction::FileTransfer});
  items.push({"Dungeon", Book, HomeMenuAction::Dungeon});
  items.push({tr(STR_SETTINGS_TITLE), Settings, HomeMenuAction::Settings});
}

HomeMenuEntries buildHomeMenuItems(bool hasOpdsServers, bool hasReadingStats, bool hasBookmarks, bool hasClippings) {
  HomeMenuEntries items;
  appendHomeMenuItems(items, hasOpdsServers, hasReadingStats, hasBookmarks, hasClippings);
  return items;
}

HomeMenuEntries buildMinimalMenuItems(bool hasOpdsServers, bool hasReadingStats, bool hasBookmarks, bool hasClippings) {
  HomeMenuEntries items;
  if (SETTINGS.isLibraryFileBrowserSwapped()) {
    items.push({tr(STR_BROWSE_FILES), Folder, HomeMenuAction::BrowseFiles});
  } else {
    items.push({tr(STR_LIBRARY), Library, HomeMenuAction::Library});
  }

  if (hasOpdsServers) {
    items.push({tr(STR_OPDS_BROWSER), Opds, HomeMenuAction::OpdsBrowser});
  }
  if (hasBookmarks || hasClippings) {
    items.push({savedItemsLabel(hasBookmarks, hasClippings), BookmarkIcon, HomeMenuAction::Bookmarks});
  }
  if (hasReadingStats) {
    items.push({tr(STR_READING_STATS), Chart, HomeMenuAction::ReadingStats});
  }

  items.push({tr(STR_FILE_TRANSFER), Transfer, HomeMenuAction::FileTransfer});
  items.push({"Dungeon", Book, HomeMenuAction::Dungeon});
  return items;
}

HomeMenuEntries buildSelectableHomeMenuItems(bool hasOpdsServers, bool hasReadingStats, bool hasBookmarks,
                                            bool hasClippings, bool includeContinueReading) {
  HomeMenuEntries items;
  if (includeContinueReading) {
    items.push({tr(STR_CONTINUE_READING), Book, HomeMenuAction::ContinueReading});
  }
  appendHomeMenuItems(items, hasOpdsServers, hasReadingStats, hasBookmarks, hasClippings);
  return items;
}

HomeMenuAction homeActionForInitialMenuItem(HomeMenuItem item) {
  switch (item) {
    case HomeMenuItem::FILE_BROWSER:
      return HomeMenuAction::BrowseFiles;
    case HomeMenuItem::LIBRARY:
      return HomeMenuAction::Library;
    case HomeMenuItem::OPDS_BROWSER:
      return HomeMenuAction::OpdsBrowser;
    case HomeMenuItem::FILE_TRANSFER:
      return HomeMenuAction::FileTransfer;
    case HomeMenuItem::SETTINGS_MENU:
      return HomeMenuAction::Settings;
    case HomeMenuItem::NONE:
    default:
      return HomeMenuAction::ContinueReading;
  }
}
...
}  // namespace
