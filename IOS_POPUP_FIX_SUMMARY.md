# iOS Popup Crash Fix Summary

## Problem
The iOS application was crashing when showing popup dialogs. The reasons were:
1. Missing iOS-specific implementation of `TVPShowSimpleMessageBox`
2. Manual `retain()`/`release()` calls conflicting with ARC
3. Potential thread safety issues when UI operations were called from non-main threads

## Solution Implemented

### 1. iOS Native Popup Implementation (`iOSUtils.mm`)
- Created proper iOS implementation using `UIAlertController`
- Thread-safe implementation that routes all UI calls to main thread
- Proper synchronization using condition variables
- Correctly declared extern variable for `TVPMainThreadID`

### 2. Memory Management Fix in `MessageBox.cpp`
- Replaced manual `retain()`/`release()` with `RefPtr` on iOS
- Prevents memory management conflicts with ARC
- Maintains object lifecycle safety

### 3. Thread Safety
- Automatic detection and routing of non-main thread calls to main thread
- Uses Cocos2d's `performFunctionInCocosThread` for thread-safe UI operations

## Files Modified/Added
1. `src/core/environ/ios/iOSUtils.mm` - **NEW** iOS popup implementation
2. `src/core/environ/ui/MessageBox.cpp` - **MODIFIED** memory management fix
3. `.github/workflows/release.yml` - **UPDATED** CI/CD workflow

## Tags Created
- `v1.0.1` - Initial fix
- `v1.0.2` - Renamed to proper .mm extension
- `v1.0.3` - Added extern declaration for TVPMainThreadID

## How to Use
These changes will be incorporated into the next iOS build. The popup dialogs should now work correctly without crashing.
