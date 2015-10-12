precision highp float;
varying vec3 v_normal;
varying vec4 v_joint;
varying vec4 v_weight;
uniform vec3 u_light0Color;
uniform vec4 u_ambient;
uniform vec4 u_diffuse;
uniform vec4 u_emission;
uniform vec4 u_reflective;
uniform vec4 u_specular;
void main(void) {
vec3 normal = normalize(v_normal);
vec4 color = vec4(0., 0., 0., 0.);
vec4 diffuse = vec4(0., 0., 0., 1.);
vec3 diffuseLight = vec3(0., 0., 0.);
vec4 emission;
vec4 reflective;
vec4 ambient;
vec3 ambientLight = vec3(0., 0., 0.);
vec4 specular;
vec3 specularLight = vec3(0., 0., 0.);
{
float diffuseIntensity;
float specularIntensity;
ambientLight += u_light0Color;
}
ambient = u_ambient;
diffuse = u_diffuse;
emission = u_emission;
reflective = u_reflective;
specular = u_specular;
diffuse.xyz += reflective.xyz;
ambient.xyz *= ambientLight;
color.xyz += ambient.xyz;
specular.xyz *= specularLight;
color.xyz += specular.xyz;
diffuse.xyz *= diffuseLight;
color.xyz += diffuse.xyz;
color.xyz += emission.xyz;
gl_FragColor = vec4(color.rgb * diffuse.a, diffuse.a);
}
