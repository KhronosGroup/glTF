precision highp float;
attribute vec3 position;
attribute vec3 normal;
varying vec3 v_normal;
uniform mat3 u_normalMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
attribute vec2 uv;
varying vec2 v_texcoord0;
varying vec3 v_light0Direction;
uniform mat4 u_light0Transform;
void main(void) {
vec4 pos = modelViewMatrix * vec4(position,1.0);
v_normal = u_normalMatrix * normal;
v_texcoord0 = uv;
v_light0Direction = mat3(u_light0Transform) * vec3(0.,0.,1.);
gl_Position = projectionMatrix * pos;
}
