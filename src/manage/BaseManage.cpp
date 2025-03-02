#include <manage/BaseManage.h>
#include <utils/File.h>

namespace saber {

const std::vector<std::filesystem::path>& BaseManage::GetResource() const { return m_resource_storage; }

bool BaseManage::CheckResExit(const std::filesystem::path& flie_path) {
  return m_resource_register.count(flie_path) > 0;
}

void BaseManage::Serialize(const std::filesystem::path& file_name) {
  YAML::Emitter out;
  out << YAML::BeginSeq;
  for (const auto& path : m_resource_storage) {
    if (m_resource_register.count(path) > 0) {
      const auto& meta_path = m_resource_register.at(path);
      out << path.string();
      out << meta_path.string();
    } else {
      CORE_ERROR("Unknown error..");
    }
  }
  out << YAML::EndSeq;

  ::utils::File::write_yml_file(file_name.string(), out);
}

void BaseManage::DeSerialize(const std::filesystem::path& file_name) {
  // YAML::Parser parser;
  YAML::Node doc;
  ::utils::File::parser_yml_file(file_name.c_str(), doc);

  for (std::size_t i = 0; i < doc.size(); i += 2) {
    const auto& path = doc[i].as<std::string>();
    const auto& meta_path = doc[i + 1].as<std::string>();
    m_resource_storage.emplace_back(path);
    m_resource_register[path] = meta_path;
  }
}

bool BaseManage::CheckPathExit() {
  // 检查内存内文件在磁盘内是否存在
  // false 不存在 出错
  // true 存在
  for (auto& origin_path : m_resource_storage) {
    CORE_ASERT(m_resource_register.count(origin_path) > 0, "Unexpect Error");
    auto& meta_path = m_resource_register.at(origin_path);
    if (!std::filesystem::exists(origin_path)) {
      return false;
    }
    if (!std::filesystem::exists(meta_path)) {
      return false;
    }
  }
  return true;
}

}  // namespace saber
