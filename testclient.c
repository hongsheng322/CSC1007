#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const int SIZE = 100;
    int fd, rtn;
    char *msg = (char*)malloc(SIZE * sizeof(char));
    char c[SIZE];

    printf("Welcome to CSC1007 User to Kernel Program!\n");

    //Opening driver
    printf("Opening driver...");
    fd = open("/dev/test1", O_RDWR);  //open the driver
    if (fd == -1)                     //if fail to open the driver (the drive is not in kernel)
    {
        perror("open /dev/test1");
        printf("Failed!\n");
        exit(EXIT_FAILURE);
    }
    printf("Success!\n");

    //Get user input
    printf("Please enter your message: ");
    if (fgets(msg, SIZE, stdin) == NULL){
        exit(EXIT_FAILURE);
    }
    msg[strcspn(msg, "\n")] = 0;        //remove /n from input
    printf("\nYou entered the message: \"%s\" which is %d char long\n", msg, strlen(msg));

    //Writing to driver
    printf("Attempting to write to driver...");
    rtn = write(fd, msg, strlen(msg));   //write to driver systemcall
    if (rtn < 0)
    {
        printf(" Failed!\n", rtn);
        perror("WRITE ERROR:");
    }
    else
    {
        printf(" Success!\n");
    }

    //Wait for user input
    printf("\n\nPress \"ENTER\" to continue...");
    char temp;
    fflush(stdin);
    do{
        temp = fgetc(stdin);
        if (temp == 0x0A){
            break;
        }
    } while(1);

    //Reading from driver
    printf("\nAttempting to read from driver...");
    rtn = read(fd, &c, SIZE);            //read driver systemcall
    if (rtn < 0)
    {
        printf(" Failed!\n");
        perror("READ ERROR:");
    } 
    else 
    {
        printf(" Success!\n");
        printf("Message from kernel space: \"%s\" which is %d char long\n", c, strlen(c));
    }

    exit(EXIT_SUCCESS);
}