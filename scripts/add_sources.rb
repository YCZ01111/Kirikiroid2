#!/usr/bin/env ruby
require 'xcodeproj'

pbx_path = ARGV[0] || 'build/Kirikiroid2/proj.ios_mac/Kirikiroid2.xcodeproj'
source_dir = ARGV[1] || 'build/Kirikiroid2/Classes/'

project = Xcodeproj::Project.open(pbx_path)
target = project.targets.first

added = 0
Dir.glob(File.join(source_dir, '**/*.{cpp,mm,m,c}')).each do |file|
  rel_path = file.sub(File.dirname(source_dir) + '/', '')
  existing = project.files.find { |f| f.path == rel_path }
  if existing.nil?
    group_path = File.dirname(rel_path)
    group = project.main_group.find_subpath(group_path, true)
    file_ref = group.new_file(rel_path)
    target.source_build_phase.add_file_reference(file_ref)
    added += 1
    puts "  Added: #{rel_path}"
  end
end

Dir.glob(File.join(source_dir, '**/*.h')).each do |file|
  rel_path = file.sub(File.dirname(source_dir) + '/', '')
  existing = project.files.find { |f| f.path == rel_path }
  if existing.nil?
    group_path = File.dirname(rel_path)
    group = project.main_group.find_subpath(group_path, true)
    group.new_file(rel_path)
    added += 1
    puts "  Added header: #{rel_path}"
  end
end

project.save
puts "Total files added: #{added}"
