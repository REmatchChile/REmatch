from dataclasses import dataclass

import pyrematch as re
import pytest


@dataclass
class ExceptionCase:
    """Test case"""

    name: str
    pattern: str


syntax_test_cases = [
    ExceptionCase("invalid nested variable", r"!x{a!x{b}}"),
    ExceptionCase("unmatched parenthesis", r"([a-z]+"),
    ExceptionCase("unclosed char class", r"[0-9"),
    ExceptionCase("unclosed quantifier", r"a{3,5"),
    ExceptionCase("nothing to repeat", r"+abc"),
    ExceptionCase("invalid quantifier range", r"!x{ab{4,3}}"),
    ExceptionCase("out of order character range", r"[z-a]"),
    ExceptionCase("anchors inside capture", r"!x{ab$}"),
    ExceptionCase("multi spanner in standard query", r"!x{a}+"),
    ExceptionCase("empty capture", r"!x{a*}"),
]


@pytest.mark.parametrize(
    "test_case",
    syntax_test_cases,
    ids=[c.name for c in syntax_test_cases],
)
def test_syntax_exception(test_case: ExceptionCase):
    """test_syntax_exception"""
    pytest.raises(re.REmatchException, re.reql, test_case.pattern)


def test_memory_exception():
    """test_memory_exception"""
    pattern = "!x{.+}"
    document = "a" * 200
    query = re.reql(pattern, max_mempool_duplications=0)
    with pytest.raises(re.REmatchException, match="Memory limit"):
        query.findall(document)


def test_complexity_exception():
    """test_complexity_exception"""
    pattern = "!x{" + "a" * 100 + "}"
    document = "a" * 20000
    query = re.reql(pattern, max_deterministic_states=200)
    with pytest.raises(re.REmatchException, match="too complex"):
        query.findall(document)


def test_variable_limit():
    """test_variable_limit"""
    pattern = "".join(f"!x{i}{{a}}" for i in range(16))
    with pytest.raises(re.REmatchException, match="Variable limit"):
        re.reql(pattern)
