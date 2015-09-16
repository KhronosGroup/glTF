precision highp float;
varying vec3 v_normal;
uniform vec3 u_light0Color;
uniform vec3 u_light1Color;
varying vec3 v_light1Direction;
uniform float u_shininess;
uniform vec3 u_light2Color;
varying vec3 v_light2Direction;
varying vec2 v_texcoord0;
uniform sampler2D u_ambient;
uniform sampler2D u_diffuse;
uniform vec4 u_emission;
varying vec2 v_texcoord1;
uniform sampler2D u_reflective;
uniform vec4 u_specular;
uniform float u_transparency;
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
{
float diffuseIntensity;
float specularIntensity;
vec3 l = normalize(v_light1Direction);
vec3 h = normalize(l+vec3(0.,0.,1.));
diffuseIntensity = max(dot(normal,l), 0.);
specularIntensity = pow(max(0.0,dot(normal,h)),u_shininess);
specularLight += u_light1Color * specularIntensity;
diffuseLight += u_light1Color * diffuseIntensity;
}
{
float diffuseIntensity;
float specularIntensity;
vec3 l = normalize(v_light2Direction);
vec3 h = normalize(l+vec3(0.,0.,1.));
diffuseIntensity = max(dot(normal,l), 0.);
specularIntensity = pow(max(0.0,dot(normal,h)),u_shininess);
specularLight += u_light2Color * specularIntensity;
diffuseLight += u_light2Color * diffuseIntensity;
}
ambient = texture2D(u_ambient, v_texcoord0);
diffuse = texture2D(u_diffuse, v_texcoord0);
emission = u_emission;
reflective = texture2D(u_reflective, v_texcoord1);
specular = u_specular;
diffuse.xyz += reflective.xyz;
ambient.xyz *= ambientLight;
color.xyz += ambient.xyz;
specular.xyz *= specularLight;
color.xyz += specular.xyz;
diffuse.xyz *= diffuseLight;
color.xyz += diffuse.xyz;
color.xyz += emission.xyz;
gl_FragColor = vec4(color.rgb * diffuse.a, diffuse.a * u_transparency);
}
