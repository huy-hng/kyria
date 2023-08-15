import re
from typing import NamedTuple
from pathlib import Path


BASE_PATH = Path('/home/huy/repositories/kyria/config/includes')
LAYERS_PATH = BASE_PATH / 'layers'
path_to_combo_includes = BASE_PATH / 'generated' / 'include_combos.dtsi'
combos_directory = '../combos/'


SAME_HAND_TIMEOUT = 40
OPPOSITE_HAND_TIMEOUT = 45
LEFT_SIDE_CODES = [0, 1, 2, 3, 4, 5, 12, 13, 14, 15, 16, 17, 24, 25, 26, 27, 28, 29, 30, 31, 40, 41, 42, 43, 44,]  # noqa
RIGHT_SIDE_CODES = [6,  7,  8,  9, 10, 11, 18, 19, 20, 21, 22, 23, 32, 33, 34, 35, 36, 37, 38, 39, 45, 46, 47, 48, 49]  # noqa


class ComboFile(NamedTuple):
    path: Path
    match_bindings: list[str]
    bindings_prefix: str
    key_position: str
    timeout_left: int
    timeout_right: int


# TODO: LCTRL + other keys
combo_files = [
    ComboFile(
        path=LAYERS_PATH / 'base.keymap',
        match_bindings=['&kp'],
        bindings_prefix='&combo_modifier LSHIFT ',
        key_position='t_l',
        timeout_left=SAME_HAND_TIMEOUT,
        timeout_right=OPPOSITE_HAND_TIMEOUT,
    ),
    ComboFile(
        path=LAYERS_PATH / 'symbols.keymap',
        match_bindings=['&kp'],
        bindings_prefix='&combo_layer SYMBOLS ',
        key_position='t_r',
        timeout_left=OPPOSITE_HAND_TIMEOUT,
        timeout_right=SAME_HAND_TIMEOUT
    ),
    ComboFile(
        path=LAYERS_PATH / 'os_layer.keymap',
        match_bindings=['&kp'],
        bindings_prefix='&combo_layer OS ',
        key_position='t_lo',
        timeout_left=SAME_HAND_TIMEOUT,
        timeout_right=OPPOSITE_HAND_TIMEOUT
    ),
]


def write(path, lines):
    with open(BASE_PATH + path, 'w') as f:
        f.writelines(lines)


def create_combo(name, binding, key1, key2, timeout):
    return [
        f'combo_{name}_{key1}_{key2} {{',
        f'\tbindings = <{binding}>;',
        f'\tkey-positions = <{key1} {key2}>;',
        f'\ttimeout-ms = <{timeout}>;',
        f'\tslow-release;',
        f'}};'
    ]


def check_if_correct_binding(binding, match_bindings: list[str]):
    for b in match_bindings:
        if binding.startswith(b):
            return True


def create_combo_text(combo_file: ComboFile, bindings: list[str]):
    combos = []
    for i, binding in enumerate(bindings):
        binding = binding.replace('\n', '')

        if check_if_correct_binding(binding, combo_file.match_bindings):
            binding = binding.split(' ')
            behavior = binding[0]
            param = binding[1]
            name = filter_alpha_numeric(param)
            param = combo_file.bindings_prefix + param

            timeout = combo_file.timeout_right
            if i in LEFT_SIDE_CODES:
                timeout = combo_file.timeout_left

            combo = create_combo(
                name, param, combo_file.key_position, i, timeout)
            combos.append(combo)
    return combos


def indent_array(lines, indentation=1) -> list[str]:
    for i, line in enumerate(lines):
        lines[i] = indentation * '\t' + line
    return lines


def filter_alpha_numeric(text):
    val = filter(str.isalnum, text)
    return ''.join(val)


def clean_lines(text: str):
    lines = text.splitlines()
    lines = [line.lstrip().rstrip() for line in lines]

    return [line for line in lines if len(line) > 0 and not line.startswith('//')]


def filter_bindings_from_file_working(content):
    bindings_match = re.split(bindings_pattern, content)[1]
    spaces_match = re.split(spaces_pattern, bindings_match)
    if bindings_match is None:
        return

    return filter(lambda x: not x.startswith('\n'), spaces_match)


# has to be preceeded by space or tab
#                      |              any char or new line until `>;`
#                      v                   v
# bindings_pattern = r'[ \t]bindings = <((.|\n)*?)>;'
bindings_pattern = r'[ \t]bindings = <(.*?)>;'
spaces_pattern = ' {2,}+'


# NOTE: this only returns the first found key bindings
def filter_bindings_from_file(content) -> list[str]:
    bindings_match = re.findall(bindings_pattern, content, flags=re.DOTALL)
    # if bindings_match is None:
    #     return ['']

    for match in bindings_match:
        lines = clean_lines(match)

        elements = []
        for line in lines:
            elems = re.split(spaces_pattern, line)
            elements.extend(elems)

        if len(elements) == 50:
            return elements

    return ['']


def create_file_contents(combo_file: ComboFile):
    file_content = combo_file.path.read_text()

    bindings = filter_bindings_from_file(file_content)
    combos = create_combo_text(combo_file, bindings)

    lines = ['/ {\n', '\tcombos {\n']

    for combo in combos:
        combo = indent_array(combo, 2)
        combo.append('\n')
        lines.append('\n'.join(combo))

    lines.append('\t};\n};')

    return lines


def main():
    combo_includes = ''
    for combo_file in combo_files:
        lines = create_file_contents(combo_file)

        file = combo_file.path.with_suffix('.combo').name

        path = BASE_PATH / 'combos' / file
        path.write_text(''.join(lines))

        rel_path = combos_directory + file
        combo_includes += f'#include "{rel_path}"\n'

    path_to_combo_includes.write_text(combo_includes)


if __name__ == '__main__':
    main()
