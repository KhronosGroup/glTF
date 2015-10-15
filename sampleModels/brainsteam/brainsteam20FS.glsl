precision highp float;
varying vec3 v_normal;
varying vec4 v_joint;
varying vec4 v_weight;
uniform vec4 u_diffuse;
uniform vec4 u_emission;
void main(void) {
vec3 normal = normalize(v_normal);
vec4 color = vec4(0., 0., 0., 0.);
vec4 diffuse = vec4(0., 0., 0., 1.);
vec4 emission;
diffuse = u_diffuse;
emission = u_emission;
diffuse.xyz *= max(dot(normal,vec3(0.,0.,1.)), 0.);
color.xyz += diffuse.xyz;
color.xyz += emission.xyz;
color = vec4(color.rgb * diffuse.a, diffuse.a);
gl_FragColor = color;
}
