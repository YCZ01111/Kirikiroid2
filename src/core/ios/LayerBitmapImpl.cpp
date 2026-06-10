//---------------------------------------------------------------------------
// iOS implementation for LayerBitmapImpl
// Provides tTVPBitmap and tTVPNativeBaseBitmap with stub font rendering
//---------------------------------------------------------------------------
#define _USE_MATH_DEFINES
#include "tjsCommHead.h"

#include <memory>
#include <stdlib.h>
#include <math.h>

#include "LayerBitmapIntf.h"
#include "LayerBitmapImpl.h"
#include "MsgIntf.h"
#include "ComplexRect.h"
#include "tvpgl.h"
#include "tjsHashSearch.h"
#include "EventIntf.h"
#include "SysInitImpl.h"
#include "StorageIntf.h"
#include "DebugIntf.h"
#include "StringUtil.h"
#include "BitmapBitsAlloc.h"
#include "RenderManager.h"
#include "FontSystem.h"
#include "FontRasterizer.h"
#include "PrerenderedFont.h"
#include "CharacterData.h"
#include "ConfigManager/IndividualConfigManager.h"

//---------------------------------------------------------------------------
// prototypes
//---------------------------------------------------------------------------
void TVPClearFontCache();
void TVPInitWindowOptions();
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Stub font rasterizer for iOS (no FreeType)
//---------------------------------------------------------------------------
class iOSStubFontRasterizer : public FontRasterizer {
	int refCount;
public:
	iOSStubFontRasterizer() : refCount(1) {}
	void AddRef() override { refCount++; }
	void Release() override { if(--refCount == 0) delete this; }
	void ApplyFont(tTVPNativeBaseBitmap *bmp, bool force) override {}
	void ApplyFont(const tTVPFont& font) override {}
	void GetTextExtent(tjs_char ch, tjs_int &w, tjs_int &h) override { w = 0; h = 0; }
	tjs_int GetAscentHeight() override { return 0; }
	tTVPCharacterData* GetBitmap(const tTVPFontAndCharacterData & font, tjs_int aofsx, tjs_int aofsy) override { return nullptr; }
	void GetGlyphDrawRect(const ttstr & text, tTVPRect& area) override { area.left = area.top = area.right = area.bottom = 0; }
};

//---------------------------------------------------------------------------
// default FONT retrieve function
//---------------------------------------------------------------------------
FontSystem* TVPFontSystem = NULL;
static tjs_int TVPGlobalFontStateMagic = 0;

enum {
	FONT_RASTER_STUB,
	FONT_RASTER_MAX
};
static FontRasterizer* TVPFontRasterizers[FONT_RASTER_MAX];
static bool TVPFontRasterizersInit = false;
static tjs_int TVPCurrentFontRasterizers = FONT_RASTER_STUB;

void TVPInializeFontRasterizers() {
	if( TVPFontRasterizersInit == false ) {
		TVPFontRasterizers[FONT_RASTER_STUB] = new iOSStubFontRasterizer();
		TVPFontSystem = new FontSystem();
		TVPFontRasterizersInit = true;
	}
}
void TVPUninitializeFontRasterizers() {
	for( tjs_int i = 0; i < FONT_RASTER_MAX; i++ ) {
		if( TVPFontRasterizers[i] ) {
			TVPFontRasterizers[i]->Release();
			TVPFontRasterizers[i] = NULL;
		}
	}
	if( TVPFontSystem ) {
		delete TVPFontSystem;
		TVPFontSystem = NULL;
	}
}
static tTVPAtExit
	TVPUninitializeFontRaster(TVP_ATEXIT_PRI_RELEASE, TVPUninitializeFontRasterizers);

void TVPSetFontRasterizer( tjs_int index ) {
	if( TVPCurrentFontRasterizers != index && index >= 0 && index < FONT_RASTER_MAX ) {
		TVPCurrentFontRasterizers = index;
		TVPClearFontCache();
		TVPGlobalFontStateMagic++;
	}
}
tjs_int TVPGetFontRasterizer() {
	return TVPCurrentFontRasterizers;
}
FontRasterizer* GetCurrentRasterizer() {
	return TVPFontRasterizers[TVPCurrentFontRasterizers];
}

