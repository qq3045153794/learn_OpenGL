#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec2 uv2;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec3 binormal;
layout(location = 6) in ivec4 bone_id;
layout(location = 7) in vec4 bone_wt;

out _vtx {
    out vec3 _position;
    out vec3 _normal;
    out vec2 _uv;
    out vec2 _uv2;
    out vec3 _tangent;
    out vec3 _binormal;
    out vec4 _frag_pos_light_space;
};

layout (std140) uniform Matrices {
    mat4 projection;
    mat4 view;
    vec4 position;
    vec4 direction;
}camera;


uniform bool is_animation;
uniform mat4 bone_transform[600];
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

mat4 CalcBoneTransform() {
  mat4 T = mat4(0.0);
  for(uint i = 0U; i < 4U; i++) {
    if(bone_id[i] >= 0) {
      T += (bone_transform[bone_id[i]] * bone_wt[i]);
    }
  }
  return T;
}


void main()
{
    mat4 BT = is_animation? CalcBoneTransform() : mat4(1.0);
    gl_Position = camera.projection * camera.view * model * BT * vec4(position, 1.0);

    _position = vec3(model * BT * vec4(position, 1.0));
    _normal   = normalize(vec3(model * BT * vec4(normal, 0.0)));
    _tangent  = normalize(vec3(model * BT * vec4(tangent, 0.0)));
    _binormal = normalize(vec3(model * BT * vec4(binormal, 0.0)));
    _uv = uv;
    _frag_pos_light_space = lightSpaceMatrix * vec4(_position, 1.0);
}
