import os
import re

def check_file(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Split by lines
    lines = content.split('\n')
    for i, line in enumerate(lines):
        if re.match(r'^\s*(public\s+)?(class|interface|fun|val|var)\s+[A-Z]', line) or \
           re.match(r'^\s*(public\s+)?fun\s+[a-z]', line):
            # Check if previous lines have /**
            # This is a rudimentary check
            if not ('/**' in lines[i-1] or '*/' in lines[i-1] or '*/' in lines[i-2]):
                if 'override' not in line and 'private' not in line and 'internal' not in line:
                    print(f"Missing KDoc in {filepath}:{i+1} -> {line.strip()}")

for root, _, files in os.walk('/data/data/com.termux/files/home/novelua/modules'):
    for f in files:
        if f.endswith('.kt') and 'test' not in root:
            check_file(os.path.join(root, f))
