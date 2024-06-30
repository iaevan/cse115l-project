#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Define buffer sizes
#define USERNAME_SIZE 50
#define PASSWORD_SIZE 50
#define PRODUCT_NAME_SIZE 50
#define CARD_NUMBER_SIZE 20

// Function prototypes
void signUp();
void login();
void addProduct();
void initiatePurchase();
void printInvoice(char items[][PRODUCT_NAME_SIZE], int quantities[], float prices[], int itemCount, char paymentMethod[], float total, float vat, float amountReceived, float change, char cardType[]);
int isValidCard(char cardNumber[]);
int isMasterCard(char cardNumber[]);
int isVisa(char cardNumber[]);
void clearScreen();
void pauseScreen();

const char *IDPASS_FILE = "IdPass.txt";
const char *PRODUCT_FILE = "ProductList.txt";
const char *HISTORY_FILE = "PurchaseHistory.txt";

int main() {
    int choice;

    while (1) {
        clearScreen();
        printf("Welcome to CSE115 LAB PROJECT\n");
        printf("1. Sign Up\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                signUp();
                break;
            case 2:
                login();
                break;
            case 3:
                exit(0);
            default:
                printf("Invalid choice, please try again.\n");
                pauseScreen();
        }
    }

    return 0;
}

void signUp() {
    char username[USERNAME_SIZE], password[PASSWORD_SIZE];
    FILE *file = fopen(IDPASS_FILE, "a+");

    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    printf("Enter Username: ");
    scanf("%s", username);
    printf("Enter Password: ");
    scanf("%s", password);

    fprintf(file, "%s %s\n", username, password);
    fclose(file);

    printf("Sign Up Successful! Your ID is: %s\n", username);
    pauseScreen();
}

void login() {
    char username[USERNAME_SIZE], password[PASSWORD_SIZE], storedUsername[USERNAME_SIZE], storedPassword[PASSWORD_SIZE];
    FILE *file = fopen(IDPASS_FILE, "r");

    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    printf("Enter Username: ");
    scanf("%s", username);
    printf("Enter Password: ");
    scanf("%s", password);

    int loginSuccess = 0;
    while (fscanf(file, "%49s %49s", storedUsername, storedPassword) != EOF) {
        if (strcmp(username, storedUsername) == 0 && strcmp(password, storedPassword) == 0) {
            loginSuccess = 1;
            break;
        }
    }
    fclose(file);

    if (loginSuccess) {
        int choice;
        while (1) {
            clearScreen();
            printf("Hello %s\n", username);
            printf("1. Initiate a Purchase\n");
            printf("2. Add a Product\n");
            printf("3. Log out\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1:
                    initiatePurchase();
                    break;
                case 2:
                    addProduct();
                    break;
                case 3:
                    return;
                default:
                    printf("Invalid choice, please try again.\n");
                    pauseScreen();
            }
        }
    } else {
        printf("Invalid Username or Password!\n");
        pauseScreen();
    }
}

void addProduct() {
    char productName[PRODUCT_NAME_SIZE];
    float productPrice;
    FILE *file = fopen(PRODUCT_FILE, "a");

    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    printf("Enter Product Name: ");
    scanf("%s", productName);
    printf("Enter Product Price: ");
    scanf("%f", &productPrice);

    fprintf(file, "%s %.2f\n", productName, productPrice);
    fclose(file);

    printf("Product Added Successfully!\n");
    pauseScreen();
}

void displayProducts() {
    char productName[PRODUCT_NAME_SIZE];
    float productPrice;
    FILE *file = fopen(PRODUCT_FILE, "r");

    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    printf("Product List:\n");
    while (fscanf(file, "%49s %f", productName, &productPrice) != EOF) {
        printf("%s - BDT %.2f\n", productName, productPrice);
    }
    fclose(file);
    pauseScreen();
}

