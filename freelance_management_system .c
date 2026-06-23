#include <stdio.h>

#define MAX_USERS 100
#define MAX_PROJECTS 100
#define MAX_STR_LEN 100

#define TRUE 1
#define FALSE 0

// --- ENUMS & DATA STRUCTURES ---

typedef enum { ROLE_ADMIN = 0, ROLE_FREELANCER, ROLE_CUSTOMER } UserRole;

typedef enum { 
    PROJ_PENDING = 0,   // Just posted
    PROJ_REQUESTED,     // Customer requested a specific freelancer
    PROJ_IN_PROGRESS,   // Freelancer accepted
    PROJ_SUBMITTED,     // Freelancer submitted work
    PROJ_COMPLETED      // Customer verified
} ProjectStatus;

typedef enum { PAY_UNPAID = 0, PAY_PAID } PaymentStatus;

typedef struct {
    int id;
    char username[MAX_STR_LEN];
    char password[MAX_STR_LEN];
    UserRole role;
    int isActive;
} User;

typedef struct {
    int user_id;
    char skills[MAX_STR_LEN];
    int experience_years;
    float hourly_rate;
} FreelancerProfile;

typedef struct {
    int id;
    int customer_id;
    int freelancer_id; // -1 if not assigned
    char title[MAX_STR_LEN];
    char description[MAX_STR_LEN];
    char required_skills[MAX_STR_LEN];
    char deadline[MAX_STR_LEN];
    ProjectStatus status;
    PaymentStatus payment_status;
} Project;

// --- GLOBAL VARIABLES (Simulated Database) ---
User users[MAX_USERS];
FreelancerProfile freelancers[MAX_USERS];
Project projects[MAX_PROJECTS];

int numUsers = 0;
int numFreelancers = 0;
int numProjects = 0;
int loggedInUserId = -1;

// --- CUSTOM UTILITY FUNCTIONS ---

// Replaces strcpy
void my_strcpy(char *dest, const char *src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// Replaces strcmp
int my_strcmp(const char *s1, const char *s2) {
    int i = 0;
    while (s1[i] != '\0' && s1[i] == s2[i]) {
        i++;
    }
    return s1[i] - s2[i];
}

// Replaces strstr (Returns 1 if sub is found in str, 0 otherwise)
int contains_string(const char *str, const char *sub) {
    if (sub[0] == '\0') return TRUE;
    for (int i = 0; str[i] != '\0'; i++) {
        int found = TRUE;
        for (int j = 0; sub[j] != '\0'; j++) {
            if (str[i + j] == '\0' || str[i + j] != sub[j]) {
                found = FALSE;
                break;
            }
        }
        if (found == TRUE) return TRUE;
    }
    return FALSE;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Replaces strcspn for newlines
void removeNewline(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            str[i] = '\0';
            break;
        }
    }
}

const char* getProjectStatusStr(ProjectStatus status) {
    switch(status) {
        case PROJ_PENDING: return "Pending";
        case PROJ_REQUESTED: return "Requested";
        case PROJ_IN_PROGRESS: return "In Progress";
        case PROJ_SUBMITTED: return "Submitted";
        case PROJ_COMPLETED: return "Completed";
        default: return "Unknown";
    }
}

const char* getPaymentStatusStr(PaymentStatus status) {
    return status == PAY_PAID ? "PAID" : "UNPAID";
}

int findUserIndex(int id) {
    for (int i = 0; i < numUsers; i++) {
        if (users[i].id == id && users[i].isActive) return i;
    }
    return -1;
}

int findFreelancerIndex(int user_id) {
    for (int i = 0; i < numFreelancers; i++) {
        if (freelancers[i].user_id == user_id) return i;
    }
    return -1;
}

// --- SYSTEM INITIALIZATION ---
void initSystem() {
    // Create default Admin
    users[numUsers].id = 1;
    my_strcpy(users[numUsers].username, "admin");
    my_strcpy(users[numUsers].password, "admin123");
    users[numUsers].role = ROLE_ADMIN;
    users[numUsers].isActive = TRUE;
    numUsers++;
}

// --- REGISTRATION & LOGIN ---

