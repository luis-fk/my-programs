#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void split_digits (int senha, int *digitos, int size, int parameter);
void compare (int *number, int *try);
void print_to_file(int digit, int parameter);

FILE *file;
/* - the frequency array keeps track of how many times a number appears on the number 
   to be guessed and the try from the user. This is necessary because in cases where the try
   from the user has a number appearing more times than the actual number, we don't compare it
   multiple times. For instance, is the number is 10001 and the try is 10000, we should only say
   that the first digit (1) is in the correct position and give no information about the extra 1
   on the last digit.

   - digits_number and digits_try store the digits of the number and the try from the user

   - checked is an arrays that keeps track is a number has already been checked or not*/
int frequency[10], digits_number[5], digits_try[5], checked[5];
int try, printed = 0;

int main (int argc, char **argv){
    int number, size_number, size_try;

    /* number is the one to be guessed and try is the number inputed by the user*/
    number = atoi(argv[1]);
    try = atoi(argv[2]);
    
    /* check the size of the number and the try */
    size_number = log10(number) + 1;
    size_try = log10(try) + 1;

    /* open file */
    file = fopen("numlee.txt", "a");

    /* "x" and "y" sintax will be replace later with jinja to create divs*/
    if (number == try){
        fprintf(file, "xYou got it!y");
        fclose(file);
        return 0;
    }

    /* functions that split the number into arrays*/
    split_digits(number, digits_number, size_number, 0);
    split_digits(try, digits_try, size_try, 1);

    fprintf(file, "x");

    /* function that compares the two numbers*/
    compare(digits_number, digits_try);

    if (printed == 0)
        fprintf (file, "No number matches for %d!#", try);

    fprintf(file, "y");

    fclose(file);
    return 0;
}

void split_digits (int number, int *digits, int size, int parameter){
    int i;

    for (i = size; i > 0; i--){
        digits[i - 1] = number % 10;
        frequency[number % 10]++;
        number /= 10; 
    }

    /* a parameter is passed to this function just to differentiate between
       the nuber and the try from the user. This for loop below is used in
       cases where the guess from the user or the number itself don't have
       5 digits. This is necessary so the program below does not confuse
       extra 0s at the end of number*/
    for(i = size; i < 5; i++){
        digits[i] = 99 + parameter;
    }
}

void compare (int *number, int *try){
    int i, j, check;

    for (i = 0; i < 5; i++){
        check = -1;
        for (j = 0; j < 5; j++){
            /* if the two numbers are the same, have the same index, the frequency matches and it
               hasn't been cheked yet, the number is in the correct position and it can be
               appended onto the file */
            if (try[i] == number[j] &&  i == j && frequency[try[i]] > 1 && checked[i] == 0){
                check = 1;
                break;
            }
            /* if the two numbers are the same, have different indexes, the frequency matches and
               it hasn't been checked yet, then the number from the try is in the incorrect positon.
               However, the function doesn't break here because it can still have a correct match
               from the if condition above*/
            if (try[i] == number[j] && i != j && frequency[try[i]] > 1 && checked[i] == 0)
                check = 0;
        }
        /* depending on which if condition above got triggered, we call the function to
            print the correct, incorrect or no message*/
        if (check == 1)
            print_to_file(i, 1);
        else if (check == 0)
            print_to_file(i, 0);
        else if (check == -1)
            continue;

        frequency[try[i]] -= 2;
        checked[i] = 1;
    }
}

void print_to_file(int digit, int parameter){
    printed++;

    /* any weird formating like parenthesis or hashtags in the senteces below are to be used
       with jinja to create line breaks, divs, and stylize words to be displayed on the site*/

    /* if parameter is 1, then the number in a certain position is in its correct position.*/
    if (parameter == 1){
        if (digit == 0)
            fprintf (file, "%dst digit in the (correct) position for %d!#", digit + 1, try);
        else if (digit == 1)
            fprintf (file, "%dnd digit in the (correct) position for %d!#", digit + 1, try);
        else if (digit == 2)
            fprintf (file, "%drd digit in the (correct) position for %d!#", digit + 1, try);
        else
            fprintf (file, "%dth digit in the (correct) position for %d!#", digit + 1, try);
        
        return;
    }
    else{
        /* if the paremeter is 0, then the number in a certain position is in its incorrect position*/
        if (digit == 0)
        fprintf (file, "%dst digit in the incorrect position for %d!#", digit + 1, try);
        else if (digit == 1)
            fprintf (file, "%dnd digit in the incorrect position for %d!#", digit + 1, try);
        else if (digit == 2)
            fprintf (file, "%drd digit in the incorrect position for %d!#", digit + 1, try);
        else
            fprintf (file, "%dth digit in the incorrect position for %d!#", digit + 1, try);

        return;
    }    
}