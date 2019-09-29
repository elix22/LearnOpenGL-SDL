
attribute vec3 aPos;
attribute vec3 aColor;

varying vec3 vColor;
void main()
{
	gl_Position = vec4(aPos, 1.0);
	vColor = aColor;
}