#ifndef _SRC_EDITOR_CONTEN_BROWER_PANEL_H_
#define _SRC_EDITOR_CONTEN_BROWER_PANEL_H_

#include <filesystem>
#include <optional>

namespace saber {
class ContentBrowerPanel {
 public:
  ContentBrowerPanel();
  void OnImGuiRender();

 private:
  void DrawTree();
  void DrawTreeRecursive(const std::filesystem::path& current_path);
  void DrawContent();
  void DrawSave();
  void DrawSetting();
  std::filesystem::path m_current_directory;
  std::optional<std::filesystem::path> m_selected_directory;
};
}  // namespace saber

#endif
