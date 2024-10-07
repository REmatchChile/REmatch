import os

from skbuild import setup

PROJECT_NAME = "pyrematch"
ROOT_DIR = os.path.abspath(os.path.dirname(__file__))
with open(os.path.join(ROOT_DIR, "python/pyrematch/README.md"), "r") as f:
    LONG_DESCRIPTION = f.read()


def get_version():
    with open(os.path.join(ROOT_DIR, "python/pyrematch/__init__.py")) as f:
        for line in f:
            if line.startswith("__version__"):
                delim = '"' if '"' in line else "'"
                return line.split(delim)[1]
        else:
            raise RuntimeError("Unable to find version string.")


setup(
    name=PROJECT_NAME,
    version=get_version(),
    description=(
        "Python bindings for REmatch, an information extraction focused regex library"
        " that uses constant delay algoirthms"
    ),
    long_description=LONG_DESCRIPTION,
    long_description_content_type="text/markdown",
    url="https://rematch.cl/",
    author="Vicente Calisto, Oscar Cárcamo, Nicolás Van Sint Jan, Gustavo Toro",
    author_email=(
        "vecalisto@uc.cl, oscar.carcamoz@uc.cl, nicovsj@uc.cl, gustavo.toro@uc.cl"
    ),
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
    ],
    keywords=(
        "regex, rematch, regular expression, information extraction, text search,"
        " pattern matching"
    ),
    license="MIT",
    python_requires=">=3.9",
    packages=[f"{PROJECT_NAME}"],
    package_dir={"": "python"},
    cmake_install_dir=f"python/{PROJECT_NAME}",
)
