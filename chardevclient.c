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

    printf("Welcome to CSC1007 User to Kernel Program!\n"); // Welcome message on command line

    // OPEN DRIVER
    printf("Opening driver..."); // Update activity on command line
    fd = open("/dev/chardev", O_RDWR); // Open the device driver at location /dev/chardev
    if (fd == -1) //if fail to open the driver (the driver is not in kernel; not loaded)
    {
        perror("open /dev/chardev"); // POSIX error function; Print message to stderr
        printf("Failed!\n"); // Update user of failure on command line
        exit(EXIT_FAILURE); // Exit with EXIT_FAILURE=1
    }
    printf("Success!\n"); // Print if no error; Update user of success on command line

    // GET USER INPUT
    printf("Please enter your message: "); // Asks for user input on command line
    if (fgets(msg, SIZE, stdin) == NULL){ // Check for null input by user; Cannot create pointer to strign buffer
        exit(EXIT_FAILURE); // Exit with EXIT_FAILURE=1
    }
    msg[strcspn(msg, "\n")] = 0; //remove \n from input; Remove terminating null
    printf("\nYou entered the message: \"%s\" which is %d char long\n", msg, strlen(msg)); // Update user of message length on command line

    // WRITING TO DEVICE DRIVER
    printf("Attempting to write to driver..."); // Update user of writing to driver on command line
    rtn = write(fd, msg, strlen(msg)); // System call for writing to device driver
    if (rtn < 0) // Check for error when writing to driver
    {
        printf(" Failed!\n", rtn); // Update user on error with error code from write() on command line
        perror("WRITE ERROR:"); // POSIX error function; Print message to stderr
    }
    else
    {
        printf(" Success!\n"); // Update user on successful write to driver on command line
    }

    // PAUSE THE PROGRAM
    printf("\n\nPress \"ENTER\" to continue..."); // Wait for user input before continuing with reading from driver
    char temp;
    fflush(stdin); // Flush output buffer; Clean up stream
    do{
        temp = fgetc(stdin); // Check standard in
        if (temp == 0x0A){ // Until \n or end of line
            break; // Then break
        }
    } while(1); // If not keep checking standard in

    // READING FROM DRIVER
    printf("\nAttempting to read from driver..."); // Update user of reading from driver on command line
    rtn = read(fd, &c, SIZE); // System call to read from driver
    if (rtn < 0) // Check for error when reading from driver
    {
        printf(" Failed!\n", rtn); // Update user on error with error code from read() on command line
        perror("READ ERROR:"); // POSIX error function; Print message to stderr
    } 
    else 
    {
        printf(" Success!\n"); // Update user on successful read on command line
        printf("Message from kernel space: \"%s\" which is %d char long\n", c, strlen(c)); // // Update user of message length on command line
    } 
    // User can check if what is written and what is read matches. 
    // This proves that the read / write to and from user application device and kernel is accurate.

    exit(EXIT_SUCCESS); // Exit with EXIT_SUCCESS=0
}