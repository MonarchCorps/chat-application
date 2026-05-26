#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/select.h>

#define MAX_CLIENTS 50

void server(void)
{
    const int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("Failed to create socket");
        exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    const int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    const int bind_value = bind(server_fd, (const struct sockaddr*)&addr, sizeof(addr));
    if (bind_value == -1)
    {
        perror("Failed to bind address");
        exit(1);
    }

    const int listen_value = listen(server_fd, 3);
    if (listen_value == -1)
    {
        perror("Failed to mark socket");
        exit(1);
    }

    socklen_t addrlen = sizeof(addr);

    int clients[MAX_CLIENTS];
    char usernames[MAX_CLIENTS][256];
    int client_count = 0;

    for (int i = 0; i < MAX_CLIENTS; i++) clients[i] = -1;

    while (1)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        int max_fd = server_fd;

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i] != -1)
            {
                FD_SET(clients[i], &readfds);
                if (clients[i] > max_fd) max_fd = clients[i];
            }
        }

        select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(server_fd, &readfds))
        {
            const int client_id = accept(server_fd, (struct sockaddr*)&addr, &addrlen);
            if (client_id == -1)
            {
                perror("Could not accept client request");
                continue;
            }

            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (clients[i] == -1)
                {
                    clients[i] = client_id;
                    read(client_id, usernames[i], sizeof(usernames[i]));
                    usernames[i][strcspn(usernames[i], "\n")] = 0;
                    client_count++;

                    // join notification
                    char notice[300];
                    snprintf(notice, sizeof(notice), "*** %s has joined the chat ***\n", usernames[i]);
                    for (int j = 0; j < MAX_CLIENTS; j++)
                    {
                        if (clients[j] != -1 && j != i)
                            write(clients[j], notice, strlen(notice));
                    }
                    printf("%s", notice);
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i] != -1 && FD_ISSET(clients[i], &readfds))
            {
                char buffer[8192];
                const ssize_t bytes_read = read(clients[i], buffer, sizeof(buffer) - 1);

                if (bytes_read <= 0)
                {
                    // leave notification
                    char notice[300];
                    snprintf(notice, sizeof(notice), "*** %s has left the chat ***\n", usernames[i]);
                    for (int j = 0; j < MAX_CLIENTS; j++)
                    {
                        if (clients[j] != -1 && j != i)
                            write(clients[j], notice, strlen(notice));
                    }
                    printf("%s", notice);

                    close(clients[i]);
                    clients[i] = -1;
                    client_count--;
                }
                else
                {
                    buffer[bytes_read] = '\0';
                    char formatted[8192];
                    snprintf(formatted, sizeof(formatted), "[%s]: %s", usernames[i], buffer);
                    for (int j = 0; j < MAX_CLIENTS; j++)
                    {
                        if (clients[j] != -1 && j != i)
                            write(clients[j], formatted, strlen(formatted));
                    }
                    printf("%s", formatted);
                }
            }
        }
    }
}
