precision highp float;
attribute vec3 a_position;
attribute vec3 a_normal;
varying vec3 v_normal;
uniform mat3 u_normalMatrix;
uniform mat4 u_modelViewMatrix;
uniform mat4 u_projectionMatrix;
varying vec2 v_texcoord0;
varying vec3 v_light1Direction;
varying vec3 v_position;
uniform mat4 u_light1Transform;
varying vec3 v_light2Direction;
uniform mat4 u_light2Transform;
void main(void) {
vec4 pos = u_modelViewMatrix * vec4(a_position,1.0);
v_normal = u_normalMatrix * a_normal;
vec3 normalizedVert = normalize(vec3(pos));
vec3 r = reflect(normalizedVert, v_normal);
r.z += 1.0;
float m = 2.0 * sqrt(dot(r,r));
v_texcoord0 = (r.xy / m) + 0.5;
v_position = pos.xyz;
v_light1Direction = u_light1Transform[3].xyz - pos.xyz;
v_light2Direction = u_light2Transform[3].xyz - pos.xyz;
gl_Position = u_projectionMatrix * pos;
}
