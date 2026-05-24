//
// Created by David Okocha on 23/05/2026.
//


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void client(void)
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
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    const socklen_t addrlen = sizeof(addr);

    const int connection_id = connect(server_fd, (const struct sockaddr*)&addr, addrlen);
    if (connection_id == -1)
    {
        perror("Failed to connect");
        exit(1);
    }

    char username[256];
    printf("Enter username: ");
    fflush(stdout);

    const char* fg_name = fgets(username, sizeof(username), stdin);

    if (fg_name == NULL)
    {
        fprintf(stderr, "Failed to read username\n");
        exit(1);
    }

    username[strcspn(username, "\n")] = 0;
    write(server_fd, username, strlen(username));

    char buffer[8192];
    while (fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
        write(server_fd, buffer, strlen(buffer));
        memset(buffer, 0, sizeof(buffer));
        read(server_fd, buffer, sizeof(buffer));
        printf("%s", buffer);
    }
}
