#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>
#include <ctype.h>

#define PUERTO 8081
#define TAM_BUFFER 1024

int es_numero(const char *str)
{
    // Verifica si la cadena está vacía
    if (*str == '\0')
    {
        return 0;
    }

    // Verifica cada carácter en la cadena
    while (*str)
    {
        if (!isdigit((unsigned char)*str))
        {
            return 0; // No es un número
        }
        str++;
    }
    return 1; // Es un número
}

char *generarNombreUsuario(int longitud)
{
    char *nombre = (char *)malloc((longitud + 1) * sizeof(char));
    char vocales[] = "aeiou";
    char consonantes[] = "bcdfghjklmnpqrstvwxyz";
    int esVocal = rand() % 2; // 0 para consonante, 1 para vocal

    for (int i = 0; i < longitud; i++)
    {
        if (esVocal)
        {
            nombre[i] = vocales[rand() % strlen(vocales)];
            esVocal = 0; // Cambiar a consonante
        }
        else
        {
            nombre[i] = consonantes[rand() % strlen(consonantes)];
            esVocal = 1; // Cambiar a vocal
        }
    }

    nombre[longitud] = '\0'; // Terminar la cadena
    return nombre;
}

char *generarcontrasenia(int longitud)
{
    char *contrasenia = (char *)malloc((longitud + 1) * sizeof(char));
    char mayusculas[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char minusculas[] = "abcdefghijklmnopqrstuvwxyz";
    char numeros[] = "0123456789";
    char todos[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    // Primer caracter aleatorio (mayúscula o minúscula)
    int esMayuscula = rand() % 2;
    if (esMayuscula)
    {
        contrasenia[0] = mayusculas[rand() % strlen(mayusculas)];
    }
    else
    {
        contrasenia[0] = minusculas[rand() % strlen(minusculas)];
    }

    // Resto de caracteres aleatorios
    for (int i = 1; i < longitud; i++)
    {
        contrasenia[i] = todos[rand() % strlen(todos)];
    }

    contrasenia[longitud] = '\0'; // Terminar la cadena
    return contrasenia;
}

int main()
{
    WSADATA wsaData;
    SOCKET servidor, cliente;
    struct sockaddr_in servidor_credenciales, cliente_credenciales;
    int addr_len = sizeof(cliente_credenciales);
    char buffer[TAM_BUFFER];
    int longitud;

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("Error al inicializar Winsock\n");
        return 1;
    }

    // Crear socket del servidor
    servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor == INVALID_SOCKET)
    {
        printf("Error al crear socket del servidor\n");
        WSACleanup();
        return 1;
    }

    // Establecer dirección y puerto del servidor
    servidor_credenciales.sin_family = AF_INET;
    servidor_credenciales.sin_addr.s_addr = INADDR_ANY;
    servidor_credenciales.sin_port = htons(PUERTO);

    // Vincular socket del servidor a dirección y puerto
    if (bind(servidor, (struct sockaddr *)&servidor_credenciales, sizeof(servidor_credenciales)) == SOCKET_ERROR)
    {
        printf("Error al vincular socket del servidor\n");
        closesocket(servidor);
        WSACleanup();
        return 1;
    }

    // Escuchar conexiones entrantes
    if (listen(servidor, 1) == SOCKET_ERROR)
    {
        printf("Error al escuchar conexiones entrantes\n");
        closesocket(servidor);
        WSACleanup();
        return 1;
    }

    printf("Servidor escuchando en puerto %d...\n", PUERTO);

    // Aceptar conexión entrante
    cliente = accept(servidor, (struct sockaddr *)&cliente_credenciales, &addr_len);
    if (cliente == INVALID_SOCKET)
    {
        printf("Error al aceptar conexión entrante\n");
    }

    printf("Conexion establecida con cliente %s:%d\n", inet_ntoa(cliente_credenciales.sin_addr), ntohs(cliente_credenciales.sin_port));

    // Recibir longitud del nombre de usuario

    int opcion = 0;

    while (opcion != 3)
    {
        recv(cliente, buffer, TAM_BUFFER, 0);

        if (es_numero(buffer) == 1)
        {
            printf("El valor ingresado es un numero.\n");
            opcion = atoi(buffer);
            switch (opcion)
            {

            case 1:
                recv(cliente, buffer, TAM_BUFFER, 0);
                longitud = atoi(buffer);
                if (longitud < 5 || longitud > 15)
                {
                    printf("ERROR: LONGITUD ERRONEA: %i\n", longitud);
                    char *mensaje = "Longitud invalida. Debe ser entre 5 y 15.";
                    send(cliente, mensaje, strlen(mensaje), 0);
                    free(mensaje); // Liberar memoria
                }
                else
                {
                    char *nombre = generarNombreUsuario(longitud);
                    send(cliente, nombre, strlen(nombre), 0);
                    printf("Nombre de usuario generado: %s\n", nombre);
                    free(nombre); // Liberar memoria
                }
                break;

            case 2:
                recv(cliente, buffer, TAM_BUFFER, 0);
                longitud = atoi(buffer);

                if (longitud < 8 || longitud > 50)
                {
                    printf("ERROR: LONGITUD ERRONEA: %i\n", longitud);
                    char *mensaje = "Longitud invalida. Debe ser entre 8 y 50.";
                    send(cliente, mensaje, strlen(mensaje), 0);
                    free(mensaje); // Liberar memoria
                }
                else
                {
                    char *nombre = generarcontrasenia(longitud);
                    send(cliente, nombre, strlen(nombre), 0);
                    printf("Contrasenia generada: %s\n", nombre);
                    free(nombre); // Liberar memoria
                }
                break;
            case 3:
                printf("Cerrando conexion servidor...\n");
                break;
            default:
                printf("Opcion incorrecta \n");
                break;
            }
        }
        else
        {
            char *mensaje = "La opcion ingresada no es un numero";
            send(cliente, mensaje, strlen(mensaje), 0);
            printf("El valor ingresado no es un numero.\n");
        }
    }
    // Cerrar socket del servidor
    closesocket(servidor);
    // Limpiar Winsock
    WSACleanup();

    return 0;
}