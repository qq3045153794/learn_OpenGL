#ifndef _SRC_MANAGE_CONFIG_MANAGE_H_
#define _SRC_MANAGE_CONFIG_MANAGE_H_

#include <manage/BaseManage.h>

namespace saber {

class ConfigManage : public PublicSingleton<ConfigManage> {
 public:
  void Init();
  void Import(const std::filesystem::path& from_path, const std::filesystem::path& to_path);
  void SaveConfigSettings();
  // 暂时相对路径
  std::filesystem::path icons_path;
  std::filesystem::path config_path;
  std::filesystem::path system_texture_path;
  std::filesystem::path fonts_path;
  std::filesystem::path shader_path;
  std::filesystem::path content_path;
  std::filesystem::path content_scenes_path;

  // switch
  bool enable_shadow;
};

}  // namespace saber

#endif
