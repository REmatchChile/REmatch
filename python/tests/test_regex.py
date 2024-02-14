import pytest
import pyrematch as re
from collections import namedtuple
from match_mock import MatchMock

FinditerCase = namedtuple("FinditerCase", ["pattern", "document", "expected"])
FindoneCase = namedtuple("FindoneCase", ["pattern", "document", "expected"])
CheckCase = namedtuple("CheckCase", ["pattern", "document", "expected"])

test_regex_finditer_cases = [
    FinditerCase(
        r"!x{a}", "abba", [MatchMock({"x": (0, 1)}), MatchMock({"x": (3, 4)})]
    ),
    FinditerCase(r"abba", "abba", [MatchMock()]),
    FinditerCase(r"!x{docx}", "This is a document", []),
    FinditerCase(
        r"!x{doc|document}",
        "This is a document",
        [
            MatchMock({"x": (10, 13)}),
            MatchMock({"x": (10, 18)}),
        ],
    ),
    FinditerCase(
        r"!x{110(0|1)+};",
        "1100101100011101;",
        [
            MatchMock({"x": (0, 16)}),
            MatchMock({"x": (6, 16)}),
            MatchMock({"x": (12, 16)}),
        ],
    ),
    FinditerCase(
        r"!x{a*!y{b+}a*}",
        "abab",
        [
            MatchMock({"x": (1, 2), "y": (1, 2)}),
            MatchMock({"x": (0, 2), "y": (1, 2)}),
            MatchMock({"x": (1, 3), "y": (1, 2)}),
            MatchMock({"x": (0, 3), "y": (1, 2)}),
            MatchMock({"x": (3, 4), "y": (3, 4)}),
            MatchMock({"x": (2, 4), "y": (3, 4)}),
        ],
    ),
    FinditerCase(
        r" !site{!name{\w+}\.\w{2,3}} ",
        " google.org or google.fr ",
        [
            MatchMock({"site": (1, 11), "name": (1, 7)}),
            MatchMock({"site": (15, 24), "name": (15, 21)}),
        ],
    ),
    FinditerCase(
        r"!x{[A-Z][a-z]+}[ .]",
        "Regular Expression.",
        [
            MatchMock({"x": (8, 18)}),
            MatchMock({"x": (0, 7)}),
        ],
    ),
    FinditerCase(
        r"!!var{\w+}\{",
        "!variable1{a+}!variable2{b+}",
        [MatchMock({"var": (1, 10)}), MatchMock({"var": (15, 24)})],
    ),
    FinditerCase(
        r"!whitespace{\S+\s}",
        "aa  bb  cc",
        [
            MatchMock({"whitespace": (1, 3)}),
            MatchMock({"whitespace": (0, 3)}),
            MatchMock({"whitespace": (5, 7)}),
            MatchMock({"whitespace": (4, 7)}),
        ],
    ),
    FinditerCase(
        r"!open{<[^/<>]*>}[^<]*!close{</[^<>]*>}",
        "a <span>certain</span> html <b>tag</b>",
        [
            MatchMock({"open": (2, 8), "close": (15, 22)}),
            MatchMock({"open": (28, 31), "close": (34, 38)}),
        ],
    ),
    FinditerCase(
        r"^!x{a}",
        "aa",
        [MatchMock({"x": (0, 1)})],
    ),
    FinditerCase(
        r"!x{ent}$",
        "document sent",
        [MatchMock({"x": (10, 13)})],
    ),
    FinditerCase(
        r"!x{\w}($| )",
        "document sent",
        [
            MatchMock({"x": (7, 8)}),
            MatchMock({"x": (12, 13)}),
        ],
    ),
    FinditerCase(r"$a", "a", []),
    FinditerCase(
        r"^!x{welcome}$",
        "welcome",
        [MatchMock({"x": (0, 7)})],
    ),
    FinditerCase(
        r"^\^!\?\\!x{a\+}\*$",
        "^!?\\a+*",
        [MatchMock({"x": (4, 6)})],
    ),
    FinditerCase(
        r"!x{[^\n]}.*!y{[\n]}",
        "a\nb\nc",
        [
            MatchMock({"x": (0, 1), "y": (1, 2)}),
            MatchMock({"x": (0, 1), "y": (3, 4)}),
            MatchMock({"x": (2, 3), "y": (3, 4)}),
        ],
    ),
    FinditerCase(
        r"!x{[^\f]}.*!y{[^\v]}",
        "a\fb\vc",
        [
            MatchMock({"x": (0, 1), "y": (1, 2)}),
            MatchMock({"x": (0, 1), "y": (2, 3)}),
            MatchMock({"x": (0, 1), "y": (4, 5)}),
            MatchMock({"x": (2, 3), "y": (4, 5)}),
            MatchMock({"x": (3, 4), "y": (4, 5)}),
        ],
    ),
    FinditerCase(
        r"!x{[\t].*\v.*[\r\n]}",
        "\ta\va\ra\na",
        [
            MatchMock({"x": (0, 5)}),
            MatchMock({"x": (0, 7)}),
        ],
    ),
]

test_regex_findone_cases = [
    FindoneCase(
        r"!x{doc|document}",
        "This is a document",
        MatchMock({"x": (10, 13)}),
    ),
]

test_regex_check_cases = [
    FindoneCase(
        r"!x{doc|document}",
        "This is a document",
        True,
    ),
]


@pytest.mark.parametrize("test_case", test_regex_finditer_cases)
def test_regex_finditer(test_case: FinditerCase):
    regex = re.Regex(test_case.pattern)
    iterator = regex.finditer(test_case.document)

    for match in iterator:
        assert match in test_case.expected, f"Match {match} was not expected"
        test_case.expected.remove(match)

    assert (
        len(test_case.expected) == 0
    ), f"Expected matches were not found in the document: {test_case.expected}"


@pytest.mark.parametrize("test_case", test_regex_findone_cases)
def test_regex_findone(test_case: FindoneCase):
    regex = re.Regex(test_case.pattern)
    match = regex.findone(test_case.document)
    assert match == test_case.expected


@pytest.mark.parametrize("test_case", test_regex_check_cases)
def test_regex_check(test_case: CheckCase):
    regex = re.Regex(test_case.pattern)
    has_output = regex.check(test_case.document)
    assert has_output == test_case.expected
