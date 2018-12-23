// ToonixGLShape.cpp
//-------------------------------------------------
#include "ToonixGLShape.h"
#include <xsi_vector3.h>
// TXGLShape
//-------------------------------------------------
TXGLShape::TXGLShape(){};
TXGLShape::~TXGLShape(){};

void TXGLShape::Delete(){};

// TXGLPoint
//-------------------------------------------------
TXGLPoint::TXGLPoint(){};
TXGLPoint::~TXGLPoint(){};

void TXGLPoint::SetSize(float size)
{
	m_size = (GLfloat)size;
}

void TXGLPoint::SetPos(XSI::MATH::CVector3& pos)
{
	m_pos[0] = (GLfloat)pos.GetX();
	m_pos[1] = (GLfloat)pos.GetY();
	m_pos[2] = (GLfloat)pos.GetZ();
}

void TXGLPoint::Draw()
{
	//Set Size
	glPointSize(m_size);

	// set current color
	//glColor3f(_colors[0], _colors[1], _colors[2]);

	//draw point
	//glBegin(GL_POINTS) ;
	//glVertex3f(_pos[0],_pos[1],_pos[2]) ;
	//glEnd();
};


/*
// eGLPoint
//-------------------------------------------------
eGLPoint::eGLPoint(){};
eGLPoint::~eGLPoint(){};

void eGLPoint::SetSize(float size)
{
	PointSize = (GLfloat)size;
}

void eGLPoint::SetPos(CVector3& pos)
{
	Pos[0] = (GLfloat)pos.GetX();
	Pos[1] = (GLfloat)pos.GetY();
	Pos[2] = (GLfloat)pos.GetZ();
}

void eGLPoint::Draw()
{
	//Set Size
	glEnable(GL_POINT_SMOOTH);
	glPointSize(PointSize);

	// set current color
	glColor3f(Colors[0], Colors[1], Colors[2]);

	//draw point
	glBegin(GL_POINTS) ;
	glVertex3f(Pos[0],Pos[1],Pos[2]) ;
	glEnd();
};

// eGLLine
//-------------------------------------------------
eGLLine::eGLLine(){};
eGLLine::~eGLLine(){};

void eGLLine::SetStart(CVector3& start)
{
	Start[0] = (GLfloat)start.GetX();
	Start[1] = (GLfloat)start.GetY();
	Start[2] = (GLfloat)start.GetZ();
}

void eGLLine::SetEnd(CVector3& end)
{
	End[0] = (GLfloat)end.GetX();
	End[1] = (GLfloat)end.GetY();
	End[2] = (GLfloat)end.GetZ();
}

void eGLLine::Draw()
{
	// get current vector
	glColor3f(Colors[0], Colors[1], Colors[2]);

	// draw line
	glBegin(GL_LINES);
	glVertex3f(Start[0],Start[1],Start[2]) ;
	glVertex3f(End[0],End[1],End[2]) ;
	glEnd();
};

// eGLVector
//-------------------------------------------------
eGLVector::eGLVector(){};
eGLVector::~eGLVector(){};

void eGLVector::SetStart(CVector3& start)
{
	Start[0] = (GLfloat)start.GetX();
	Start[1] = (GLfloat)start.GetY();
	Start[2] = (GLfloat)start.GetZ();
}

void eGLVector::SetEnd(CVector3& end)
{
	End[0] = (GLfloat)end.GetX();
	End[1] = (GLfloat)end.GetY();
	End[2] = (GLfloat)end.GetZ();
}

void eGLVector::SetLeft(CVector3& left)
{
	Left[0] = (GLfloat)left.GetX();
	Left[1] = (GLfloat)left.GetY();
	Left[2] = (GLfloat)left.GetZ();
}

void eGLVector::SetRight(CVector3& right)
{
	Right[0] = (GLfloat)right.GetX();
	Right[1] = (GLfloat)right.GetY();
	Right[2] = (GLfloat)right.GetZ();
}

void eGLVector::Draw()
{
	// set current color
	glColor3f(Colors[0], Colors[1], Colors[2]);

	//arrow body
	glBegin(GL_LINES) ;
	glVertex3f(Start[0],Start[1],Start[2]) ;
	glVertex3f(End[0],End[1],End[2]) ;
	glEnd();

	//arrow head
	glBegin(GL_LINE_LOOP);
	glVertex3f(Left[0],Left[1],Left[2]) ;
	glVertex3f(Right[0],Right[1],Right[2]) ;
	glVertex3f(End[0],End[1],End[2]) ;
	glEnd();	
};

// eGLArrow
//-------------------------------------------------
eGLArrow::eGLArrow(){};
eGLArrow::~eGLArrow(){};

void eGLArrow::SetStart(CVector3& start)
{
	Start[0] = (GLfloat)start.GetX();
	Start[1] = (GLfloat)start.GetY();
	Start[2] = (GLfloat)start.GetZ();
}

void eGLArrow::SetEnd(CVector3& end)
{
	End[0] = (GLfloat)end.GetX();
	End[1] = (GLfloat)end.GetY();
	End[2] = (GLfloat)end.GetZ();
}

void eGLArrow::SetHead(CVector3& Start, CVector3& End, float height, float width, long division)
{
	Head = new GLfloat[(division+1)*3];
	HeadDivision = division;
	CVector3 Dir,Up,Side,Base,Radius,Pnt,Y;
	
	Y.Set(0,1,0);
	Dir.Sub(End,Start);
	float l = (float)Dir.GetLength();
	Up.Set(0,0,1);
	Side.Cross(Dir,Up);
	Side.NormalizeInPlace();
	Dir.NormalizeInPlace();
	Up.Cross(Dir,Side);

	CRotation Rot;
	CMatrix3 Mat;
	Mat.Set(Up.GetX(),Up.GetY(),Up.GetZ(),
				Dir.GetX(),Dir.GetY(),Dir.GetZ(),
					Side.GetX(),Side.GetY(),Side.GetZ());

	Base.LinearlyInterpolate(End,Start,1/l * height);

	CTransformation Tra;
	Tra.SetTranslation(Base);
	Tra.SetRotationFromMatrix3(Mat);

	float angle = 0;
	float angleincr = 2*(float)PI/division;
	Radius.Set(width/2,0,0);
	for(long x=0;x<=division;x++)
	{
		Rot.SetFromAxisAngle(Y,angle);
		Mat = Rot.GetMatrix();
		Pnt.MulByMatrix3(Radius,Mat);
		Pnt.MulByTransformationInPlace(Tra);
		Head[x*3] = (GLfloat)Pnt.GetX();
		Head[x*3+1] = (GLfloat)Pnt.GetY();
		Head[x*3+2] = (GLfloat)Pnt.GetZ();
		angle+=angleincr;	
	}
}

void eGLArrow::Delete()
{
	if(Head)delete[]Head;
}

void eGLArrow::Draw()
{
	// set current vector
	glColor3f(Colors[0], Colors[1], Colors[2]);

	//arrow body
	glBegin(GL_LINES) ;
	glVertex3f(Start[0],Start[1],Start[2]) ;
	glVertex3f(End[0],End[1],End[2]) ;
	glEnd();

	//arrow head
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(End[0],End[1],End[2]);
	for(long g=0;g<=HeadDivision;g++)
	{
		glVertex3f(Head[g*3],Head[g*3+1],Head[g*3+2]);
	}
	glEnd();
};
*/