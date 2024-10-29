
<p align="center"><img src="https://github.com/REmatchChile/REmatch-docs/blob/main/rematch2.png" alt="REmatch"></p>


# PyREmatch: REmatch bindings for Python

Python bindings for REmatch, an information extraction focused regex library that uses constant delay algorithms.

* [REmatch's Official Website](https://rematch.cl/)
* [GitHub Repository](https://github.com/REmatchChile/REmatch-javascript)
* [PyREmatch Tutorial](https://github.com/REmatchChile/REmatch/wiki/pyREmatch-tutorial)


## Installation

You can install the latest release version from PyPI:

```bash
pip install pyrematch
```

Or you can build from the source code:

```bash
git clone git@github.com:REmatchChile/REmatch.git
cd REmatch
pip install .
```


## Usage

```python
import pyrematch as REmatch

# Define the document and the REQL pattern
document = "cperez@gmail.com\npvergara@ing.uc.cl\njuansoto@uc.cl"
pattern = r"@!domain{(\w+\.)+\w+}(\n|$)"

# Create a REQL query
query = REmatch.reql(pattern)

# Execute the query and print the results
for match in query.finditer(document):
    print(match)
```
