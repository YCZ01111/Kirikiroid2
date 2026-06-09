//---------------------------------------------------------------------------
// iOS stub implementation for BasicDrawDevice
// Provides tTVPBasicDrawDevice, tTJSNI_BasicDrawDevice, tTJSNC_BasicDrawDevice
//---------------------------------------------------------------------------
#include "tjsCommHead.h"
#include "BasicDrawDevice.h"
#include "LayerBitmapIntf.h"

//---------------------------------------------------------------------------
// tTVPBasicDrawDevice
//---------------------------------------------------------------------------
tTVPBasicDrawDevice::tTVPBasicDrawDevice()
{
	Direct3D = nullptr;
	Direct3DDevice = nullptr;
	Texture = nullptr;
	TextureBuffer = nullptr;
	TargetWindow = nullptr;
	IsMainWindow = false;
	DrawUpdateRectangle = false;
	BackBufferDirty = false;
	TextureWidth = 0;
	TextureHeight = 0;
	ShouldShow = false;
	VsyncInterval = 0;
}

tTVPBasicDrawDevice::~tTVPBasicDrawDevice()
{
}

void tTVPBasicDrawDevice::InvalidateAll()
{
}

void tTVPBasicDrawDevice::TryRecreateWhenDeviceLost()
{
}

void tTVPBasicDrawDevice::TJS_INTF_METHOD AddLayerManager(iTVPLayerManager * manager)
{
}

void tTVPBasicDrawDevice::TJS_INTF_METHOD SetDestRectangle(const tTVPRect & rect)
{
}

void tTVPBasicDrawDevice::TJS_INTF_METHOD NotifyLayerResize(iTVPLayerManager * manager)
{
}

void tTVPBasicDrawDevice::TJS_INTF_METHOD Show()
{
}

void tTVPBasicDrawDevice::TJS_INTF_METHOD StartBitmapCompletion(iTVPLayerManager * manager)
{
}

void tTVPBasicDrawDevice::TJS_INTF_METHOD NotifyBitmapCompleted(iTVPLayerManager * manager,
	tjs_int x, tjs_int y, tTVPBaseTexture * bmp,
	const tTVPRect &cliprect, tTVPLayerType type, tjs_int opacity)
{
}

void tTVPBasicDrawDevice::TJS_INTF_METHOD EndBitmapCompletion(iTVPLayerManager * manager)
{
}

void tTVPBasicDrawDevice::TJS_INTF_METHOD SetShowUpdateRect(bool b)
{
}

//---------------------------------------------------------------------------
// tTJSNI_BasicDrawDevice
//---------------------------------------------------------------------------
tTJSNI_BasicDrawDevice::tTJSNI_BasicDrawDevice()
{
	Device = nullptr;
}

tTJSNI_BasicDrawDevice::~tTJSNI_BasicDrawDevice()
{
	if(Device) delete Device;
}

tjs_error TJS_INTF_METHOD tTJSNI_BasicDrawDevice::Construct(tjs_int numparams, tTJSVariant **param,
	iTJSDispatch2 *tjs_obj)
{
	Device = new tTVPBasicDrawDevice();
	return TJS_S_OK;
}

void TJS_INTF_METHOD tTJSNI_BasicDrawDevice::Invalidate()
{
	if(Device) { delete Device; Device = nullptr; }
}

//---------------------------------------------------------------------------
// tTJSNC_BasicDrawDevice
//---------------------------------------------------------------------------
tjs_uint32 tTJSNC_BasicDrawDevice::ClassID = (tjs_uint32)(&tTJSNC_BasicDrawDevice::ClassID);

tTJSNC_BasicDrawDevice::tTJSNC_BasicDrawDevice() : tTJSNativeClass(TJS_W("BasicDrawDevice"))
{
}

iTJSNativeInstance *tTJSNC_BasicDrawDevice::CreateNativeInstance()
{
	return new tTJSNI_BasicDrawDevice();
}
