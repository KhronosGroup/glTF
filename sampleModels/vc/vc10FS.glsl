precision highp float;
varying vec3 v_normal;
uniform vec3 u_light0Color;
uniform vec3 u_light1Color;
varying vec3 v_light1Direction;
uniform float u_shininess;
uniform vec3 u_light2Color;
varying vec3 v_light2Direction;
uniform vec4 u_ambient;
uniform vec4 u_diffuse;
uniform vec4 u_reflective;
uniform vec4 u_specular;
void main(void) {
vec3 normal = normalize(v_normal);
vec4 color = vec4(0., 0., 0., 0.);
vec4 diffuse = vec4(0., 0., 0., 1.);
vec3 diffuseLight = vec3(0., 0., 0.);
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
ambient = u_ambient;
diffuse = u_diffuse;
reflective = u_reflective;
specular = u_specular;
diffuse.xyz += reflective.xyz;
ambient.xyz *= ambientLight;
color.xyz += ambient.xyz;
specular.xyz *= specularLight;
color.xyz += specular.xyz;
diffuse.xyz *= diffuseLight;
color.xyz += diffuse.xyz;
gl_FragColor = vec4(color.rgb * diffuse.a, diffuse.a);
}
