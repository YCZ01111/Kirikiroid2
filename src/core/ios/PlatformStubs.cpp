//---------------------------------------------------------------------------
// iOS platform stub implementations
// Provides missing symbols that are normally implemented in win32/ files
//---------------------------------------------------------------------------
#include "tjsCommHead.h"

#include "SysInitIntf.h"
#include "SystemIntf.h"
#include "EventIntf.h"
#include "StorageIntf.h"
#include "PluginIntf.h"
#include "ScriptMgnIntf.h"
#include "ClipboardIntf.h"
#include "MsgIntf.h"
#include "DebugIntf.h"
#include "TimerIntf.h"
#include "LayerIntf.h"
#include "WindowIntf.h"
#include "MenuItemIntf.h"
#include "VideoOvlIntf.h"
#include "SoundBufferBaseIntf.h"
#include "CDDAIntf.h"
#include "MIDIIntf.h"
#include "WaveIntf.h"
#include "Platform.h"
#include "tvpfontstruc.h"
#include "TVPSysFont.h"
#include "BitmapInfomation.h"
#include "GraphicsLoaderIntf.h"
#include "win32/StorageImpl.h"
#include "win32/SysInitImpl.h"
#include "win32/SystemImpl.h"
#include "win32/WindowImpl.h"
#include "win32/SystemControl.h"
#include "ui/extension/UIExtension.h"

#include <string>

//---------------------------------------------------------------------------
// SDL stub (not needed on iOS)
//---------------------------------------------------------------------------
extern "C" void SDL_SetMainReady(void) {}

//---------------------------------------------------------------------------
// Global variables from SysInitImpl
//---------------------------------------------------------------------------
ttstr TVPNativeProjectDir;
bool TVPTerminated = false;
bool TVPTerminateOnNoWindowStartup = false;
int TVPTerminateCode = 0;

//---------------------------------------------------------------------------
// SysInit stubs
//---------------------------------------------------------------------------
void TVPBeforeSystemInit() {}
void TVPAfterSystemInit() {}
void TVPBeforeSystemUninit() {}
void TVPAfterSystemUninit() {}
void TVPInitializeBaseSystems() {}
bool TVPExecuteUserConfig() { return false; }
void TVPEnsureDataPathDirectory() {}
bool TVPCheckPrintDataPath() { return false; }

//---------------------------------------------------------------------------
// System stubs
//---------------------------------------------------------------------------
void TVPDoSaveSystemVariables() {}
void TVPPostApplicationActivateEvent() {}
void TVPPostApplicationDeactivateEvent() {}
ttstr TVPReadAboutStringFromResource() { return ttstr(); }

//---------------------------------------------------------------------------
// Platform stubs
//---------------------------------------------------------------------------
std::vector<std::string> TVPGetAppStoragePath() {
	std::vector<std::string> paths;
	paths.push_back("./");
	return paths;
}

bool TVPCheckStartupPath(const std::string &path) { return true; }

std::string TVPGetPackageVersionString() { return "1.0.0"; }

tjs_int TVPGetSystemFreeMemory() { return 512; }
tjs_int TVPGetSelfUsedMemory() { return 0; }

std::string TVPGetCurrentLanguage() { return "ja"; }

bool TVP_stat(const tjs_char *name, tTVP_stat &s) {
	memset(&s, 0, sizeof(s));
	return false;
}

bool TVP_stat(const char *name, tTVP_stat &s) {
	memset(&s, 0, sizeof(s));
	return false;
}

//---------------------------------------------------------------------------
// Storage stubs
//---------------------------------------------------------------------------
void TVPPreNormalizeStorageName(ttstr &name) {}

bool TVPCheckExistentLocalFile(const ttstr &name) {
	return false; // stub
}

bool TVPCheckExistentLocalFolder(const ttstr &name) {
	return false; // stub
}

void TVPGetLocalFileListAt(const ttstr &name, const std::function<void(const ttstr&, tTVPLocalFileInfo*)>& cb) {
	// stub - no file listing on iOS
}

