#include <huxerui/huxerui.h>

#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <unistd.h>
#include <string>
#include <vector>

using namespace huxerui;

namespace {

const std::array<std::string, 4> kThemes = {"PRTS-Final", "PRTS-Final-Static", "Bibata-Modern-Classic", "Adwaita"};
const std::array<std::string, 4> kThemeLabels = {"PRTS 动画", "PRTS 静态", "Bibata", "Adwaita"};


namespace fs = std::filesystem;

std::string ExecutableDir() {
  char buffer[4096];
  const ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
  if (length <= 0) {
    return {};
  }
  buffer[length] = '\0';
  return fs::path(buffer).parent_path().string();
}

std::string HomeDir() {
  const char* home = std::getenv("HOME");
  return home != nullptr ? std::string(home) : std::string();
}

std::string FindFile(const std::vector<std::string>& candidates) {
  for (const auto& path : candidates) {
    if (!path.empty() && fs::exists(path)) {
      return path;
    }
  }
  return {};
}

std::string HelperPath() {
  const auto dir = ExecutableDir();
  return FindFile({
      dir + "/helper.py",
      HomeDir() + "/.local/opt/prts-cursor/helper.py",
      HomeDir() + "/.config/prts-cursor-gui/helper.py",
  });
}

std::string Run(const std::string& command) {
  std::string output;
  FILE* pipe = popen(command.c_str(), "r");
  if (pipe == nullptr) {
    return {};
  }
  char buffer[512];
  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    output += buffer;
  }
  pclose(pipe);
  while (!output.empty() && (output.back() == '\n' || output.back() == '\r')) {
    output.pop_back();
  }
  return output;
}

std::vector<std::string> Lines(const std::string& text) {
  std::vector<std::string> out;
  std::string current;
  for (const char ch : text) {
    if (ch == '\n') {
      if (!current.empty()) {
        out.push_back(current);
      }
      current.clear();
    } else if (ch != '\r') {
      current += ch;
    }
  }
  if (!current.empty()) {
    out.push_back(current);
  }
  return out;
}

std::string Field(const std::string& text, const std::string& key) {
  const std::string prefix = key + "=";
  const auto start = text.find(prefix);
  if (start == std::string::npos) {
    return {};
  }
  const auto end = text.find('\n', start);
  return text.substr(start + prefix.size(), end == std::string::npos ? std::string::npos : end - start - prefix.size());
}

std::string ShortLabel(const std::string& name) {
  std::string out = name;
  if (out.rfind("cursor.", 0) == 0) {
    out = out.substr(7);
  }
  if (out.size() > 16) {
    out = out.substr(0, 15) + "…";
  }
  return out;
}

std::size_t ThemeIndex(const std::string& name) {
  for (std::size_t i = 0; i < kThemes.size(); ++i) {
    if (kThemes[i] == name) {
      return i;
    }
  }
  return 2;
}

struct Initial {
  std::size_t theme = 2;
  float size = 24.0F;
  std::size_t effect = 0;
  float radius = 96.0F;
  std::vector<std::string> effects;
};

const Initial& Current() {
  static const Initial cached = [] {
    Initial value;
    value.effects = Lines(Run(std::string("python3 ") + HelperPath() + " --effects 2>/dev/null"));
    const auto out = Run(std::string("python3 ") + HelperPath() + " --get 2>/dev/null");
    const auto theme = Field(out, "theme");
    if (!theme.empty()) {
      value.theme = ThemeIndex(theme);
      value.size = static_cast<float>(std::atof(Field(out, "size").c_str()));
      value.radius = static_cast<float>(std::atof(Field(out, "radius").c_str()));
      const auto effect = Field(out, "effect");
      for (std::size_t i = 0; i < value.effects.size(); ++i) {
        if (value.effects[i] == effect) {
          value.effect = i + 1;
        }
      }
    }
    return value;
  }();
  return cached;
}

std::string EffectName(const std::vector<std::string>& effects, std::size_t index) {
  if (index == 0 || index > effects.size()) {
    return "off";
  }
  return effects[index - 1];
}


std::function<void()> g_toggle;
std::function<void()> g_follow;

std::string TrayIconPath() {
  const auto dir = ExecutableDir();
  return FindFile({
      dir + "/icon.png",
      HomeDir() + "/.local/opt/prts-cursor/icon.png",
      "icon.png",
  });
}

