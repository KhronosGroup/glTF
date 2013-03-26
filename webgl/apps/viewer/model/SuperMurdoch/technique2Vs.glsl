precision highp float;
attribute vec3 a_normal;
uniform mat3 u_normalMatrix;
varying vec3 v_normal;
attribute vec3 a_position;
uniform mat4 u_worldviewMatrix;
uniform mat4 u_projectionMatrix;
void main(void) {
v_normal = normalize(u_normalMatrix * a_normal);
gl_Position = u_projectionMatrix * u_worldviewMatrix * vec4(a_position,1.0);
}
