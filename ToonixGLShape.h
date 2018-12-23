#ifndef TOONIX_GLSHAPE_H
#define TOONIX_GLSHAPE_H

#include "ToonixGL.h"
#include <xsi_vector3.h>

class TXGLShape
{
public:
	TXGLShape();
	virtual ~TXGLShape();

	virtual void Draw() = 0;
	virtual void Delete();
	void SetColor(float r, float g, float b){m_colors[0]=r;m_colors[1]=g;m_colors[2]=b;};
	void SetID(int id){m_id = id;};

protected:
	GLfloat m_colors[3];
	int m_id;						// unique identifier

};

class TXGLTest : public TXGLShape
{
public:
	TXGLTest(){};
	~TXGLTest(){};
	void Draw();

};

class TXGLPoint : public TXGLShape
{
	public:
	TXGLPoint();
	~TXGLPoint();

	void Draw();

	void SetPos(XSI::MATH::CVector3&);
	void SetSize(float);

protected:
	GLfloat m_pos[3];
	GLfloat m_size;
};

/*
class TXGLLine : public TXGLShape
{
	public:
	TXGLLine();
	~TXGLLine();

	void Draw();

	void Set(TXLine*);

private:
	GLfloat Pos[3];
	GLfloat PointSize;

};
*/
#endif // _TOONIX_GLSHAPE_H_
