

void main()
{
#if defined(RED_COLOR)
	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
#elif defined(BLUE_COLOR)
	gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
#else
	gl_FragColor = vec4(1.0, 0.5, 0.8, 1.0);
#endif

}