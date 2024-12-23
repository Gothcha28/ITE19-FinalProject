#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function prototypes
int romanToDecimal(char roman[]);
void decimalToWords(int num, char *result);
void processLine(char line[], FILE *outputFile);
void splitAndProcess(char *expression, char *operator, FILE *outputFile);

// Main function
int main() {
    FILE *inputFile, *outputFile;
    char line[256];

    inputFile = fopen("Input.txt", "r");
    outputFile = fopen("Output.txt", "w");

    if (inputFile == NULL || outputFile == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    while (fgets(line, sizeof(line), inputFile)) {
        processLine(line, outputFile);
    }

    fclose(inputFile);
    fclose(outputFile);

    printf("Conversion completed. Check Output.txt\n");
    return 0;
}

// Function to convert Roman numeral to decimal
int romanToDecimal(char roman[]) {
    int values[128] = {0}; // ASCII table for Roman numeral values
    values['I'] = 1;
    values['V'] = 5;
    values['X'] = 10;
    values['L'] = 50;
    values['C'] = 100;
    values['D'] = 500;
    values['M'] = 1000;

    int total = 0;
    int i;
    for (i = 0; roman[i] != '\0'; i++) {
        if (values[roman[i]] < values[roman[i + 1]]) {
            total -= values[roman[i]];
        } else {
            total += values[roman[i]];
        }
    }
    return total;
}

// Function to convert decimal number to words
void decimalToWords(int num, char *result) {
    const char *ones[] = {"", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine"};
    const char *teens[] = {"Ten", "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"};
    const char *tens[] = {"", "", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety"};
    const char *thousands[] = {"", "Thousand", "Million", "Billion"};

    char temp[100];
    result[0] = '\0';

    if (num == 0) {
        strcpy(result, "Zero");
        return;
    }

    int place = 0;
    while (num > 0) {
        int chunk = num % 1000;
        if (chunk > 0) {
            char chunkWords[50] = "";

            if (chunk / 100 > 0) {
                sprintf(temp, "%s Hundred ", ones[chunk / 100]);
                strcat(chunkWords, temp);
            }

            int remainder = chunk % 100;
            if (remainder >= 10 && remainder < 20) {
                strcat(chunkWords, teens[remainder - 10]);
            } else {
                if (remainder / 10 > 0) {
                    sprintf(temp, "%s ", tens[remainder / 10]);
                    strcat(chunkWords, temp);
                }
                if (remainder % 10 > 0) {
                    strcat(chunkWords, ones[remainder % 10]);
                }
            }

            if (thousands[place][0] != '\0') {
                sprintf(temp, " %s ", thousands[place]);
                strcat(chunkWords, temp);
            }

            strcat(chunkWords, result);
            strcpy(result, chunkWords);
        }

        num /= 1000;
        place++;
    }
}

// Function to process a single line from the input file
void processLine(char line[], FILE *outputFile) {
    // Remove newline or trailing whitespace
    char *newline = strchr(line, '\n');
    if (newline) *newline = '\0';

    // Check for all operators
    char *plus = strstr(line, "+");
    char *minus = strstr(line, "-");
    char *multiply = strstr(line, "*");
    char *divide = strstr(line, "/");

    if (plus != NULL && (minus == NULL || plus < minus) && (multiply == NULL || plus < multiply) && (divide == NULL || plus < divide)) {
        // "+" appears first
        splitAndProcess(line, "+", outputFile);
    } else if (minus != NULL && (multiply == NULL || minus < multiply) && (divide == NULL || minus < divide)) {
        // "-" appears first
        splitAndProcess(line, "-", outputFile);
    } else if (multiply != NULL && (divide == NULL || multiply < divide)) {
        // "*" appears first
        splitAndProcess(line, "*", outputFile);
    } else if (divide != NULL) {
        // "/" appears first
        splitAndProcess(line, "/", outputFile);
    }
}

// Function to split and process the Roman numeral expression
void splitAndProcess(char *expression, char *operator, FILE *outputFile) {
    char roman1[50] = {0}, roman2[50] = {0};
    int result = 0;

    // Tokenize using the operator
    char *token = strtok(expression, operator);
    if (token != NULL) {
        strncpy(roman1, token, sizeof(roman1) - 1);
        token = strtok(NULL, operator);
        if (token != NULL) {
            strncpy(roman2, token, sizeof(roman2) - 1);

            // Trim extra spaces
            strtok(roman1, " ");
            strtok(roman2, " ");

            // Convert to decimal
            int decimal1 = romanToDecimal(roman1);
            int decimal2 = romanToDecimal(roman2);

            // Perform the appropriate operation
            if (strcmp(operator, "+") == 0) {
                result = decimal1 + decimal2;
            } else if (strcmp(operator, "-") == 0) {
                result = decimal1 - decimal2;
            } else if (strcmp(operator, "*") == 0) {
                result = decimal1 * decimal2;
            } else if (strcmp(operator, "/") == 0) {
                if (decimal2 == 0) {
                    fprintf(outputFile, "Division by zero error\n");
                    return;
                }
                result = decimal1 / decimal2;
            }

            // Convert result to words
            char resultWords[256] = {0};
            decimalToWords(result, resultWords);
            fprintf(outputFile, "%s\n", resultWords);
        }
    }
}
