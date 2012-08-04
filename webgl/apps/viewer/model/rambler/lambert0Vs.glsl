precision highp float;
 attribute vec3 vert;
 attribute vec3 normal;
 varying vec3 v_normal;
 uniform mat4 u_mvMatrix;
 uniform mat3 u_normalMatrix;
 uniform mat4 u_projMatrix;
 
 void main(void) {
 v_normal = normalize(u_normalMatrix * normal);
 gl_Position = u_projMatrix * u_mvMatrix * vec4(vert,1.0);
 }