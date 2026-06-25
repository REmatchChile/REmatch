"""Match"""

import pyrematch as re


class Match:
    """Match Mock used to be compared with matches in pyrematch."""

    def __init__(self, mapping=None):
        self.mapping = mapping if mapping else {}
        self._variables = list(self.mapping.keys())

    def span(self, var):
        """Returns the span corresponding to the passed var."""
        return self.mapping[var]

    def variables(self):
        """Returns the list of variables in the mapping."""
        return self._variables

    def __repr__(self) -> str:
        variable_repr = [
            f"'{var}': |{span[0]}, {span[1]}>" for var, span in self.mapping.items()
        ]

        if variable_repr:
            return "{" + ", ".join(variable_repr) + "}"

        return "Empty match"

    def __eq__(self, other):
        if set(self.variables()) != set(other.variables()):
            return False
        for var in self._variables:
            if self.mapping[var] != other.span(var):
                return False
        return True


class MultiMatch:
    """MultiMatch Mock used to be compared with matches in pyrematch."""

    def __init__(self, mapping=None):
        self.mapping = mapping if mapping else {}
        self._variables = list(self.mapping.keys())

    def spans(self, var):
        """Returns the spans corresponding to the passed var."""
        return self.mapping[var]

    def variables(self):
        """Returns the list of variables in the mapping."""
        return self._variables

    def __repr__(self) -> str:
        vars_repr = []

        for var, span_list in self.mapping.items():
            list_repr = [f"|{span[0]}, {span[1]}>" for span in span_list]

            if list_repr:
                list_repr = f"'{var}': [" + ", ".join(list_repr) + "]"
                vars_repr.append(list_repr)

        if vars_repr:
            return "{" + ", ".join(vars_repr) + "}"

        return "Empty match"

    def __eq__(self, other):
        # the pyrematch version might be an empty match and still contain variables
        # Ex. !x{a}|b
        if isinstance(other, re.pyrematch.MultiMatch):
            if not set(self.variables()) <= set(other.variables()):
                return False

            vars_diff = set(other.variables()) - set(self.variables())
            for var in vars_diff:
                if other.spans(var):
                    return False

        for var in self._variables:
            if sorted(self.mapping[var]) != sorted(other.spans(var)):
                return False

        return True