void registerUser() {
    if (numUsers >= MAX_USERS) {
        printf("System full. Cannot register more users.\n");
        return;
    }

    int roleChoice;
    printf("\n--- Registration ---\n");
    printf("1. Register as Freelancer\n");
    printf("2. Register as Customer\n");
    printf("Select role: ");
    scanf("%d", &roleChoice);
    clearInputBuffer();

    if (roleChoice != 1 && roleChoice != 2) {
        printf("Invalid choice.\n");
        return;
    }

    User newUser;
    newUser.id = numUsers + 1;
    newUser.isActive = TRUE;
    newUser.role = (roleChoice == 1) ? ROLE_FREELANCER : ROLE_CUSTOMER;

    printf("Enter Username: ");
    fgets(newUser.username, MAX_STR_LEN, stdin);
    removeNewline(newUser.username);

    // Check if username exists
    for(int i = 0; i < numUsers; i++) {
        if(my_strcmp(users[i].username, newUser.username) == 0 && users[i].isActive) {
            printf("Username already exists!\n");
            return;
        }
    }

    printf("Enter Password: ");
    fgets(newUser.password, MAX_STR_LEN, stdin);
    removeNewline(newUser.password);

    users[numUsers++] = newUser;

    // Freelancer specific form
    if (newUser.role == ROLE_FREELANCER) {
        FreelancerProfile fp;
        fp.user_id = newUser.id;

        printf("Enter your Skills (comma separated): ");
        fgets(fp.skills, MAX_STR_LEN, stdin);
        removeNewline(fp.skills);

        printf("Enter Experience (Years): ");
        scanf("%d", &fp.experience_years);

        printf("Enter Hourly Rate ($): ");
        scanf("%f", &fp.hourly_rate);
        clearInputBuffer();

        freelancers[numFreelancers++] = fp;
    }
    
    printf("\nRegistration Successful! Your User ID is %d. Please login.\n", newUser.id);
}

int login() {
    char username[MAX_STR_LEN];
    char password[MAX_STR_LEN];

    printf("\n--- Login ---\n");
    printf("Username: ");
    fgets(username, MAX_STR_LEN, stdin);
    removeNewline(username);

    printf("Password: ");
    fgets(password, MAX_STR_LEN, stdin);
    removeNewline(password);

    for (int i = 0; i < numUsers; i++) {
        if (users[i].isActive && my_strcmp(users[i].username, username) == 0 && my_strcmp(users[i].password, password) == 0) {
            loggedInUserId = users[i].id;
            printf("Login successful! Welcome, %s.\n", username);
            return TRUE;
        }
    }
    printf("Invalid credentials or deactivated account.\n");
    return FALSE;
}

// --- ADMIN PANEL FUNCTIONS ---

void adminAddFreelancer() {
    // Simulating admin registering a freelancer manually
    printf("\n-- Admin: Add Freelancer --\n");
    User newUser;
    newUser.id = numUsers + 1;
    newUser.isActive = TRUE;
    newUser.role = ROLE_FREELANCER;

    printf("Enter Username: ");
    fgets(newUser.username, MAX_STR_LEN, stdin);
    removeNewline(newUser.username);

    printf("Enter Password: ");
    fgets(newUser.password, MAX_STR_LEN, stdin);
    removeNewline(newUser.password);

    users[numUsers++] = newUser;

    FreelancerProfile fp;
    fp.user_id = newUser.id;
    printf("Enter Skills: ");
    fgets(fp.skills, MAX_STR_LEN, stdin);
    removeNewline(fp.skills);
    printf("Enter Experience (Years): ");
    scanf("%d", &fp.experience_years);
    printf("Enter Hourly Rate ($): ");
    scanf("%f", &fp.hourly_rate);
    clearInputBuffer();

    freelancers[numFreelancers++] = fp;
    printf("Freelancer added successfully.\n");
}

void adminDeleteFreelancer() {
    int id;
    printf("Enter Freelancer User ID to delete: ");
    scanf("%d", &id);
    clearInputBuffer();

    int idx = findUserIndex(id);
    if (idx != -1 && users[idx].role == ROLE_FREELANCER) {
        users[idx].isActive = FALSE;
        printf("Freelancer ID %d deactivated.\n", id);
    } else {
        printf("Freelancer not found.\n");
    }
}

void adminViewFreelancers() {
    printf("\n--- Registered Freelancers ---\n");
    for (int i = 0; i < numUsers; i++) {
        if (users[i].role == ROLE_FREELANCER && users[i].isActive) {
            int fIdx = findFreelancerIndex(users[i].id);
            if(fIdx != -1) {
                printf("ID: %d | Name: %s | Skills: %s | Exp: %d yrs | Rate: $%.2f/hr\n",
                    users[i].id, users[i].username, freelancers[fIdx].skills, 
                    freelancers[fIdx].experience_years, freelancers[fIdx].hourly_rate);
            }
        }
    }
}

