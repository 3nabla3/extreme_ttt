class AppConfig {
public:
  AppConfig() {
    // if there is a settings.ini in the current dir use that
    std::filesystem::path configFilePath = std::filesystem::current_path() / "settings.ini";

    if (!std::filesystem::exists(configFilePath)) {
      // if not use the one in the parent
      configFilePath = std::filesystem::current_path().parent_path() / "settings.ini";
    }

    Init(configFilePath);
  }
  AppConfig(const std::filesystem::path& configFilePath) { Init(configFilePath); }

  std::string GetXPlayerType() const { return Get("Players", "player_x_type"); }
  std::string GetOPlayerType() const { return Get("Players", "player_o_type"); }
  uint8_t GetAiPlayerDepth() const { return std::stoi(Get("AIplayer", "depth")); }

private:
  void Init(const std::filesystem::path& configFilePath) {
    // configFilePath.
    INIReader reader(configFilePath);
    int result = reader.ParseError();

    if (result == -1) {
      SPDLOG_WARN(
          "File {} not found, using default values",
          configFilePath.string());

      LoadDefaults();
      return;
    }

    if (result != 0) {
      SPDLOG_ERROR(
          "Failed to parse {} at line {}, using default values",
          configFilePath.string(),
          result);

      LoadDefaults();
      return;
    }

    LoadSettings(reader);
    SPDLOG_INFO(
        "Loaded settings from {}",
        configFilePath.string());

    PrintSettings();
  }

  using SettingKey = std::pair<std::string, std::string>;

  struct SettingKeyHash {
    std::size_t operator()(const SettingKey& key) const {
      return std::hash<std::string>{}(key.first) ^ std::hash<std::string>{}(key.second);
    }
  };

  std::string Get(const std::string& section, const std::string& name) const {
    SettingKey key{section, name};
    auto it = settings.find(key);
    return (it != settings.end()) ? it->second : defaultSettings.at(key);
  }

  void LoadSettings(const INIReader& reader) {
    for (const auto& [key, defaultValue] : defaultSettings) {
      std::string value = reader.Get(key.first, key.second, defaultValue);
      // I dont want case sensitivity in the config file
      ToLowerCase(value);
      settings[key] = value;
    }
  }

  void LoadDefaults() {
    for (const auto& [key, defaultValue] : defaultSettings) {
      settings[key] = defaultValue;
    }
  }

  void PrintSettings() {
    size_t maxLength = 0;
    for (const auto& [key, value] : settings) {
      maxLength = std::max(maxLength, key.first.length() + key.second.length() + 1); // +1 for the dot
    }

    for (const auto& [key, value] : settings) {
      std::string fullKey = key.first + "." + key.second;
      size_t padding = maxLength - fullKey.length() + 8; // +8 for minimum one tab

      std::string tabString(padding, ' ');
      SPDLOG_INFO("{}{}{}", fullKey, tabString, value);
    }
  }

  void ToLowerCase(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
  }

  std::unordered_map<SettingKey, std::string, SettingKeyHash> settings;

  const std::unordered_map<SettingKey, std::string, SettingKeyHash> defaultSettings = {
      {{"Players", "player_x_type"}, "human"},
      {{"Players", "player_o_type"}, "ai"},
      {{"AIplayer", "depth"}, "3"}};
};
