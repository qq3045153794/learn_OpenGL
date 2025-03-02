#include "utils/File.h"

#include "core/Log.h"
namespace utils {
std::string File::read_file(const std::string& file_path) {
  std::string out;
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    file.open(file_path);
    std::stringstream file_stream;
    file_stream << file.rdbuf();
    out = file_stream.str();
    file.close();
  } catch (std::ifstream::failure& e) {
    file.close();
    CORE_ERROR("ERROR::SHADER::FILE_NO_SUCCESFULLY_READ");
  } catch (std::exception& e) {
    file.close();
    CORE_ERROR("Read file failure..");
  }
  return out;
}

void File::write_yml_file(const std::string& file_path, YAML::Emitter& out) {

  std::ofstream outfile(file_path);
  outfile << out.c_str();
  outfile.close();
}

void File::parser_yml_file(const std::string& file_path, YAML::Node& doc) {
  std::ifstream fin(file_path.c_str());
  doc = YAML::Load(fin);
}

}  // namespace utils
