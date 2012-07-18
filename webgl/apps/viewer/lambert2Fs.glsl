precision highp float;
 uniform vec3 u_diffuseColor;
 varying vec3 v_normal;
 uniform float u_transparency;
 
 void main(void) { 
 vec3 normal = normalize(v_normal);
 float lambert = max(dot(normal,vec3(0.,0.,1.)), 0.);
 gl_FragColor = vec4(u_diffuseColor.xyz *lambert, u_transparency); 
 }