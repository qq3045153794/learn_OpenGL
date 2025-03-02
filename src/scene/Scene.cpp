
#include "scene/Scene.h"

#include <system/EnvironmentSystem.h>
#include <system/RenderSystem.h>

#include "core/Log.h"
#include "scene/RenderCommand.h"
#include "scene/ui.h"

namespace scene {

Scene::Scene() {
  // 注意顺序
  m_systems.push_back(std::make_shared<saber::system::EnvironmentSystem>(this));
  m_systems.push_back(std::make_shared<saber::system::RenderSystem>(this));
}

Scene::Scene(const std::string& title) : m_title(title) {
  // 注意顺序
  m_systems.push_back(std::make_shared<saber::system::EnvironmentSystem>(this));
  m_systems.push_back(std::make_shared<saber::system::RenderSystem>(this));
}

Scene::~Scene() {
  CORE_INFO("call ~Scene");
  registry.each([this](auto id) { CORE_TRACE("Destroying entity: {0}", directory.at(id)); });
  registry.clear();
}

Entity Scene::create_entity(const std::string& name, component::ETag tag) {
  Entity e = {name, registry.create(), &registry};
  e.AddComponent<component::Transform>();
  e.AddComponent<component::Tag>(tag, name);
  directory.emplace(e.id, e.name);
  return e;
}


void Scene::delete_entity(const entt::entity& id) {
  auto find_item = std::find(render_queue.begin(), render_queue.end(), id);
  if (find_item != render_queue.end()) {
    render_queue.erase(find_item);
  }
  registry.destroy(id);
}

void Scene::registry_shader(GLuint shader_id) { shader_id_set.insert(shader_id); }

void Scene::exclude_entity(const entt::entity& id) { exclude_entitys.insert(id); }

bool Scene::is_exclude_entity(const entt::entity& id) { return exclude_entitys.count(id); }

void Scene::Awake() {
  for (auto& system : m_systems) {
    system->Awake();
  }
}

void Scene::OnUpdateRuntime() {
  for (auto& system : m_systems) {
    system->OnUpdateRuntime();
  }
}

void Scene::OnEditorRumtime(::scene::Entity& editor_camera) {
  for (auto& system : m_systems) {
    system->OnEditorRumtime(editor_camera);
  }
}

}  // namespace scene
