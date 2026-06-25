from dataclasses import dataclass
import pyrematch as re
import pytest
from tests.match_mock import Match


@dataclass
class FindoneCase:
    """Test case."""

    name: str
    pattern: str
    document: str
    expected: Match | None


cases = [
    FindoneCase(
        "simple",
        r"!x{a}",
        "abba",
        Match({"x": (0, 1)}),
    ),
    FindoneCase(
        "empty match",
        r"abba",
        "abba",
        Match(),
    ),
    FindoneCase(
        "no matches",
        r"!x{docx}",
        "This is a document",
        None,
    ),
    FindoneCase(
        "alternation",
        r"!x{doc|document}",
        "This is a document",
        Match({"x": (10, 13)}),
    ),
    FindoneCase(
        "alternation plus",
        r"!x{110(0|1)+};",
        "1100101100011101;",
        Match({"x": (12, 16)}),
    ),
    FindoneCase(
        "nested variables a",
        r"!x{a*!y{b+}a*}",
        "abab",
        Match({"x": (1, 2), "y": (1, 2)}),
    ),
    FindoneCase(
        "nested variables b",
        r" !site{!name{\w+}\.\w{2,3}} ",
        " google.org or google.fr ",
        Match({"site": (1, 11), "name": (1, 7)}),
    ),
    FindoneCase(
        "character class",
        r"!x{[A-Z][a-z]+}[ .]",
        "Regular Expression.",
        Match({"x": (0, 7)}),
    ),
    FindoneCase(
        "character class escape",
        r"!!var{\w+}\{",
        "!variable1{a+}!variable2{b+}",
        Match({"var": (1, 10)}),
    ),
    FindoneCase(
        "character class escape",
        r"!whitespace{\S+\s}",
        "aa  bb  cc",
        Match({"whitespace": (1, 3)}),
    ),
    FindoneCase(
        "negation",
        r"!open{<[^/<>]*>}[^<]*!close{</[^<>]*>}",
        "a <span>certain</span> html <b>tag</b>",
        Match({"open": (2, 8), "close": (15, 22)}),
    ),
    FindoneCase(
        "anchors",
        r"^!x{a}",
        "aa",
        Match({"x": (0, 1)}),
    ),
    FindoneCase(
        "anchors",
        r"!x{ent}$",
        "document sent",
        Match({"x": (10, 13)}),
    ),
    FindoneCase(
        "alternation with anchors",
        r"!x{\w}($| )",
        "document sent",
        Match({"x": (7, 8)}),
    ),
    FindoneCase(
        "anchors",
        r"^!x{welcome}$",
        "welcome",
        Match({"x": (0, 7)}),
    ),
    FindoneCase(
        "escape characters",
        r"^\^!\?\\!x{a\+}\*$",
        "^!?\\a+*",
        Match({"x": (4, 6)}),
    ),
    FindoneCase(
        "character class with newline",
        r"!x{[^\n]}.*!y{[\n]}",
        "a\nb\nc",
        Match({"x": (0, 1), "y": (1, 2)}),
    ),
    FindoneCase(
        "escape characters",
        r"!x{[^\f]}.*!y{[^\v]}",
        "a\fb\vc",
        Match({"x": (0, 1), "y": (1, 2)}),
    ),
    FindoneCase(
        "escape characters",
        r"!x{[\t].*\v.*[\r\n]}",
        "\ta\va\ra\na",
        Match({"x": (0, 5)}),
    ),
]


@pytest.mark.parametrize(
    "test_case",
    cases,
    ids=[c.name for c in cases],
)
def test_findone(test_case: FindoneCase):
    """Test findone"""
    regex = re.reql(test_case.pattern)

    actual = regex.findone(test_case.document)
    assert actual == test_case.expected
