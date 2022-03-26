#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define FRAMEMAX 6
#define FRAMEMIN 3
#define REFPAGEMAX 30
#define REFPAGEMIN 20

int n = 0;
int refpSize = 0;
int *frame;
int *refp;
char refbuff[REFPAGEMAX * 3];

void InputPageNumber();
bool CheckValidInput();
int FirstInFirstOut();
int OptimalPageReplacement();
int LeastRecentlyUsed();
void PrintStep(int steps, int change);
void PrintRefPage(int from, int change);
void PrintAge(int *age);
void ClearFrames();
int FindIn(int *ptr, int from, int to, int val);

int main(){
    //Ask user to input number of frames (n) and allocate memory accordingly for frame
    do{
        printf("Enter number of frames: (%d-%d)\n", FRAMEMIN, FRAMEMAX);
        scanf("%d", &n);
    } while (n < FRAMEMIN || n > FRAMEMAX);
    frame = (int*)calloc(n, sizeof(int));
    if (frame == NULL)
    {
        printf("Memory not allocated");
        exit(0);
    }
    ClearFrames();
    printf("Number of frames chosen: %d\n", n);

    //Ask user to input number of reference page  (refpSize) and allocate memory accordingly for refPage
    do
    {
        printf("\nEnter number of reference pages: (%d-%d)\n", REFPAGEMIN, REFPAGEMAX);
        scanf("%d", &refpSize);
    } while (refpSize < REFPAGEMIN || refpSize > REFPAGEMAX);
    refp = (int*)calloc(refpSize, sizeof(int));
    if (refp == NULL){
        printf("Memory not allocated");
        exit(0);
    }
    printf("Number of reference pages chosen: %d\n", refpSize);

    //Check for validity in input
    do
    {
        InputPageNumber();
    } while (!CheckValidInput());

    //Ask user to input their mode of choice
    int mode = -1;
    do
    {
        printf("\nChoose a Page Replacement Algorithhm:  1. FIFO  2. Optimal  3. LRU  4. Compare All\n");
        scanf("%d", &mode);
    } while (mode < 1 || mode > 4);
    switch(mode){
        case 1:
            printf("Algorithm chosen: FIFO\n\n");
            printf("\nThere are %d page faults in this page replacement process.", FirstInFirstOut());
            break;
        case 2:
            printf("Algorithm chosen: Optimal\n\n");
            printf("\nThere are %d page faults in this page replacement process.", OptimalPageReplacement());
            break;
        case 3:
            printf("Algorithm chosen: LRU\n\n");
            printf("\nThere are %d page faults in this page replacement process.", LeastRecentlyUsed());
            break;
        case 4:         //Compare results between algorithms
            printf("\nComparison: FIFO: %d Optimal: %d LRU: %d", FirstInFirstOut(), OptimalPageReplacement(), LeastRecentlyUsed());
            break;
        default:
            printf("Error?");
    }

    return 0;
}

int FirstInFirstOut(){
    ClearFrames();
    int nextIndex = 0; //counter to keep track of index of frame to place reference page intp
    int steps = 1;
    for (int i = 0; i < refpSize; i++){
        int pageVal = *(refp + i);
        if (nextIndex > n - 1){ //reset index
            nextIndex = 0;
        }
        if (FindIn(frame, 0, n, pageVal) != -1)
        {
            //page value is found in frame, skip current RefPage
        }
        else
        {
            *(frame + nextIndex) = pageVal;
            PrintStep(steps, nextIndex);
            steps++;
            nextIndex++;
        }
    }
    return steps - 1;
}

int OptimalPageReplacement(){
    ClearFrames();
    int frameIndex = 0; //which frame are we on?
    int longestDistance = 0;
    int frameToReplace = -1;
    int steps = 1;
    for (int i = 0; i < refpSize; i++){
        int pageVal = *(refp + i);
        // printf("Current Reference Page: %d\n", pageVal);
        while (true){
            if (FindIn(frame, 0, n, pageVal) != -1)
            {
                //page value is found in frame, skip current RefPage
            }
            else if (FindIn(frame, 0, n, -1) != -1) //if empty frame is available
            {
                frameIndex = FindIn(frame, 0, n, -1);
                *(frame + frameIndex) = *(refp + i); //assign current page to current frame
                PrintStep(steps, frameIndex);
                steps++;
            }
            else{
                frameIndex = 0;
                int mode = 0;       //-n for all frames don't contain future references, -n < mode < n for some frames contain references, n for all frames contain references
                for (int x = 0; x < n; x++){        //for every page, find the page containing the value with the longest distance
                    int dist = FindIn(refp, i, refpSize, *(frame + x));     //Use FindIn() method to find position of x
                    if (dist != -1)
                    {
                        mode++; //increment mode everytime a page is found in future reference
                        // printf("Page %d is %d references away\n", *(frame + x), dist);
                        if (dist > longestDistance) //compare and save longest distance
                        {
                            longestDistance = dist;
                            frameIndex = x;
                        }
                    }
                    else if (dist == -1){           //decrement mode everytime a page is not found in future reference
                        mode--;
                        longestDistance = refpSize;
                        frameIndex = x;
                    }
                }
                if (mode == n)  //if all pages in frames have future references
                {
                    PrintRefPage(i + 1, longestDistance);
                }
                else if (mode == -n) //if all of the current pages have no future references, we randomize
                {
                    printf("No future references found. Choosing a random frame...\n");
                    time_t t;
                    srand((unsigned) time(&t));
                    frameIndex = rand() % (n - 1);
                }

                *(frame + frameIndex) = *(refp + i); //assign value of i-th element of refp to frame
                PrintStep(steps, frameIndex);
                steps++;
                longestDistance = 0; //reset
            }
            break;
        }
    }
    return steps - 1;
}

