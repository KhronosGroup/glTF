precision highp float;
uniform vec3 u_diffuseColor;
varying vec3 v_normal;
varying vec2 v_envTexcoord;
uniform sampler2D u_reflectionTexture;
uniform float u_reflectionIntensity;
uniform float u_transparency;
 varying vec4 v_vert;
uniform float u_shininess;
uniform vec3 u_light;
uniform vec3 u_specularColor;

 void main(void) { 
 	vec3 normal = normalize(v_normal);
 	float lambert = max(dot(normal,vec3(0.,0.,1.)), 0.);
 	vec4 env = texture2D( u_reflectionTexture , v_envTexcoord.xy) *  u_reflectionIntensity;

	vec3 e = normalize(v_vert.xyz);
	vec3 r = reflect(-u_light, normal);
	float specular = pow( max(dot(r, e), 0.0), u_shininess );

 	gl_FragColor = vec4((u_specularColor * specular) +  (u_diffuseColor.xyz * lambert) + env.xyz, u_transparency); 
}
