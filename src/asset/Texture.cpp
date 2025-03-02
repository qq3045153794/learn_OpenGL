#include "asset/Texture.h"

#include <iostream>
#include <memory>

#include "asset/FBO.h"
#include "core/Debug.h"
#include "core/Log.h"
#include "stb_image.h"
#include "utils/image.h"
namespace asset {

Texture::Texture(const GLchar* img_path, bool flip, GLuint levels)
    : m_target(GL_TEXTURE_2D), m_levels(levels), m_flip(flip), m_image_path(img_path) {
  const auto& image = utils::Image(img_path, flip);
  m_width = image.get_width();
  m_height = image.get_height();
  m_image_format = image.get_img_format();
  m_internal_format = image.get_ine_format();
  glGenTextures(1, &m_id);
  glBindTexture(m_target, m_id);

  if (image.is_hdr()) {
    glTexImage2D(m_target, 0, m_internal_format, m_width, m_height, 0, m_image_format, GL_FLOAT, image.get_buffer());
  } else {
    glTexImage2D(m_target, 0, m_internal_format, m_width, m_height, 0, m_image_format, GL_UNSIGNED_BYTE,
                 image.get_buffer());
  }

  set_sampler_state();
  CORE_INFO("Texture 2D (id = {})", m_id);
}

Texture::Texture(const std::vector<std::string>& path_vec, GLuint levels)
    : m_target(GL_TEXTURE_CUBE_MAP), m_levels(levels) {
  glGenTextures(1, &m_id);
  glBindTexture(m_target, m_id);

  int idx = 0;
  for (const auto& img_path : path_vec) {
    const auto& image = utils::Image(img_path);
    m_width = image.get_width();
    m_height = image.get_height();
    m_image_format = image.get_img_format();
    m_internal_format = image.get_ine_format();

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + idx, 0, m_internal_format, m_width, m_height, 0, m_image_format,
                 GL_UNSIGNED_BYTE, image.get_buffer());
    ++idx;
  }

  set_sampler_state();
}

Texture::Texture(const GLchar* path, int resolution, GLuint levels) : m_target(GL_TEXTURE_CUBE_MAP), m_levels(levels) {
  // 加载hdr图片 转换时候转换成cubemap 注意hdr图像加载进texture为float类型
  // (不会进行归一化) 转换成的cubemap也是float

  const auto& image = utils::Image(path);
  m_width = image.get_width();
  m_height = image.get_height();
  m_image_format = image.get_img_format();
  m_internal_format = image.get_ine_format();
  // 创建 hdr 2D texture
  GLuint hdr_id;
  glGenTextures(1, &hdr_id);
  glBindTexture(GL_TEXTURE_2D, hdr_id);
  glTexImage2D(GL_TEXTURE_2D, 0, m_internal_format, m_width, m_height, 0, m_image_format, GL_FLOAT, image.get_buffer());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  auto cubemap_fbo = std::make_unique<FBO>(resolution, resolution);

  cubemap_fbo->set_depth_texture();
  const size_t faces = 6;
  // 创建 cube_texture_map
  glGenTextures(1, &m_id);
  glBindTexture(m_target, m_id);
  for (size_t i = 0; i < faces; i++) {
    // 我们存储的是float格式 由于采样的纹理是hdr
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, resolution, resolution, 0, GL_RGB, GL_FLOAT,
                 nullptr);
  }
  glBindTexture(m_target, 0);
  set_sampler_state();

  const float kFOV = 90.0F;
  const float kAspect = 1.0F;
  const float kZNear = 0.1F;
  const float kZFar = 100.F;
  glm::mat4 proj = glm::perspective(glm::radians(kFOV), kAspect, kZNear, kZFar);

  glm::mat4 views[] = {
      glm::lookAt(glm::vec3(0.0F, 0.0F, 0.0F), glm::vec3(1.0F, 0.0F, 0.0F), glm::vec3(0.0F, -1.0F, 0.0F)),
      glm::lookAt(glm::vec3(0.0F, 0.0F, 0.0F), glm::vec3(-1.0F, 0.0F, 0.0F), glm::vec3(0.0F, -1.0F, 0.0F)),
      glm::lookAt(glm::vec3(0.0F, 0.0F, 0.0F), glm::vec3(0.0F, 1.0F, 0.0F), glm::vec3(0.0F, 0.0F, 1.0F)),
      glm::lookAt(glm::vec3(0.0F, 0.0F, 0.0F), glm::vec3(0.0F, -1.0F, 0.0F), glm::vec3(0.0F, 0.0F, -1.0F)),
      glm::lookAt(glm::vec3(0.0F, 0.0F, 0.0F), glm::vec3(0.0F, 0.0F, 1.0F), glm::vec3(0.0F, -1.0F, 0.0F)),
      glm::lookAt(glm::vec3(0.0F, 0.0F, 0.0F), glm::vec3(0.0F, 0.0F, -1.0F), glm::vec3(0.0F, -1.0F, 0.0F))};

  // clang-format off
  // 创建cubek
  static float data[] = {
      // back face
      -1.0F,
      -1.0F,
      -1.0F,
      1.0F,
      1.0F,
      -1.0F,
      1.0F,
      -1.0F,
      -1.0F,
      1.0F,
      1.0F,
      -1.0F,
      -1.0F,
      -1.0F,
      -1.0F,
      -1.0F,
      1.0F,
      -1.0F,
      // front face
      -1.0F,
      -1.0F,
      1.0F,
      1.0F,
      -1.0F,
      1.0F,
      1.0F,
      1.0F,
      1.0F,
      1.0F,
      1.0F,
      1.0F,
      -1.0F,
      1.0F,
      1.0F,
      -1.0F,
      -1.0F,
      1.0F,
      // left face
      -1.0F,
      1.0F,
      1.0F,
      -1.0F,
      1.0F,
      -1.0F,
      -1.0F,
      -1.0F,
      -1.0F,
      -1.0F,
      -1.0F,
      -1.0F,
      -1.0F,
      -1.0F,
      1.0F,
      -1.0F,
      1.0F,
      1.0F,
      // right face
      1.0F,
      1.0F,
      1.0F,
      1.0F,
      -1.0F,
      -1.0F,
      1.0F,
      1.0F,
      -1.0F,
      1.0F,
      -1.0F,
      -1.0F,
      1.0F,
      1.0F,
      1.0F,
      1.0F,
      -1.0F,
      1.0F,
      // bottom face
      -1.0F,
      -1.0F,
      -1.0F,
      1.0F,
      -1.0F,
      -1.0F,
      1.0F,
      -1.0F,
      1.0F,
      1.0F,
      -1.0F,
      1.0F,
      -1.0F,
      -1.0F,
      1.0F,
      -1.0F,
      -1.0F,
      -1.0F,
      // top face
      -1.0F,
      1.0F,
      -1.0F,
      1.0F,
      1.0F,
      1.0F,
      1.0F,
      1.0F,
      -1.0F,
      1.0F,
      1.0F,
      1.0F,
      -1.0F,
      1.0F,
      -1.0F,
      -1.0F,
      1.0F,
      1.0F,
  };

  // clang-format on
  auto cube_vbo = std::make_unique<VBO>(sizeof(data), data, GL_STATIC_DRAW);
  auto cube_vao = std::make_unique<VAO>();

  cube_vao->set_vbo(*cube_vbo, 0U, 3U, 3U * sizeof(int), 0, GL_FLOAT);

  auto cubemap_shader =
      std::make_unique<Shader>("../resource/shader/hdr2cubemap.vs", "../resource/shader/hdr2cubemap.fs");

  cubemap_shader->bind();
  cubemap_shader->set_uniform("projection", proj);
  cubemap_shader->set_uniform("texture_0", 0);
  // 将hdr转换成cubemap 通过FBO将6个方向的渲染画面写入cubemap
  glViewport(0, 0, resolution, resolution);
  cubemap_fbo->bind();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, hdr_id);
  for (size_t i = 0; i < faces; i++) {
    cubemap_shader->set_uniform("view", views[i]);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_id, 0);

    glClearColor(1.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const uint32_t kVecticesCount = 36U;
    cube_vao->bind();
    glDrawArrays(GL_TRIANGLES, 0, kVecticesCount);
    cube_vao->ubind();
  }
  cubemap_fbo->ubind();

  glDeleteTextures(1, &hdr_id);
}

