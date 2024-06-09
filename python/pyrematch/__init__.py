from .pyrematch import Flags, reql

__version__ = "2.0.8"

from ._pyrematch import (
    AnchorInsideCaptureException,
    ComplexQueryException,
    EmptyWordCaptureException,
    InvalidCharacterException,
    InvalidEscapeException,
    InvalidRangeException,
    MemoryLimitExceededException,
    QuerySyntaxException,
    SameNestedVariableException,
    UnhandledExpressionException,
    VariableLimitExceededException,
    VariableNotFoundException,
    VariableNotFoundInCatalogException,
    MultiSpannersNotAllowedException,
)

__all__ = [
    "reql",
    "Flags",
    "QuerySyntaxException",
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
