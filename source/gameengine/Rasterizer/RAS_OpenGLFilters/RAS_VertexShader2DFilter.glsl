in vec4 filterPosition;
in vec4 filterTexCoord;
out vec4 bgl_TexCoord;

void main(void)
{
	gl_Position = filterPosition;
	bgl_TexCoord = filterTexCoord;
}

