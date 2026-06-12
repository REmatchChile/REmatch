from dataclasses import dataclass
import pyrematch as re
import pytest
from tests import options
from os import path


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
        "multi_check_a",
        True,
    ),
    CheckCase(
        "multi check no output",
        r"^(a+ )*!x{a+} (a+ )*!x{b+}( a+)*$",
        "multi_check_a",
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
    reader = re.Reader(path.join(options.DOCUMENTS_PATH, f"{test_case.document}.txt"))
    actual = regex.check(reader)

    assert actual == test_case.expected
