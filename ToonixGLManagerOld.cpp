#include "ToonixRegister.h"
#include "ToonixGLManager.h"

TXLineMapItem* TXLineMapManager::Get(CString in_name)		// get item
{
	TXLineMapItem* item = NULL;

	TXLineMap *original = _map;
	TXLineMap *browser = _map;
	TXLineMap *entry = NULL;

	while(browser)
	{
		if (browser->_item)
		{
			if (browser->_item->_name == in_name)
			{
				item = browser->_item;
				browser = NULL;
			}
			else browser = browser->_next;
		}
		else browser = browser->_next;
	}

	return item;
}

void TXLineMapManager::Add(CString in_name, TXLineMapItem* item)		// add item
{
	TXLineMap *original = _map;
	TXLineMap *entry = NULL;

	entry = new TXLineMap;
	entry->_item = item;

	item->_name = in_name;
	item->Init();

	entry->_next = original;
	_map = entry;
}

void TXLineMapManager::Delete(CString in_name)						// delete item
{
	TXLineMap* browser = _map;
	TXLineMap* prev = NULL;

	while(browser)
	{
		if (browser->_item)
		{
			if (browser->_item->_name == in_name)
			{
				browser->_item->Delete();
				delete browser->_item;

				if (prev)
					prev->_next = browser->_next;
				else
					_map = browser->_next;

				delete browser;
				browser = NULL;
			}
			else
			{
				prev = browser;
				browser = browser->_next;
			}
		}
		else
		{
			prev = browser;
			browser = browser->_next;
		}
	}
}

void TXLineMapManager::RenderAll(CGraphicSequencer& sequencer)	// render every entry in the map
{
	TXLineMap* browser = _map;
	while(browser)
	{
		if (browser->_item)
		{
			browser->_item->Render(sequencer);
		}
		browser = browser->_next;
	}
}

void TXLineMapManager::Wipe()									// delete every entry in the map
{
	TXLineMap* browser = _map;
	TXLineMap* prev = NULL;

	while(browser)
	{
		if (browser->_item)
		{
			browser->_item->Delete();
			browser->_item;

			if (prev)
				prev->_next = browser->_next;
			else
				_map = browser->_next;

			delete browser;
			browser = NULL;
		}
		else
		{
			prev = browser;
			browser = browser->_next;
		}
	}

	_map = NULL;
}