void adminSearchFreelancer() {
    char skill[MAX_STR_LEN];
    printf("Enter skill to search for: ");
    fgets(skill, MAX_STR_LEN, stdin);
    removeNewline(skill);

    printf("\n--- Search Results ---\n");
    for (int i = 0; i < numFreelancers; i++) {
        if (contains_string(freelancers[i].skills, skill) == TRUE) {
            int uIdx = findUserIndex(freelancers[i].user_id);
            if (uIdx != -1) {
                printf("ID: %d | Name: %s | Skills: %s\n", users[uIdx].id, users[uIdx].username, freelancers[i].skills);
            }
        }
    }
}

void adminUpdateFreelancer() {
    int id;
    printf("Enter Freelancer User ID to update: ");
    scanf("%d", &id);
    clearInputBuffer();

    int fIdx = findFreelancerIndex(id);
    int uIdx = findUserIndex(id);
    if (fIdx != -1 && uIdx != -1 && users[uIdx].role == ROLE_FREELANCER) {
        printf("Enter new Hourly Rate ($): ");
        scanf("%f", &freelancers[fIdx].hourly_rate);
        clearInputBuffer();
        printf("Freelancer updated successfully.\n");
    } else {
        printf("Freelancer not found.\n");
    }
}

void adminViewCustomers() {
    printf("\n--- Registered Customers ---\n");
    for (int i = 0; i < numUsers; i++) {
        if (users[i].role == ROLE_CUSTOMER && users[i].isActive) {
            printf("ID: %d | Name: %s\n", users[i].id, users[i].username);
        }
    }
}

void adminViewProjects() {
    printf("\n--- All System Projects ---\n");
    for (int i = 0; i < numProjects; i++) {
        printf("ProjID: %d | Title: %s | Status: %s | Payment: %s\n", 
            projects[i].id, projects[i].title, getProjectStatusStr(projects[i].status), getPaymentStatusStr(projects[i].payment_status));
    }
}

void adminMenu() {
    int choice;
    do {
        printf("\n======= ADMIN PANEL =======\n");
        printf("1. Add Freelancer\n");
        printf("2. Delete Freelancer\n");
        printf("3. View Freelancers\n");
        printf("4. Search Freelancer\n");
        printf("5. Update Freelancer Rate\n");
        printf("6. View Customers\n");
        printf("7. View Projects\n");
        printf("8. Logout\n");
        printf("Choose option: ");
        scanf("%d", &choice);
        clearInputBuffer();

        switch (choice) {
            case 1: adminAddFreelancer(); break;
            case 2: adminDeleteFreelancer(); break;
            case 3: adminViewFreelancers(); break;
            case 4: adminSearchFreelancer(); break;
            case 5: adminUpdateFreelancer(); break;
            case 6: adminViewCustomers(); break;
            case 7: adminViewProjects(); break;
            case 8: printf("Logging out...\n"); loggedInUserId = -1; break;
            default: printf("Invalid choice.\n");
        }
    } while (loggedInUserId != -1);
}

// --- FREELANCER PANEL FUNCTIONS ---

void freelancerNotifications() {
    int fIdx = findFreelancerIndex(loggedInUserId);
    if(fIdx == -1) return;

    printf("\n--- NOTIFICATIONS ---\n");
    int hasNotif = FALSE;
    for(int i = 0; i < numProjects; i++) {
        // Notification 1: Project requested directly to them
        if (projects[i].freelancer_id == loggedInUserId && projects[i].status == PROJ_REQUESTED) {
            printf("[!] CUSTOMER REQUEST: You have a new request for project '%s' (ID: %d)\n", projects[i].title, projects[i].id);
            hasNotif = TRUE;
        }
        // Notification 2: Open project matches expertise
        if (projects[i].status == PROJ_PENDING && contains_string(freelancers[fIdx].skills, projects[i].required_skills) == TRUE) {
            printf("[!] NEW GIG: Project '%s' (ID: %d) matches your expertise!\n", projects[i].title, projects[i].id);
            hasNotif = TRUE;
        }
        // Notification 3: Payment received
        if (projects[i].freelancer_id == loggedInUserId && projects[i].status == PROJ_COMPLETED && projects[i].payment_status == PAY_PAID) {
            printf("[!] PAYMENT: Your payment for project '%s' has been released!\n", projects[i].title);
            hasNotif = TRUE;
        }
    }
    if (hasNotif == FALSE) printf("No new notifications.\n");
    printf("---------------------\n");
}

void freelancereViewRequestsAndManage() {
    printf("\n--- My Project Requests & Active Projects ---\n");
    for (int i = 0; i < numProjects; i++) {
        if (projects[i].freelancer_id == loggedInUserId) {
            printf("ID: %d | Title: %s | Status: %s | Deadline: %s\n", 
                projects[i].id, projects[i].title, getProjectStatusStr(projects[i].status), projects[i].deadline);
        }
    }
}