void InstallTray(ApplicationContext&) {
  const auto icon = TrayIconPath();
  if (icon.empty()) {
    return;
  }
  const auto app = UseApplication();
  const auto tray = app.SystemTray();
  tray.OnActivate([] {
    if (g_toggle) {
      g_toggle();
    }
  });
  tray.Show(ImageAsset::FromFile(File(icon)), SystemTrayOptions{
      .tooltip = "PRTS 光标",
      .menu = {
          MenuEntry(MenuItem("显示 / 隐藏", [] { if (g_toggle) g_toggle(); })),
          MenuEntry(MenuItem("跟随 Niri", [] { if (g_follow) g_follow(); })),
          MenuEntry(MenuSection{}),
          MenuEntry(MenuItem("退出", [app] { app.Quit(); })),
      },
  });
}

}  // namespace

View App() {
  const WindowHandle window = UseWindow();
  const auto tasks = UseTaskScope();
  const Initial init = Current();
  const auto& effects = init.effects;

  std::vector<std::string> labels{"关闭"};
  labels.insert(labels.end(), effects.begin(), effects.end());

  auto themeIndex = UseState(init.theme);
  auto size = UseState(init.size);
  auto effectIndex = UseState(init.effect);
  auto radius = UseState(init.radius);
  auto status = UseState<std::string>("改动即时生效");

  const auto apply = [=] {
    const std::string theme = kThemes[themeIndex.Get() < kThemes.size() ? themeIndex.Get() : 2];
    const std::string result = Run(std::string("python3 ") + HelperPath() +
                                   " --set --theme " + theme +
                                   " --size " + std::to_string(static_cast<int>(size.Get())) +
                                   " --effect " + EffectName(effects, effectIndex.Get()) +
                                   " --radius " + std::to_string(static_cast<int>(radius.Get())) + " 2>&1");
    status = result.empty() || result.rfind("config: ok", 0) == 0 ? "已应用" : result;
  };

  const auto followNiri = [=] {
    themeIndex = std::size_t{2};
    size = 24.0F;
    effectIndex = std::size_t{0};
    radius = 96.0F;
    apply();
  };

  g_toggle = [window] {
    static bool visible = true;
    if (visible) {
      window.Hide();
    } else {
      window.Show();
      window.Activate();
    }
    visible = !visible;
  };
  g_follow = followNiri;

  constexpr std::size_t columnCount = 4;
  std::vector<View> columns[4];
  for (std::size_t i = 0; i < labels.size(); ++i) {
    columns[i % columnCount].push_back(
        RadioButton(ShortLabel(labels[i]), effectIndex.Get() == i).With(FontSize(12.0F))
            .OnChanged([effectIndex, i, apply](bool on) mutable {
              if (on) {
                effectIndex = i;
                apply();
              }
            }));
  }
  std::vector<View> grid;
  for (std::size_t c = 0; c < columnCount; ++c) {
    grid.push_back(Column(std::move(columns[c])).With(Spacing(0.0F)));
  }

  return FlatTheme{Column{
          Row{
              Text("PRTS 光标", TextRole::Title),
              Spacer().With(Grow()),
          }.With(Spacing(6.0F), WindowDragRegion{}),
          Divider(),
          Row{
              Text("主题", TextRole::Label),
              SegmentedButton(std::vector<StringVariant>(kThemeLabels.begin(), kThemeLabels.end()), themeIndex)
                  .OnChanged([themeIndex, apply](std::size_t next) mutable {
                    themeIndex = next;
                    apply();
                  }),
          }.With(Spacing(10.0F)),
          Row{
              Text("大小", TextRole::Label),
              Text::Format("{} px", static_cast<int>(size.Get())),
          }.With(Spacing(6.0F)),
          Slider(size).Range(16.0F, 72.0F).Step(1.0F)
              .OnCommitted([size, apply](float next) mutable {
                size = next;
                apply();
              }),
          Row{
              Text("光效半径", TextRole::Label),
              Text::Format("{}", static_cast<int>(radius.Get())),
          }.With(Spacing(6.0F)),
          Slider(radius).Range(32.0F, 256.0F).Step(8.0F)
              .OnCommitted([radius, apply](float next) mutable {
                radius = next;
                apply();
              }),
          Text("光标光效（单选）", TextRole::Label),
          Row(std::move(grid)).With(Spacing(14.0F)),
          Row{
              Button("跟随 Niri").OnClick(followNiri),
              Text(status.Get(), TextRole::Label),
          }.With(Spacing(12.0F)),
      }.With(Padding(14.0F), Spacing(6.0F), Background(Color::White()), CornerRadius(12.0F),
             Border{.color = Color::Rgb(226, 228, 236), .width = 1.0F})};

}

const Application application{
    App,
    {.window = {.title = "PRTS 光标", .initial_size = {640.0F, 560.0F}, .minimum_size = Size{640.0F, 560.0F},
                  .chrome_mode = WindowChromeMode::Custom, .title_bar_height = 46.0F},
     .application_hooks = {InstallTray}},
};
