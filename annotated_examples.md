# Annotated Example

## The Scene

## The Shader Source Code

The following example shows the GLSL source code for a Phong shader program that will be referenced by the example JSON used throughout this section. The vertex shader computes the screen space position of each vertex, using the world space transform defined in `u_modelViewMatrix` and the perspective projection defined in `u_projectionMatrix`. 

In addition, the shader computes the varying values `v_normal`, `v_texcoord0` and `v_light0Direction` that will be used in the fragment shader. The normal vector and light direction are computed using matrices that are based on the state of the current camera and lights. The runtime determines how to pass these values based on semantics defined in the associated technique.

```
precision highp float;
attribute vec3 a_position;
attribute vec3 a_normal;
varying vec3 v_normal;
uniform mat3 u_normalMatrix;
uniform mat4 u_modelViewMatrix;
uniform mat4 u_projectionMatrix;
attribute vec2 a_texcoord0;
varying vec2 v_texcoord0;
varying vec3 v_light0Direction;
uniform mat4 u_light0Transform;
void main(void) {
vec4 pos = u_modelViewMatrix * vec4(a_position,1.0);
v_normal = u_normalMatrix * a_normal;
v_texcoord0 = a_texcoord0;
v_light0Direction = mat3(u_light0Transform) * vec3(0.,0.,1.);
gl_Position = u_projectionMatrix * pos;
}
```

The fragment shader computes the fragment color using the varying values `v_normal`, `v_texcoord0` and `v_light0Direction` computed by the vertex shader, as well as several material properties defined as uniform variables, including ambient color, emissive color and a diffuse texture.

```
precision highp float;
varying vec3 v_normal;
uniform vec4 u_ambient;
varying vec2 v_texcoord0;
uniform sampler2D u_diffuse;
uniform vec4 u_emission;
uniform vec4 u_specular;
uniform float u_shininess;
varying vec3 v_light0Direction;
uniform vec3 u_light0Color;
void main(void) {
vec3 normal = normalize(v_normal);
vec4 color = vec4(0., 0., 0., 0.);
vec4 diffuse = vec4(0., 0., 0., 1.);
vec3 diffuseLight = vec3(0., 0., 0.);
vec4 emission;
vec4 ambient;
vec4 specular;
ambient = u_ambient;
diffuse = texture2D(u_diffuse, v_texcoord0);
emission = u_emission;
specular = u_specular;
vec3 specularLight = vec3(0., 0., 0.);
{
float specularIntensity = 0.;
float attenuation = 1.0;
vec3 l = normalize(v_light0Direction);
vec3 h = normalize(l+vec3(0.,0.,1.));
specularIntensity = max(0., pow(max(dot(normal,h), 0.) , u_shininess)) * attenuation;
specularLight += u_light0Color * specularIntensity;
diffuseLight += u_light0Color * max(dot(normal,l), 0.) * attenuation;
}
specular.xyz *= specularLight;
color.xyz += specular.xyz;
diffuse.xyz *= diffuseLight;
color.xyz += diffuse.xyz;
color.xyz += emission.xyz;
color = vec4(color.rgb * diffuse.a, diffuse.a);
gl_FragColor = color;
}

```


