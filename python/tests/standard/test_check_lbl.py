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
        "check lbl is true",
        r"!x{.+}",
        "0\n23\n",
        True,
    ),
    CheckCase(
        "check lbl is false",
        r"!x{.{3}}",
        "0\n23\n",
        False,
    ),
]


@pytest.mark.parametrize(
    "test_case",
    cases,
    ids=[c.name for c in cases],
)
def test_check_lbl(test_case: CheckCase):
    """Test check lbl"""
    regex = re.reql(test_case.pattern, re.Flags.LINE_BY_LINE)
    actual = regex.check(test_case.document)

    assert (
        actual == test_case.expected
    )
