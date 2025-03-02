#ifndef _SRC_SYSTEM_ENVIRONMENT_SYSTEM_H_
#define _SRC_SYSTEM_ENVIRONMENT_SYSTEM_H_

#include <system/System.h>
namespace saber {
namespace system {
class EnvironmentSystem : public System {
public:
  EnvironmentSystem() = default;
  EnvironmentSystem(scene::Scene* scene);
  virtual ~EnvironmentSystem() = default;

  virtual void Awake() override;
  virtual void OnUpdateRuntime() override;
  virtual void OnEditorRumtime(scene::Entity& editor_camera) override;
private:
  void SetIBL();
  void SetUBO();
};

}  // namespace system
}  // namespace saber
#endif
