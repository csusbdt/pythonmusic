from ctypes import CDLL
from ctypes import c_int
from ctypes import c_double
import time

module = CDLL("./module.so")
module.set_oscillator.argtypes = (c_int, c_double, c_double)

module.init()

module.set_oscillator(0, 120.0, 1.0)

time.sleep(2)

module.set_oscillator(1, 150.0, 1.0)

time.sleep(2)

module.set_oscillator(0, 0.0, 0.0)
module.set_oscillator(1, 0.0, 0.0)

time.sleep(2)

# module.stop()

print("OK\n");