//---------------------------------------------------------------------------
#define TVP_CH_MAX_CACHE_COUNT 1300
#define TVP_CH_MAX_CACHE_COUNT_LOW 100
#define TVP_CH_MAX_CACHE_HASH_SIZE 512
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Pre-rendered font management
//---------------------------------------------------------------------------
tTJSHashTable<ttstr, tTVPPrerenderedFont *> TVPPrerenderedFonts;

struct tTVPPrerenderedFontMap
{
	tTVPFont Font;
	tTVPPrerenderedFont * Object;
};
static std::vector<tTVPPrerenderedFontMap> TVPPrerenderedFontMapVector;

void TVPMapPrerenderedFont(const tTVPFont & font, const ttstr & storage)
{
	ttstr fn = TVPSearchPlacedPath(storage);
	tTVPPrerenderedFont * object;
	tTVPPrerenderedFont ** found = TVPPrerenderedFonts.Find(fn);
	if(!found) {
		object = new tTVPPrerenderedFont(fn);
	} else {
		object = *found;
		object->AddRef();
	}
	std::vector<tTVPPrerenderedFontMap>::iterator i;
	for(i = TVPPrerenderedFontMapVector.begin(); i !=TVPPrerenderedFontMapVector.end(); i++) {
		if(i->Font == font) {
			i->Object->Release();
			i->Object = object;
			break;
		}
	}
	if(i == TVPPrerenderedFontMapVector.end()) {
		tTVPPrerenderedFontMap map;
		map.Font = font;
		map.Object = object;
		TVPPrerenderedFontMapVector.push_back(map);
	}
	TVPGlobalFontStateMagic++;
	TVPClearFontCache();
}

void TVPUnmapPrerenderedFont(const tTVPFont & font)
{
	std::vector<tTVPPrerenderedFontMap>::iterator i;
	for(i = TVPPrerenderedFontMapVector.begin(); i !=TVPPrerenderedFontMapVector.end(); i++) {
		if(i->Font == font) {
			i->Object->Release();
			TVPPrerenderedFontMapVector.erase(i);
			TVPGlobalFontStateMagic++;
			TVPClearFontCache();
			return;
		}
	}
}

static void TVPUnmapAllPrerenderedFonts()
{
	std::vector<tTVPPrerenderedFontMap>::iterator i;
	for(i = TVPPrerenderedFontMapVector.begin(); i !=TVPPrerenderedFontMapVector.end(); i++) {
		i->Object->Release();
	}
	TVPPrerenderedFontMapVector.clear();
	TVPGlobalFontStateMagic++;
}
static tTVPAtExit TVPUnmapAllPrerenderedFontsAtExit(TVP_ATEXIT_PRI_PREPARE, TVPUnmapAllPrerenderedFonts);

static tTVPPrerenderedFont * TVPGetPrerenderedMappedFont(const tTVPFont &font)
{
	std::vector<tTVPPrerenderedFontMap>::iterator i;
	for(i = TVPPrerenderedFontMapVector.begin(); i !=TVPPrerenderedFontMapVector.end(); i++) {
		if(i->Font == font) {
			i->Object->AddRef();
			return i->Object;
		}
	}
	return NULL;
}

//---------------------------------------------------------------------------
typedef tTJSRefHolder<tTVPCharacterData> tTVPCharacterDataHolder;
typedef tTJSHashCache<tTVPFontAndCharacterData, tTVPCharacterDataHolder,
	tTVPFontHashFunc, TVP_CH_MAX_CACHE_HASH_SIZE> tTVPFontCache;
tTVPFontCache TVPFontCache(TVP_CH_MAX_CACHE_COUNT);

void TVPSetFontCacheForLowMem()
{
	TVPFontCache.SetMaxCount(TVP_CH_MAX_CACHE_COUNT_LOW);
}
void TVPClearFontCache()
{
	TVPFontCache.Clear();
}

struct tTVPClearFontCacheCallback : public tTVPCompactEventCallbackIntf
{
	virtual void TJS_INTF_METHOD OnCompact(tjs_int level)
	{
		if(level >= TVP_COMPACT_LEVEL_MINIMIZE) {
			TVPClearFontCache();
		}
	}
} static TVPClearFontCacheCallback;
static bool TVPClearFontCacheCallbackInit = false;

