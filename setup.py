from skbuild import setup

PROJECT_NAME = "pyrematch"

setup(
    name=PROJECT_NAME,
    version="0.0.1",
    description="Python bindings for REmatch",
    packages=[f"{PROJECT_NAME}"],
    package_dir={"": "python"},
    cmake_install_dir=f"python/{PROJECT_NAME}",
)