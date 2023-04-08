# CI-0123 ESJOJO

## Problema

Plasmar diferentes conceptos de las áreas de redes y sistemas operativos en la programación un servicio que podrá ser atendido de manera redundante por varios servidores que pueden correr en máquinas distintas.

Este proyecto crea un programa que obtiene y muestra las piezas de figuras Lego para la construcción de una figura en específico para el usuario.

Específicamente debe construyen programas para:

• Servidores intermedios

• Servidores de piezas

• Clientes buscadores de piezas

### Primera entrega 📒

En la primera etapa de proyecto se realiza un programa que a través de un despliegue de menú en el shell permite al usuario solicitar las figuras de Lego que desea y se le muestre la información de las piezas requeridas.

#### Requerimientos

1. Obtener el menú de figuras a través del servidor web
2. Obtener las piezas requeridas para la figura solicitada por el cliente a través del servidor web

#### CLIENTES

Crear un cliente que utilice el protocolo HTTP para solicitar a un servidor una figura específica y obtener la información de las piezas necesarias para construir el objeto solicitado.


##### Resolución

###### Clase Client

Se crea la clase Client que utiliza el protocolo HTTP para conectarse al servidor web https://os.ecci.ucr.ac.cr/lego/ y realizar solicitudes con la finalidad de conseguir información de menú de figuras Lego disponibles y las piezas requeridas para la figura solicitada, se programa sockets con acceso seguro (SSL).

Se implementa los siguientes métodos para la correcta funcionalidad de la clase Client:

connectServer(): Establece conexión segura SSL con el servidor web usando la dirección IP y el puerto 80.

makeRequest(): Se encarga de crear un socket, si no ha sido creado, establecer conexión con el request específico y llamar a processRequest().

inAnimalArray(std::string animal): se encarga de agregar un animal al arreglo interno de animales, y si está el animal dentro del arreglo entonces solo confirma si este ya se encuentra presente. 

regexAnalyzer(bool requestMenu, std::string& line): analiza segmentos de código dados, dependiendo de si son del menú o de una figura específica, y realiza operaciones relacionadas con lo leído.  

processRequest(bool requestMenu): Procesa la respuesta de servidor web después de que se realiza una solicitud, se utiliza la biblioteca regex para el análisis de lenguaje html que el servidor web construye, que por medio de expresiones regulares se saca la información solicitada. En caso que se haya pedido el menú, se saca los nombres de las figuras por medio de la respuesta de servidor web y los agrega en el vector de figuras que tiene el Client. Por otro lado, si se solicita las piezas de una figura, se saca y despliega la información correspondiente


## Manual de Usuario 📃

### Compilación y Ejecución del código
Para poder compilar y correr el código, se provee un archivo Makefile que asiste en la compilación y construcción del programa. Con esto, para la compilación es solo necesario el siguiente comando:

`make`

Este comando corre el makefile y crea el ejecutable dentro de la carpeta bin/. El ejecutable será del nombre de la carpeta común, en cuyo caso, a como es proveído, sería ‘lego-figure-maker’.

Para poder correr el programa desde la carpeta común sería entonces el siguiente comando:

`bin/lego-figure-maker`

De querer borrar el ejecutable y todos los archivos relacionados generados, utilizar el siguiente comando:

`make clean`

Posterior a la ejecución solo es necesario seleccionar las opciones dadas por el output en consola para poder navegar dentro del programa. 

Para la compilación de los casos de prueba, se puede usar el siguiente comando para facilitar la tare:

`make test`


### Detener la Ejecución

En caso de que desee finalizar la ejecución del programa, presionaremos en nuestro dispositivo la letra Ctrl+C o con 0 en el input, tal como es indicado por la salida del programa. 


## Ejemplo de Ejecución 📷

Cuando se inicia el programa se deberá parecer a lo siguiente:
![InicioPrograma](/images/ExampleExecution1.png "Example")

Cuando el usuario pide por la figura "blacksheep" y luego solicita cerrar el programa se deberá parecer a lo siguiente:

![PeticionYCierre](/images/ExampleExecution2.png "Example")

# Protocolo

 Establecer los esquemas de comunicación:

◦ Entre los clientes y los servidores intermedios se comunican por medio de una red pública con el puerto 80 (HTTP)

◦ Entre los servidores intermedios y los servidores de piezas se comunican por medio de una red privada, en un puerto diferente a 80



*Cliente:* Solicita el menú de figuras y las piezas necesarias a través de la solicitud GET de protocolo HTTP que se envía a través de la URL


*Servidor Intermedio:* Contiene el mapa de rutas. Este mapa de rutas se debe actualizar cuando identifica que se agrega un nuevo servidor de piezas. 

*Servidores de piezas:* Realiza una revisión de los modelos que almacena y es quien brinda las piezas solicitadas por el cliente.


Valorar el uso de datos encriptados para las comunicaciones
Encriptar datos con AES para el envío de datos en lo posible para toda conexión.


Protocolo de comunicación para adicionar o eliminar servidores de piezas a servidores intermedios o viceversa (interacción):


1. Primer Caso: Servidor intermedio se levanta primero. 


2. Segundo Caso: Servidor de pieza se levanta primero que el servidor intermedio 


Para ambos casos se establece conexión con el cliente por medio del Socket.
Uso de un archivo de texto manejado por el servidor de intermedio que contenga la información IP y el puerto de cada servidor de pieza levantado, este archivo de texto se debe de actualizar cada vez que se descarte o levante un servidor de pieza. Para el primer caso el servidor Intermedio solamente establece las conexiones a través de dicha información, en caso de problema de segundo caso, esta se resuelve con que el servidor Intermedio esté tratando de realizar las conexiones constantemente a través de la información. 


Paso de datos:

Se puede definir un formato propietario diferente al de html para los datos enviados.
Se puede también definir que al enviarse los datos y que estos, a la mitad de un contenido, no caben dentro del mensaje enviado, se indique donde ocurrió tal interrupción, o simplemente guardarlo para este ser enviado dentro del siguiente mensaje. 

## Integrantes 👥

• Esteban Porras Herrera - C06044

• Joseph Stuart Valverde Kong - C18100

• Johana Wu Nie - C08591

## Evaluaciones 🌐

Evaluado por: Esteban Porras Herrera

• Esteban Porras Herrera - C06044          : 100/100

• Joseph Stuart Valverde Kong - C18100  : 100/100

• Johana Wu Nie - C08591 		       : 100/100

Evaluado por: Joseph Stuart Valverde Kong

• Esteban Porras Herrera - C06044          : 100/100

• Joseph Stuart Valverde Kong - C18100  : 100/100

• Johana Wu Nie - C08591 		       : 100/100

Evaluado por: Johana Wu Nie

• Esteban Porras Herrera - C06044          : 100/100

• Joseph Stuart Valverde Kong - C18100  : 100/100

• Johana Wu Nie - C08591 		       : 100/100
