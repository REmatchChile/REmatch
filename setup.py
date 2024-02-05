import codecs
import os

from skbuild import setup

PROJECT_NAME = "pyrematch"
ROOT_DIR = os.path.abspath(os.path.dirname(__file__))
LONG_DESCRIPTION = codecs.open(
    os.path.join(ROOT_DIR, "python/pyrematch/README.md")
).read()
VERSION = codecs.open(os.path.join(ROOT_DIR, "VERSION")).read()

setup(
    name=PROJECT_NAME,
    version=VERSION,
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
        "regex, rematch, regular, information extraction, text search, pattern matching"
    ),
    license="MIT",
    python_requires=">=3.9",
    packages=[f"{PROJECT_NAME}"],
    package_dir={"": "python"},
    cmake_install_dir=f"python/{PROJECT_NAME}",
)
