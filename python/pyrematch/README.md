
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

Here is an example that prints all the matches using the `finditer` function.

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

The `Query` object contains also other useful methods. To get a single match, you can use:

```python
query.findone(document)
```

To find all the matches, you can use:

```python
query.findall(document)
```

To find a limited number of matches, you can use:

```python
limit = 10
query.findmany(document, limit)
```

To check if a match exists, you can use:

```python
query.check(document)
```

You can read more about this in the [PyREmatch Tutorial](https://github.com/REmatchChile/REmatch/wiki/pyREmatch-tutorial).
