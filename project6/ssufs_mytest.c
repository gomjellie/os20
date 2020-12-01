#include "ssufs-ops.h"

int main()
{
    char str1[]  = "!data1--32 Bytes of Data-------!!-------32 Bytes of Data-------!!data1--32 Bytes of Data-------!!-------32 Bytes of Data-------!";
    char str2[] = "!data2--32 Bytes of Data-------!!-------32 Bytes of Data-------!";
    char str3[] = "!data3--32 Bytes of Data-------!!-------32 Bytes of Data-------!";
    char str4[] = "!data4--32 Bytes of Data-------!!-------32 Bytes of Data-------!";
    ssufs_formatDisk();

    ssufs_create("f1.txt");
    ssufs_create("f2.txt");
    int fd1 = ssufs_open("f1.txt");
    int fd2 = ssufs_open("f2.txt");

    printf("Write Data: %d\n", ssufs_write(fd1, str1, BLOCKSIZE));
    printf("Write Data: %d\n", ssufs_write(fd2, str2, BLOCKSIZE));
    printf("Seek: %d\n", ssufs_lseek(fd1, -BLOCKSIZE));
    printf("Write Data: %d\n", ssufs_write(fd1, str3, BLOCKSIZE));
    printf("Write Data: %d\n", ssufs_write(fd2, str4, BLOCKSIZE / 2));
    printf("Seek: %d\n", ssufs_lseek(fd2, 0));

    char buf[BLOCKSIZE]; buf[0] = '\0';
    printf("Seek: %d\n", ssufs_lseek(fd1, -BLOCKSIZE));
    printf("Read Data: %d\n", ssufs_read(fd1, buf, BLOCKSIZE / 2));
    printf("READ DATA RESULT: %s \n", buf);
    ssufs_dump();
    ssufs_delete("f1.txt");
    ssufs_delete("f2.txt");
    ssufs_dump();
}
