//
// Created by David Okocha on 23/05/2026.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

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

    while (1)
    {
        const int client_id = accept(server_fd, (struct sockaddr*)&addr, &addrlen);
        if (client_id == -1)
        {
            perror("Could not accept client request");
            exit(1);
        }

        char username[256];

        read(client_id, username, sizeof(username));
        username[strcspn(username, "\n")] = 0;

        char message[8192];

        while (1)
        {
            const ssize_t bytes_read = read(client_id, message, sizeof(message));

            if (bytes_read == -1)
            {
                perror("Connection error");
                exit(1);
            }

            message[bytes_read] = '\0';

            if (bytes_read == 0)
            {
                printf("Client disconnected");
                break;
            }

            char formatted[8192];
            snprintf(formatted, sizeof(formatted), "[%s]: %s", username, message);
            write(client_id, formatted, strlen(formatted));
        }
    }
}
