// ToonixGLHair
//------------------------------------------------
#include "ToonixGLShape.h"

class TXGLLine : public TXGLShape
{
public:
	TXGLLine(){m_line=NULL;};
	TXGLLine(TXLine* l){m_line=l;};
	~TXGLLine(){};

	void Draw();

	void Set(TXLine* l){m_line=l;};

private:
	TXLine* m_line;

};

class TXGLHair : public TXGLShape
{
	TXGLHair(){};
	~TXGLHair(){};
	void Set();
	void Draw();
	void Clear();

protected:
	std::vector<TXChain*> m_hairs;
}