static tTVPCharacterData * TVPGetCharacter(const tTVPFontAndCharacterData & font,
	tTVPNativeBaseBitmap *bmp, tTVPPrerenderedFont *pfont, tjs_int aofsx, tjs_int aofsy)
{
	if(!TVPClearFontCacheCallbackInit) {
		TVPAddCompactEventHook(&TVPClearFontCacheCallback);
		TVPClearFontCacheCallbackInit = true;
	}

	tjs_uint32 hash = tTVPFontCache::MakeHash(font);
	tTVPCharacterDataHolder * ptr = TVPFontCache.FindAndTouchWithHash(font, hash);
	if(ptr) return ptr->GetObject();

	// Not in cache - try rasterizer
	tTVPCharacterData * data = nullptr;
	if(pfont) {
		const tTVPPrerenderedCharacterItem *item = pfont->Find(font.Character);
		if(item) {
			data = new tTVPCharacterData();
			data->BlackBoxX = item->Width;
			data->BlackBoxY = item->Height;
			data->Metrics.CellIncX = item->IncX;
			data->Metrics.CellIncY = item->IncY;
			data->OriginX = item->OriginX + aofsx;
			data->OriginY = -item->OriginY + aofsy;
			data->Antialiased = font.Antialiased;
			data->FullColored = false;
			data->Blured = font.Blured;
			data->BlurWidth = font.BlurWidth;
			data->BlurLevel = font.BlurLevel;
			if(data->BlackBoxX && data->BlackBoxY) {
				tjs_int newpitch = (((item->Width -1)>>2)+1)<<2;
				data->Pitch = newpitch;
				data->Alloc(newpitch * item->Height);
				pfont->Retrieve(item, data->GetData(), newpitch);
			}
		}
	}
	if(!data) {
		data = GetCurrentRasterizer()->GetBitmap(font, aofsx, aofsy);
	}
	if(data) {
		tTVPCharacterDataHolder holder(data);
		TVPFontCache.AddWithHash(font, hash, holder);
	}
	return data;
}


//---------------------------------------------------------------------------
// tTVPBitmap
//---------------------------------------------------------------------------
tTVPBitmap::tTVPBitmap(tjs_uint width, tjs_uint height, tjs_uint bpp)
{
	TVPInitWindowOptions();
	RefCount = 1;
	Allocate(width, height, bpp);
}

tTVPBitmap::tTVPBitmap(tjs_uint width, tjs_uint height, tjs_uint bpp, void* bits)
{
	TVPInitWindowOptions();
	RefCount = 1;
	BitmapInfo = new BitmapInfomation( width, height, bpp );
	Width = width;
	Height = height;
	PitchBytes = BitmapInfo->GetPitchBytes();
	PitchStep = PitchBytes;
	try {
		Bits = bits;
		if( bpp == 8 ) {
			Palette = new tjs_uint[DEFAULT_PALETTE_COUNT];
			ActualPalCount = 0;
		} else {
			Palette = NULL;
			ActualPalCount = 0;
		}
	} catch(...) {
		delete BitmapInfo;
		BitmapInfo = NULL;
		throw;
	}
}

tTVPBitmap::~tTVPBitmap()
{
	tTVPBitmapBitsAlloc::Free(Bits);
	delete BitmapInfo;
	if( Palette ) delete Palette;
}

tTVPBitmap::tTVPBitmap(const tTVPBitmap & r)
{
	TVPInitWindowOptions();
	RefCount = 1;
	Allocate(r.GetWidth(), r.GetHeight(), r.GetBPP());
	*BitmapInfo = *r.BitmapInfo;
	if(r.Bits) memcpy(Bits, r.Bits, r.BitmapInfo->GetImageSize());
	if(r.Palette) {
		memcpy(Palette, r.Palette, sizeof(tjs_uint)*DEFAULT_PALETTE_COUNT);
		ActualPalCount = r.ActualPalCount;
	}
	PitchBytes = r.PitchBytes;
	PitchStep = r.PitchStep;
}