Texture::Texture(GLenum target, GLuint width, GLuint height, GLuint i_format, GLuint levels)
    : m_width(width),
      m_height(height),
      m_internal_format(i_format),
      m_image_format(i_format),
      m_target(target),
      m_levels(levels) {
  switch (m_target) {
    case GL_TEXTURE_2D: {
      glGenTextures(1, &m_id);
      glBindTexture(m_target, m_id);
      if (m_image_format == GL_RGB || m_image_format == GL_RGBA) {
        glTexImage2D(m_target, m_levels, m_internal_format, m_width, m_height, 0, m_image_format, GL_UNSIGNED_BYTE,
                     nullptr);
      } else if (m_image_format == GL_RG16F || m_image_format == GL_RG32F) {
        glTexImage2D(m_target, m_levels, m_internal_format, m_width, m_height, 0, GL_RG, GL_FLOAT, nullptr);
      } else if (m_image_format == GL_DEPTH_COMPONENT) {
        glTexImage2D(m_target, m_levels, m_internal_format, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                     nullptr);
      } else {
        CORE_ASERT(0, "Unknown err...");
      }
      glBindTexture(m_target, 0);
      break;
    }
    case GL_TEXTURE_CUBE_MAP: {
      const size_t faces = 6U;
      glGenTextures(1, &m_id);
      glBindTexture(m_target, m_id);
      for (size_t i = 0; i < faces; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_FLOAT, nullptr);
      }
      glBindTexture(m_target, 0);
      break;
    }
  }

  set_sampler_state();
}

Texture::~Texture() {
  CORE_INFO("~Texture (id : {})", m_id);
  glDeleteTextures(1, &m_id);
  CHECK_ERROR();
}

GLuint Texture::get_id() const { return m_id; }

void Texture::set_sampler_state() const {
  GLuint min_filter = m_levels > 0 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

  glBindTexture(m_target, m_id);

  if (m_image_format == GL_RG16F || m_image_format == GL_RGB16F) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else if (m_image_format == GL_DEPTH_COMPONENT) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLfloat borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  } else {
    glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  if (m_target == GL_TEXTURE_CUBE_MAP) {
    glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_REPEAT);
  }

  if (m_levels > 0) {
    glGenerateMipmap(m_target);
  }

  glBindTexture(m_target, 0);
}

void Texture::bind(GLuint unit) const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(m_target, m_id);
}

void Texture::ubind(GLuint unit) const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(m_target, 0);
}

GLuint Texture::Width() const { return m_width; }
GLuint Texture::Height() const { return m_height; }

}  // namespace asset
