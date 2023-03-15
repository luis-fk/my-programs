#include <stdio.h>
#include <stdlib.h>

int collatz_steps(unsigned long long int number);

int number_of_steps = -1;
FILE *file;

int main (int argc, char **argv){
    unsigned long long int number = strtoull(argv[1], NULL, 10);

    /* opens file to where values will be written to */
    file = fopen("collatz.txt", "w");
    collatz_steps(number);

    /* number of steps will be the last number written to the file*/
    fprintf(file, "%d\n", number_of_steps);

    fclose(file);

    return 0;
}

int collatz_steps(unsigned long long int number){
    number_of_steps++;
    /* the steps will be written onto the file as the recursion keeps going
       until it reaches 1*/
    fprintf(file, "%llu\n", number);

    if (number == 1)
        return 0;
    else if (number % 2 == 0)
        return collatz_steps(number /= 2);
    else
        return collatz_steps((number * 3) + 1);
}
