import os
import re

def add_kdoc_to_file(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()

    lines = content.split('\n')
    new_lines = []
    
    i = 0
    while i < len(lines):
        line = lines[i]
        match = re.match(r'^(\s*)(public\s+)?(class|interface|fun|val|var)\s+([a-zA-Z0-9_]+)', line)
        if match and 'override' not in line and 'private' not in line and 'internal' not in line:
            # Check if there is already a comment
            has_comment = False
            if i > 0 and ('*/' in lines[i-1] or '/**' in lines[i-1]):
                has_comment = True
            if i > 1 and '*/' in lines[i-2]:
                has_comment = True
                
            if not has_comment:
                indent = match.group(1)
                decl_type = match.group(3)
                name = match.group(4)
                
                desc = name
                if decl_type in ['class', 'interface']:
                    desc = f"Represents {name}."
                elif decl_type == 'fun':
                    desc = f"Executes {name}."
                elif decl_type in ['val', 'var']:
                    desc = f"Property {name}."
                    
                kdoc = f"{indent}/**\n{indent} * {desc}\n{indent} */"
                new_lines.append(kdoc)
        
        new_lines.append(line)
        i += 1
        
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write('\n'.join(new_lines))

for root, _, files in os.walk('/data/data/com.termux/files/home/novelua/modules'):
    for f in files:
        if f.endswith('.kt') and 'test' not in root:
            add_kdoc_to_file(os.path.join(root, f))
