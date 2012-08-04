precision highp float;
 varying vec3 v_normal;
 varying vec2 v_texcoord;
 uniform sampler2D u_diffuseTexture;
 
 void main(void) { 
 vec3 normal = normalize(v_normal);
 float lambert = max(dot(normal,vec3(0.,0.,1.)), 0.);
 vec4 color = texture2D(u_diffuseTexture, v_texcoord);
 gl_FragColor = vec4(color.rgb * lambert, 1.); 
 }