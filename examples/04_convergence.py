from NIntegrate import *
import numpy as np
from util import plot

def ngon(center, radius, segments):
    alpha = np.linspace(0, 2 * np.pi, segments, False)
    x = np.cos(alpha)
    y = np.sin(alpha)
    return center + radius * np.array([x, y]).T

data = list()

for n in range(3, 360):
    polygons = [
        ngon((0, 0), 1.0, n)
    ]

    faces = tessellate(polygons)
    points = integration_points(faces, 2)
    area = integrate(lambda u, v: 1, points)

    data.append([n, area])

print(area, flush=True)

import matplotlib.pyplot as plt

x, y = np.asarray(data).T
plt.plot([0, n], [np.pi, np.pi])
plt.plot(x, y)
plt.show()