void freelancerApproveReject() {
    int pId, choice;
    printf("Enter Project ID to manage request: ");
    scanf("%d", &pId);
    clearInputBuffer();

    for (int i = 0; i < numProjects; i++) {
        if (projects[i].id == pId && projects[i].freelancer_id == loggedInUserId && projects[i].status == PROJ_REQUESTED) {
            printf("1. Approve/Accept Project\n");
            printf("2. Reject Project\n");
            printf("Choice: ");
            scanf("%d", &choice);
            clearInputBuffer();

            if (choice == 1) {
                projects[i].status = PROJ_IN_PROGRESS;
                printf("Project Accepted! You can now start working.\n");
            } else if (choice == 2) {
                projects[i].status = PROJ_PENDING; // Goes back to open pool
                projects[i].freelancer_id = -1;
                printf("Project Rejected.\n");
            } else {
                printf("Invalid choice.\n");
            }
            return;
        }
    }
    printf("No pending request found with that ID.\n");
}

void freelancerSubmitProject() {
    int pId;
    printf("Enter Project ID to submit work: ");
    scanf("%d", &pId);
    clearInputBuffer();

    for (int i = 0; i < numProjects; i++) {
        if (projects[i].id == pId && projects[i].freelancer_id == loggedInUserId && projects[i].status == PROJ_IN_PROGRESS) {
            projects[i].status = PROJ_SUBMITTED;
            printf("Project submitted successfully! Waiting for customer verification.\n");
            return;
        }
    }
    printf("Project not found or not in progress.\n");
}

void freelancerViewPayments() {
    printf("\n--- My Payments ---\n");
    for (int i = 0; i < numProjects; i++) {
        if (projects[i].freelancer_id == loggedInUserId && projects[i].status == PROJ_COMPLETED) {
            printf("Project '%s' | Payment Status: %s\n", projects[i].title, getPaymentStatusStr(projects[i].payment_status));
        }
    }
}


void freelancerMenu() {
    int choice;
    do {
        freelancerNotifications();
        printf("\n======= FREELANCER PANEL =======\n");
        printf("1. View Project Requests & Deadlines\n");
        printf("2. Approve/Reject Project Request\n");
        printf("3. Submit Completed Project\n");
        printf("4. View Payments\n");
        printf("5. Logout\n");
        printf("Choose option: ");
        scanf("%d", &choice);
        clearInputBuffer();

        switch (choice) {
            case 1: freelancereViewRequestsAndManage(); break;
            case 2: freelancerApproveReject(); break;
            case 3: freelancerSubmitProject(); break;
            case 4: freelancerViewPayments(); break;
            case 5: printf("Logging out...\n"); loggedInUserId = -1; break;
            default: printf("Invalid choice.\n");
        }
    } while (loggedInUserId != -1);
}

// --- CUSTOMER PANEL FUNCTIONS ---

void customerNotifications() {
    printf("\n--- NOTIFICATIONS ---\n");
    int hasNotif = FALSE;
    for(int i = 0; i < numProjects; i++) {
        if (projects[i].customer_id == loggedInUserId) {
            if (projects[i].status == PROJ_IN_PROGRESS) {
                printf("[!] UPDATE: Freelancer accepted project '%s'. Work is in progress.\n", projects[i].title);
                hasNotif = TRUE;
            } else if (projects[i].status == PROJ_SUBMITTED) {
                printf("[!] REVIEW: Freelancer submitted project '%s'. Ready for verification.\n", projects[i].title);
                hasNotif = TRUE;
            }
        }
    }
    if (hasNotif == FALSE) printf("No new notifications.\n");
    printf("---------------------\n");
}

void customerAddProject() {
    if (numProjects >= MAX_PROJECTS) {
        printf("Max projects reached.\n");
        return;
    }
    Project p;
    p.id = numProjects + 1;
    p.customer_id = loggedInUserId;
    p.freelancer_id = -1;
    p.status = PROJ_PENDING;
    p.payment_status = PAY_UNPAID;

    printf("Enter Project Title: ");
    fgets(p.title, MAX_STR_LEN, stdin);
    removeNewline(p.title);

    printf("Enter Project Description: ");
    fgets(p.description, MAX_STR_LEN, stdin);
    removeNewline(p.description);

    printf("Enter Required Skills (e.g., C,Python): ");
    fgets(p.required_skills, MAX_STR_LEN, stdin);
    removeNewline(p.required_skills);

    printf("Enter Deadline (YYYY-MM-DD): ");
    fgets(p.deadline, MAX_STR_LEN, stdin);
    removeNewline(p.deadline);

    projects[numProjects++] = p;
    printf("Project posted successfully! ID: %d\n", p.id);
}

