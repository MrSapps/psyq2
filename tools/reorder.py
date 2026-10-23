#!/usr/bin/env python3
"""Put the function definitions of C files into a given order.

usage: reorder.py ORDER_FILE file.c...     (ORDER_FILE: one function name per line)

The file is split into top-level items (preprocessor lines, declarations,
function definitions, each with the comments and blank lines in front of it).
Items that are not definitions of a listed function keep their relative order
and go first; the listed functions follow in ORDER_FILE order.
"""
import re
import sys


def items(src):
    """Split src into top-level items: (text, function name or None)."""
    out = []
    i, n = 0, len(src)
    start = 0
    depth = 0
    head_end = None          # where the first '{' of the item was seen

    def flush(end, name=None):
        nonlocal start
        out.append([src[start:end], name])
        start = end

    while i < n:
        c = src[i]
        if src.startswith('/*', i):
            i = src.index('*/', i) + 2
            continue
        if src.startswith('//', i):
            i = src.index('\n', i)
            continue
        if c in '"\'':
            j = i + 1
            while src[j] != c:
                j += 2 if src[j] == '\\' else 1
            i = j + 1
            continue
        if c == '#' and depth == 0 and only_ws_comments(src[start:i]):
            # preprocessor line (with continuations)
            j = i
            while True:
                e = src.index('\n', j)
                if src[e - 1] == '\\':
                    j = e + 1
                    continue
                break
            flush(e + 1)
            i = e + 1
            continue
        if c == '{':
            if depth == 0 and head_end is None:
                head_end = i
            depth += 1
        elif c == '}':
            depth -= 1
            if depth == 0:
                head = strip_comments(src[start:head_end])
                if '=' not in head and not re.match(r'\s*typedef\b', head) \
                        and ')' in head:
                    m = re.search(r'(\w+)\s*\([^()]*(\([^()]*\)[^()]*)*\)\s*(\w[^;{]*)?$', head.strip(), re.S)
                    if not m and KR_HEAD.match(head):
                        m = re.search(r'(\w+)\s*\(', head)
                    e = i + 1
                    if src[e:e + 1] == '\n':
                        e += 1
                    flush(e, m.group(1) if m else None)
                    head_end = None
        elif c == ';' and depth == 0 and not KR_HEAD.match(strip_comments(src[start:i])):
            e = i + 1
            if src[e:e + 1] == '\n':
                e += 1
            flush(e)
            head_end = None
            i = e
            continue
        i += 1
    if start < n:
        out.append([src[start:], None])
    return out


# an old-style definition: its parameter declarations end in ';' before the body
KR_HEAD = re.compile(r'\s*[\w\s*]*?\b\w+\s*\(\s*\w+(\s*,\s*\w+)*\s*\)\s*[^()]*$', re.S)


def strip_comments(s):
    s = re.sub(r'/\*.*?\*/', ' ', s, flags=re.S)
    return re.sub(r'//[^\n]*', ' ', s)


def only_ws_comments(s):
    return strip_comments(s).strip() == ''


def reorder(src, order):
    its = items(src)
    rank = {name: k for k, name in enumerate(order)}
    rest = [t for t, name in its if name not in rank]
    funcs = sorted(((rank[name], t) for t, name in its if name in rank))
    body = ''.join(rest).rstrip('\n') + '\n'
    for _, t in funcs:
        if not t.startswith('\n'):
            t = '\n' + t.lstrip('\n')
        body += t
    return body


def main():
    order = [l.strip() for l in open(sys.argv[1]) if l.strip()]
    for path in sys.argv[2:]:
        src = open(path).read()
        new = reorder(src, order)
        if new != src:
            open(path, 'w').write(new)


if __name__ == '__main__':
    main()
