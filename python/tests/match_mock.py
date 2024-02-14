class MatchMock:
    def __init__(self, mapping={}):
        self.mapping = mapping
        self.variables = list(mapping.keys())

    def span(self, var):
        return self.mapping[var]

    def __repr__(self) -> str:
        variable_repr = [
            f"{var} -> [{self.mapping[var][0]}, {self.mapping[var][1]}>"
            for var in self.variables
        ]

        if variable_repr:
            return ", ".join(variable_repr)

        return "Empty match"

    def __eq__(self, other):
        for var in self.variables:
            if self.mapping[var] != other.span(var):
                return False
        return True
