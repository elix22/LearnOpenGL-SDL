
precision highp float;

attribute vec3 aPos;
attribute vec2 aTexCoord;

varying vec2 vTexCoord;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	vTexCoord = aTexCoord;
}