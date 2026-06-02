#!/bin/bash
set -e

BUILD_DIR="$1"
if [ -z "$BUILD_DIR" ]; then
    echo "Usage: $0 <build_dir>"
    exit 1
fi

echo "Patching cocos2d-x files for Xcode 15/iOS 17 SDK compatibility..."

CCCTRL_FILE="$BUILD_DIR/cocos2d/cocos/base/CCController-iOS.mm"
if [ -f "$CCCTRL_FILE" ]; then
    cat > "$CCCTRL_FILE" << 'EOF'
#include "CCController.h"
#include "platform/CCPlatformConfig.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#include "ccMacros.h"
#include "CCEventController.h"

NS_CC_BEGIN

class ControllerImpl
{
public:
    ControllerImpl(Controller* controller)
    : _controller(controller)
    {}

    Controller* _controller;
};

Controller::Controller()
: _controllerTag(TAG_UNSET)
, _impl(new ControllerImpl(this))
, _connectEvent(nullptr)
, _keyEvent(nullptr)
, _axisEvent(nullptr)
, _deviceId(0)
{
    init();
}

Controller::~Controller()
{
    delete _impl;
    delete _connectEvent;
    delete _keyEvent;
    delete _axisEvent;
}

void Controller::registerListeners() {}

bool Controller::isConnected() const
{
    return false;
}

void Controller::receiveExternalKeyEvent(int externalKeyCode, bool receive)
{
}

void Controller::startDiscoveryController() {}

void Controller::stopDiscoveryController() {}

NS_CC_END

#endif
EOF
    echo "Patched CCController-iOS.mm with stub implementation (no GameController.h)"
else
    echo "WARNING: CCController-iOS.mm not found at $CCCTRL_FILE"
fi

CCCTRL_APPLE="$BUILD_DIR/cocos2d/cocos/base/CCController-apple.mm"
if [ -f "$CCCTRL_APPLE" ]; then
    cat > "$CCCTRL_APPLE" << 'EOF'
#include "CCController.h"
#include "platform/CCPlatformConfig.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#include "ccMacros.h"
#include "CCEventController.h"

NS_CC_BEGIN

class ControllerImpl
{
public:
    ControllerImpl(Controller* controller)
    : _controller(controller)
    {}

    Controller* _controller;
};

Controller::Controller()
: _controllerTag(TAG_UNSET)
, _impl(new ControllerImpl(this))
, _connectEvent(nullptr)
, _keyEvent(nullptr)
, _axisEvent(nullptr)
, _deviceId(0)
{
    init();
}

Controller::~Controller()
{
    delete _impl;
    delete _connectEvent;
    delete _keyEvent;
    delete _axisEvent;
}

void Controller::registerListeners() {}

bool Controller::isConnected() const
{
    return false;
}

void Controller::receiveExternalKeyEvent(int externalKeyCode, bool receive)
{
}

void Controller::startDiscoveryController() {}

void Controller::stopDiscoveryController() {}

NS_CC_END

#endif
EOF
    echo "Patched CCController-apple.mm with stub implementation"
fi

python3 scripts/patch_cocos2d.py "$BUILD_DIR"

echo "All patches applied successfully."
