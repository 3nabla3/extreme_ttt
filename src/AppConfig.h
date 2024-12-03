class AppConfig {
public:
  AppConfig(int argc, char* argv[]) {
    SPDLOG_DEBUG("argv[0]: {}", argv[0]);

    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
      SPDLOG_INFO("Usage: {} <config file>", argv[0]);
      exit(0);
    }

    if (argc > 2) {
      SPDLOG_ERROR("Usage: {} <config file>", argv[0]);
      exit(1);
    }

    if (argc == 2) {
      if (!std::filesystem::exists(argv[1])) {
        SPDLOG_ERROR("Config file {} does not exist", argv[1]);
        exit(1);
      }
      Init(argv[1]);
      return;
    }

    // by default use the root of the git repo
    Init(std::filesystem::path(__FILE__).parent_path().parent_path() / "settings.ini");
  }

  std::string GetXPlayerType() const { return Get("Players", "player_x_type"); }
  std::string GetOPlayerType() const { return Get("Players", "player_o_type"); }
  uint8_t GetAiPlayerDepth() const { return static_cast<uint8_t>(std::stoi(Get("AIplayer", "depth"))); }

private:
  void Init(const std::filesystem::path& configFilePath) {
    // configFilePath.
    INIReader reader(configFilePath.string());
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
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
      return static_cast<char>(std::tolower(c));
    });
  }

  std::unordered_map<SettingKey, std::string, SettingKeyHash> settings;

  const std::unordered_map<SettingKey, std::string, SettingKeyHash> defaultSettings = {
      {{"Players", "player_x_type"}, "human"},
      {{"Players", "player_o_type"}, "ai"},
      {{"AIplayer", "depth"}, "3"}};
};
