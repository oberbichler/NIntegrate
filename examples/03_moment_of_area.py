from NIntegrate import *
from util import plot

# define integration domain
polygons = [[(1, 0),  # (0)
             (1, 2),  # (1)
             (0, 2)]] # (2)

# tessellate domain
faces = tessellate(polygons)

# calculate area and centroid (degree=1 would be enough)
s_x, s_y, area = integrate_vector(lambda x, y: [x, y, 1], faces, 10)
s_x /= area
s_y /= area

# calculate moment of inertia (degree=2 would be enough)
I_x, I_y = integrate_vector(lambda x, y: [(y - s_y)**2, (x - s_x)**2], faces, 10) / area

print('area =', area, flush=True)
print('s_x  =', s_x,  flush=True)
print('s_y  =', s_y,  flush=True)
print('I_x  =', I_x,  flush=True)
print('I_y  =', I_y,  flush=True)

assert abs(area - 1        ) < 10e-6
assert abs(s_x  - 2/3      ) < 10e-6
assert abs(s_y  - 4/3      ) < 10e-6
assert abs(I_x  - 1*2**3/36) < 10e-6
assert abs(I_y  - 1**3*2/36) < 10e-6

plot(faces, [((s_x, s_y), area)])