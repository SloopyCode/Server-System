#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define SERVER_VERSION "1.5"

typedef struct {
    int sock;
    char username[50];
} Client;

Client clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
int server_running = 1;

#define RESET       "\x1b[0m"
#define BOLD        "\x1b[1m"
#define UNDERLINE   "\x1b[4m"
#define RED         "\x1b[31m"
#define GREEN       "\x1b[32m"
#define YELLOW      "\x1b[33m"
#define BLUE        "\x1b[34m"
#define MAGENTA     "\x1b[35m"
#define CYAN        "\x1b[36m"
#define WHITE       "\x1b[37m"

void log_message(const char *message) {
    FILE *log_file = fopen("server_log.txt", "a");
    if (log_file) {
        time_t now = time(NULL);
        fprintf(log_file, "[%s] %s\n", strtok(ctime(&now), "\n"), message);
        fclose(log_file);
    }
    printf(CYAN "%s\n" RESET, message);
}

void list_clients() {
    pthread_mutex_lock(&clients_mutex);
    printf(GREEN "Aktive Benutzer:\n" RESET);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sock != 0) {
            printf(" - %s\n", clients[i].username);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void kick_client(const char *username) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].sock != 0 && strcmp(clients[i].username, username) == 0) {
            char buffer[BUFFER_SIZE];
            snprintf(buffer, sizeof(buffer), "INFO: Du wurdest vom Server getrennt.\n");
            send(clients[i].sock, buffer, strlen(buffer), 0);
            printf(RED "Benutzer '%s' wurde gekickt.\n" RESET, username);
            close(clients[i].sock);
            clients[i].sock = 0;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void broadcast_message(const char *message) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].sock != 0) {
            send(clients[i].sock, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    printf(GREEN "Broadcast gesendet: %s\n" RESET, message);
}

void add_client(int client_sock, const char *username) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].sock == 0) {
            clients[i].sock = client_sock;
            strcpy(clients[i].username, username);
            char log[BUFFER_SIZE];
            snprintf(log, sizeof(log), "Benutzer '%s' ist beigetreten.", username);
            log_message(log);
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(int client_sock) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].sock == client_sock) {
            char log[BUFFER_SIZE];
            snprintf(log, sizeof(log), "Benutzer '%s' hat die Verbindung getrennt.", clients[i].username);
            log_message(log);
            clients[i].sock = 0;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

int count_clients() {
    int count = 0;
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].sock != 0) {
            count++;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    return count;
}

void *delayed_shutdown(void *arg) {
    int seconds = *((int *)arg);
    free(arg);

    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "INFO: Der Server wird in %d Sekunden heruntergefahren.\n", seconds);
    broadcast_message(buffer);

    sleep(seconds);
    printf(RED "Der Server wird jetzt heruntergefahren...\n" RESET);
    exit(0);
}

void *server_commands(void *arg) {
    while (server_running) {
        char command[BUFFER_SIZE];
        printf(BLUE "> " RESET);
        fflush(stdout);

        if (fgets(command, sizeof(command), stdin) != NULL) {
            command[strcspn(command, "\n")] = '\0';

            if (strcmp(command, "-list") == 0) {
                list_clients();
            } else if (strncmp(command, "-kick", 5) == 0) {
                char *username = strtok(command + 6, " ");
                if (username) {
                    kick_client(username);
                } else {
                    printf(RED "Fehler: Kein Benutzername angegeben.\n" RESET);
                }
            } else if (strncmp(command, "-broadcast", 10) == 0) {
                char *message = strtok(command + 11, "\n");
                if (message) {
                    broadcast_message(message);
                } else {
                    printf(RED "Fehler: Keine Nachricht angegeben.\n" RESET);
                }
            } else if (strncmp(command, "-shutdown w", 11) == 0) {
                char *seconds_str = strtok(command + 12, " ");
                if (seconds_str) {
                    int seconds = atoi(seconds_str);
                    if (seconds > 0) {
                        int *arg = malloc(sizeof(int));
                        *arg = seconds;
                        pthread_t shutdown_thread;
                        pthread_create(&shutdown_thread, NULL, delayed_shutdown, arg);
                        pthread_detach(shutdown_thread);
                    } else {
                        printf(RED "Fehler: Ungültige Zeitangabe.\n" RESET);
                    }
                } else {
                    printf(RED "Fehler: Keine Zeitangabe gemacht.\n" RESET);
                }
            } else if (strcmp(command, "-info") == 0) {
                printf("  Server-Version: " CYAN "%s\n" RESET, SERVER_VERSION);
                printf("Aktive Verbindungen: " CYAN "%d\n" RESET, count_clients());
                printf("      Leth\n\n");
            } else if (strcmp(command, "-help") == 0) {
                printf("Verfügbare Befehle:\n");
                printf(" " CYAN "-list" RESET "                  Zeige aktive Benutzer an\n");
                printf(" " CYAN "-kick <user>" RESET "           Trenne einen Benutzer\n");
                printf(" " CYAN "-broadcast <message>" RESET "   Sende eine Nachricht an alle Benutzer\n");
                printf(" " CYAN "-shutdown w <sekunden>" RESET " Führe einen verzögerten Shutdown aus\n");
                printf(" " CYAN "-info" RESET "                  Zeige Server-Informationen\n");
                printf(" " CYAN "-help" RESET "                  Zeige diese Hilfe an\n\n");
            } else {
                printf(RED "Unbekannter Server-Befehl: %s\n" RESET, command);
            }
        }
    }
    return NULL;
}

void *handle_client(void *arg) {
    int client_sock = *((int *)arg);
    free(arg);

    char username[50];
    recv(client_sock, username, sizeof(username), 0);
    add_client(client_sock, username);

    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_sock, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            remove_client(client_sock);
            break;
        }
    }

    close(client_sock);
    return NULL;
}

int main() {
    int server_sock, new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_sock, MAX_CLIENTS);

    printf(BLUE "Server läuft auf Version %s, Port %d.\n" RESET, SERVER_VERSION, PORT);

    pthread_t command_thread;
    pthread_create(&command_thread, NULL, server_commands, NULL);

    while (server_running) {
        addr_size = sizeof(client_addr);
        new_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);

        int *new_sock_ptr = malloc(sizeof(int));
        *new_sock_ptr = new_sock;

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, new_sock_ptr);
        pthread_detach(tid);
    }

    return 0;
}
