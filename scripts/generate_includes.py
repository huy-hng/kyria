import json
from pathlib import Path

BASE_PATH = Path('/home/huy/repositories/kyria')
INCLUDES_PATH = BASE_PATH / 'config' / 'includes'

path_to_json = BASE_PATH / 'scripts' / 'layers.json'
path_to_defines = INCLUDES_PATH / 'generated' / 'define_layers.dtsi'
path_to_includes = INCLUDES_PATH / 'generated' / 'include_layers.dtsi'

layers_directory = '../layers/'
extension = '.keymap'

def main():
    defines = ''
    includes = ''

    files = json.loads(path_to_json.read_text())
    layer_index = 0
    for path, names in files.items():
        includes += f'#include "{layers_directory}{path}{extension}"\n'

        if names is None:
            continue

        if type(names) == str:
            names = [names]

        for n in names:
            defines += f'#define {n} {layer_index}\n'
            layer_index += 1

    path_to_includes.write_text(includes)
    path_to_defines.write_text(defines)


if __name__ == '__main__':
    main()
