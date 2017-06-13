# NIntegrate

[![AppVeyor](https://img.shields.io/appveyor/ci/oberbichler/NIntegrate.svg)]()

**Was kann man damit machen?**

Die Bibliothek ermöglicht es, ein polygonales Gebiet in Drei- und Vierecke aufzuteilen, die Integrationspunkte zu bestimmen und damit schließlich eine Funktion über das Gebiet zu integrieren.

![animation](https://github.com/oberbichler/NIntegrate/blob/master/images/animation.gif)

Dazu sind 4 Schritte notwendig:

> Hinweis:
> Die Erklärung bezieht sich auf Python. Das Ganze funktioniert in C++ aber analog.

## 1. Ränder des Integrationsgebiets definieren

Die Ränder des Integrationsgebietes werden als Liste von Polygonen definiert. Ein Polygon ist nichts anderes als eine Liste mit Punkten.

Diese Polygone können von anderen Funktionen stammen oder man erstellt sie manuell.

Hier ein einfaches Beispiel, das ihr auch im Ordner [examples](https://github.com/oberbichler/NIntegrate/tree/master/examples) findet:

``` python
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
```

Damit erzeugt man folgendes Integrationsgebiet:

![polygon](https://github.com/oberbichler/NIntegrate/blob/master/images/polygon.png)

> Hinweis:
> Die Orientierung der Polygone (Umlaufrichtung) spielt keine Rolle. Es können beliebig viele Außen- und Innenloops definiert werden. Inseln sind auch kein Problem.

## 2. Integrationsgebiet in Drei- und Vierecke aufteilen

Mithilfe der Funktion `tessellate` wird das Integrationsgebiet in Drei- und Vierecke unterteilt. Dazu muss man nur die Ränder des Gebiets angeben.

``` python
# tessellate domain
faces = tessellate(polygons)
```

Die Funktion liefert dann eine Liste mit den Teilflächen. Jede Teilfläche ist als Liste der Eckpunkte gespeichert (Dreieck = Liste mit 3 Punkten, Viereck = Liste mit 4 Punkten) 

``` python
>> [[(0,1),(1,1),(1, 1),(0,2)], [(0,2),(1,1.8),(1.8,1.8),(2,2)], …
```

Hier der Plot zum vorherigen Beispiel:

![tessellation](https://github.com/oberbichler/NIntegrate/blob/master/images/tessellation.png)

> Hinweis:
> Der Algorithmus unterteilt zunächst in Dreiecke und versucht dann möglichst viele konvexe Vierecke zu erkennen, um die Anzahl der später zu berechnenden Gaußpunkte zu reduzieren. Die Erkennungsrate für Vierecke ist nicht optimal. Ich habe auch schon mit anderen Algorithmen experimentiert, die hierfür bessere Ergebnisse liefern. Meiner Meinung nach lohnt sich der Mehraufwand jedoch nicht.


## 3. Integrationspunkte ermitteln

Die Funktion `integration_points` berechnet schließlich die Integrationspunkte. Dazu wird ihr eine Liste mit Drei- und Vierecken (z.B. die aus Schritt 2). Außerdem muss der Grad für die Integration angegeben werden.

``` python
# get integration points
degree = 4
points = integration_points(faces, degree)
```

Die berechneten Punkte aus dem vorherigen Beispiel:

![integration_points](https://github.com/oberbichler/NIntegrate/blob/master/images/integration_points.png)

> Hinweis:
> Man kann für den Grad nur einen einzigen Wert angeben. Eine Unterscheidung für den Integrationsgrad in u- und v-Richtung macht bei Dreiecken keinen Sinn. Da die Vierecke i.d.R. auch nicht mehr orthogonal in der Ebene liegen, habe ich diese Unterscheidung komplett weggelassen.

## 4. Funktion integrieren
Die Funktionen `integrate`, `integrate_vector` und `integrate_matrix` können schließlich Funktionen über das Integrationsgebiet integrieren. Dazu übergibt man ihnen die zu integrierende Funktion und eine Liste mit Integrationspunkten. Die Integrationpunkte können aus Schritt 3 stammen, aus einem JSON-File gelesen werden, manuell angegeben werden,...

Als Beispiel wird hier die Funktion `f(u, v) = 1` integriert. Das Ergebnis entspricht der Fläche.

Die Funktion kann als `lambda` angegeben werden

``` python
# calculate area: intergrate f(u, v) = 1
function = lambda u, v: 1
area = integrate(function, points)
```

oder als `def`

``` python
def function(u, v):
    return 1

area = integrate(function, points)
```

Alternativ kann anstelle der Integrationpunkte auch die Liste mit Flächen aus 2. und der Integrationsgrad angegeben werden. Intern wird dabei Schritt 3 automatisch durchgeführt.

``` python
area = integrate(function, faces, degree)
```

`integrate_vector` und `integrate_matrix` können Funktionen integrieren, welche nicht einen skalaren Rückgabewert besitzen sondern einen Vektor bzw. eine Matrix ausgeben. Das Beispiel [03_moment_of_area.py](https://github.com/oberbichler/NIntegrate/blob/master/examples/03_moment_of_area.py) zeigt beispielhaft wie damit der Schwerpunkt und die Trägheitsmomente eines Querschnitts berechnet werden können. Die Berechnung der Steifigkeitsmatrix kann analog erfolgen.

