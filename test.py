import numpy as np

import pybind11_obj_test

points = pybind11_obj_test.test(np.array([0.0, 1.0]), np.array([2.0, 3.0]))

print("\n---- IN PYTHON ----")
print(f"points: {points}")
print("---- PYTHON DONE ----\n")
