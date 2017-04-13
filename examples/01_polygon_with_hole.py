from NIntegrate import *
from util import plot

# example polygon:
#
# (0)------------(5)
#  |     (6)--(8) |
#  |        \  |  |
# (1)----(2)  (7) |
#         |       |
#         |       |
#        (3)-----(4)
#
# Note:
# The orientation of the polygon is not important

# define integration domain
polygons = [[(0.0, 2.0),  # (0) outer loop
             (0.0, 1.0),  # (1)
             (1.0, 1.0),  # (2)
             (1.0, 0.0),  # (3)
             (2.0, 0.0),  # (4)
             (2.0, 2.0)], # (5)
            [(1.0, 1.8),  # (6) inner loop
             (1.8, 1.0),  # (7)
             (1.8, 1.8)]] # (8)

# tessellate domain
faces = tessellate(polygons)

# get integration points
points = integration_points(faces, 4)

# calculate area: intergrate f(u, v) = 1
area = integrate(lambda u, v: 1, points)

print(area, flush=True)

expected_area = 3 - 0.8**2 / 2
assert abs(area - expected_area) < 10e-6

plot(faces, points)