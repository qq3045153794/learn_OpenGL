/*
 * @Author       : xietao
 * @Date         : 2022-12-29 14:09:16
 * @LastEditors  : xietao
 * @LastEditTime : 2023-01-06 16:30:24
 * @FilePath     : TinyRenderer/src/component/Material.h
 * @Description  :
 *
 * Copyright (c) 2022, All Rights Reserved.
 */
#ifndef _SRC_COMPONENT_MATERIAL_H_
#define _SRC_COMPONENT_MATERIAL_H_

#include <scene/SerializeEntity.h>

#include <any>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

#include "asset/Buffer.h"
#include "asset/Shader.h"
#include "asset/Texture.h"
#include "core/Log.h"

namespace component {

template <typename T>
class UboData {
 public:
  UboData() = default;
  UboData(const std::string& name, std::shared_ptr<::asset::Shader> shader, const T& data)
      : m_name(name), m_shader(shader), m_data(data), m_array_ptr(nullptr) {}

  UboData(const std::string& name, std::shared_ptr<::asset::Shader> shader, const std::vector<T>* data)
      : m_name(name), m_shader(shader), m_array_ptr(data) {}
  ~UboData() = default;
  void bind() const {
    if (!m_array_ptr) {
      m_shader->bind();
      m_shader->set_uniform(m_name.c_str(), m_data);
    } else {
      m_shader->bind();
      std::size_t array_length = (*m_array_ptr).size();
      for (std::size_t i = 0U; i < array_length; i++) {
        std::string label = m_name + "[" + std::to_string(i) + "]";
        m_shader->set_uniform(label.c_str(), (*m_array_ptr)[i]);
      }
    }
  }

  std::string m_name;
  std::shared_ptr<::asset::Shader> m_shader{nullptr};
  T m_data;
  const std::vector<T>* m_array_ptr{nullptr};
};

class Material {
 public:
  friend ::scene::SerializeObject;
  // 引用 : https://zhuanlan.zhihu.com/p/260973533
  enum class pbr_u : uint16_t {
    albedo = 0U,     // 反射率 向量 sRGB 主要体现纹理和颜色
    metalness = 1U,  // 金属度 标量 [0..1]
    roughness = 2U,  // 粗糙度 标量 [0..1]
    ao = 3U,         // 反射率 标量 [0..1]
    specular = 4U
  };

  // 纹理采样数据
  enum class pbr_t : uint16_t {
    albedo = 5U,     // 反射率 向量 sRGB 主要体现纹理和颜色
    metalness = 6U,  // 金属度 标量 [0..1]
    roughness = 7U,  // 粗糙度 标量 [0..1]
    ao = 8U,         // 反射率 标量 [0..1]
    normal = 9U,     // 法线贴图 向量 切线空间
    irradiance_map = 2U,
    prefilter_map = 3U,
    brdf_LUT_map = 4U
  };

  enum class default_t : uint16_t { image_1 = 1U, image_2 = 2U };

  enum class ShadingModel : uint16_t {
    COSTEM = 0U,
    DEFAULT = 1U,  // 贴图
    PBR = 2U,      // PBR光照模型
  };

  using ubo_variant =
      std::variant<UboData<int>, UboData<bool>, UboData<GLfloat>, UboData<GLuint>, UboData<glm::vec2>,
                   UboData<glm::vec3>, UboData<glm::vec4>, UboData<glm::mat2>, UboData<glm::mat3>, UboData<glm::mat4>>;

  static std::map<GLuint, std::string> uniform_dictionary;
  static std::map<GLuint, std::string> texture_dictionary;

  Material(std::shared_ptr<asset::Shader> shader);
  Material(ShadingModel shadering_model);
  void set_texture(GLuint u_id, std::shared_ptr<asset::Texture> texture);
  void bind() const;
  void ubind() const;
  std::shared_ptr<asset::Shader> m_shader;
  void set_shader(std::shared_ptr<asset::Shader> shader);

  template <typename T>
  void set_uniform(const GLchar* name, const T& val) {
    m_shader->bind();
    m_shader->set_uniform(name, val);
  }

  template <typename T>
  void set_bound_arrary(const GLchar* name, GLuint size, const std::vector<T>* array_ptr) {
    // 设置骨骼代表有动画
    set_uniform(200U, true);
    m_uniforms_cache[name] = UboData<T>(name, m_shader, array_ptr);
  }

  template <typename T>
  void set_uniform(GLuint uid, const T& val) {
    m_shader->bind();
    if (uniform_dictionary.find(uid) != uniform_dictionary.end()) {
      m_shader->set_uniform(uniform_dictionary[uid].c_str(), val);

      m_uniforms_cache[uniform_dictionary[uid]] = UboData<T>(uniform_dictionary[uid], m_shader, val);
    } else {
      CORE_ERROR("Can't find valid uniform (uid = {})", uid);
    }
  }

  void bind_texture(pbr_t pbr, std::shared_ptr<asset::Texture> texture);

  template <typename T>
  void bind_uniform(pbr_u pbr, const T& val) {
    GLuint uid = static_cast<GLuint>(pbr);
    set_uniform(uid, val);
  }

  ubo_variant get_uniform(pbr_u pbr);
  ubo_variant get_uniform(const std::string& key);
  std::shared_ptr<::asset::Texture> get_texture(GLuint uid);
  std::shared_ptr<::asset::Texture> get_texture(pbr_t pbr);

 public:
  ShadingModel m_shading_model{ShadingModel::COSTEM};

 private:
  std::unordered_map<GLuint, std::shared_ptr<asset::Texture>> m_textures;
  std::unordered_map<std::string, ubo_variant> m_uniforms_cache;
};

}  // namespace component

#endif
