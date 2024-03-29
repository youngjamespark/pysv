# This file was automatically generated by SWIG (http://www.swig.org).
# Version 4.0.2
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.

from sys import version_info as _swig_python_version_info
if _swig_python_version_info < (2, 7, 0):
    raise RuntimeError("Python 2.7 or later required")

# Import the low-level C/C++ module
if __package__ or "." in __name__:
    from . import _pysv
else:
    import _pysv

try:
    import builtins as __builtin__
except ImportError:
    import __builtin__

def _swig_repr(self):
    try:
        strthis = "proxy of " + self.this.__repr__()
    except __builtin__.Exception:
        strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)


def _swig_setattr_nondynamic_instance_variable(set):
    def set_instance_attr(self, name, value):
        if name == "thisown":
            self.this.own(value)
        elif name == "this":
            set(self, name, value)
        elif hasattr(self, name) and isinstance(getattr(type(self), name), property):
            set(self, name, value)
        else:
            raise AttributeError("You cannot add instance attributes to %s" % self)
    return set_instance_attr


def _swig_setattr_nondynamic_class_variable(set):
    def set_class_attr(cls, name, value):
        if hasattr(cls, name) and not isinstance(getattr(cls, name), property):
            set(cls, name, value)
        else:
            raise AttributeError("You cannot add class attributes to %s" % cls)
    return set_class_attr


def _swig_add_metaclass(metaclass):
    """Class decorator for adding a metaclass to a SWIG wrapped class - a slimmed down version of six.add_metaclass"""
    def wrapper(cls):
        return metaclass(cls.__name__, cls.__bases__, cls.__dict__.copy())
    return wrapper


class _SwigNonDynamicMeta(type):
    """Meta class to enforce nondynamic attributes (no new attributes) for a class"""
    __setattr__ = _swig_setattr_nondynamic_class_variable(type.__setattr__)


class pysv_state(object):
    thisown = property(lambda x: x.this.own(), lambda x, v: x.this.own(v), doc="The membership flag")
    __repr__ = _swig_repr
    f = property(_pysv.pysv_state_f_get, _pysv.pysv_state_f_set)
    a = property(_pysv.pysv_state_a_get, _pysv.pysv_state_a_set)
    c = property(_pysv.pysv_state_c_get, _pysv.pysv_state_c_set)
    hang = property(_pysv.pysv_state_hang_get, _pysv.pysv_state_hang_set)
    n = property(_pysv.pysv_state_n_get, _pysv.pysv_state_n_set)
    s = property(_pysv.pysv_state_s_get, _pysv.pysv_state_s_set)
    sq = property(_pysv.pysv_state_sq_get, _pysv.pysv_state_sq_set)
    p = property(_pysv.pysv_state_p_get, _pysv.pysv_state_p_set)
    q = property(_pysv.pysv_state_q_get, _pysv.pysv_state_q_set)
    max = property(_pysv.pysv_state_max_get, _pysv.pysv_state_max_set)
    refdB = property(_pysv.pysv_state_refdB_get, _pysv.pysv_state_refdB_set)
    rmsdB = property(_pysv.pysv_state_rmsdB_get, _pysv.pysv_state_rmsdB_set)
    maxP = property(_pysv.pysv_state_maxP_get, _pysv.pysv_state_maxP_set)
    maxN = property(_pysv.pysv_state_maxN_get, _pysv.pysv_state_maxN_set)
    DClevel = property(_pysv.pysv_state_DClevel_get, _pysv.pysv_state_DClevel_set)
    ActivityFactor = property(_pysv.pysv_state_ActivityFactor_get, _pysv.pysv_state_ActivityFactor_set)
    ActiveSpeechLevel = property(_pysv.pysv_state_ActiveSpeechLevel_get, _pysv.pysv_state_ActiveSpeechLevel_set)
    rmsPkF = property(_pysv.pysv_state_rmsPkF_get, _pysv.pysv_state_rmsPkF_set)
    ActPkF = property(_pysv.pysv_state_ActPkF_get, _pysv.pysv_state_ActPkF_set)
    Gain = property(_pysv.pysv_state_Gain_get, _pysv.pysv_state_Gain_set)

    def __init__(self):
        _pysv.pysv_state_swiginit(self, _pysv.new_pysv_state())
    __swig_destroy__ = _pysv.delete_pysv_state

# Register pysv_state in _pysv:
_pysv.pysv_state_swigregister(pysv_state)


def calculate(FileIn):
    return _pysv.calculate(FileIn)

def normalize(FileIn, FileOut, targetdB):
    return _pysv.normalize(FileIn, FileOut, targetdB)

def samplerate_change(FileIn, FileOut, out_samplerate):
    return _pysv.samplerate_change(FileIn, FileOut, out_samplerate)


