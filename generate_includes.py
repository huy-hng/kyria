import json

layers_directory = 'keymaps/'
layers_extension = '.keymap'

path_to_json = './includes/layers.json'
path_to_defines = './includes/layer_defines.dtsi'
path_to_includes = './includes/layer_includes.dtsi'

def get_files():
    with open(path_to_json) as f:
        return json.load(f)

def main():
    defines = ''
    includes = ''

    layer_index = 0
    for path, names in get_files().items():
        includes += f'#include "{layers_directory}{path}{layers_extension}"\n'

        if names is None:
            continue

        if type(names) == str:
            names = [names]

        for n in names:
            defines += f'#define {n} {layer_index}\n'
            layer_index += 1

    with open(path_to_includes, 'w') as f:
        f.write(includes)

    with open(path_to_defines, 'w') as f:
        f.write(defines)

if __name__ == '__main__':
    main()
