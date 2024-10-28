#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Defines
#define MAX_BOATS 100
#define MAX_NAME_LENGTH 50
#define MAX_PLACEINFO_LENGTH 20

//Type defs
typedef enum {
    slip,
    land,
    trailor,
    storage,
    noPlace
} PlaceType;

typedef struct {
    char name[MAX_NAME_LENGTH];
    int length;
    PlaceType place;
    char placeInfo[MAX_PLACEINFO_LENGTH];
    double balance;
} BoatType;

PlaceType StringToPlaceType(char *PlaceString) {
    if (!strcasecmp(PlaceString, "slip")) return slip;
    if (!strcasecmp(PlaceString, "land")) return land;
    if (!strcasecmp(PlaceString, "trailor")) return trailor;
    if (!strcasecmp(PlaceString, "storage")) return storage;
    return noPlace;
}

char* PlaceToString(PlaceType place) {
    switch (place) {
        case slip: return "slip";
        case land: return "land";
        case trailor: return "trailor";
        case storage: return "storage";
        case noPlace: return "noPlace";
        default: return "unknown";
    }
}

//Loads the inventory written in the csv file
void loadInventory(const char *filename, BoatType **boats, int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    *count = 0;
    //Malloc Error
    *boats = malloc(MAX_BOATS * sizeof(BoatType));
    if (*boats == NULL) {
        perror("Failed to allocate memory for boats");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), file)) {
        //Checks to see if the maximum number of boats were entered
        if (*count >= MAX_BOATS) {
            printf("Max boat limit reached.\n");
            break;
        }

        BoatType boat;
        char placeStr[MAX_PLACEINFO_LENGTH];
        
        if (sscanf(line, "%49[^,],%d,%19[^,],%19[^,],%lf",
                   boat.name, &boat.length, placeStr, boat.placeInfo, &boat.balance) == 5) {
            boat.place = StringToPlaceType(placeStr);
            (*boats)[(*count)++] = boat;
        } else {
            printf("Error reading line: %s", line);
        }
    }
    //Closes the file
    fclose(file);
}

//Saves the updated inventory into the csv file
void saveInventory(const char *filename, BoatType *boats, int count) {
    FILE *file = fopen(filename, "w");
    //Checks for file opening errors
    if (!file) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < count; i++) {
        fprintf(file, "%s,%d,%s,%s,%.2f\n", 
                boats[i].name, boats[i].length, 
                PlaceToString(boats[i].place), 
                boats[i].placeInfo, 
                boats[i].balance);
    }
    //Closes the file
    fclose(file);
}

//Function to display the inventory
void displayInventory(BoatType *boats, int count) {
    printf("Current Inventory:\n");
    for (int i = 0; i < count; i++) {
        printf("%-20s %2d' %-8s %-6s Owes $%7.2f\n", 
               boats[i].name, boats[i].length, 
               PlaceToString(boats[i].place), 
               boats[i].placeInfo, 
               boats[i].balance);
    }
}

//Function to add a boat to the inventory
void addBoat(BoatType *boats, int *count) {
    if (*count >= MAX_BOATS) {
        printf("Inventory full. Cannot add more boats.\n");
        return;
    }

    BoatType boat;
    char placeStr[MAX_PLACEINFO_LENGTH];

    //Correct format to add a boat into the csv file
    printf("Enter boat details in CSV format (name,length,place,placeInfo,balance): ");
    scanf(" %49[^,],%d,%19[^,],%19[^,],%lf", boat.name, &boat.length, placeStr, boat.placeInfo, &boat.balance);
    boat.place = StringToPlaceType(placeStr);

    boats[(*count)++] = boat;
}

//Function to remove a boat from the inventory
void removeBoat(BoatType *boats, int *count) {
    char name[MAX_NAME_LENGTH];
    printf("Enter the boat name to remove: ");
    scanf(" %49[^\n]", name);


    //Removes the boat from the boats array
    for (int i = 0; i < *count; i++) {
        if (strcasecmp(boats[i].name, name) == 0) {
            for (int j = i; j < *count - 1; j++) {
                boats[j] = boats[j + 1];
            }
            (*count)--;
            printf("Boat '%s' removed.\n", name);
            return;
        }
    }
    //Displays if user entered a non-existing boat
    printf("Boat '%s' not found.\n", name);
}

//Function to add a payment for a boat
void recordPayment(BoatType *boats, int count) {
    char name[MAX_NAME_LENGTH];
    double payment;
    printf("Enter the boat name: ");
    scanf(" %49[^\n]", name);


    for (int i = 0; i < count; i++) {
        if (strcasecmp(boats[i].name, name) == 0) {
            printf("Enter the payment amount: ");
            scanf("%lf", &payment);

            //Checks if user entered an amount to pay more than the amount owed
            if (payment > boats[i].balance) {
                printf("That is more than the amount owed, $%.2f\n", boats[i].balance);
                return;
            }
            //Updates the amount owed for the boat
            boats[i].balance -= payment;
            printf("Payment recorded for boat '%s'. New balance: $%.2f\n", boats[i].name, boats[i].balance);
            return;
        }
    }
    //Displays if user entered a non-existing boat
    printf("Boat '%s' not found.\n", name);
}

//Applies a monthy charge to the boats as a 10% fee
void applyMonthlyCharges(BoatType *boats, int count) {
    for (int i = 0; i < count; i++) {
        boats[i].balance *= 1.1;
    }
    printf("Monthly charges applied.\n");
}

//Compares boats to sort alphabetically
int compareBoats(const void *a, const void *b) {
    BoatType *boatA = (BoatType *)a;
    BoatType *boatB = (BoatType *)b;
    return strcmp(boatA->name, boatB->name);
}

//Sorts the boats in the inventory alphabetically
void sortInventory(BoatType *boats, int count) {
    qsort(boats, count, sizeof(BoatType), compareBoats);
    printf("Inventory sorted.\n");
}

//Main function
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <BoatData.csv>\n", argv[0]);
        return 1;
    }

    BoatType *boats;
    int count;

    loadInventory(argv[1], &boats, &count);

    char choice;
    //Welcome Message
    printf("Welcome to the Boat Management System!\n");
    //Menu
    do {
        printf("(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, (S)ort, e(X)it: ");
        scanf(" %c", &choice);

        switch (choice) {
            case 'I':
            case 'i':
                displayInventory(boats, count);
                break;
            case 'A':
            case 'a':
                addBoat(boats, &count);
                break;
            case 'R':
            case 'r':
                removeBoat(boats, &count);
                break;
            case 'P':
            case 'p':
                recordPayment(boats, count);
                break;
            case 'M':
            case 'm':
                applyMonthlyCharges(boats, count);
                break;
            case 'S':
            case 's':
                sortInventory(boats, count);
                break;
            case 'X':
            case 'x':
                saveInventory(argv[1], boats, count);
                printf("Exiting and saving data. Thanks for using the system!\n");
                break;
            default:
                printf("Invalid option.\n");
                break;
        }
    } while (choice != 'X' && choice != 'x');

    //Free all memory
    free(boats);
    return 0;
}
