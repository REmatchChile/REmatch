from .pyrematch import Flags, multi_reql, reql

__version__ = "1.1.0"

from ._pyrematch import (
    AnchorInsideCaptureException,
    ComplexQueryException,
    EmptyWordCaptureException,
    InvalidCharacterException,
    InvalidEscapeException,
    InvalidRangeException,
    MemoryLimitExceededException,
    MultiSpannersNotAllowedException,
    QuerySyntaxException,
    SameNestedVariableException,
    UnhandledExpressionException,
    VariableLimitExceededException,
    VariableNotFoundException,
    VariableNotFoundInCatalogException,
)

__all__ = [
    "Flags",
    "multi_reql",
    "reql",
    "AnchorInsideCaptureException",
    "ComplexQueryException",
    "EmptyWordCaptureException",
    "InvalidCharacterException",
    "InvalidEscapeException",
    "InvalidRangeException",
    "MemoryLimitExceededException",
    "MultiSpannersNotAllowedException",
    "QuerySyntaxException",
    "SameNestedVariableException",
    "UnhandledExpressionException",
    "VariableLimitExceededException",
    "VariableNotFoundException",
    "VariableNotFoundInCatalogException",
]
