#!/usr/bin/env ruby
require 'xcodeproj'

pbx_path = ARGV[0] || 'build/Kirikiroid2/proj.ios_mac/Kirikiroid2.xcodeproj'
source_dir = ARGV[1] || 'build/Kirikiroid2/Classes/'

project = Xcodeproj::Project.open(pbx_path)

ios_target = project.targets.find { |t| t.name.include?('iOS') } || project.targets.first
puts "Using target: #{ios_target.name}"

classes_group = project.main_group.find_subpath('Classes', false)
if classes_group
  puts "Removing old Classes group files from project..."
  classes_group.files.each do |f|
    build_file = ios_target.source_build_phase.files.find { |bf| bf.file_ref == f }
    ios_target.source_build_phase.remove_build_file(build_file) if build_file
    f.remove_from_project
  end
  classes_group.remove_from_project
end

added = 0
Dir.glob(File.join(source_dir, '**/*.{cpp,mm,m,c}')).each do |file|
  # Skip platform-specific directories not needed for iOS
  next if file.include?('/win32/') || file.include?('/ARM/') || file.include?('/android/') || file.include?('/sdl/') || file.include?('/linux/')
  # Skip FFmpeg-dependent movie code (no FFmpeg on iOS)
  next if file.include?('/ffmpeg/')
  next if file.include?('/movie/krmovie')
  # Skip plugins that depend on FFmpeg
  next if file.include?('layerExMovie')
  next if file.include?('SimpleMediaFilePlayer')
  # Skip YUVSprite (uses _polyInfo from cocos2d-x 3.8+, not available in 3.6)
  next if file.include?('YUVSprite')
  # Skip XP3ArchiveRepack (depends on 7zip SDK not available on iOS)
  next if file.include?('XP3ArchiveRepack')
  # Skip VorbisWaveDecoder (depends on libvorbis not available on iOS)
  next if file.include?('VorbisWaveDecoder')
  # Skip UtilStreams (depends on libarchive and 7zip SDK)
  next if file.include?('UtilStreams')
  # Skip UIExtension (depends on cocostudio not in cocos2d-x 3.6)
  next if file.include?('UIExtension')
  # Skip ResampleImage (depends on missing aligned_allocator.h)
  next if file.include?('ResampleImage')
  # Skip LoadJXR (depends on JXRGlue.h / jxrlib not available on iOS)
  next if file.include?('LoadJXR')
  # Skip LoadBPG (depends on libbpg not available on iOS)
  next if file.include?('LoadBPG')
  # Skip LoadJPEG (depends on turbojpeg not available on iOS)
  next if file.include?('LoadJPEG')
  # Skip FreeType/FreeTypeFontRasterizer (conflicts with FreeType2 lib headers on case-insensitive macOS, only used by win32)
  next if file.include?('FreeTypeFontRasterizer')
  next if file.end_with?('/FreeType.cpp')
  # Skip FFWaveDecoder (depends on FFmpeg/libavutil not available on iOS)
  next if file.include?('FFWaveDecoder')
  # Skip 7zArchive (depends on 7zip SDK not available on iOS)
  next if file.include?('7zArchive')
  # Skip XP3RepackForm (depends on XP3ArchiveRepack which needs 7zip SDK)
  next if file.include?('XP3RepackForm')
  rel_path = file.sub(File.dirname(source_dir) + '/', '')
  xcode_path = '../' + rel_path
  group_path = File.dirname(rel_path)
  group = project.main_group.find_subpath(group_path, true)
  file_ref = group.new_file(xcode_path)
  ios_target.source_build_phase.add_file_reference(file_ref)
  added += 1
  puts "  Added: #{xcode_path}"
end

Dir.glob(File.join(source_dir, '**/*.h')).each do |file|
  rel_path = file.sub(File.dirname(source_dir) + '/', '')
  xcode_path = '../' + rel_path
  existing = project.files.find { |f| f.path == xcode_path }
  if existing.nil?
    group_path = File.dirname(rel_path)
    group = project.main_group.find_subpath(group_path, true)
    group.new_file(xcode_path)
    added += 1
    puts "  Added header: #{xcode_path}"
  end
end

project.save
puts "Total files added: #{added}"