void initiatePurchase() {
    char productName[PRODUCT_NAME_SIZE], productList[10][PRODUCT_NAME_SIZE];
    float productPrice, productPrices[10];
    int productQuantities[10];
    int productCount = 0, i, choice, quantity;
    FILE *file = fopen(PRODUCT_FILE, "r");

    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    clearScreen();
    printf("Product List:\n");
    while (fscanf(file, "%49s %f", productName, &productPrice) != EOF) {
        printf("%d. %s - BDT %.2f\n", productCount + 1, productName, productPrice);
        strncpy(productList[productCount], productName, PRODUCT_NAME_SIZE);
        productList[productCount][PRODUCT_NAME_SIZE - 1] = '\0'; // Ensure null termination
        productPrices[productCount] = productPrice;
        productQuantities[productCount] = 0;
        productCount++;
        if (productCount % 10 == 0) {
            printf("Press Enter to see more...");
            getchar();
        }
    }
    fclose(file);

    printf("Enter the product number to purchase (0 to finish): ");
    while (scanf("%d", &choice) && choice != 0) {
        if (choice > 0 && choice <= productCount) {
            printf("Enter quantity for %s: ", productList[choice - 1]);
            scanf("%d", &quantity);
            productQuantities[choice - 1] += quantity;
        } else {
            printf("Invalid choice! Try again.\n");
        }
        printf("Enter the product number to purchase (0 to finish): ");
    }

    float total = 0;
    for (i = 0; i < productCount; i++) {
        if (productQuantities[i] > 0) {
            total += productPrices[i] * productQuantities[i];
        }
    }
    float vat = total * 0.15;
    total += vat;

    char paymentMethod[10], cardType[10];
    float amountReceived = 0, change = 0;

    printf("Select payment method (cash/card): ");
    scanf("%s", paymentMethod);

    if (strcmp(paymentMethod, "cash") == 0) {
        do {
            printf("Enter amount received (greater than or equal to total amount %.2f): ", total);
            scanf("%f", &amountReceived);
            if (amountReceived < total) {
                printf("Amount received must be greater than or equal to total amount.\n");
            }
        } while (amountReceived < total);
        change = amountReceived - total;
    } else if (strcmp(paymentMethod, "card") == 0) {
        char cardNumber[CARD_NUMBER_SIZE];
        int isValid = 0;
        while (!isValid) {
            printf("Enter card number: ");
            scanf("%s", cardNumber);

            if (isValidCard(cardNumber)) {
                isValid = 1;
                if (isMasterCard(cardNumber)) {
                    strcpy(cardType, "MasterCard");
                } else if (isVisa(cardNumber)) {
                    strcpy(cardType, "VISA");
                } else {
                    strcpy(cardType, "Unknown");
                }
            } else {
                printf("Invalid card number! Please try again.\n");
            }
        }
    } else {
        printf("Invalid payment method!\n");
        return;
    }

    printInvoice(productList, productQuantities, productPrices, productCount, paymentMethod, total, vat, amountReceived, change, cardType);
}

void printInvoice(char items[][PRODUCT_NAME_SIZE], int quantities[], float prices[], int itemCount, char paymentMethod[], float total, float vat, float amountReceived, float change, char cardType[]) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    printf("\nCSE115 LAB PROJECT\n");
    printf("Date: %02d-%02d-%04d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    printf("Time: %02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
    printf("=====================================\n");
    printf("Item\t\tQuantity\tPrice\n");
    printf("=====================================\n");

    for (int i = 0; i < itemCount; i++) {
        if (quantities[i] > 0) {
            printf("%-15s\t%d\t\t%.2f\n", items[i], quantities[i], prices[i] * quantities[i]);
        }
    }

    printf("=====================================\n");
    printf("VAT (15%%)\t\t\t%.2f\n", vat);
    printf("Total\t\t\t\t%.2f\n", total);

    if (strcmp(paymentMethod, "cash") == 0) {
        printf("Payment Method: Cash\n");
        printf("Amount Received: %.2f\n", amountReceived);
        printf("Change: %.2f\n", change);
    } else if (strcmp(paymentMethod, "card") == 0) {
        printf("Payment Method: Card (%s)\n", cardType);
    }

    // Write purchase history to file
    FILE *file = fopen(HISTORY_FILE, "a");
    if (file != NULL) {
        fprintf(file, "Date: %02d-%02d-%04d Time: %02d:%02d:%02d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
        for (int i = 0; i < itemCount; i++) {
            if (quantities[i] > 0) {
                fprintf(file, "%s %d %.2f\n", items[i], quantities[i], prices[i] * quantities[i]);
            }
        }
        fprintf(file, "VAT: %.2f Total: %.2f\n", vat, total);
        fprintf(file, "Payment Method: %s\n", paymentMethod); // Include payment method in the history file
        if (strcmp(paymentMethod, "cash") == 0) {
            fprintf(file, "Amount Received: %.2f Change: %.2f\n", amountReceived, change);
        } else if (strcmp(paymentMethod, "card") == 0) {
            fprintf(file, "Card Type: %s\n", cardType);
        }
        fclose(file); // Close the file after writing
    } else {
        printf("Error writing to purchase history file.\n");
    }

    printf("=====================================\n");
    printf("Thank you for your purchase!\n");
    pauseScreen();
}


int isValidCard(char cardNumber[]) {
    int len = strlen(cardNumber);
    int sum = 0, doubleDigit = 0;
    
    for (int i = len - 1; i >= 0; i--) {
        int digit = cardNumber[i] - '0';
        if (doubleDigit) {
            digit *= 2;
            if (digit > 9) digit -= 9;
        }
        sum += digit;
        doubleDigit = !doubleDigit;
    }
    return (sum % 10 == 0);
}

int isMasterCard(char cardNumber[]) {
    return (cardNumber[0] == '5' && (cardNumber[1] >= '1' && cardNumber[1] <= '5'));
}

int isVisa(char cardNumber[]) {
    return (cardNumber[0] == '4');
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    printf("Press Enter to continue...");
    while (getchar() != '\n');
    getchar();
}
