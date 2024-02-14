import pytest
import pyrematch as re

test_regex_syntax_exception_cases = [
    r"!{a}",
    r"!123{a}",
    r"a{i}",
    r"!x+{a}",
    r"[a-]",
    r"!x{ab)}",
    r"!x{}",
]

test_anchor_inside_capture_exception_cases = [
    r"!x{^a}",
    r"!x{a$}",
    r"!x{!y{^}}",
    r"!x{!y{a}$}",
    r"$a!x{a^}",
]


@pytest.mark.parametrize("pattern", test_regex_syntax_exception_cases)
def test_regex_syntax_exception(pattern: str):
    with pytest.raises(re.RegexSyntaxException):
        re.Regex(pattern)


@pytest.mark.parametrize("pattern", test_anchor_inside_capture_exception_cases)
def test_anchor_inside_capture_exception(pattern: str):
    with pytest.raises(re.AnchorInsideCaptureException):
        re.Regex(pattern)


@pytest.mark.parametrize("pattern", [r"!x{a*}", r"!x{a|b*}"])
def test_empty_word_capture_exception(pattern: str):
    with pytest.raises(re.EmptyWordCaptureException):
        re.Regex(pattern)


def test_invalid_escape_exception():
    with pytest.raises(re.InvalidEscapeException):
        re.Regex(r"!x{\e}")


def test_invalid_range_exception():
    with pytest.raises(re.InvalidRangeException):
        re.Regex(r"!x{[z-a]}")


def test_same_nested_variable_exception():
    with pytest.raises(re.SameNestedVariableException):
        re.Regex(r"!x{!x{a}}")


def test_variable_limit_exceeded_exception():
    pattern = ""
    for i in range(34):
        pattern += "!a" + str(i) + r"{a}"
    with pytest.raises(re.VariableLimitExceededException):
        re.Regex(pattern)


def test_variable_not_found_exception():
    regex = re.Regex(r"!x{a}")
    match = regex.findone("a")
    assert match
    with pytest.raises(re.VariableNotFoundException):
        match.span("y")
