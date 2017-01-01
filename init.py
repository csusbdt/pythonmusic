from ctypes import CDLL
from ctypes import c_int
from ctypes import c_double

module = CDLL("./module.so")
module.init()

o = module.set_oscillator
o.argtypes = (c_int, c_double, c_double)

