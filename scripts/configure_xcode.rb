#!/usr/bin/env ruby
require 'xcodeproj'

pbx_path = ARGV[0] || 'build/Kirikiroid2/proj.ios_mac/Kirikiroid2.xcodeproj'

project = Xcodeproj::Project.open(pbx_path)

ios_target = project.targets.find { |t| t.name.include?('iOS') } || project.targets.first
puts "Configuring target: #{ios_target.name}"

header_search_paths = [
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
  '$(SRCROOT)/../cocos2d',
  '$(SRCROOT)/../cocos2d/cocos',
  '$(SRCROOT)/../cocos2d/cocos/platform/ios',
  '$(SRCROOT)/../cocos2d/cocos/platform',
  '$(SRCROOT)/../cocos2d/external',
  '$(SRCROOT)/../cocos2d/external/include',
  '$(SRCROOT)/../cocos2d/external/sources',
  '$(SRCROOT)/../cocos2d/external/webp/include/ios',
  '$(SRCROOT)/../cocos2d/external/jpeg/include/ios',
  '$(SRCROOT)/../cocos2d/external/png/include/ios',
  '$(SRCROOT)/../cocos2d/external/tiff/include/ios',
  '$(SRCROOT)/../cocos2d/external/zip/include'
]

ios_target.build_configurations.each do |config|
  config.build_settings['HEADER_SEARCH_PATHS'] = header_search_paths
  config.build_settings['GCC_PREPROCESSOR_DEFINITIONS'] ||= ['$(inherited)']
  config.build_settings['GCC_PREPROCESSOR_DEFINITIONS'] << 'TJS_TEXT_OUT_CRLF'
  config.build_settings['GCC_PREPROCESSOR_DEFINITIONS'] << '__STDC_CONSTANT_MACROS'
  config.build_settings['GCC_PREPROCESSOR_DEFINITIONS'] << 'USE_UNICODE_FSTRING'
  config.build_settings['CLANG_CXX_LANGUAGE_STANDARD'] = 'c++11'
  config.build_settings['CLANG_CXX_LIBRARY'] = 'libc++'
  config.build_settings['GCC_ENABLE_CPP_EXCEPTIONS'] = 'YES'
  config.build_settings['GCC_ENABLE_CPP_RTTI'] = 'YES'
  config.build_settings['IPHONEOS_DEPLOYMENT_TARGET'] = '12.0'
  config.build_settings['ENABLE_BITCODE'] = 'NO'
  config.build_settings['CODE_SIGNING_ALLOWED'] = 'NO'
  puts "  Configured build settings for: #{config.name}"
end

project.save
puts "Xcode project configured successfully."
