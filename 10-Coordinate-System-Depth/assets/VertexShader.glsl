
precision highp float;

attribute vec3 aPos;
attribute vec2 aTexCoord;

varying vec2 vTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	vTexCoord = aTexCoord;
}