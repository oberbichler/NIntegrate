import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
from matplotlib.collections import PatchCollection
import numpy as np

def plot(faces, points=None):
    fig, ax = plt.subplots()

    if faces:
        patches = PatchCollection([Polygon(face) for face in faces], linewidths=1, edgecolors='black', color='gray', alpha=0.3)
        ax.add_collection(patches)

    if points:
        x, y = np.asarray([location for location, _ in points]).T
        ax.plot(x, y, 'rx')

    ax.axis('equal')
    ax.autoscale_view(True, True, True)

    plt.show()