#!/usr/bin/env python3
import sys
import os

build_dir = sys.argv[1] if len(sys.argv) > 1 else 'build/Kirikiroid2'

assets_mgr = os.path.join(build_dir, 'cocos2d/extensions/assets-manager/AssetsManager.cpp')

if os.path.exists(assets_mgr):
    with open(assets_mgr, 'r') as f:
        content = f.read()

    if 'system(command.c_str())' in content:
        lines = content.split('\n')
        new_lines = []
        i = 0
        patched = False
        while i < len(lines):
            line = lines[i]
            if (not patched and
                line.strip() == '#else' and
                i + 1 < len(lines) and 'rm -r' in lines[i + 1]):
                indent = len(line) - len(line.lstrip())
                prefix = ' ' * indent
                new_lines.append(prefix + '#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)')
                new_lines.append(prefix + '    FileUtils::getInstance()->removeDirectory(_storagePath.c_str());')
                new_lines.append(line)
                patched = True
            else:
                new_lines.append(line)
            i += 1
        if patched:
            content = '\n'.join(new_lines)
            with open(assets_mgr, 'w') as f:
                f.write(content)
            print('Patched AssetsManager.cpp: added iOS branch using FileUtils::removeDirectory()')
        else:
            print('AssetsManager.cpp: no matching #else+rm -r pattern found')
    else:
        print('AssetsManager.cpp: no system() call found, skipping')
else:
    print(f'WARNING: AssetsManager.cpp not found at {assets_mgr}')

print('All Python patches applied successfully.')
