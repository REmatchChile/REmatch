## REmatch — Java binding

Este proyecto realiza un binding de la librería **REmatch** con **Java** utilizando **JavaCPP**, y así exponer funcionalidades a través de una librería `.jar`.

---

### ⚙️ **Requerimientos**

1. **Instalar REmatch** con **librería compartida** (*shared*):
    ```bash
    cmake -Bbuild/Release -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON
    cmake --build build/Release/
    ```
2. **JavaCPP** como dependencia externa. Se agregan las librerías asociadas a `/java/javacpp/libs`
    ```
    📚 libs
        ├─ 📄 javacpp.jar
        ├─ 📄 javacpp-linux-x86_64.jar
        ├─ 📄 javacpp-platform.jar
    ```
    En la página de descargas de JavaCPP, descargar el archivo `javacpp-platform-1.5.X-bin.zip`.

    **URL**:```https://github.com/bytedeco/javacpp/releases```

3. **Importante** → Cuando escribas patrones, recuerda que los `/` deben escaparse como `//`.

---

### 🏗️ **Compilar en local (repositorio)**

1. Situarse en la carpeta `java/`.
2. Crear carpeta `build`:

    ```
    mkdir build && cd build
    ```
3. Desde `build/`:
    ```
    cmake .. && make
    ```
    Esto ejecutará el archivo `Main.java` ubicado en `java/rematch/`, donde puedes realizar pruebas con las funciones expuestas.

---
### 📦 **Generar `.jar` para exportar la librería**

1. Situarse en `java/`.
2. Ejecutar:
    ```
    cmake -Bbuild -DBUILD_JAR_LIBRARY=ON 
    cmake --build build
    ```
    El archivo `.jar` se generará dentro de la carpeta `build/`.

---
### 📝 **Importante**

La primera vez que se ejecuta:

```
cmake .. && make
```
Se crea automáticamente el archivo compartido `libjniREmatch.so`.
Si se vuelve a correr el comando, **no se recompila** porque el `.so` ya existe en: `java/build/jni/`

---
### 🧪 **Ejemplo**

Se asume que:

- La librería `REmatch.so` ya existe en `/usr/local/lib` para que los `.class` puedan acceder a ella.
- **JavaCPP** se solicita como **dependencia externa** (su `.jar`).


1. Compilar el `.jar` desde `java/build/`

2. Crear estructura para proyecto de prueba

    ```
    📦 REmatch
    ├─ 📂 src
    │  └─ 📂 targets
    │     └─ 📂 java
    │        └─ 📂 build
    │           └─ 📄 rematch.jar   # Copiar a javarematch-example
    └─ 🧪 javarematch-example
        ├─ 📚 libs
        │  ├─ 📄 javacpp.jar
        │  ├─ 📄 javacpp-linux-x86_64.jar
        │  ├─ 📄 javacpp-platform.jar
        │  └─ 📄 rematch.jar
        └─ 🧩 TestJavaREmatch.java
    ```
    El archivo `TestJavaREmatch.java` testea las funciones que se exponen en la librería.

3. Compilar y ejecutar ejemplo

	Compilar:
	
		javac -cp libs/rematch.jar:libs/javacpp.jar -d out/ TestJavaREmatch.java
	
	Ejecutar:
	
		java -cp out:libs/rematch.jar:libs/javacpp.jar TestJavaREmatch

---

### ❓ **Preguntas frecuentes**

1. ¿Cómo se maneja el que `libjniREmatch.so` no se deba compilar cada vez que se quiere probar `Main.java`  en local?
	
    En el primer momento en que se intenta compilar `Main.java`, se crea un archivo `compiled.stamp` que se guarda en `build/`.
	
	Al momento de querer compilar otra vez el archivo principal, se revisa si ese archivo existe, si es así (ya existe `libjniREmatch.so`), se salta la parte de compilación de la librería.
		
		
2. ¿Cómo se crea el `.jar`?

	A partir del CMake en java, se tiene la opción `BUILD_JAR_LIBRARY`. Si ésta se entrega en el comando de compilación como `=ON`, se crea el .jar y se guarda en la misma carpeta `build/`.
		
	La librería se crea en base a la carpeta `jar-build` que contiene todos los archivos que deben ser guardados para el funcionamiento de la librería. Esta carpeta se crea solo para poder generar el .jar, luego se borra automáticamente.
		
		
3. ¿Qué contiene el `.jar`?

    | **Carpeta / Archivo** | **Descripción** |
    |------------------------|------------------|
    | `libjniREmatch.so`    | 🧩 Librería compartida generada por **CMake** para el **binding** entre **C++** y **Java**. |
    | `javacpp/`            | 📦 Contiene las definiciones de **clases Java** usadas **internamente**. |
    | `rematch/`            | 🚀 **Clases públicas** expuestas al usuario que utilizan `javacpp/` para invocar `libjniREmatch.so`. |


4. ¿Qué se necesita para poder utilizar el .jar en un proyecto externo?

	`javacpp/`  como dependencia externa (3 `.jar` iniciales)
	```
	📦 Proyecto
    └── 📂 libs
        ├── 📄 javacpp.jar
        ├── 📄 javacpp-linux-x86_64.jar
        ├── 📄 javacpp-platform.jar
        └── 📄 rematch.jar
    └── 📄 TestJavaREmatch.java
    ```
