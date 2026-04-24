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
        "multi check",
        r"^(a+ )*!x{a+} (a+ )*!x{a+}( a+)*$",
        "aaa aa",
        True,
    ),
    CheckCase(
        "multi check no output",
        r"^(a+ )*!x{a+} (a+ )*!x{a+}( a+)*$",
        "bbb bb",
        False,
    ),
]


@pytest.mark.parametrize(
    "test_case",
    cases,
    ids=[c.name for c in cases],
)
def test_multi_check(test_case: CheckCase):
    """Test multi check"""
    regex = re.multi_reql(test_case.pattern)
    actual = regex.check(test_case.document)

    assert actual == test_case.expected
