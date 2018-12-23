//-------------------------------------------------
// TXGLLine.h
//-------------------------------------------------
#ifndef _TOONIX_GLLINE_H_
#define _TOONIX_GLLINE_H_

#include "ToonixGLShape.h"
#include "ToonixLine.h"

class TXGLLine : public TXGLShape
{
public:
	TXGLLine(){m_line=NULL;};
	TXGLLine(TXLine* l){m_line=l;};
	~TXGLLine(){Delete();};

	void Draw();

	void Set(TXLine* l){m_line=l;};

private:
	TXLine* m_line;

};

#endif
