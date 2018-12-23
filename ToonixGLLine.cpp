//-------------------------------------------------
// TXGLLine.cpp
//-------------------------------------------------
#include "ToonixGLLine.h"

// VBO extension function pointers
void TXGLLine::Draw()
{
	Application().LogMessage(L" GL Line Draw Funcxtion Called...");
	if(!m_line)Application().LogMessage(L"OGL Line is invalid...");
	if(m_line == NULL)return;

	std::vector<TXChain* >::iterator c = m_line->m_chains.begin();
	std::vector<TXPoint*>::iterator p;

	GLfloat x,y,z;
	GLfloat r,g,b,a;
	//glColor3f(0.0,0.0,0.0);

	/*
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	*/
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glPointSize(32);
	//glColor3f(0,0,0);

	ULONG nbp = m_line->GetNbPoints();
	ULONG size_t = nbp*3;
	float * pos = new float[size_t];
	int id = 0;

	for(;c<m_line->m_chains.end();c++)
	{	
		// Gather Datas
		p = (*c)->m_points.begin();
		for(;p<(*c)->m_points.end();p++)
		{
			(*p)->m_pos.Get(pos[id],pos[id+1],pos[id+2]);
			id+=3;
		}
	}

	
	if(size_t)
	{
		Application().LogMessage(L" Try to use VAOs & VBOs...");
		// Create Vertex Array Object
		GLuint vao;
		glGenVertexArrays(1,&vao);
		glBindVertexArray(vao);

		// Create Vertex Buffer Object
		GLuint vbo;
		glGenBuffers(1,&vbo);
		glBindBuffer(GL_ARRAY_BUFFER,vbo);

		// bind data
		glBufferData(GL_ARRAY_BUFFER,size_t*sizeof(float),pos,GL_STREAM_DRAW);
		
		// enable pointers
		//glEnableClientState(GL_VERTEX_ARRAY);

		// use buffers
		glBindVertexArray(vao);
		//glBindBuffer(GL_ARRAY_BUFFER, vbo);
		//glVertexPointer(3, GL_FLOAT, 0, (char *)NULL);

		// draw
		//if((*c)->_closed)glDrawArrays(GL_LINE_LOOP, 0,nbp-1);
		//else glDrawArrays(GL_LINE_STRIP, 0,nbp);
		glDrawArrays(GL_LINE_STRIP,0,nbp);
		//glDrawElements(
		
		// disable pointers
		//glDisableClientState(GL_VERTEX_ARRAY);

		//delete
		glDeleteBuffers(1,&vbo);
		glDeleteVertexArrays(1,&vao);
	}
	delete[]pos;

	glDisable(GL_LINE_SMOOTH);
	glLineWidth(1);
	
	
	//Application().LogMessage(L"OpenGL Draw Line Called...");
	
}
