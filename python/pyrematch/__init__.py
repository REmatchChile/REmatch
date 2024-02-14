from .pyrematch import Flags, Regex

__version__ = "2.0.6"

from ._pyrematch import (
    AnchorInsideCaptureException,
    ComplexQueryException,
    EmptyWordCaptureException,
    InvalidCharacterException,
    InvalidEscapeException,
    InvalidRangeException,
    MemoryLimitExceededException,
    RegexSyntaxException,
    SameNestedVariableException,
    UnhandledExpressionException,
    VariableLimitExceededException,
    VariableNotFoundException,
    VariableNotFoundInCatalogException,
)

__all__ = [
    "Regex",
    "Flags",
    "RegexSyntaxException",
    "AnchorInsideCaptureException",
    "ComplexQueryException",
    "EmptyWordCaptureException",
    "InvalidCharacterException",
    "InvalidEscapeException",
    "InvalidRangeException",
    "MemoryLimitExceededException",
    "SameNestedVariableException",
    "UnhandledExpressionException",
    "VariableLimitExceededException",
    "VariableNotFoundException",
    "VariableNotFoundInCatalogException",
]
