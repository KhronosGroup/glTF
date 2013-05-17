precision highp float;
varying vec3 v_normal;
varying vec2 v_texcoord0;
uniform sampler2D u_diffuseTexture;
void main(void) {
vec3 normal = normalize(v_normal);
float lambert = max(dot(normal,vec3(0.,0.,1.)), 0.);
vec4 color = vec4(0., 0., 0., 0.);
color = color + texture2D(u_diffuseTexture, v_texcoord0) * vec4(lambert,lambert,lambert,1.);
gl_FragColor = vec4(color.rgb * color.a, color.a);
}
