precision highp float;
varying vec3 v_normal;
uniform vec4 u_diffuse;
uniform vec4 u_specular;
uniform float u_shininess;
uniform vec3 u_light0Color;
varying vec3 v_light1Direction;
varying vec3 v_position;
uniform vec3 u_light1Color;
void main(void) {
vec3 normal = normalize(v_normal);
vec4 color = vec4(0., 0., 0., 0.);
vec4 diffuse = vec4(0., 0., 0., 1.);
vec3 diffuseLight = vec3(0., 0., 0.);
vec4 specular;
diffuse = u_diffuse;
specular = u_specular;
vec3 specularLight = vec3(0., 0., 0.);
vec3 ambientLight = vec3(0., 0., 0.);
{
ambientLight += u_light0Color;
}
{
float specularIntensity = 0.;
float attenuation = 1.0;
vec3 l = normalize(v_light1Direction);
vec3 viewDir = -normalize(v_position);
float phongTerm = max(0.0, dot(reflect(-l,normal), viewDir));
specularIntensity = max(0., pow(phongTerm , u_shininess)) * attenuation;
specularLight += u_light1Color * specularIntensity;
diffuseLight += u_light1Color * max(dot(normal,l), 0.) * attenuation;
}
specular.xyz *= specularLight;
color.xyz += specular.xyz;
diffuse.xyz *= diffuseLight;
color.xyz += diffuse.xyz;
color = vec4(color.rgb * diffuse.a, diffuse.a);
gl_FragColor = color;
}
