precision highp float;
attribute vec3 a_position;
attribute vec3 a_normal;
varying vec3 v_normal;
uniform mat3 u_normalMatrix;
uniform mat4 u_worldViewMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_light1Transform;
varying vec3 v_light1Direction;
uniform mat4 u_light2Transform;
varying vec3 v_light2Direction;
void main(void) {
vec4 pos = u_worldViewMatrix * vec4(a_position,1.0);
v_normal = normalize(u_normalMatrix * a_normal);
v_light1Direction = normalize(mat3(u_light1Transform) * vec3(0.,0.,1.));
v_light2Direction = normalize(mat3(u_light2Transform) * vec3(0.,0.,1.));
gl_Position = u_projectionMatrix * pos;
}
