import pyrematch as re


def test_match_start_end():
    regex = re.Regex(r"!x{a}")
    match = regex.findone("aaa")
    assert match.start("x") == 0
    assert match.end("x") == 1


def test_match_span():
    regex = re.Regex(r"!x{b}")
    match = regex.findone("aba")
    assert match.span("x") == (1, 2)


def test_match_groups():
    regex = re.Regex(r"^!x{a+}!y{b+}$")
    match = regex.findone("aaaabbbb")
    assert match.group(0) == "aaaa"
    assert match.group(1) == "bbbb"
    assert match.group("x") == "aaaa"
    assert match.group("y") == "bbbb"


def test_match_empty():
    regex = re.Regex(r"a")
    match = regex.findone("aa")
    assert match.empty()


def test_match_variables():
    regex = re.Regex(r"!x{!z{a}b}!y{c}")
    match = regex.findone("abc")
    assert match.variables() == ["x", "y", "z"]
