import os
import sys
from setuptools import setup
from Cython.Build import cythonize


from distutils.sysconfig import get_config_var
from setuptools.command.build_ext import build_ext as _build_ext

build_dir = os.environ.get("TARDIR", ".")




################

class build_ext(_build_ext):
    def get_ext_filename(self, ext_name):
        ext_path = ext_name.split('.')
        ext_suffix = ".so" 
        return os.path.join(*ext_path) + ext_suffix


################

setup(
    name="utils",
    ext_modules=cythonize(
        "utils.pyx",
        compiler_directives={
            "language_level": "3",
            "boundscheck": False,
            "wraparound": False,
        },
    ),
    cmdclass={'build_ext': build_ext}
)
