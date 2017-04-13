from NIntegrate import *
import numpy as np
from util import plot

def ngon(center, radius, segments):
    alpha = np.linspace(0, 2 * np.pi, segments, False)
    x = np.cos(alpha)
    y = np.sin(alpha)
    return center + radius * np.array([x, y]).T

polygons = [
    ngon(( 0.0, 0.0), 2.0, 8), # outer loop
    ngon(( 1.0, 0.0), 0.5, 8), # inner loop
    ngon((-1.0, 0.0), 0.8, 8), # inner loop
    ngon((-1.0, 0.0), 0.4, 8)  # island
]

faces = tessellate(polygons)
points = integration_points(faces, 3)
area = integrate(lambda u, v: 1, points)

print(area, flush=True)

plot(faces, points)