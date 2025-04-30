import tkinter as tk
import serial
import time

# Configuración del puerto serial (ajusta el puerto según tu sistema)
arduino = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
time.sleep(2)  # Esperar un poco para que el puerto serial esté listo

# Configurar la ventana principal
root = tk.Tk()
root.title("Controlador del Manipulador Cartesiano")
root.configure(bg='#2C2C2C')  # Fondo gris oscuro

# Ajustar el tamaño de la ventana de forma proporcional a los botones de 100x100px
root.geometry('750x570')  # Ventana más grande para acomodar los botones más grandes
root.focus_set()    # Establecer el enfoque en la ventana principal

# Variable para controlar el estado de la garra
garra_abierta = False

# Funciones de control para cada eje
def xMove(direction):
    command = f"X{direction}"
    arduino.write(command.encode())
    print(f"Enviando comando: {command}")

def yMove(direction):
    command = f"Y{direction}"
    arduino.write(command.encode())
    print(f"Enviando comando: {command}")

def zMove(direction):
    command = f"Z{direction}"
    arduino.write(command.encode())
    print(f"Enviando comando: {command}")

def controlar_garra():
    global garra_abierta
    command = 'GOPEN' if garra_abierta else 'GCLOSE'
    arduino.write(command.encode())
    print(f"Enviando comando: {command}")
    
    garra_abierta = not garra_abierta

#Funciones de control
def mover_eje_x(pos):
    print(f"Moviendo eje X en dirección: {pos}")

def mover_eje_y(pos):
    print(f"Moviendo eje Y en dirección: {pos}")

def mover_eje_z(pos):
    print(f"Moviendo eje Z en dirección: {pos}")

def controlar_garra():
    global garra_abierta
    if garra_abierta:
        print("Cerrando garra")
        boton_garra_abrir.config(text="Cerrar Garra", relief="raised")
    else:
        print("Abriendo garra")
        boton_garra_abrir.config(text="Abrir Garra", relief="sunken")
    
    # Cambiar el estado de la garra
    garra_abierta = not garra_abierta

# Configuración de los botones
boton_config = {'bg': '#2C2C2C', 'fg': 'white', 'highlightbackground': '#7F7F7F', 'highlightthickness': 2, 'activebackground': '#5A5A5A', 'width': 10, 'height': 5, 'font': ('Helvetica', 16)}

# Crear botones para cada eje y la garra
boton_x_mas = tk.Button(root, text="X-", command=lambda: xMove('+'), **boton_config)
boton_x_menos = tk.Button(root, text="X+", command=lambda: xMove('-'), **boton_config)
boton_y_mas = tk.Button(root, text="Y+", command=lambda: mover_eje_y('+'), **boton_config)
boton_y_menos = tk.Button(root, text="Y-", command=lambda: mover_eje_y('-'), **boton_config)

boton_z_mas = tk.Button(root, text="Z+", command=lambda: mover_eje_z('+'), **boton_config)
boton_z_menos = tk.Button(root, text="Z-", command=lambda: mover_eje_z('-'), **boton_config)

# Botón grande para abrir y cerrar la garra
boton_garra_abrir = tk.Button(root, text="Abrir Garra", command=controlar_garra, bg='#2C2C2C', fg='white', highlightbackground='#7F7F7F', highlightthickness=2, activebackground='#5A5A5A', width=45, height=3, font=('Helvetica', 16))

# Hacer simétrica la disposición con pesos en las columnas
root.grid_columnconfigure(0, weight=1)  # Columna 0
root.grid_columnconfigure(1, weight=1)  # Columna 1
root.grid_columnconfigure(2, weight=1)  # Columna 2
root.grid_columnconfigure(3, weight=1)  # Columna 3

# Filas también con peso para distribución simétrica
root.grid_rowconfigure(0, weight=1)
root.grid_rowconfigure(1, weight=1)
root.grid_rowconfigure(2, weight=1)
root.grid_rowconfigure(3, weight=1)

# Colocar los botones en la ventana de forma simétrica

# Disposición del eje X y Y en forma de cruz
boton_y_mas.grid(row=0, column=1, padx=10, pady=10)  # Y+
boton_x_menos.grid(row=1, column=0, padx=10, pady=10)  # X-
boton_x_mas.grid(row=1, column=2, padx=10, pady=10)  # X+
boton_y_menos.grid(row=2, column=1, padx=10, pady=10)  # Y-

# Disposición del eje Z como una barra vertical a la derecha
boton_z_mas.grid(row=0, column=3, padx=10, pady=10)  # Z+
boton_z_menos.grid(row=2, column=3, padx=10, pady=10)  # Z-

# Botón de abrir/cerrar garra debajo de todo, ocupando todas las columnas
boton_garra_abrir.grid(row=3, column=0, columnspan=4, padx=10, pady=20)  # Botón grande para la garra

# Función para manejar las teclas presionadas
def manejar_tecla(event):
    if event.char == 'a':
        boton_x_menos.config(relief="sunken")
        xMove('-')
    elif event.char == 'd':
        boton_x_mas.config(relief="sunken")
        xMove('+')
    elif event.char == 'w':
        boton_y_mas.config(relief="sunken")
        mover_eje_y('+')
    elif event.char == 's':
        boton_y_menos.config(relief="sunken")
        mover_eje_y('-')

def manejar_tecla_arriba(event):
    if event.keysym == 'Up':
        boton_z_mas.config(relief="sunken")
        mover_eje_z('+')
    elif event.keysym == 'Down':
        boton_z_menos.config(relief="sunken")
        mover_eje_z('-')

# Función para manejar cuando se suelta la tecla
def manejar_tecla_suelta(event):
    boton_x_mas.config(relief="raised")
    boton_x_menos.config(relief="raised")
    boton_y_mas.config(relief="raised")
    boton_y_menos.config(relief="raised")
    boton_z_mas.config(relief="raised")
    boton_z_menos.config(relief="raised")

# Función para manejar la tecla de espacio (toggle de la garra)
def manejar_tecla_espacio(event):
    if event.keysym == 'space':
        controlar_garra()

# Vincular las teclas con sus funciones correspondientes
root.bind('<KeyPress>', manejar_tecla)
root.bind('<KeyRelease>', manejar_tecla_suelta)
root.bind('<KeyPress-Up>', manejar_tecla_arriba)
root.bind('<KeyPress-Down>', manejar_tecla_arriba)
root.bind('<space>', manejar_tecla_espacio)

# Iniciar el loop de la interfaz
root.mainloop()

# Cerrar la conexión serial al salir
arduino.close()