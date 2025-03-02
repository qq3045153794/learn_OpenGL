/*
 * @Author       : xietao
 * @Date         : 2022-09-14 14:38:12
 * @LastEditors  : xietao
 * @LastEditTime : 2022-12-27 17:08:24
 * @FilePath     : TinyRender/src/asset/Texture.h
 * @brief  :
 *
 * Copyright (c) 2022, All Rights Reserved.
 */
#ifndef _SRC_ASSET_TEXTURE_H_
#define _SRC_ASSET_TEXTURE_H_

#include <memory>
#include <string>
#include <vector>

#include "glad/glad.h"
#include <filesystem>
#include  <optional>

namespace asset {
class Texture {
 public:
  Texture(const std::vector<std::string>& path_vec, GLuint levels = 0);
  Texture(const GLchar* path, bool flip = false, GLuint levels = 0);
  Texture(GLenum target, GLuint width, GLuint height, GLuint i_format,
          GLuint levels = 0);
  Texture(const GLchar* path, GLint resolution, GLuint levels = 0);
  ~Texture();
  GLuint get_id() const;
  GLuint Width() const;
  GLuint Height() const;
  void bind(GLuint unit) const;
  void ubind(GLuint unit) const;

 private:
  void set_sampler_state() const;
  GLuint m_id;
  GLuint m_width, m_height;
  GLuint m_internal_format;
  GLuint m_image_format;
  GLuint m_target;
 public:
  GLuint m_levels = 0;
  bool m_flip;
  std::optional<std::filesystem::path> m_image_path;
};

}  // namespace asset

#endif
