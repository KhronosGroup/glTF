precision highp float;
 attribute vec3 vert;
 attribute vec3 normal;
 attribute vec2 texcoord;
 varying vec3 v_normal;
 uniform mat4 u_mvMatrix;
 uniform mat3 u_normalMatrix;
 uniform mat4 u_projMatrix;
 varying vec2 v_envTexcoord;
 varying vec2 v_texcoord;
 varying vec4 v_vert;
 
 void main(void) {
 	vec4 vert4 = vec4(vert,1.0);
 	v_vert = u_mvMatrix * vert4;
	vec3 n = normalize( u_normalMatrix * normal );
	vec3 u = normalize( vec3(v_vert) );
 	v_normal = n;
	vec3 r = reflect( u, n );
	r.z += 1.0;
	float m = 2.0 * sqrt( dot(r,r) );
	v_envTexcoord = (r.xy / m) + 0.5;
 	v_texcoord = vec2(texcoord.x, 1. - texcoord.y);
 	gl_Position = u_projMatrix * v_vert;
 }
