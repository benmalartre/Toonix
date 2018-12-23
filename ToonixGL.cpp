//---------------------------------------------------------
//		Toonix OpenGL .cpp
//---------------------------------------------------------
#include "ToonixGL.h"

bool IsGLExtensionSupported(const char* extName)
{
	/*
	Search for extName in the extensions string.  Use of strstr()
	is not sufficient because extension names can be prefixes of
	other extension names.  Could use strtok() but the constant
	string returned by glGetString can be in read-only memory.
	char *p = (char *) glGetString(GL_EXTENSIONS);
	char *end;
	size_t extNameLen;

	extNameLen = strlen(extName);
	end = p + strlen(p);

	while (p < end)
	{
		size_t n = strcspn(p, " ");
		if ((extNameLen == n) && (strncmp(extName, p, n) == 0))
		{
			return true;
		}
		p += (n + 1);
	}

	return false;
	*/
	GL3WglProc ext = gl3wGetProcAddress(extName);
	if (ext)return true;
	else return false;
}