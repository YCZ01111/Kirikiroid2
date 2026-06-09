//---------------------------------------------------------------------------
// iOS stub implementation for MenuItemImpl
// Provides tTJSNI_MenuItem with minimal functionality
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#include "MenuItemImpl.h"
#include "MenuItemIntf.h"

//---------------------------------------------------------------------------
// tTJSNI_MenuItem
//---------------------------------------------------------------------------
tTJSNI_MenuItem::tTJSNI_MenuItem()
{
	Caption = TJS_W("");
	Shortcut = TJS_W("");
	IsAttched = false;
	IsChecked = false;
	IsEnabled = true;
	IsRadio = false;
	IsVisible = true;
	GroupIndex = 0;
}

tjs_error TJS_INTF_METHOD tTJSNI_MenuItem::Construct(tjs_int numparams, tTJSVariant **param,
	iTJSDispatch2 *tjs_obj)
{
	return tTJSNI_BaseMenuItem::Construct(numparams, param, tjs_obj);
}

void TJS_INTF_METHOD tTJSNI_MenuItem::Invalidate()
{
	tTJSNI_BaseMenuItem::Invalidate();
}

void tTJSNI_MenuItem::MenuItemClick()
{
}

bool tTJSNI_MenuItem::CanDeliverEvents() const
{
	return true;
}

void tTJSNI_MenuItem::Add(tTJSNI_MenuItem * item)
{
	Children.push_back(item);
}

void tTJSNI_MenuItem::Insert(tTJSNI_MenuItem *item, tjs_int index)
{
	if(index >= 0 && index <= (tjs_int)Children.size())
		Children.insert(Children.begin() + index, item);
	else
		Children.push_back(item);
}

void tTJSNI_MenuItem::Remove(tTJSNI_MenuItem *item)
{
	for(auto it = Children.begin(); it != Children.end(); ++it)
	{
		if(*it == item)
		{
			Children.erase(it);
			break;
		}
	}
}

void tTJSNI_MenuItem::SetCaption(const ttstr & caption)
{
	Caption = caption;
}

void tTJSNI_MenuItem::GetCaption(ttstr & caption) const
{
	caption = Caption;
}

void tTJSNI_MenuItem::SetChecked(bool b)
{
	IsChecked = b;
}

bool tTJSNI_MenuItem::GetChecked() const
{
	return IsChecked;
}

void tTJSNI_MenuItem::SetEnabled(bool b)
{
	IsEnabled = b;
}

bool tTJSNI_MenuItem::GetEnabled() const
{
	return IsEnabled;
}

void tTJSNI_MenuItem::SetGroup(tjs_int g)
{
	GroupIndex = g;
}

tjs_int tTJSNI_MenuItem::GetGroup() const
{
	return GroupIndex;
}

void tTJSNI_MenuItem::SetRadio(bool b)
{
	IsRadio = b;
}

bool tTJSNI_MenuItem::GetRadio() const
{
	return IsRadio;
}

void tTJSNI_MenuItem::SetShortcut(const ttstr & shortcut)
{
	Shortcut = shortcut;
}

void tTJSNI_MenuItem::GetShortcut(ttstr & shortcut) const
{
	shortcut = Shortcut;
}

void tTJSNI_MenuItem::SetVisible(bool b)
{
	IsVisible = b;
}

bool tTJSNI_MenuItem::GetVisible() const
{
	return IsVisible;
}

tjs_int tTJSNI_MenuItem::GetIndex() const
{
	return 0;
}

void tTJSNI_MenuItem::SetIndex(tjs_int newIndex)
{
}

tjs_int tTJSNI_MenuItem::TrackPopup(tjs_uint32 flags, tjs_int x, tjs_int y) const
{
	return 0;
}

void* tTJSNI_MenuItem::GetMenuItemHandleForPlugin() const
{
	return nullptr;
}
