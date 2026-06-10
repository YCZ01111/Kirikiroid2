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
#include "win32/StorageImpl.h"
#include "win32/SysInitImpl.h"
#include "win32/SystemImpl.h"
#include "win32/WindowImpl.h"
#include "win32/SystemControl.h"
#include "ui/extension/UIExtension.h"

#include <sys/stat.h>
#include <dirent.h>

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
	struct stat st;
	if(stat(name, &st) != 0) return false;
	s.st_mode = st.st_mode;
	s.st_size = st.st_size;
	s.st_atime = st.st_atime;
	s.st_mtime = st.st_mtime;
	s.st_ctime = st.st_ctime;
	return true;
}

bool TVP_stat(const char *name, tTVP_stat &s) {
	struct stat st;
	if(stat(name, &st) != 0) return false;
	s.st_mode = st.st_mode;
	s.st_size = st.st_size;
	s.st_atime = st.st_atime;
	s.st_mtime = st.st_mtime;
	s.st_ctime = st.st_ctime;
	return true;
}

//---------------------------------------------------------------------------
// Storage stubs
//---------------------------------------------------------------------------
void TVPPreNormalizeStorageName(ttstr &name) {}

bool TVPCheckExistentLocalFile(const ttstr &name) {
	struct stat st;
	return stat(name.AsStdString().c_str(), &st) == 0 && !S_ISDIR(st.st_mode);
}

bool TVPCheckExistentLocalFolder(const ttstr &name) {
	struct stat st;
	return stat(name.AsStdString().c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

void TVPGetLocalFileListAt(const ttstr &name, const std::function<void(const ttstr&, tTVPLocalFileInfo*)>& cb) {
	std::string path = name.AsStdString();
	DIR *dir = opendir(path.c_str());
	if(!dir) return;
	struct dirent *entry;
	while((entry = readdir(dir)) != nullptr) {
		if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
		std::string fullpath = path + "/" + entry->d_name;
		struct stat st;
		if(stat(fullpath.c_str(), &st) == 0) {
			tTVPLocalFileInfo info;
			info.NativeName = entry->d_name;
			info.Mode = S_ISDIR(st.st_mode) ? S_IFDIR : S_IFREG;
			info.Size = st.st_size;
			info.AccessTime = st.st_atime;
			info.ModifyTime = st.st_mtime;
			info.CreationTime = st.st_ctime;
			cb(ttstr(entry->d_name), &info);
		}
	}
	closedir(dir);
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
