from os import mkdir


def get_lines(filename):
    i = 1
    with open(filename, 'rt') as fp:

        for line in fp:

            line_stripped = line.strip()

            if line_stripped == '':
                continue

            precontent, content = line_stripped.split(':')

            if precontent.startswith('#'):
                type_ = 'title'
            elif precontent.startswith('Regex'):
                type_ = 'regex'
            elif precontent.startswith('Doc'):
                type_ = 'doc'
            else:
                raise ValueError(f'Unknown definition in line {i}')

            yield type_, content.strip()
            i += 1


def to_snake_case(string):
    return string.lower().replace(' ', '_')


def spanners_to_text(spanner_list):
    text = ''
    for spanner in spanner_list:
        pos1 = spanner.find('[')
        pos2 = spanner.find(']')
        last_pos2 = -1
        while (pos1 != -1) and (pos2 != -1):
            var_name = spanner[last_pos2 + 1:pos1]
            start, end = spanner[pos1 + 1:pos2].split(',')
            text += f'{var_name} = |{start},{end}>\t'
            last_pos2 = pos2
            pos1 = spanner.find('[', pos1 + 1)
            pos2 = spanner.find(']', pos2 + 1)
        text += '\n'
    return text


def write_test_folder(title, regex, doc, doc_number, spanners_text):
    test_name = f'test_{title}{doc_number}'
    try:
        mkdir(test_name)
    except FileExistsError:
        pass
    with open(f'{test_name}/document.txt', 'wt') as fp_doc:
        fp_doc.write(doc)
    with open(f'{test_name}/regex.txt', 'wt') as fp_regex:
        fp_regex.write(regex)
    with open(f'{test_name}/spanners.txt', 'wt') as fp_spanners:
        fp_spanners.write(spanners_text)


if __name__ == "__main__":

    filename = 'test_document.txt'
    lines = get_lines(filename)

    type_, content = next(lines)
    if type_ != 'title':
        raise ValueError('Test title expected (starting with "#")')
    title = to_snake_case(content)

    while title:

        type_, content = next(lines)
        if type_ != 'regex':
            raise ValueError('Regex expected after title (starting with "Regex:")')
        regex = content

        doc_number = 1
        type_, content = next(lines)
        while type_ == 'doc':
            doc, *spanners = content.split('-')
            spanners_text = spanners_to_text(spanners)

            write_test_folder(title, regex, doc, doc_number, spanners_text)

            type_, content = next(lines, (None, None))
            doc_number += 1

        if type_ is None:
            break

        if type_ != 'title':
            raise ValueError('Test title expected after last doc (starting with "#")')

        title = to_snake_case(content)
