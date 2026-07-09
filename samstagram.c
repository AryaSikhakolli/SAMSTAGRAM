#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
int adj[50][50];
char users[50][50];
char passwords[50][50]; 
int userCount=0;
/* -------------------- TREES -------------------- */
typedef struct AVLNode {
    char name[50];
    int idx;
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
} AVLNode;
AVLNode *avlRoot=NULL;
int max(int a, int b) {
    return (a>b)?a:b;
}
int heightAVL(AVLNode *node) {
    if (node) {
        return node->height;
    }
    return 0;
}
AVLNode* createAVLNode(char *name,int idx) {
    AVLNode *node=(AVLNode*)malloc(sizeof(AVLNode));
    strcpy(node->name, name);
    node->idx=idx;
    node->left=NULL;
    node->right=NULL;
    node->height=1;
    return node;
}
AVLNode* rightRotate(AVLNode *y) {
    AVLNode *x=y->left;
    AVLNode *T2=x->right;
    x->right=y;
    y->left=T2;
    y->height=max(heightAVL(y->left), heightAVL(y->right))+1;
    x->height=max(heightAVL(x->left), heightAVL(x->right))+1;
    return x;
}
AVLNode* leftRotate(AVLNode *x) {
    AVLNode *y=x->right;
    AVLNode *T2=y->left;
    y->left=x;
    x->right=T2;
    x->height=max(heightAVL(x->left), heightAVL(x->right))+1;
    y->height=max(heightAVL(y->left), heightAVL(y->right))+1;
    return y;
}
int getBalance(AVLNode *node) {
    if (node) {
        return heightAVL(node->left)-heightAVL(node->right);
    }
    return 0;
}
AVLNode* insertAVL(AVLNode *node,char *name,int idx) {
    if (!node) {
        return createAVLNode(name, idx);
    }
    int cmp=strcmp(name, node->name);
    if (cmp<0) {
        node->left=insertAVL(node->left, name, idx);
    } else if (cmp>0) {
        node->right=insertAVL(node->right, name, idx);
    } else {
        return node;
    }
    node->height=1+max(heightAVL(node->left), heightAVL(node->right));
    int balance=getBalance(node);
    if (balance>1&&strcmp(name, node->left->name)<0) {
        return rightRotate(node);
    }
    if (balance<-1&&strcmp(name, node->right->name)>0) {
        return leftRotate(node);
    }
    if (balance>1&&strcmp(name, node->left->name)>0) {
        node->left=leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance<-1&&strcmp(name, node->right->name)<0) {
        node->right=rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}
int searchAVL(AVLNode *node,char *name) {
    if (!node) {
        return -1;
    }
    int cmp=strcmp(name, node->name);
    if (cmp==0) {
        return node->idx;
    } else if (cmp<0) {
        return searchAVL(node->left, name);
    } else {
        return searchAVL(node->right, name);
    }
}
void inorderAVL(AVLNode *node) {
    if (!node) {
        return;
    }
    inorderAVL(node->left);
    printf("%s\n",node->name);
    inorderAVL(node->right);
}
/* -------------------- Trie-------------------- */
typedef struct TrieNode {
    struct TrieNode *children[26];
    int isEndOfWord;
} TrieNode;
TrieNode *trieRoot=NULL;
TrieNode* createTrieNode() {
    TrieNode *node=(TrieNode*)malloc(sizeof(TrieNode));
    node->isEndOfWord=0;
    for (int i=0; i<26; i++) {
        node->children[i]=NULL;
    }
    return node;
}
void insertTrie(char *word) {
    if (!trieRoot) {
        trieRoot=createTrieNode();
    }
    TrieNode *curr=trieRoot;
    for (int i=0; word[i]; i++) {
        char c=tolower(word[i]);
        if (c<'a'||c>'z') {
            continue;
        }
        int idx=c-'a';
        if (!curr->children[idx]) {
            curr->children[idx]=createTrieNode();
        }
        curr=curr->children[idx];
    }
    curr->isEndOfWord=1;
}
void displayTrie(TrieNode *node,char *prefix) {
    if(node->isEndOfWord){
        printf("- %s\n",prefix);
    }
    int len=strlen(prefix);  
    for(int i=0;i<26;i++) {
        if(node->children[i]){
            prefix[len]='a'+i;   
            prefix[len+1]='\0';  
            displayTrie(node->children[i],prefix);
            prefix[len]='\0';
        }
    }
}

void searchPrefix(char *prefix) {
    if (!trieRoot) {
        printf("No users in network.\n");
        return;
    }
    TrieNode *curr=trieRoot;
    for (int i=0; prefix[i]; i++) {
        char c=tolower(prefix[i]);
        if (c<'a'||c>'z') {
            continue;
        }
        int idx=c-'a';
        if (!curr->children[idx]) {
            printf("No matches for '%s'.\n", prefix);
            return;
        }
        curr=curr->children[idx];
    }
    printf("Usernames starting with '%s':\n", prefix);
    displayTrie(curr,(char*)prefix);
}
/* -------------------- BASIC FUNCTIONS -------------------- */
int findUserIndex(char *name) {
    return searchAVL(avlRoot, name);
}
int authenticate(int idx) {
    char pass[50];
    printf("Enter password for %s: ",users[idx]);
    scanf("%s", pass);
    if (strcmp(pass, passwords[idx])==0) {
        return 1;
    } else {
        printf("Incorrect password!\n");
        return 0;
    }
}

void addUser() {
    char name[50], pass[50];
    printf("Enter user name: ");
    scanf("%s", name);
    if (findUserIndex(name)!=-1) {
        printf("User already exists!\n");
        return;
    }
    if (userCount>=50) {
        printf("User limit reached.\n");
        return;
    }
    printf("Set password for user: ");
    scanf("%s", pass);
    strcpy(users[userCount], name);
    strcpy(passwords[userCount], pass);
    avlRoot=insertAVL(avlRoot, name, userCount);
    insertTrie(name);
    userCount++;
    printf("User '%s' added successfully!\n", name);
}
void addFriendship() {
    char u1[50], u2[50];
    printf("Enter first user: ");
    scanf("%s", u1);
    printf("Enter second user: ");
    scanf("%s", u2);
    int i=findUserIndex(u1);
    int j=findUserIndex(u2);
    if (i==-1||j==-1) {
        printf("User not found!\n");
        return;
    }
    if (i==j) {
        printf("Cannot friend themselves!\n");
        return;
    }
    if (adj[i][j]) {
        printf("Already friends!\n");
        return;
    }
    if (!authenticate(i)) {
        return;
    }
    adj[i][j]=1;
    adj[j][i]=1;
    printf("%s and %s are now friends!\n", u1, u2);
}
void removeFriendship() {
    char u1[50], u2[50];
    printf("Enter first user: ");
    scanf("%s", u1);
    printf("Enter second user: ");
    scanf("%s", u2);
    int i=findUserIndex(u1);
    int j=findUserIndex(u2);
    if (i==-1||j==-1) {
        printf("User not found!\n");
        return;
    }
    if (!adj[i][j]) {
        printf("They are not friends!\n");
        return;
    }
    if (!authenticate(i)) {
        return;
    }
    adj[i][j]=0;
    adj[j][i]=0;
    printf("Friendship between %s and %s removed.\n", u1, u2);
}
void showFriends() {
    char name[50];
    printf("Enter user name: ");
    scanf("%s", name);
    int idx=findUserIndex(name);
    if (idx==-1) {
        printf("User not found!\n");
        return;
    }
    printf("Friends of %s: ", name);
    int count=0;
    for (int i=0; i<userCount; i++) {
        if (adj[idx][i]) {
            printf("%s ", users[i]);
            count++;
        }
    }
    if (count==0) {
        printf("No friends yet!");
    }
    printf("\n");
}
void showMutualFriends() {
    char u1[50], u2[50];
    printf("Enter first user: ");
    scanf("%s", u1);
    printf("Enter second user: ");
    scanf("%s", u2);
    int i=findUserIndex(u1);
    int j=findUserIndex(u2);
    if (i==-1||j==-1) {
        printf("User not found!\n");
        return;
    }
    printf("Mutual friends of %s and %s: ", u1, u2);
    int count=0;
    for (int k=0; k<userCount; k++) {
        if (adj[i][k]&&adj[j][k]){
            printf("%s ", users[k]);
            count++;
        }
    }
    if (count==0) {
        printf("None");
    }
    printf("\n");
}
void suggestFriends() {
    char name[50];
    printf("Enter user name: ");
    scanf("%s", name);
    int idx=findUserIndex(name);
    if (idx==-1) {
        printf("User not found!\n");
        return;
    }
    int suggested[50]={0};
    for (int i=0; i<userCount; i++) {
        if (adj[idx][i]) {
            for (int j=0; j<userCount; j++) {
                if (adj[i][j]&&j!=idx&&!adj[idx][j]) {
                    suggested[j]++;
                }
            }
        }
    }
    printf("Friend suggestions for %s:\n", name);
    int count=0;
    for (int i=0; i<userCount; i++) {
        if (suggested[i]) {
            printf(" - %s (%d mutual)\n", users[i], suggested[i]);
            count++;
        }
    }
    if (count==0) {
        printf("No suggestions available.\n");
    }
}
void mostFriends() {
    int max=0;
    int index=-1;
    for (int i=0; i<userCount; i++) {
        int deg=0;
        for (int j=0; j<userCount; j++) {
            if (adj[i][j]) {
                deg++;
            }
        }
        if (deg>max) {
            max=deg;
            index=i;
        }
    }
    if (index!=-1) {
        printf("User with Most Friends: %s (%d friends)\n", users[index], max);
    } else {
        printf("No users found!\n");
    }
}
void noFriends() {
    int found=0;
    printf("Users with No friends: ");
    for (int i=0; i<userCount; i++) {
        int deg=0;
        for (int j=0; j<userCount; j++) {
            if (adj[i][j]) {
                deg++;
            }
        }
        if (deg==0) {
            printf("%s ", users[i]);
            found=1;
        }
    }
    if (!found) {
        printf("None");
    }
    printf("\n");
}
void networkStats() {
    int totalEdges=0;
    for (int i=0; i<userCount; i++) {
        for (int j=i+1; j<userCount; j++) {
            if (adj[i][j]) {
                totalEdges++;
            }
        }
    }
    double density=(userCount>1)?(2.0*totalEdges)/(userCount*(userCount-1)):0;
    printf("Network has %d users and %d friendships.\n", userCount, totalEdges);
    printf("Average connectivity (density): %.2f\n", density);
}
/* -------------------- Menu -------------------- */
int main() {
    int choice;
    printf("\n==================== SAMTAGRAM ====================\n");
    while (1) {
        printf("------------------------------------------------------------\n");
        printf("1. Add User\n");
        printf("2. Add Friendship\n");
        printf("3. Remove Friendship\n");
        printf("4. Show Friends\n");
        printf("5. Show Mutual Friends\n");
        printf("6. Suggest Friends\n");
        printf("7. User with Most Friends\n");
        printf("8. Users with no Friends\n");
        printf("9. Statistics\n");
        printf("10. List Users\n");
        printf("11. Search Users\n");
        printf("12. Exit\n");
        printf("------------------------------------------------------------\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                addUser();
                break;
            case 2:
                addFriendship();
                break;
            case 3:
                removeFriendship();
                break;
            case 4:
                showFriends();
                break;
            case 5:
                showMutualFriends();
                break;
            case 6:
                suggestFriends();
                break;
            case 7:
                mostFriends();
                break;
            case 8:
                noFriends();
                break;
            case 9:
                networkStats();
                break;
            case 10:
                printf("Users (alphabetical order):\n");
                inorderAVL(avlRoot);
                break;
            case 11: {
                char prefix[50];
                printf("Enter username: ");
                scanf("%s", prefix);
                searchPrefix(prefix);
                break;
            }
            case 12:
                printf("Exiting... Goodbye! \n");
                return 0;
            default:
                printf("Invalid choice! Please try again.\n");
                break;
        }
    }
    return 0;
}
