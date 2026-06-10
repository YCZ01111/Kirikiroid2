//---------------------------------------------------------------------------
// iOS stub implementation for TVPSysFont
// Provides TVPGetFontList, TVPGetCharSetFromFaceName, and tTVPSysFont
// Note: TVPGetAllFontList and TVPGetDefaultFontName are already in FontImpl.cpp
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#include "tvpfontstruc.h"
#include "TVPSysFont.h"
#include "FontSystem.h"
#include "MsgIntf.h"

extern FontSystem* TVPFontSystem;

void TVPGetFontList(std::vector<ttstr> & list, tjs_uint32 flags, const tTVPFont & font) {
	TVPGetAllFontList(list);
}

tjs_uint8 TVPGetCharSetFromFaceName( const tjs_char* face ) {
	return 0; // ANSI_CHARSET
}

//---------------------------------------------------------------------------
// tTVPSysFont stub implementation
//---------------------------------------------------------------------------
tTVPSysFont::tTVPSysFont() {
}

tTVPSysFont::tTVPSysFont( const tTVPFont &font ) {
}

tTVPSysFont::~tTVPSysFont() {
}

int tTVPSysFont::GetAscentHeight() {
	return 0;
}

void tTVPSysFont::Assign( const tTVPSysFont* font ) {
}

void tTVPSysFont::Assign( const tTVPFont &font ) {
}
