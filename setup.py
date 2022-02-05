from glob import glob
from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext

ext_modules = [
    Pybind11Extension(
        "pybind11_obj_test",
        sorted(glob("src/*.cpp")),
    ),
]

setup(
    name="pybind11_obj_test",
    version="0.1.0",
    packages=[],
    url="",
    license="MIT",
    python_requires=">=3.9",
    cmdclass={"build_ext": build_ext},
    ext_modules=ext_modules,
)
