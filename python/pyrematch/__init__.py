from .pyrematch import Flags, Regex, MultiRegex

__version__ = "2.0.7"

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
    MultiSpannersNotAllowedException,
)

__all__ = [
    "Regex",
    "MultiRegex",
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
    "MultiSpannersNotAllowedException",
]