int LeastRecentlyUsed()
{
    ClearFrames();
    int steps = 1;
    int frameIndex = 0;
    int *age = (int *)calloc(n, sizeof(int));
    for (int i = 0; i < n; i++)             //create age array corresponding to frames array representing age for each frame
    {
        *(age + i) = 0;
    }

    for (int i = 0; i < refpSize; i++){
        PrintAge(age);
        int pageVal = *(refp + i);
        if (FindIn(frame, 0, n, pageVal) != -1) //if pageVal is in frame
        {
            //page value is found in frame, skip current RefPage, reset age of frame containting pageVal
            *(age + FindIn(frame,0, n, pageVal)) = -1;
        }
        else if (FindIn(frame, 0, n, -1) != -1) //if empty frame is available
        {
            frameIndex = FindIn(frame, 0, n, -1);
            *(frame + frameIndex) = *(refp + i); //assign current page to current frame
            *(age + frameIndex) = -1;            //reset recently changed frame
            PrintStep(steps, frameIndex);
            steps++;
        }
        else    //if not found
        {
            int oldest = 0;
            for (int z = 0; z < n; z++) //find the oldest frame
            {
                if (*(age + z) > oldest){
                    oldest = *(age + z);
                    frameIndex = z;
                }
            }
            *(frame + frameIndex) = *(refp + i);
            *(age + frameIndex) = -1; //reset recently changed frame
            PrintStep(steps, frameIndex);
            steps++;
        }
        for (int z = 0; z < n; z++){ //age every frame by 1
            *(age + z) += 1;
        }
    }

    return steps - 1;
}

int FindIn(int *ptr, int from, int to, int val){    //iterate through an array "from" and "to" to find a the "val", return position upon found, else return -1
    int position = 0;
    for (int c = from; c < to; c++)
    {
        if (*(ptr + c) == val)
        {
            return position;
        }
        position ++;
    }
    return -1;
}

void PrintStep(int steps, int change){  //Print step in correct format and highlight the page that is being swapped
    printf("Step %d:  ", steps);
    for (int i = 0; i < n; i++){        //Color changing for easy visualisation
        if (i == change){
            printf("\033[1;31m");
        }
        else{
            printf("\033[0m");
        }
        if (i == n - 1){
            printf("%3d", *(frame + i));
        }
        else{
            printf("%3d  ", *(frame + i));
        }
    }
    printf("\033[0m\n");
}

void PrintAge(int *age)     //Print age of each frame in Age: x x x format
{
    printf("\033[0;34mAge:     ");
    for (int i = 0; i < n; i++)
    {
        if (i == n - 1)
        {
            printf("%3d", *(age + i));
        }
        else
        {
            printf("%3d  ", *(age + i));
        }
    }
    printf("\033[0;37m\n");
}

void PrintRefPage(int from, int change){        //Print remaining future page reference and highlight the furthest page
    printf("Furthest reference: (");
    for (int i = from; i < refpSize; i++)
    {
        if (i == from - 1 + change)
        {
            printf("\033[0;32m");
        }
        else
        {
            printf("\033[0m");
        }
        if (i == refpSize - 1)
        {
            printf("%3d  \033[0m", *(refp + i));
        }
        else
        {
            printf("%3d ", *(refp + i));
        }
    }
    printf(")\n");
}

void InputPageNumber(){     //Let user input page reference numbers as a string and input will be parse approperiately.
    int choice = - 1;       
    do
    {
        printf("\nWould you like to randomly generate page reference numbers?  1. Yes  2. No\n"); //Allow user to randomly generate numbers if they wish for faster testing
        scanf("%d", &choice);
    } while (choice < 1 || choice > 2);
    if (choice == 2){
        printf("\nEnter a set of reference page numbers (0-%d) seperated by a space or comma for %d frames: \n", refpSize - 1, n);
        fflush(stdin);
        gets(refbuff);
        char *cursor = refbuff;
        int i = 0;
        while (cursor != refbuff + strlen(refbuff))
        {
            int x = strtol(cursor, &cursor, 10);
            while (*cursor == ' ' || *cursor == ',')
                cursor++;
            *(refp + i) = x;
            i++;
        }
    }
    else{
        time_t t;
        srand((unsigned) time(&t));
        for (int i = 0; i < refpSize; i++)
            *(refp + i) = rand()% (int)(ceil(refpSize / 1.5f));
    }
}

bool CheckValidInput(){     //Check if input is valid
    for (int z = 0; z < refpSize; z++)
    {
        int val = *(refp + z);
        if (val > refpSize - 1 || val < 0)
        {
            printf("Invalid value (%d) detected! Please re-enter the values.\n\n", val);
            return false;
        }
    }

    printf("Your input is: ");
    for (int z = 0; z < refpSize; z++){
        int val = *(refp + z);
        if (z == refpSize - 1)
        {
            printf("%d\n", val);
        }
        else
        {
            printf("%d, ", val);
        }
    }

    return true;
}

void ClearFrames(){         //Clear current frames for reusability
    for (int i = 0; i < n; i++)
    {
        *(frame + i) = -1;
    }
}