void tTVPBitmap::Allocate(tjs_uint width, tjs_uint height, tjs_uint bpp)
{
	BitmapInfo = new BitmapInfomation( width, height, bpp );
	Width = width;
	Height = height;
	PitchBytes = BitmapInfo->GetPitchBytes();
	PitchStep = PitchBytes;
	try {
		Bits = tTVPBitmapBitsAlloc::Alloc(BitmapInfo->GetImageSize(), width, height);
		if( bpp == 8 ) {
			Palette = new tjs_uint[DEFAULT_PALETTE_COUNT];
			ActualPalCount = 0;
		} else {
			Palette = NULL;
			ActualPalCount = 0;
		}
	} catch(...) {
		delete BitmapInfo;
		BitmapInfo = NULL;
		throw;
	}
}

void * tTVPBitmap::GetScanLine(tjs_uint l) const
{
	if((tjs_int)l>=BitmapInfo->GetHeight()) {
		TVPThrowExceptionMessage(TVPScanLineRangeOver, ttstr((tjs_int)l),
			ttstr((tjs_int)BitmapInfo->GetHeight()-1));
	}
	return l * PitchBytes + (tjs_uint8*)Bits;
}

void tTVPBitmap::SetPaletteCount( tjs_uint count ) {
	if( !Is8bit() ) TVPThrowExceptionMessage(TVPInvalidOperationFor32BPP);
	if( count >= DEFAULT_PALETTE_COUNT )
		TVPThrowExceptionMessage(TJSRangeError);
	ActualPalCount = count;
}


//---------------------------------------------------------------------------
// tTVPNativeBaseBitmap
//---------------------------------------------------------------------------
tTVPNativeBaseBitmap::tTVPNativeBaseBitmap()
{
	TVPInializeFontRasterizers();
	Font = TVPFontSystem->GetDefaultFont();
	PrerenderedFont = NULL;
	FontChanged = true;
	GlobalFontState = -1;
	TextWidth = TextHeight = 0;
	Bitmap = nullptr;
}

tTVPNativeBaseBitmap::tTVPNativeBaseBitmap(const tTVPNativeBaseBitmap & r)
{
	TVPInializeFontRasterizers();
	Bitmap = r.Bitmap;
	if (Bitmap) Bitmap->AddRef();
	Font = r.Font;
	PrerenderedFont = NULL;
	FontChanged = true;
	TextWidth = TextHeight = 0;
}

tTVPNativeBaseBitmap::~tTVPNativeBaseBitmap()
{
	if (Bitmap) Bitmap->Release();
	if(PrerenderedFont) PrerenderedFont->Release();
}

tjs_uint tTVPNativeBaseBitmap::GetWidth() const
{
	return Bitmap->GetWidth();
}

void tTVPNativeBaseBitmap::SetWidth(tjs_uint w)
{
	SetSize(w, Bitmap->GetHeight());
}

tjs_uint tTVPNativeBaseBitmap::GetHeight() const
{
	return Bitmap->GetHeight();
}

void tTVPNativeBaseBitmap::SetHeight(tjs_uint h)
{
	SetSize(Bitmap->GetWidth(), h);
}

void tTVPNativeBaseBitmap::SetSize(tjs_uint w, tjs_uint h, bool keepimage)
{
	if (w == 0) w = 1;
	if (h == 0) h = 1;
	if (Bitmap->GetWidth() != w || Bitmap->GetHeight() != h) {
		iTVPTexture2D *newbitmap;
		if (keepimage)
			newbitmap = GetRenderManager()->CreateTexture2D(w, h, Bitmap);
		else
			newbitmap = GetRenderManager()->CreateTexture2D(nullptr, 0, w, h, Bitmap->GetFormat());
		Bitmap->Release();
		Bitmap = newbitmap;
		FontChanged = true;
	}
}

void tTVPNativeBaseBitmap::SetSizeAndImageBuffer(tTVPBitmap* bmp)
{
	iTVPTexture2D *newbitmap = GetRenderManager()->CreateTexture2D(bmp);
	Bitmap->Release();
	Bitmap = newbitmap;
	FontChanged = true;
}

tjs_uint tTVPNativeBaseBitmap::GetBPP() const
{
	switch (Bitmap->GetFormat()) {
	case TVPTextureFormat::Gray: return 8;
	case TVPTextureFormat::RGBA: return 32;
	case TVPTextureFormat::RGB: return 24;
	default: return 0;
	}
}

