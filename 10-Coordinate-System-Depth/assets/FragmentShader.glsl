
precision mediump float;


uniform sampler2D texture1;
uniform sampler2D texture2;

varying vec2 vTexCoord;

void main()
{
	gl_FragColor = mix(texture2D(texture1, vTexCoord), texture2D(texture2, vTexCoord), 0.2);
}