bool TVPSaveStreamToFile(tTJSBinaryStream *st, tjs_uint64 offset, tjs_uint64 size, ttstr outpath) {
	return false;
}

bool TVPCheckIsVideoFile(const char *uri) { return false; }

//---------------------------------------------------------------------------
// Clipboard stubs
//---------------------------------------------------------------------------
bool TVPClipboardHasFormat(tTVPClipboardFormat format) { return false; }
void TVPClipboardSetText(const ttstr &text) {}
bool TVPClipboardGetText(ttstr &text) { return false; }

//---------------------------------------------------------------------------
// Input box stub
//---------------------------------------------------------------------------
int TVPShowSimpleInputBox(ttstr &text, const ttstr &caption, const ttstr &prompt, const std::vector<ttstr> &vecButtons) {
	return 0;
}

//---------------------------------------------------------------------------
// Event stubs
//---------------------------------------------------------------------------
static bool TVPSystemEventDisabled = false;
void TVPSetSystemEventDisabledState(bool en) { TVPSystemEventDisabled = en; }
bool TVPGetSystemEventDisabledState() { return TVPSystemEventDisabled; }
void TVPBeginContinuousEvent() {}

//---------------------------------------------------------------------------
// Sound stubs
//---------------------------------------------------------------------------
void TVPUnlockSoundMixer() {}
void TVPResetVolumeToAllSoundBuffer() {}

//---------------------------------------------------------------------------
// XP3 Repack stub
//---------------------------------------------------------------------------
void TVPProcessXP3Repack(const std::string &dir) {}

//---------------------------------------------------------------------------
// CreateNativeClass stubs
//---------------------------------------------------------------------------
tTJSNativeClass * TVPCreateNativeClass_Debug() { return nullptr; }
tTJSNativeClass * TVPCreateNativeClass_Layer() { return nullptr; }
tTJSNativeClass * TVPCreateNativeClass_Timer() { return nullptr; }
tTJSNativeClass * TVPCreateNativeClass_System() { return nullptr; }
tTJSNativeClass * TVPCreateNativeClass_Window() { return nullptr; }
tTJSNativeClass * TVPCreateNativeClass_Plugins() { return nullptr; }
tTJSNativeClass * TVPCreateNativeClass_MenuItem() { return nullptr; }
tTJSNativeClass * TVPCreateNativeClass_Storages() { return nullptr; }
tTJSNativeClass * TVPCreateNativeClass_VideoOverlay() { return nullptr; }
tTJSNativeClass * TVPCreateNativeClass_CDDASoundBuffer() { return nullptr; }
tTJSNativeClass * TVPCreateNativeClass_MIDISoundBuffer() { return nullptr; }
tTJSNativeClass * TVPCreateNativeClass_WaveSoundBuffer() { return nullptr; }

//---------------------------------------------------------------------------
// BitmapInfomation stubs
//---------------------------------------------------------------------------
BitmapInfomation::BitmapInfomation(tjs_uint width, tjs_uint height, tjs_int bpp) {
}
BitmapInfomation::~BitmapInfomation() {
}

//---------------------------------------------------------------------------
// tTJSNI_Window::Close stub
//---------------------------------------------------------------------------
void tTJSNI_Window::Close() {
}

//---------------------------------------------------------------------------
// tTVPSystemControl stubs
//---------------------------------------------------------------------------
tTVPSystemControl *TVPSystemControl = nullptr;
bool TVPSystemControlAlive = false;

tTVPSystemControl::tTVPSystemControl() {
	ContinuousEventCalling = false;
	AutoShowConsoleOnError = false;
	EventEnable = true;
	LastCompactedTick = 0;
	LastCloseClickedTick = 0;
	LastShowModalWindowSentTick = 0;
	LastRehashedTick = 0;
	MixedIdleTick = 0;
}

void tTVPSystemControl::SystemWatchTimerTimer() {
}

