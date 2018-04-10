from distutils.core import setup, Extension

# the c++ extension module
extension_mod = Extension("hello", ["hello.c"])

setup(name = "hello", ext_modules=[extension_mod])