bool tTVPNativeBaseBitmap::Is32BPP() const
{
	return Bitmap->GetFormat() != TVPTextureFormat::Gray;
}

bool tTVPNativeBaseBitmap::Is8BPP() const
{
	return Bitmap->GetFormat() == TVPTextureFormat::Gray;
}

bool tTVPNativeBaseBitmap::IsOpaque() const
{
	return Bitmap->IsOpaque();
}

bool tTVPNativeBaseBitmap::Assign(const tTVPNativeBaseBitmap &rhs)
{
	if(this == &rhs || Bitmap == rhs.Bitmap) return false;
	Bitmap->Release();
	Bitmap = rhs.Bitmap;
	Bitmap->AddRef();
	Font = rhs.Font;
	FontChanged = true;
	return true;
}

bool tTVPNativeBaseBitmap::AssignBitmap(const tTVPNativeBaseBitmap &rhs)
{
	if(this == &rhs || Bitmap == rhs.Bitmap) return false;
	Bitmap->Release();
	Bitmap = rhs.Bitmap;
	Bitmap->AddRef();
	FontChanged = true;
	return true;
}

bool tTVPNativeBaseBitmap::AssignTexture(iTVPTexture2D *tex)
{
	if (Bitmap == tex) return false;
	Bitmap->Release();
	Bitmap = tex;
	Bitmap->AddRef();
	FontChanged = true;
	return true;
}

const void * tTVPNativeBaseBitmap::GetScanLine(tjs_uint l) const
{
	return Bitmap->GetScanLineForRead(l);
}

void * tTVPNativeBaseBitmap::GetScanLineForWrite(tjs_uint l)
{
	Independ();
	return Bitmap->GetScanLineForWrite(l);
}

tjs_int tTVPNativeBaseBitmap::GetPitchBytes() const
{
	if (Bitmap) return Bitmap->GetPitch();
	return 0;
}

void tTVPNativeBaseBitmap::Independ()
{
	if (Bitmap->IsIndependent() && !Bitmap->IsStatic()) return;
	iTVPTexture2D *newb = GetRenderManager()->CreateTexture2D(Bitmap->GetWidth(), Bitmap->GetHeight(), Bitmap);
	Bitmap->Release();
	Bitmap = newb;
	FontChanged = true;
}

void tTVPNativeBaseBitmap::IndependNoCopy()
{
	if (!Bitmap->IsStatic() && Bitmap->IsIndependent()) return;
	Recreate();
}

void tTVPNativeBaseBitmap::Recreate()
{
	Recreate(Bitmap->GetWidth(), Bitmap->GetHeight(), Bitmap->GetFormat() == TVPTextureFormat::Gray ? 8 : 32);
}

void tTVPNativeBaseBitmap::Recreate(tjs_uint w, tjs_uint h, tjs_uint bpp)
{
	Bitmap->Release();
	Bitmap = GetRenderManager()->CreateTexture2D(nullptr, 0, w, h, bpp == 8 ? TVPTextureFormat::Gray : TVPTextureFormat::RGBA);
	FontChanged = true;
}

bool tTVPNativeBaseBitmap::IsIndependent() const
{
	return Bitmap->IsIndependent() && !Bitmap->IsStatic();
}

void tTVPNativeBaseBitmap::ApplyFont()
{
	if(FontChanged || GlobalFontState != TVPGlobalFontStateMagic) {
		FontChanged = false;
		GlobalFontState = TVPGlobalFontStateMagic;
		CachedText.Clear();
		TextWidth = TextHeight = 0;
		if(PrerenderedFont) PrerenderedFont->Release();
		PrerenderedFont = TVPGetPrerenderedMappedFont(Font);
		GetCurrentRasterizer()->ApplyFont(this, true);
		RadianAngle = Font.Angle * (M_PI/1800);
		double angle90 = RadianAngle + M_PI_2;
		AscentOfsX = 0;
		AscentOfsY = 0;
		FontHash = tTJSHashFunc<ttstr>::Make(Font.Face);
		FontHash ^= Font.Height ^ Font.Flags ^ Font.Angle;
	} else {
		GetCurrentRasterizer()->ApplyFont(this, false);
	}
}

void tTVPNativeBaseBitmap::SetFont(const tTVPFont &font)
{
	Font = font;
	FontChanged = true;
}