void customerViewAvailableFreelancers() {
    printf("\n--- Available Freelancers & Rates ---\n");
    for (int i = 0; i < numUsers; i++) {
        if (users[i].role == ROLE_FREELANCER && users[i].isActive) {
            int fIdx = findFreelancerIndex(users[i].id);
            if(fIdx != -1) {
                printf("Freelancer ID: %d | Name: %s | Skills: %s | Rate: $%.2f/hr\n",
                    users[i].id, users[i].username, freelancers[fIdx].skills, freelancers[fIdx].hourly_rate);
            }
        }
    }
}

void customerRequestFreelancer() {
    int pId, fId;
    printf("Enter your Project ID: ");
    scanf("%d", &pId);
    printf("Enter Freelancer ID to request: ");
    scanf("%d", &fId);
    clearInputBuffer();

    // Verify ownership and valid freelancer
    int projFound = FALSE;
    for (int i = 0; i < numProjects; i++) {
        if (projects[i].id == pId && projects[i].customer_id == loggedInUserId) {
            projFound = TRUE;
            if (projects[i].status != PROJ_PENDING && projects[i].status != PROJ_REQUESTED) {
                printf("Project is already in progress or completed.\n");
                return;
            }
            int uIdx = findUserIndex(fId);
            if (uIdx != -1 && users[uIdx].role == ROLE_FREELANCER) {
                projects[i].freelancer_id = fId;
                projects[i].status = PROJ_REQUESTED;
                printf("Freelancer requested successfully!\n");
            } else {
                printf("Invalid Freelancer ID.\n");
            }
            break;
        }
    }
    if(projFound == FALSE) printf("Project not found or you don't own it.\n");
}

void customerVerifyAndPay() {
    int pId;
    printf("Enter Project ID to verify and pay: ");
    scanf("%d", &pId);
    clearInputBuffer();

    for (int i = 0; i < numProjects; i++) {
        if (projects[i].id == pId && projects[i].customer_id == loggedInUserId) {
            if (projects[i].status == PROJ_SUBMITTED) {
                projects[i].status = PROJ_COMPLETED;
                projects[i].payment_status = PAY_PAID;
                printf("Project Verified! Payment sent successfully to Freelancer ID: %d.\n", projects[i].freelancer_id);
            } else {
                printf("Project is not in 'Submitted' state yet.\n");
            }
            return;
        }
    }
    printf("Project not found.\n");
}

void customerMenu() {
    int choice;
    do {
        customerNotifications();
        printf("\n======= CUSTOMER PANEL =======\n");
        printf("1. Post a New Project\n");
        printf("2. View Available Freelancers & Rates\n");
        printf("3. Request Freelancer for Project\n");
        printf("4. Verify Project Reception & Send Payment\n");
        printf("5. Logout\n");
        printf("Choose option: ");
        scanf("%d", &choice);
        clearInputBuffer();

        switch (choice) {
            case 1: customerAddProject(); break;
            case 2: customerViewAvailableFreelancers(); break;
            case 3: customerRequestFreelancer(); break;
            case 4: customerVerifyAndPay(); break;
            case 5: printf("Logging out...\n"); loggedInUserId = -1; break;
            default: printf("Invalid choice.\n");
        }
    } while (loggedInUserId != -1);
}


// --- MAIN ENTRY POINT ---

int main() {
    initSystem();
    int mainChoice;

    while (1) {
        printf("\n========================================\n");
        printf("       FREELANCE MANAGEMENT SYSTEM      \n");
        printf("========================================\n");
        printf("1. Login\n");
        printf("2. Register\n");
        printf("3. Exit\n");
        printf("Choose option: ");
        
        if (scanf("%d", &mainChoice) != 1) {
            clearInputBuffer();
            continue;
        }
        clearInputBuffer();

        switch (mainChoice) {
            case 1:
                if (login() == TRUE) {
                    // Route to correct panel based on user role
                    int uIdx = findUserIndex(loggedInUserId);
                    if (uIdx != -1) {
                        UserRole role = users[uIdx].role;
                        if (role == ROLE_ADMIN) adminMenu();
                        else if (role == ROLE_FREELANCER) freelancerMenu();
                        else if (role == ROLE_CUSTOMER) customerMenu();
                    }
                }
                break;
            case 2:
                registerUser();
                break;
            case 3:
                printf("Exiting System... Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
    return 0;
}