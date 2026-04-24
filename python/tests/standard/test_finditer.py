from dataclasses import dataclass
import pyrematch as re
import pytest
from tests.match_mock import Match


@dataclass
class FinditerCase:
    """Test case."""

    name: str
    pattern: str
    document: str
    expected: list[Match]


test_regex_finditer_cases = [
    FinditerCase(
        "simple",
        r"!x{a}",
        "abba",
        [Match({"x": (0, 1)}), Match({"x": (3, 4)})],
    ),
    FinditerCase(
        "empty match",
        r"abba",
        "abba",
        [Match()],
    ),
    FinditerCase(
        "no matches",
        r"!x{docx}",
        "This is a document",
        [],
    ),
    FinditerCase(
        "alternation",
        r"!x{doc|document}",
        "This is a document",
        [
            Match({"x": (10, 13)}),
            Match({"x": (10, 18)}),
        ],
    ),
    FinditerCase(
        "alternation plus",
        r"!x{110(0|1)+};",
        "1100101100011101;",
        [
            Match({"x": (0, 16)}),
            Match({"x": (6, 16)}),
            Match({"x": (12, 16)}),
        ],
    ),
    FinditerCase(
        "nested variables a",
        r"!x{a*!y{b+}a*}",
        "abab",
        [
            Match({"x": (1, 2), "y": (1, 2)}),
            Match({"x": (0, 2), "y": (1, 2)}),
            Match({"x": (1, 3), "y": (1, 2)}),
            Match({"x": (0, 3), "y": (1, 2)}),
            Match({"x": (3, 4), "y": (3, 4)}),
            Match({"x": (2, 4), "y": (3, 4)}),
        ],
    ),
    FinditerCase(
        "nested variables b",
        r" !site{!name{\w+}\.\w{2,3}} ",
        " google.org or google.fr ",
        [
            Match({"site": (1, 11), "name": (1, 7)}),
            Match({"site": (15, 24), "name": (15, 21)}),
        ],
    ),
    FinditerCase(
        "character class",
        r"!x{[A-Z][a-z]+}[ .]",
        "Regular Expression.",
        [
            Match({"x": (8, 18)}),
            Match({"x": (0, 7)}),
        ],
    ),
    FinditerCase(
        "character class escape",
        r"!!var{\w+}\{",
        "!variable1{a+}!variable2{b+}",
        [
            Match({"var": (1, 10)}),
            Match({"var": (15, 24)}),
        ],
    ),
    FinditerCase(
        "character class escape",
        r"!whitespace{\S+\s}",
        "aa  bb  cc",
        [
            Match({"whitespace": (1, 3)}),
            Match({"whitespace": (0, 3)}),
            Match({"whitespace": (5, 7)}),
            Match({"whitespace": (4, 7)}),
        ],
    ),
    FinditerCase(
        "negation",
        r"!open{<[^/<>]*>}[^<]*!close{</[^<>]*>}",
        "a <span>certain</span> html <b>tag</b>",
        [
            Match({"open": (2, 8), "close": (15, 22)}),
            Match({"open": (28, 31), "close": (34, 38)}),
        ],
    ),
    FinditerCase(
        "anchors",
        r"^!x{a}",
        "aa",
        [Match({"x": (0, 1)})],
    ),
    FinditerCase(
        "anchors",
        r"!x{ent}$",
        "document sent",
        [Match({"x": (10, 13)})],
    ),
    FinditerCase(
        "alternation with anchors",
        r"!x{\w}($| )",
        "document sent",
        [
            Match({"x": (7, 8)}),
            Match({"x": (12, 13)}),
        ],
    ),
    FinditerCase(
        "anchors",
        r"^!x{welcome}$",
        "welcome",
        [Match({"x": (0, 7)})],
    ),
    FinditerCase(
        "escape characters",
        r"^\^!\?\\!x{a\+}\*$",
        "^!?\\a+*",
        [Match({"x": (4, 6)})],
    ),
    FinditerCase(
        "character class with newline",
        r"!x{[^\n]}.*!y{[\n]}",
        "a\nb\nc",
        [
            Match({"x": (0, 1), "y": (1, 2)}),
            Match({"x": (0, 1), "y": (3, 4)}),
            Match({"x": (2, 3), "y": (3, 4)}),
        ],
    ),
    FinditerCase(
        "escape characters",
        r"!x{[^\f]}.*!y{[^\v]}",
        "a\fb\vc",
        [
            Match({"x": (0, 1), "y": (1, 2)}),
            Match({"x": (0, 1), "y": (2, 3)}),
            Match({"x": (0, 1), "y": (4, 5)}),
            Match({"x": (2, 3), "y": (4, 5)}),
            Match({"x": (3, 4), "y": (4, 5)}),
        ],
    ),
    FinditerCase(
        "escape characters",
        r"!x{[\t].*\v.*[\r\n]}",
        "\ta\va\ra\na",
        [
            Match({"x": (0, 5)}),
            Match({"x": (0, 7)}),
        ],
    ),
]


@pytest.mark.parametrize(
    "test_case",
    test_regex_finditer_cases,
    ids=[c.name for c in test_regex_finditer_cases],
)
def test_finditer(test_case: FinditerCase):
    """Test finditer"""
    regex = re.reql(test_case.pattern)
    iterator = regex.finditer(test_case.document)

    for match in iterator:
        assert match in test_case.expected, f"Match {match} was not expected"
        test_case.expected.remove(match)

    assert (
        len(test_case.expected) == 0
    ), f"Expected matches were not found in the document: {test_case.expected}"
