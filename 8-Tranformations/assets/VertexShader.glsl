
precision highp float;

uniform mat4 uTransform;

attribute vec3 aPos;
attribute vec2 aTexCoord;

varying vec2 vTexCoord;

void main()
{
	gl_Position = uTransform *  vec4(aPos, 1.0);
	vTexCoord = aTexCoord;
}