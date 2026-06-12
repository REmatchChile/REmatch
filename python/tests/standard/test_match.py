import pyrematch as re
import pytest


@pytest.fixture(name="match")
def fixture_match():
    """fixture_match"""
    document = "aa--bb__cc"
    query = re.reql(r"!x{\w{3,}}")
    return query.findone(document)


def test_match_group(match):
    """test_match_group"""
    expected = "bb_"
    assert match.group("x") == expected


def test_match_copy(match):
    """test_match_copy"""
    match2 = match
    expected = "bb_"
    assert match.group("x") == expected
    assert match2.group("x") == expected


def test_match_span_functions(match):
    """test_match_span_functions"""
    assert match.empty() is False
    assert match.variables() == ["x"]
    assert match.span("x") == (4, 7)
    assert match.start("x") == 4
    assert match.end("x") == 7
    assert match.groupdict() == {"x": "bb_"}


def test_match_var_not_found(match):
    """test_match_var_not_found"""
    pytest.raises(re.REmatchException, match.span, "a")
    pytest.raises(re.REmatchException, match.span, 1)