extern void TVPGetAllFontList(std::vector<ttstr>& list);
void tTVPNativeBaseBitmap::GetFontList(tjs_uint32 flags, std::vector<ttstr> &list)
{
	ApplyFont();
	std::vector<ttstr> ansilist;
	TVPGetAllFontList(ansilist);
	for(std::vector<ttstr>::iterator i = ansilist.begin(); i != ansilist.end(); i++)
		list.push_back(*i);
}

void tTVPNativeBaseBitmap::MapPrerenderedFont(const ttstr & storage)
{
	ApplyFont();
	TVPMapPrerenderedFont(Font, storage);
	FontChanged = true;
}

void tTVPNativeBaseBitmap::UnmapPrerenderedFont()
{
	ApplyFont();
	TVPUnmapPrerenderedFont(Font);
	FontChanged = true;
}

//---------------------------------------------------------------------------
// Text drawing - stub implementations for iOS
//---------------------------------------------------------------------------
void tTVPNativeBaseBitmap::DrawTextSingle(const tTVPRect &destrect, tjs_int x, tjs_int y, const ttstr &text,
	tjs_uint32 color, tTVPBBBltMethod bltmode, tjs_int opa,
	bool holdalpha, bool aa, tjs_int shlevel,
	tjs_uint32 shadowcolor,
	tjs_int shwidth, tjs_int shofsx, tjs_int shofsy,
	tTVPComplexRect *updaterects)
{
	// Stub - no text rendering on iOS yet
}

void tTVPNativeBaseBitmap::DrawTextMultiple(const tTVPRect &destrect, tjs_int x, tjs_int y, const ttstr &text,
	tjs_uint32 color, tTVPBBBltMethod bltmode, tjs_int opa,
	bool holdalpha, bool aa, tjs_int shlevel,
	tjs_uint32 shadowcolor,
	tjs_int shwidth, tjs_int shofsx, tjs_int shofsy,
	tTVPComplexRect *updaterects)
{
	// Stub - no text rendering on iOS yet
}

void tTVPNativeBaseBitmap::DrawGlyph(iTJSDispatch2* glyph, const tTVPRect &destrect, tjs_int x, tjs_int y,
		tjs_uint32 color, tTVPBBBltMethod bltmode, tjs_int opa,
		bool holdalpha, bool aa, tjs_int shlevel,
		tjs_uint32 shadowcolor,
		tjs_int shwidth, tjs_int shofsx, tjs_int shofsy,
		tTVPComplexRect *updaterects)
{
	// Stub - no glyph rendering on iOS yet
}

void tTVPNativeBaseBitmap::GetTextSize(const ttstr & text)
{
	TextWidth = 0;
	TextHeight = 0;
}

tjs_int tTVPNativeBaseBitmap::GetTextWidth(const ttstr & text)
{
	if(CachedText != text) GetTextSize(text);
	return TextWidth;
}

tjs_int tTVPNativeBaseBitmap::GetTextHeight(const ttstr & text)
{
	if(CachedText != text) GetTextSize(text);
	return TextHeight;
}

double tTVPNativeBaseBitmap::GetEscWidthX(const ttstr & text)
{
	return 0;
}

double tTVPNativeBaseBitmap::GetEscWidthY(const ttstr & text)
{
	return 0;
}

double tTVPNativeBaseBitmap::GetEscHeightX(const ttstr & text)
{
	return 0;
}

double tTVPNativeBaseBitmap::GetEscHeightY(const ttstr & text)
{
	return 0;
}

void tTVPNativeBaseBitmap::GetFontGlyphDrawRect( const ttstr & text, struct tTVPRect& area )
{
	area.left = area.top = area.right = area.bottom = 0;
}

iTVPTexture2D * tTVPNativeBaseBitmap::GetTextureForRender(bool isBlendTarget, const tTVPRect *rc) {
	if (isBlendTarget || !rc) Independ();
	else {
		int w = Bitmap->GetWidth(), h = Bitmap->GetHeight();
		if (rc->left == 0 && rc->top == 0 && rc->right >= w && rc->bottom >= h) {
			IndependNoCopy();
		} else {
			Independ();
		}
	}
	return GetTexture();
}
