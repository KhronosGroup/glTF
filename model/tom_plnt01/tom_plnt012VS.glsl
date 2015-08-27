precision highp float;
attribute vec3 a_position;
attribute vec3 a_normal;
varying vec3 v_normal;
attribute vec4 a_joint;
attribute vec4 a_weight;
uniform mat4 u_jointMat[5];
uniform mat3 u_normalMatrix;
uniform mat4 u_modelViewMatrix;
uniform mat4 u_projectionMatrix;
attribute vec2 a_texcoord0;
varying vec2 v_texcoord0;
varying vec3 v_light1Direction;
uniform mat4 u_light1Transform;
varying vec3 v_light2Direction;
uniform mat4 u_light2Transform;
void main(void) {
mat4 skinMat = a_weight.x * u_jointMat[int(a_joint.x)];
skinMat += a_weight.y * u_jointMat[int(a_joint.y)];
skinMat += a_weight.z * u_jointMat[int(a_joint.z)];
skinMat += a_weight.w * u_jointMat[int(a_joint.w)];
vec4 pos = u_modelViewMatrix * skinMat * vec4(a_position,1.0);
v_normal = u_normalMatrix * mat3(skinMat)* a_normal;
v_texcoord0 = a_texcoord0;
v_light1Direction = u_light1Transform[3].xyz - pos.xyz;
v_light2Direction = u_light2Transform[3].xyz - pos.xyz;
gl_Position = u_projectionMatrix * pos;
}
