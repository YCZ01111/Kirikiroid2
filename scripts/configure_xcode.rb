#!/usr/bin/env ruby
require 'xcodeproj'

pbx_path = ARGV[0] || 'build/Kirikiroid2/proj.ios_mac/Kirikiroid2.xcodeproj'

project = Xcodeproj::Project.open(pbx_path)

ios_target = project.targets.find { |t| t.name.include?('iOS') } || project.targets.first
puts "Configuring target: #{ios_target.name}"

header_search_paths = [
  '$(SRCROOT)/../Classes',
  '$(SRCROOT)/../Classes/base',
  '$(SRCROOT)/../Classes/base/win32',
  '$(SRCROOT)/../Classes/environ',
  '$(SRCROOT)/../Classes/environ/win32',
  '$(SRCROOT)/../Classes/environ/ios',
  '$(SRCROOT)/../Classes/environ/cocos2d',
  '$(SRCROOT)/../Classes/environ/ui',
  '$(SRCROOT)/../Classes/msg',
  '$(SRCROOT)/../Classes/msg/win32',
  '$(SRCROOT)/../Classes/extension',
  '$(SRCROOT)/../Classes/sound',
  '$(SRCROOT)/../Classes/sound/win32',
  '$(SRCROOT)/../Classes/tjs2',
  '$(SRCROOT)/../Classes/utils',
  '$(SRCROOT)/../Classes/utils/win32',
  '$(SRCROOT)/../Classes/visual',
  '$(SRCROOT)/../Classes/visual/win32',
  '$(SRCROOT)/../Classes/plugins',
  '$(SRCROOT)/../Classes/movie',
  '$(SRCROOT)/../Classes/vendor/onig',
  '$(SRCROOT)/../Classes/vendor/lz4',
  '$(SRCROOT)/../Classes/vendor',
  '$(SRCROOT)/../cocos2d',
  '$(SRCROOT)/../cocos2d/cocos',
  '$(SRCROOT)/../cocos2d/cocos/base',
  '$(SRCROOT)/../cocos2d/cocos/platform/ios',
  '$(SRCROOT)/../cocos2d/cocos/platform',
  '$(SRCROOT)/../cocos2d/extensions',
  '$(SRCROOT)/../cocos2d/cocos/editor-support',
  '$(SRCROOT)/../cocos2d/external',
  '$(SRCROOT)/../cocos2d/external/include',
  '$(SRCROOT)/../cocos2d/external/sources',
  '$(SRCROOT)/../cocos2d/external/xxhash/include',
  '$(SRCROOT)/../cocos2d/external/xxhash',
  '$(SRCROOT)/../cocos2d/external/webp/include/ios',
  '$(SRCROOT)/../cocos2d/external/jpeg/include/ios',
  '$(SRCROOT)/../cocos2d/external/png/include/ios',
  '$(SRCROOT)/../cocos2d/external/tiff/include/ios',
  '$(SRCROOT)/../cocos2d/external/zip/include',
  '$(SRCROOT)/../cocos2d/external/curl/include/ios',
  '$(SRCROOT)/../cocos2d/external/freetype2/include/ios',
  '$(SRCROOT)/../cocos2d/external/websockets/include/ios',
  '$(SRCROOT)/../opencv2.framework/Headers'
]

preprocessor_defs = [
  '$(inherited)',
  'CC_TARGET_OS_IPHONE',
  'TJS_TEXT_OUT_CRLF',
  '__STDC_CONSTANT_MACROS',
  '__STDC_FORMAT_MACROS',
  'USE_UNICODE_FSTRING',
  'CC_TARGET_PLATFORM=CC_PLATFORM_IOS',
  'NDEBUG',
  'COCOS2DX_VERSION=0x00030600',
  'SIZEOF_INT=4',
  'SIZEOF_SHORT=2',
  'SIZEOF_LONG=8',
  'SIZEOF_VOIDP=8',
  'SIZEOF_LONG_LONG=8',
  'SIZEOF_SIZE_T=8',
  'SIZEOF_OFF_T=8'
]

ios_target.build_configurations.each do |config|
  config.build_settings['HEADER_SEARCH_PATHS'] = header_search_paths
  config.build_settings['GCC_PREPROCESSOR_DEFINITIONS'] = preprocessor_defs
  config.build_settings['CLANG_CXX_LANGUAGE_STANDARD'] = 'c++11'
  config.build_settings['CLANG_CXX_LIBRARY'] = 'libc++'
  config.build_settings['GCC_ENABLE_CPP_EXCEPTIONS'] = 'YES'
  config.build_settings['GCC_ENABLE_CPP_RTTI'] = 'YES'
  config.build_settings['IPHONEOS_DEPLOYMENT_TARGET'] = '12.0'
  config.build_settings['ENABLE_BITCODE'] = 'NO'
  config.build_settings['CODE_SIGNING_ALLOWED'] = 'NO'
  config.build_settings['LIBRARY_SEARCH_PATHS'] ||= ['$(inherited)']
  config.build_settings['FRAMEWORK_SEARCH_PATHS'] = ['$(inherited)', '$(SRCROOT)/..']
  config.build_settings['OTHER_LDFLAGS'] = ['$(inherited)', '-framework', 'opencv2']
  config.build_settings['GCC_WARN_ABOUT_MISSING_PROTOTYPES'] = 'NO'
  config.build_settings['GCC_WARN_ABOUT_RETURN_TYPE'] = 'NO'
  config.build_settings['GCC_WARN_UNUSED_VARIABLE'] = 'NO'
  config.build_settings['CLANG_WARN_SUSPICIOUS_MOVE'] = 'NO'
  config.build_settings['CLANG_WARN_OBJC_IMPLICIT_RETAIN_SELF'] = 'NO'
  puts "  Configured build settings for: #{config.name}"
end

# Add opencv2.framework to the project
frameworks_group = project.main_group.find_subpath('Frameworks', true)
unless frameworks_group
  frameworks_group = project.main_group.new_group('Frameworks')
end

# Check if opencv2.framework is already added
existing_ref = frameworks_group.files.find { |f| f.path && f.path.include?('opencv2') }
unless existing_ref
  framework_ref = frameworks_group.new_file('../opencv2.framework')
  ios_target.frameworks_build_phase.add_file_reference(framework_ref)
  puts "Added opencv2.framework to project"
end

project.save
puts "Xcode project configured successfully."
