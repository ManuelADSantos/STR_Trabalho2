#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 30

void read_textin(void);

int main(int argc, char *argv[])
{
    read_textin();
    return 0;
}

void read_textin(void)
{
    // Open file text.in to read content
    int read_file = open("../point_clouds/point_cloud1.txt", O_RDONLY);

    while (1)
    {
        // Create buffer to read to
        char read_buffer[BUF_SIZE] = "";

        // Read from file to buffer
        read(read_file, read_buffer, sizeof(read_buffer));

        // Print what's been read
        printf("%s\n", read_buffer);
    }
    // Close file text.in
    close(read_file);
}