//---------------------------------------------------------------------------
// XKPageView stubs (UIExtension is excluded from iOS build)
//---------------------------------------------------------------------------
XKPageView *XKPageView::create(cocos2d::Size size, XKPageViewDelegate *delegate) {
	return nullptr;
}
void XKPageView::addPage(cocos2d::Node *node) {
}
void XKPageView::setCurPageIndex(ssize_t idx) {
}

//---------------------------------------------------------------------------
// Additional missing symbols
//---------------------------------------------------------------------------
void TVPListDir(const std::string &path, const std::function<void(const std::string&, int)>& cb) {}

void TVPLoadBPG(void*, void*, int(*)(void*, unsigned int, unsigned int, tTVPGraphicPixelFormat),
	void*(*)(void*, int), void(*)(void*, const ttstr&, const ttstr&),
	tTJSBinaryStream*, int, tTVPGraphicLoadMode) {}

void TVPLoadJXR(void*, void*, int(*)(void*, unsigned int, unsigned int, tTVPGraphicPixelFormat),
	void*(*)(void*, int), void(*)(void*, const ttstr&, const ttstr&),
	tTJSBinaryStream*, int, tTVPGraphicLoadMode) {}

void TVPLoadJPEG(void*, void*, int(*)(void*, unsigned int, unsigned int, tTVPGraphicPixelFormat),
	void*(*)(void*, int), void(*)(void*, const ttstr&, const ttstr&),
	tTJSBinaryStream*, int, tTVPGraphicLoadMode) {}

void TVPSaveAsJPG(void*, tTJSBinaryStream*, const iTVPBaseBitmap*, const ttstr&, iTJSDispatch2*) {}
void TVPSaveAsJXR(void*, tTJSBinaryStream*, const iTVPBaseBitmap*, const ttstr&, iTJSDispatch2*) {}

void TVPCheckAbout() {}

bool TVPDeleteFile(const std::string &filename) { return false; }

std::string TVPGetAppPath() {
	return "./";
}

tjs_uint32 TVPGetVersion() { return 0x02000000; } // version 2.0.0.0

bool TVPRenameFile(const std::string &from, const std::string &to) { return false; }

tTVPArchive * TVPOpenArchive(const ttstr &name, bool ensure) { return nullptr; }
bool TVPCheckArchive(const ttstr &name) { return false; }

void TVPInvokeEvents() {}

void TVPLoadPluigins() {}

void TVPCreateFolders(const ttstr &name) {}

std::vector<std::string> TVPGetDriverPath() {
	std::vector<std::string> paths;
	paths.push_back("./");
	return paths;
}

void TVPLoadHeaderBPG(void*, tTJSBinaryStream*, iTJSDispatch2**) {}
void TVPLoadHeaderJPG(void*, tTJSBinaryStream*, iTJSDispatch2**) {}
void TVPLoadHeaderJXR(void*, tTJSBinaryStream*, iTJSDispatch2**) {}

void TVPTerminateSync(int code) {}

tjs_uint32 TVPToActualColor(tjs_uint32 color) { return color; }

void TVPGetCommandLine(const tjs_char*, tTJSVariant*) {}

void TVPLockSoundMixer() {}

void TVPSendToOtherApp(const std::string &filename) {}

void TVPAcceptSaveAsJPG(void*, const ttstr&, iTJSDispatch2**) {}
void TVPAcceptSaveAsJXR(void*, const ttstr&, iTJSDispatch2**) {}

bool TVPCheckStartupArg() { return false; }

void TVPControlAdDialog(int, int, int) {}

tTVPArchive * TVPCreateFileMedia() { return nullptr; }

void TVPDumpHWException() {}

void TVPExitApplication(int code) {}

void TVPForceSwapBuffer() {}

tjs_uint32 TVPFromActualColor(tjs_uint32 color) { return color; }

iTJSDispatch2 * TVPGetMenuDispatch(tjs_int64 tag) { return nullptr; }

void TVPInitUIExtension() {}

bool TVPWriteDataToFile(const ttstr &name, const void *data, tjs_uint size) { return false; }

tjs_uint32 TVPGetRoughTickCount32() {
	return (tjs_uint32)(clock() * 1000 / CLOCKS_PER_SEC);
}
