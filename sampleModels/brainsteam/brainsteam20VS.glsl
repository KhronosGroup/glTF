precision highp float;
attribute vec3 a_position;
attribute vec3 a_normal;
varying vec3 v_normal;
attribute vec4 a_joint;
varying vec4 v_joint;
attribute vec4 a_weight;
varying vec4 v_weight;
uniform mat4 u_jointMat[60];
uniform mat3 u_normalMatrix;
uniform mat4 u_worldViewMatrix;
uniform mat4 u_projectionMatrix;
void main(void) {
mat4 skinMat = a_weight.x * u_jointMat[int(a_joint.x)];
skinMat += a_weight.y * u_jointMat[int(a_joint.y)];
skinMat += a_weight.z * u_jointMat[int(a_joint.z)];
skinMat += a_weight.w * u_jointMat[int(a_joint.w)];
vec4 pos = u_worldViewMatrix * skinMat * vec4(a_position,1.0);
v_normal = normalize(u_normalMatrix * mat3(skinMat)* a_normal);
gl_Position = u_projectionMatrix * pos;
}
