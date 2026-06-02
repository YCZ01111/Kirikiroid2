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

NS_CC_BEGIN

class ControllerImpl
{
public:
    ControllerImpl(Controller* controller)
    : _controller(controller)
    {}

    Controller* _controller;
};

std::vector<Controller*> Controller::s_allController;

Controller::Controller()
: _controllerTag(TAG_UNSET)
, _impl(new ControllerImpl(this))
, _connectEvent(nullptr)
, _keyEvent(nullptr)
, _axisEvent(nullptr)
, _deviceId(0)
{
}

Controller::~Controller()
{
    delete _impl;
    delete _connectEvent;
    delete _keyEvent;
    delete _axisEvent;
}

void Controller::registerListeners() {}
void Controller::onConnected() {}
void Controller::onDisconnected() {}
void Controller::onButtonEvent(int keyCode, bool isPressed, float value, bool isAnalog) {}
void Controller::onAxisEvent(int axisCode, float value, bool isAnalog) {}

const Controller::KeyStatus& Controller::getKeyStatus(int keyCode)
{
    return _allKeyStatus[keyCode];
}

bool Controller::isConnected() const
{
    return false;
}

void Controller::receiveExternalKeyEvent(int externalKeyCode, bool receive)
{
}

Controller* Controller::getControllerByTag(int tag)
{
    for (auto& controller : s_allController)
    {
        if (controller->_controllerTag == tag)
            return controller;
    }
    return nullptr;
}

void Controller::startDiscoveryController() {}
void Controller::stopDiscoveryController() {}
void Controller::init() {}

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

NS_CC_BEGIN

class ControllerImpl
{
public:
    ControllerImpl(Controller* controller)
    : _controller(controller)
    {}

    Controller* _controller;
};

std::vector<Controller*> Controller::s_allController;

Controller::Controller()
: _controllerTag(TAG_UNSET)
, _impl(new ControllerImpl(this))
, _connectEvent(nullptr)
, _keyEvent(nullptr)
, _axisEvent(nullptr)
, _deviceId(0)
{
}

Controller::~Controller()
{
    delete _impl;
    delete _connectEvent;
    delete _keyEvent;
    delete _axisEvent;
}

void Controller::registerListeners() {}
void Controller::onConnected() {}
void Controller::onDisconnected() {}
void Controller::onButtonEvent(int keyCode, bool isPressed, float value, bool isAnalog) {}
void Controller::onAxisEvent(int axisCode, float value, bool isAnalog) {}

const Controller::KeyStatus& Controller::getKeyStatus(int keyCode)
{
    return _allKeyStatus[keyCode];
}

bool Controller::isConnected() const
{
    return false;
}

void Controller::receiveExternalKeyEvent(int externalKeyCode, bool receive)
{
}

Controller* Controller::getControllerByTag(int tag)
{
    for (auto& controller : s_allController)
    {
        if (controller->_controllerTag == tag)
            return controller;
    }
    return nullptr;
}

void Controller::startDiscoveryController() {}
void Controller::stopDiscoveryController() {}
void Controller::init() {}

NS_CC_END

#endif
EOF
    echo "Patched CCController-apple.mm with stub implementation"
fi

echo "All patches applied successfully."
