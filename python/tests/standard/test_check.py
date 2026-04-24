from dataclasses import dataclass
import pyrematch as re
import pytest


@dataclass
class CheckCase:
    """Test case."""

    name: str
    pattern: str
    document: str
    expected: bool


cases = [
    CheckCase(
        "simple",
        r"!x{a}",
        "abba",
        True,
    ),
    CheckCase(
        "empty match",
        r"abba",
        "abba",
        True,
    ),
    CheckCase(
        "no matches",
        r"!x{docx}",
        "This is a document",
        False,
    ),
]


@pytest.mark.parametrize(
    "test_case",
    cases,
    ids=[c.name for c in cases],
)
def test_check(test_case: CheckCase):
    """Test check"""
    regex = re.reql(test_case.pattern)
    actual = regex.check(test_case.document)

    assert actual == test_case.expected
