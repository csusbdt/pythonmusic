from ctypes import *
import ctypes
import time

module = CDLL("./module.so")
module.set_oscillator.argtypes = (ctypes.c_int, ctypes.c_double, ctypes.c_double)


module.init()

module.set_oscillator(0, 120, 1)

time.sleep(2)

module.set_oscillator(1, 150, 1)
time.sleep(2)

# module.stop()

print("OK\n");

