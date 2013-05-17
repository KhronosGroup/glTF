precision highp float;
varying vec3 v_normal;
uniform vec3 u_diffuse;
uniform float u_transparency;
void main(void) {
vec3 normal = normalize(v_normal);
float lambert = max(dot(normal,vec3(0.,0.,1.)), 0.);
vec4 color = vec4(0., 0., 0., 0.);
color = color + vec4(u_diffuse,1.) * vec4(lambert,lambert,lambert,1.);
gl_FragColor = vec4(color.rgb * color.a, color.a * u_transparency);
}
