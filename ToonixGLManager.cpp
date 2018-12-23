// ToonixGLManager.cpp
//---------------------------------------
#include "ToonixGLManager.h"

void TXGLShapeManager::AddShape(TXGLShape * shape)
{
	Application().LogMessage(L" Added Shape To Shape Manager...");
	TXGLShapeMap* original = m_map;
	TXGLShapeMap* newmap = NULL;

	newmap = new TXGLShapeMap;
	newmap->m_shape = shape;

	newmap->m_next = original;
	newmap->m_id = m_nbshapes;
	shape->SetID(m_cnt);

	m_map = newmap;
	m_nbshapes++;
	m_cnt++;
}

void TXGLShapeManager::RemoveShape(long id)
{
	Application().LogMessage(L"Remove OGL Shape ID : "+(CString)id);
	TXGLShapeMap* browser = m_map;
	TXGLShapeMap* previous = NULL;

	while(browser)
	{
		if (browser->m_shape)
		{
			if (browser->m_id == id)
			{
				browser->m_shape->Delete();
				delete browser->m_shape;

				if (previous)
					previous->m_next = browser->m_next;
				else
					m_map = browser->m_next;

				delete browser;
				browser = NULL;
				m_nbshapes--;
				m_cnt--;
			}
			else
			{
				previous = browser;
				browser = browser->m_next;
			}
		}
		else
		{
			previous = browser;
			browser = browser->m_next;
		}
	}
}

void TXGLShapeManager::DrawAll()
{
	TXGLShapeMap* browser = m_map;

	int ID = 0;
	while(browser)
	{
		ID++;
	
		if (browser->m_shape)browser->m_shape->Draw();
		if(browser->m_next)browser = browser->m_next;
		else browser = NULL;
	}
}

void TXGLShapeManager::DrawShape(long id)
{
	TXGLShapeMap* browser = m_map;

	while(browser)
	{
		if (browser->m_shape)
		{
			if (browser->m_id == id)
			{
				browser->m_shape->Draw();
				browser = NULL;
			}
			else browser = browser->m_next;
		}
		else browser = browser->m_next;
	}
}

void TXGLShapeManager::FlushAll()			// delete every entry in the map
{
	TXGLShapeMap* browser = m_map;
	TXGLShapeMap* prev = NULL;

	while(browser)
	{
		if (browser->m_shape)
		{
			delete browser->m_shape;

			if (prev)
				prev->m_next = browser->m_next;
			else
				m_map = browser->m_next;

			delete browser;
			browser = NULL;
		}
		else
		{
			prev = browser;
			browser = browser->m_next;
		}
	}
	m_nbshapes = 0;
	m_map = NULL;
}

