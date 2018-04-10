# This file was automatically generated by SWIG (http://www.swig.org).
# Version 3.0.12
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.

from sys import version_info as _swig_python_version_info
if _swig_python_version_info >= (2, 7, 0):
    def swig_import_helper():
        import importlib
        pkg = __name__.rpartition('.')[0]
        mname = '.'.join((pkg, '_example')).lstrip('.')
        try:
            return importlib.import_module(mname)
        except ImportError:
            return importlib.import_module('_example')
    _example = swig_import_helper()
    del swig_import_helper
elif _swig_python_version_info >= (2, 6, 0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_example', [dirname(__file__)])
        except ImportError:
            import _example
            return _example
        try:
            _mod = imp.load_module('_example', fp, pathname, description)
        finally:
            if fp is not None:
                fp.close()
        return _mod
    _example = swig_import_helper()
    del swig_import_helper
else:
    import _example
del _swig_python_version_info

try:
    _swig_property = property
except NameError:
    pass  # Python < 2.2 doesn't have 'property'.

try:
    import builtins as __builtin__
except ImportError:
    import __builtin__

def _swig_setattr_nondynamic(self, class_type, name, value, static=1):
    if (name == "thisown"):
        return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name, None)
    if method:
        return method(self, value)
    if (not static):
        if _newclass:
            object.__setattr__(self, name, value)
        else:
            self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)


def _swig_setattr(self, class_type, name, value):
    return _swig_setattr_nondynamic(self, class_type, name, value, 0)


def _swig_getattr(self, class_type, name):
    if (name == "thisown"):
        return self.this.own()
    method = class_type.__swig_getmethods__.get(name, None)
    if method:
        return method(self)
    raise AttributeError("'%s' object has no attribute '%s'" % (class_type.__name__, name))


def _swig_repr(self):
    try:
        strthis = "proxy of " + self.this.__repr__()
    except __builtin__.Exception:
        strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except __builtin__.Exception:
    class _object:
        pass
    _newclass = 0

class Shape(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Shape, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Shape, name)

    def __init__(self, *args, **kwargs):
        raise AttributeError("No constructor defined - class is abstract")
    __repr__ = _swig_repr
    __swig_destroy__ = _example.delete_Shape
    __del__ = lambda self: None
    __swig_setmethods__["x"] = _example.Shape_x_set
    __swig_getmethods__["x"] = _example.Shape_x_get
    if _newclass:
        x = _swig_property(_example.Shape_x_get, _example.Shape_x_set)
    __swig_setmethods__["y"] = _example.Shape_y_set
    __swig_getmethods__["y"] = _example.Shape_y_get
    if _newclass:
        y = _swig_property(_example.Shape_y_get, _example.Shape_y_set)

    def move(self, dx, dy):
        return _example.Shape_move(self, dx, dy)

    def area(self):
        return _example.Shape_area(self)

    def perimeter(self):
        return _example.Shape_perimeter(self)
    __swig_setmethods__["nshapes"] = _example.Shape_nshapes_set
    __swig_getmethods__["nshapes"] = _example.Shape_nshapes_get
    if _newclass:
        nshapes = _swig_property(_example.Shape_nshapes_get, _example.Shape_nshapes_set)
Shape_swigregister = _example.Shape_swigregister
Shape_swigregister(Shape)
cvar = _example.cvar

class Circle(Shape):
    __swig_setmethods__ = {}
    for _s in [Shape]:
        __swig_setmethods__.update(getattr(_s, '__swig_setmethods__', {}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, Circle, name, value)
    __swig_getmethods__ = {}
    for _s in [Shape]:
        __swig_getmethods__.update(getattr(_s, '__swig_getmethods__', {}))
    __getattr__ = lambda self, name: _swig_getattr(self, Circle, name)
    __repr__ = _swig_repr

    def __init__(self, r):
        this = _example.new_Circle(r)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this

    def area(self):
        return _example.Circle_area(self)

    def perimeter(self):
        return _example.Circle_perimeter(self)
    __swig_destroy__ = _example.delete_Circle
    __del__ = lambda self: None
Circle_swigregister = _example.Circle_swigregister
Circle_swigregister(Circle)

class Square(Shape):
    __swig_setmethods__ = {}
    for _s in [Shape]:
        __swig_setmethods__.update(getattr(_s, '__swig_setmethods__', {}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, Square, name, value)
    __swig_getmethods__ = {}
    for _s in [Shape]:
        __swig_getmethods__.update(getattr(_s, '__swig_getmethods__', {}))
    __getattr__ = lambda self, name: _swig_getattr(self, Square, name)
    __repr__ = _swig_repr

    def __init__(self, w):
        this = _example.new_Square(w)
        try:
            self.this.append(this)
        except __builtin__.Exception:
            self.this = this

    def area(self):
        return _example.Square_area(self)

    def perimeter(self):
        return _example.Square_perimeter(self)
    __swig_destroy__ = _example.delete_Square
    __del__ = lambda self: None
Square_swigregister = _example.Square_swigregister
Square_swigregister(Square)

# This file is compatible with both classic and new-style classes.


