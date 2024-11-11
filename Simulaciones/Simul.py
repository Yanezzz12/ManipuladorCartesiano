import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

class ManipuladorCartesiano:
    def __init__(self):
        # Inicializamos las posiciones en los ejes X, Y y Z
        self.x = 0
        self.y = 0
        self.z = 0

    def mover_a(self, x, y, z):
        """Método para mover el manipulador a la posición (x, y, z)"""
        self.x = x
        self.y = y
        self.z = z

    def obtener_posicion(self):
        """Devuelve la posición actual del end-effector"""
        return np.array([self.x, self.y, self.z])

    def simular(self, trayectorias):
        """Simula y visualiza la trayectoria del manipulador"""
        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d')

        # Configuración del gráfico
        ax.set_xlabel('Eje X')
        ax.set_ylabel('Eje Y')
        ax.set_zlabel('Eje Z')
        ax.set_title('Simulación de Manipulador Cartesiano')

        # Trazamos la trayectoria
        for punto in trayectorias:
            self.mover_a(*punto)
            posicion = self.obtener_posicion()
            ax.scatter(posicion[0], posicion[1], posicion[2], c='r')
            ax.plot([0, posicion[0]], [0, posicion[1]], [0, posicion[2]], 'b--')

        plt.show()

# Ejemplo de uso
if __name__ == "__main__":
    # Creamos una instancia del manipulador cartesiano
    manipulador = ManipuladorCartesiano()

    # Definimos algunas posiciones para mover el end-effector
    trayectorias = [
        [1, 2, 3],
        [2, 3, 4],
        [3, 5, 6],
        [4, 5, 2],
        [6, 1, 3]
    ]

    # Ejecutamos la simulación
    manipulador.simular(trayectorias)
