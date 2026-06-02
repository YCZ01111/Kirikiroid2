#!/usr/bin/env python3
import os
import sys
from pbxproj import XcodeProject

def add_sources(pbxproj_path, source_dir, target_name='Kirikiroid2'):
    project = XcodeProject.load(pbxproj_path)
    target = project.get_target_by_name(target_name)
    if not target:
        print(f"Target '{target_name}' not found, using first native target")
        targets = project.objects.get_targets_in_phase()
        if targets:
            target = targets[0]

    added = 0
    for root, dirs, files in os.walk(source_dir):
        for f in files:
            if f.endswith(('.cpp', '.mm', '.m', '.c')):
                full = os.path.join(root, f)
                rel = os.path.relpath(full, os.path.dirname(source_dir))
                existing = project.get_files_by_os_path(rel)
                if not existing:
                    project.add_file(rel, parent='Classes', target_name=target_name)
                    added += 1
                    print(f"  Added: {rel}")
            elif f.endswith('.h'):
                full = os.path.join(root, f)
                rel = os.path.relpath(full, os.path.dirname(source_dir))
                existing = project.get_files_by_os_path(rel)
                if not existing:
                    project.add_file(rel, parent='Classes', target_name=target_name)
                    added += 1
                    print(f"  Added header: {rel}")

    project.save()
    print(f"Total files added: {added}")

if __name__ == '__main__':
    pbx = sys.argv[1] if len(sys.argv) > 1 else 'build/Kirikiroid2/proj.ios_mac/Kirikiroid2.xcodeproj/project.pbxproj'
    src = sys.argv[2] if len(sys.argv) > 2 else 'build/Kirikiroid2/Classes/'
    add_sources(pbx, src)
