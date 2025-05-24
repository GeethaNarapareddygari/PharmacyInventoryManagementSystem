#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>
#define ORDER 3
#define sup_ID_len 10
#define name_len 30
#define info_len 30
#define ID_len 10
#define batch_num_len 10
#define max_suppliers 100 
#define DataBase_Size 100
#define INITIAL_CAPACITY 10
#define FILE_NAME "C:\\Users\\aksha\\OneDrive\\Documents\\2nd year stuff\\DSPD-2\\assign-2\\medication_records.txt"
#define ADD_MED "Addmedications.txt"
#define SUP_FILE_NAME "suppliers_list.txt"
#define ADD_SUP "Addsupplier.txt"
#define ADD_MEDSUP "Addmedsup.txt"
typedef enum{FAILURE,SUCCESS} status_code;
typedef enum{FALSE,TRUE} Boolean;
typedef struct Supplier_Info 
{
  int Supplier_ID;
  char Supplier_name[name_len];
  int quantity_of_med;
  char contact_info[info_len];  
  float turnover;
  int count;
}Supplier_Info;

typedef struct Supplier_node
{
  Supplier_Info sup_data[ORDER-1];
  struct Supplier_node* sup_children[ORDER];
  int sup_keys;
  Boolean is_supleaf;
    
}Supplier_node;


typedef struct Medication_Record //structure to store the  medication information
{
    int Medication_ID;
    char Medication_name[name_len];
    int Batch_number;
    int quantity_in_stock;
    float Priceperunit;
    char Expiration_date[11];
    struct Supplier_Info supplier;
    int Reorder_level;
    int Total_sales;   
}Medication_Record;


typedef struct Medication_Node_Tag
{
  Medication_Record med_data[ORDER-1];
  struct Medication_Node_Tag*med_children[ORDER];
  int med_keys;
  Boolean is_medleaf;
}Medication_node;


Medication_Record* searchMedicationID(Medication_node* root, int medicationID);

Medication_node* createBTreeNode(Boolean med_leaf) 
{
    Medication_node* node = (Medication_node*)malloc(sizeof(Medication_node));
    if (node == NULL) 
    {
        printf("Error: Memory allocation failed for B-Tree node!\n");
        return NULL;
    }
    node->is_medleaf = med_leaf;
    node->med_keys = 0;
    for (int i = 0; i < ORDER; i++)
    {
        node->med_children[i] = NULL;
    }
    return node;
}

void splitChild(Medication_node* parent, int index, Medication_node* fullChild) 
{
    Medication_node* newChild = createBTreeNode(fullChild->is_medleaf);
    int mid = (ORDER - 1) / 2;
    // Move second half of keys to new child
    newChild->med_keys = (ORDER -1)-mid-1;
    int right_keys = (ORDER - 1) - mid - 1;
    // Move second half of keys to new child
    for (int j = 0; j < right_keys; j++) 
    {
        newChild->med_data[j].Medication_ID = fullChild->med_data[mid + 1 + j].Medication_ID;
        strcpy(newChild->med_data[j].Medication_name, fullChild->med_data[mid + 1 + j].Medication_name);
        newChild->med_data[j].Batch_number=fullChild->med_data[mid + 1 + j].Batch_number;
        newChild->med_data[j].quantity_in_stock = fullChild->med_data[mid + 1 + j].quantity_in_stock;
        newChild->med_data[j].Priceperunit = fullChild->med_data[mid + 1 + j].Priceperunit;
        strcpy(newChild->med_data[j].Expiration_date, fullChild->med_data[mid + 1 + j].Expiration_date);
        newChild->med_data[j].supplier.Supplier_ID = fullChild->med_data[mid + 1 + j].supplier.Supplier_ID;
        strcpy(newChild->med_data[j].supplier.Supplier_name, fullChild->med_data[mid + 1 + j].supplier.Supplier_name);
        newChild->med_data[j].supplier.quantity_of_med = fullChild->med_data[mid + 1 + j].supplier.quantity_of_med;
        strcpy(newChild->med_data[j].supplier.contact_info, fullChild->med_data[mid + 1 + j].supplier.contact_info);
        newChild->med_data[j].supplier.turnover = fullChild->med_data[mid+ 1 + j].supplier.turnover;
        newChild->med_data[j].supplier.count = fullChild->med_data[mid + 1 +j].supplier.count;
        newChild->med_data[j].Reorder_level = fullChild->med_data[mid + 1 + j].Reorder_level;
        newChild->med_data[j].Total_sales = fullChild->med_data[mid + 1 + j].Total_sales;
    }
    // If it's not a leaf, move children pointers
    if (!fullChild->is_medleaf) {
        for (int j = 0; j < ORDER / 2; j++) {
            newChild->med_children[j] = fullChild->med_children[mid + 1 + j];
        }
    }
    fullChild->med_keys = mid;
    // Shift parent's children to make room for new child
    for (int j = parent->med_keys; j > index; j--) {
        parent->med_children[j + 1] = parent->med_children[j];
    }
    // Link new child to the parent
    parent->med_children[index + 1] = newChild;
    // Shift parent's keys to make room for middle key
    for (int j = parent->med_keys - 1; j >= index; j--) {
        parent->med_data[j + 1] = parent->med_data[j];
    }
    // Copy middle key to parent node (key at mid)
    parent->med_data[index] = fullChild->med_data[mid];
    // Increase the number of keys in the parent node
    parent->med_keys++;
}

void insertNonFull(Medication_node* node, Medication_Record NewMedication) 
{
    int i = node->med_keys - 1;

    // If the node is a leaf, find the correct position and insert
    if (node->is_medleaf) 
    {
        while (i >= 0 && NewMedication.Medication_ID < node->med_data[i].Medication_ID) 
        {
            node->med_data[i + 1] = node->med_data[i];
            i--;
        }

        //node->med_data[i + 1] = med;
        (node)->med_data[i+1].Medication_ID=NewMedication.Medication_ID;
        strcpy(node->med_data[i + 1].Medication_name,NewMedication.Medication_name);
        node->med_data[i + 1].Batch_number=NewMedication.Batch_number;
        node->med_data[i + 1].quantity_in_stock=NewMedication.quantity_in_stock;
        node->med_data[i + 1].Priceperunit=NewMedication.Priceperunit;
        strcpy(node->med_data[i + 1].Expiration_date,NewMedication.Expiration_date);
        node->med_data[i + 1].supplier.Supplier_ID=NewMedication.supplier.Supplier_ID;
        strcpy(node->med_data[i + 1].supplier.Supplier_name,NewMedication.supplier.Supplier_name);
        node->med_data[i + 1].supplier.quantity_of_med=NewMedication.supplier.quantity_of_med;
        strcpy(node->med_data[i + 1].supplier.contact_info,NewMedication.supplier.contact_info);
        node->med_data[i + 1].supplier.count=NewMedication.supplier.count;
        node->med_data[i + 1].supplier.turnover=NewMedication.supplier.turnover;
        node->med_data[i + 1].Reorder_level=NewMedication.Reorder_level;
        node->med_data[i + 1].Total_sales=NewMedication.Total_sales;
        node->med_keys++;
    }
    else 
    { 
        // Find the correct child to insert
        while (i >= 0 && NewMedication.Medication_ID < node->med_data[i].Medication_ID) {
            i--;
        }
        i++;

        // If the child is full, split before insertion
        if (node->med_children[i]->med_keys == ORDER - 1)
         {
            splitChild(node, i, node->med_children[i]);

            // Determine which of the two children to insert into
            if (NewMedication.Medication_ID > node->med_data[i].Medication_ID) 
            {
                i++;
            }
        }
        insertNonFull(node->med_children[i], NewMedication);
    }
}

status_code AddNewMedication(Medication_node** root, Medication_Record NewMedication) 
{
    int SC = SUCCESS;
    printf("entered AddNewMedications()\n");

    // Check for duplicate Medication_ID
    if (searchMedicationID(*root, NewMedication.Medication_ID) != NULL) 
    {
        printf("Error: Medication with ID %d already exists. Duplicate not allowed.\n", NewMedication.Medication_ID);
        return FAILURE;
    }

    if (*root == NULL) 
    {
        *root = createBTreeNode(TRUE);
        if (*root == NULL)
        { 
            SC = FAILURE;
        }
        else
        {
            (*root)->med_data[0] = NewMedication;
            (*root)->med_keys = 1;  
        }
    }
    else if ((*root)->med_keys == ORDER - 1) 
    {
        printf("entered else if condition\n"); 
        Medication_node* newRoot = createBTreeNode(FALSE);
        if (!newRoot)
        {
            SC = FAILURE;
        }
        else
        {
            newRoot->med_children[0] = *root;
            splitChild(newRoot, 0, *root);
            int i = (NewMedication.Medication_ID > newRoot->med_data[0].Medication_ID) ? 1 : 0;
            insertNonFull(newRoot->med_children[i], NewMedication);
            *root = newRoot;
        }
    } 
    else 
    {
        insertNonFull(*root, NewMedication);
    }

    return SC;
}

void printMedication(Medication_Record med)
{
    printf("medication ID: %d\n", med.Medication_ID);
    printf("medication name: %s\n", med.Medication_name);
    printf("Batch number: %d\n", med.Batch_number);
    printf("quantity in stock : %d\n", med.quantity_in_stock);
    printf("price per unit: %f\n", med.Priceperunit);
    printf("expiration date: %s\n", med.Expiration_date);
    printf("supplier ID; %d\n", med.supplier.Supplier_ID);
    printf("supplier name: %s\n", med.supplier.Supplier_name);
    printf("quantity supplied by supplier: %d\n", med.supplier.quantity_of_med);
    printf("contact information: %s\n", med.supplier.contact_info);
    printf("supplier turnover: %f\n", med.supplier.turnover);
    printf("supplier medication count: %d\n", med.supplier.count);
    printf("Reorder level: %d\n", med.Reorder_level);
    printf("Total sales: %d\n", med.Total_sales);
    printf("\n");
}

void traverseMed(Medication_node* root) 
{
    if (root == NULL) return;
    int i;
    for (i = 0; i < root->med_keys; i++) 
    {
        if (!root->is_medleaf) {
            traverseMed(root->med_children[i]);
        }
        //printf(" %d", root->med_data[i].Medication_ID);
        printMedication(root->med_data[i]);
    }
    if (!root->is_medleaf) {
        traverseMed(root->med_children[i]);
    }
}

Medication_node* readMedrec(char *filename) 
{
    FILE *file = fopen(filename, "r");
    status_code SC=SUCCESS;
    if (!file) 
    {
        printf(" Error: Unable to open file %s\n", filename);
        return NULL;
    }

    Medication_node *root = NULL;
    char line[256];  // Buffer to read each line

    while (fgets(line, sizeof(line), file)) 
    {
     // printf("entered while loop\n");
        Medication_Record med;
       // Medication_node *nptr;
        printf("DEBUG: Read line -> %s", line); // Debugging

        int count = sscanf(line, "%d,%29[^,],%d,%d,%f,%10[^,],%d,%29[^,],%d,%29[^,],%f,%d,%d,%d",
                           &med.Medication_ID, med.Medication_name, &med.Batch_number, &med.quantity_in_stock,
                           &med.Priceperunit, med.Expiration_date, &med.supplier.Supplier_ID, med.supplier.Supplier_name,
                           &med.supplier.quantity_of_med, med.supplier.contact_info,&med.supplier.turnover,
                           &med.supplier.count,&med.Reorder_level, &med.Total_sales);
        
        if (count == 14) 
        {

            printf("DEBUG: Successfully parsed %d fields.\n", count);
          // nptr=CreateMedication(med); 
           // printf("created the new node\n");
            if (AddNewMedication(&root,med) == FAILURE) 
            {
                printf("!!! Error: Memory allocation failed!\n");
                SC=FAILURE;
                break;
            }
            else
            {
              printf("added a new node to the tree\n");
            }
        } 
        else 
        {
            printf("!!!Warning: Skipping invalid line: %s\n", line);
            SC=FAILURE;
        }
    }

    fclose(file);
    if(SC==FAILURE)
    {
      printf(" ***Successfully loaded medication records from file.\n");
    }  
    return root;
}

Medication_Record* searchMedicationID(Medication_node* root, int medicationID) 
{
    if (root == NULL) 
    {
        return NULL; 
    }

    int low = 0, high = root->med_keys - 1, mid;
    while (low <= high) 
    {
        mid = (low + high) / 2;
        
        if (root->med_data[mid].Medication_ID == medicationID) 
        {
            return &root->med_data[mid]; // Found the medication
        } 
        else if (medicationID < root->med_data[mid].Medication_ID) 
        {
            high = mid - 1; // Search left half
        } 
        else 
        {
            low = mid + 1; // Search right half
        }
    }
    // If the node is a leaf, the medication ID does not exist
    if (root->is_medleaf) 
    {
        return NULL;
    }
    return searchMedicationID(root->med_children[low], medicationID);
}

status_code SearchMedicationName(Medication_node*root,char medname[name_len])
{
    status_code SC=SUCCESS;
    if(root==NULL)
    {
        SC=FAILURE;
    }
    else
    {
        int i;
        for (i = 0; i < root->med_keys; i++) 
        {

            if (!root->is_medleaf) 
            {
                SearchMedicationName(root->med_children[i],medname);
            }
            if(strcmp(root->med_data[i].Medication_name,medname)==0)
            {
                printMedication(root->med_data[i]);
            }
        }
        if (!root->is_medleaf) 
        {
            SearchMedicationName(root->med_children[i],medname);
        }
    }
    return SC;
}

status_code SearchMedicationSupplier(Medication_node* root, int Sup_ID)
{
    status_code SC = FAILURE;

    if (root == NULL)
    {
        return FAILURE;
    }

    int i;
    for (i = 0; i < root->med_keys; i++) 
    {
        if (!root->is_medleaf) 
        {
            if (SearchMedicationSupplier(root->med_children[i], Sup_ID) == SUCCESS)
            {
                SC = SUCCESS;
            }
        }

        if (root->med_data[i].supplier.Supplier_ID == Sup_ID)
        {
            printMedication(root->med_data[i]);
            SC = SUCCESS;
        }
    }

    if (!root->is_medleaf)
    {
        if (SearchMedicationSupplier(root->med_children[i], Sup_ID) == SUCCESS)
        {
            SC = SUCCESS;
        }
    }

    return SC;
}

status_code searchMedication(Medication_node*root)
{
    int choice=0,found=0;
    printf("1.search by medication ID\n 2.search by medication name\n 3.search by supplier ID \n choose either 1 or 2 or 3: ");
    scanf("%d",&choice);
    int MED_ID;
    char med_name[name_len];
    int SUP_ID;
    status_code SC=SUCCESS;
    if(choice==1)
    {
        SC=SUCCESS;
        printf("enter the med ID you want to search for: ");
        scanf("%d",&MED_ID);
        Medication_Record*med=searchMedicationID(root,MED_ID);
        if(med==NULL)
        {
            SC=FAILURE;
        }
        else
        {
            printf("medication ID: %d\n",med->Medication_ID);
            printf("medication name: %s\n",med->Medication_name);
            printf("Batch number: %d\n",med->Batch_number);
            printf("quantity in stock : %d\n",med->quantity_in_stock);
            printf("price per unit: %f\n",med->Priceperunit );
            printf("expiration date: %s\n",med->Expiration_date);
            printf("supplier ID; %d\n",med->supplier.Supplier_ID);
            printf("supplier name: %s\n",med->supplier.Supplier_name);
            printf("quantity supplied by supplier: %d\n",med->supplier.quantity_of_med);
            printf("contact information: %s\n",med->supplier.contact_info);
            printf("supplier turnover: %f\n",med->supplier.turnover);
            printf("supplier medication count: %d\n",med->supplier.count);
            printf("Reorder level: %d\n",med->Reorder_level);
            printf("Total sales: %d\n",med->Total_sales);
            printf("\n");
        }
       
    }

    if(choice==2)
    {
        SC=FAILURE;
        printf("enter the medication name: ");
        scanf("%s", med_name);
        SC=SearchMedicationName(root,med_name);
    }

    if(choice==3)
    {
        SC=FAILURE;
        printf("enter the supplier ID: ");
        scanf("%d",&SUP_ID);
        SC=SearchMedicationSupplier(root,SUP_ID);
    }
    return SC;
}

status_code updateMedication(Medication_node* root) 
{
    status_code SC=SUCCESS;
    int Med_ID;
    printf("enter the medication ID to get updated");
    scanf("%d",&Med_ID);
    Medication_Record* med = searchMedicationID(root, Med_ID);

    if (med == NULL)
    {
        SC=FAILURE;
    }
    else
    {
        printf("enter the medication name: ");
        scanf(" %s",med->Medication_name);
        printf("enter the batch number: ");
        scanf("%d",&med->Batch_number);
        printf("enter the quantity in stock: ");
        scanf("%d",&med->quantity_in_stock);
        printf("enter the price per unit: ");
        scanf("%f",&med->Priceperunit);
        printf("enter the expirartion date in DD-MM_YYYY format: ");
        scanf(" %s",med->Expiration_date);
        printf("enter the supplier ID: ");
        scanf("%d",&med->supplier.Supplier_ID);
        printf("enter the supplier name: ");
        scanf(" %s",med->supplier.Supplier_name);
        printf("enter quantity of medication supplied by supplier: ");
        scanf("%d",&med->supplier.quantity_of_med);
        printf("enter the turnover of the supplier: ");
        scanf("%f",&med->supplier.turnover);
        printf("enter no.of medications supplied by the supplier: ");
        scanf("%d",&med->supplier.count);
        printf("enter the contact information: ");
        scanf(" %s",med->supplier.contact_info);
        printf("enter the reorder level: ");
        scanf("%d",&med->Reorder_level);
        printf("enter the total sales: ");
        scanf("%d",&med->Total_sales);
    }
    return SC;
}

void updateMedicationsWithSupplier(Medication_node* root, Supplier_Info updated_supplier) 
{
    if (root == NULL) return;

    int i;
    for (i = 0; i < root->med_keys; i++) {
        if (!root->is_medleaf) {
            updateMedicationsWithSupplier(root->med_children[i], updated_supplier);
        }

        if (root->med_data[i].supplier.Supplier_ID == updated_supplier.Supplier_ID) {
            // Update all fields of the supplier in the medication record
            root->med_data[i].supplier = updated_supplier;

            printf("🔁 Updated Med ID %d with new supplier info.\n", root->med_data[i].Medication_ID);
        }
    }

    if (!root->is_medleaf) {
        updateMedicationsWithSupplier(root->med_children[i], updated_supplier);
    }
}




Medication_Record getPredecessor(Medication_node* node, int idx) 
{
    Medication_node* cur = node->med_children[idx];
    while (!cur->is_medleaf)
    {
        cur = cur->med_children[cur->med_keys - 1]; 
    }
    return (cur->med_data[cur->med_keys - 1]);
}

Medication_Record getSuccessor(Medication_node* node, int idx) 
{
    Medication_node* cur = node->med_children[idx + 1];
    while (!cur->is_medleaf)
    {
        cur = cur->med_children[0];
    }
    return cur->med_data[0];
}

void merge(Medication_node* node, int idx) 
{
    Medication_node* child = node->med_children[idx];
    Medication_node* sibling = node->med_children[idx + 1];

    // Pull the key from current node down into the child
    child->med_data[child->med_keys] = node->med_data[idx];

    // Copy keys from sibling to child
    for (int i = 0; i < sibling->med_keys; i++) {
        child->med_data[i + ((ORDER - 1)/2)+1] = sibling->med_data[i];
    }

    // Copy children from sibling to child (if not leaf)
    if (!child->is_medleaf) {
        for (int i = 0; i <= sibling->med_keys; i++) {
            child->med_children[i +((ORDER - 1)/2 +1)] = sibling->med_children[i];
        }
    }

    // Update key count of child
    child->med_keys += sibling->med_keys + 1;

    // Shift keys and children in parent
    for (int i = idx + 1; i < node->med_keys; i++) {
        node->med_data[i - 1] = node->med_data[i];
        node->med_children[i] = node->med_children[i + 1];
    }

    node->med_keys--;

    // Remove dangling pointer
    node->med_children[node->med_keys + 1] = NULL;

    free(sibling);
}

void borrowFromPrev(Medication_node *node, int idx) 
{
    Medication_node *child = node->med_children[idx];
    Medication_node *sibling = node->med_children[idx - 1];

    // Shift child’s keys and children to the right
    for (int i = child->med_keys - 1; i >= 0; i--)
        child->med_data[i + 1] = child->med_data[i];

    if (!child->is_medleaf) {
        for (int i = child->med_keys; i >= 0; i--)
            child->med_children[i + 1] = child->med_children[i];
    }

    // Move key from parent to child
    child->med_data[0] = node->med_data[idx - 1];

    // Move sibling’s last child if not leaf
    if (!child->is_medleaf)
        child->med_children[0] = sibling->med_children[sibling->med_keys];

    // Move sibling’s last key to parent
    node->med_data[idx - 1] = sibling->med_data[sibling->med_keys - 1];

    child->med_keys += 1;
    sibling->med_keys -= 1;
}

void borrowFromNext(Medication_node *node, int idx) 
{
    Medication_node *child = node->med_children[idx];
    Medication_node *sibling = node->med_children[idx + 1];

    // Move key from parent to child
    child->med_data[child->med_keys] = node->med_data[idx];

    // Move sibling's first child to child if not leaf
    if (!child->is_medleaf)
        child->med_children[child->med_keys + 1] = sibling->med_children[0];

    // Move sibling’s first key up to parent
    node->med_data[idx] = sibling->med_data[0];

    // Shift keys and children in sibling
    for (int i = 1; i < sibling->med_keys; ++i)
        sibling->med_data[i - 1] = sibling->med_data[i];

    if (!sibling->is_medleaf) {
        for (int i = 1; i <= sibling->med_keys; ++i)
            sibling->med_children[i - 1] = sibling->med_children[i];
    }

    child->med_keys += 1;
    sibling->med_keys -= 1;
}

void fill(Medication_node* node, int idx) 
{
    if (idx > 0 && node->med_children[idx - 1]->med_keys >= ((ORDER-1)/2 )+1) 
    {
        borrowFromPrev(node, idx);
    }
    else if (idx < node->med_keys && node->med_children[idx + 1]->med_keys >=((ORDER-1)/2) +1) 
    {
        borrowFromNext(node, idx);
    }
    else 
    {
        if (idx < node->med_keys) 
        {
            merge(node, idx);
        }
        else 
        {
            merge(node, idx - 1);
        }
    }
}

status_code deleteMedication(Medication_node** root, int Med_ID) 
{
    status_code SC = SUCCESS;

    if (*root == NULL) 
    {
        printf("Tree is empty\n");
        SC=FAILURE;
    }
    Medication_node* ptr = *root;
    int i = 0;
    while (i < ptr->med_keys && Med_ID > ptr->med_data[i].Medication_ID)
    {
        i++;
    }
    printf("%d\n",i);
    // Case 1: Key found at current node
    printf("Before check: i = %d, ptr->med_keys = %d\n", i, ptr->med_keys);
    if (i < ptr->med_keys && ptr->med_data[i].Medication_ID == Med_ID) 
    {
        if (ptr->is_medleaf) 
        {
            // Leaf node: just remove it
            for (int j = i + 1; j < ptr->med_keys; j++)
            {
                ptr->med_data[j - 1] = ptr->med_data[j];
            } 
            ptr->med_keys--;
        } 
        else 
        {
            // Internal node: try predecessor, successor or merge
            if (ptr->med_children[i] && ptr->med_children[i]->med_keys >= ((ORDER-1) / 2)+1) {
                Medication_Record pred = getPredecessor(ptr, i);
                ptr->med_data[i] = pred;
                SC = deleteMedication(&ptr->med_children[i], pred.Medication_ID);
            } else if ((i + 1 <= ptr->med_keys) && ptr->med_children[i + 1] && ptr->med_children[i + 1]->med_keys >= ((ORDER-1) / 2)+1) {
                Medication_Record succ = getSuccessor(ptr, i);
                ptr->med_data[i] = succ;
                SC = deleteMedication(&ptr->med_children[i + 1], succ.Medication_ID);
            } else {
                // Merge and then delete
                merge(ptr, i);
                SC = deleteMedication(&ptr->med_children[i], Med_ID);
            }
        }
    }

    // Case 2: Key is not present in this node
    else 
    {
        printf("ENTERING INTO ELSE THAT IS NOT EQUAL TO MED\n");
        printf("ENTERING INTO ELSE THAT IS NOT EQUAL TO MED\n");
        printf("ptr->med_keys = %d\n", ptr->med_keys);
        printf("i = %d\n", i); 
        for (int k = 0; k < ptr->med_keys; k++) {
        printf("Key[%d] = %d\n", k, ptr->med_data[k].Medication_ID);
        }
        if (ptr->is_medleaf) 
        {
            printf("Key %d not found in the tree.\n", Med_ID);
            return FAILURE;
        }
        printf("yeah\n");
        if (ptr == NULL) {
            printf("CRITICAL: ptr is NULL before checking flag\n");
            return FAILURE;
        }
        printf("yeah\n");
        Boolean flag = FALSE;
       if (ptr != NULL)
         flag = (i == ptr->med_keys);
        printf("settted flag");
        if (ptr->med_children[i] == NULL) {
            printf("Child at index %d is NULL before fill\n", i);
            return FAILURE;
        }

        // Fill child if it has less than minimum keys
        if (ptr->med_children[i]->med_keys < ((ORDER-1)/2)+1) 
        {
            fill(ptr, i);
            printf("filling is completed");
            // If child merged to left, update i
            if (flag && i >= ptr->med_keys)
                i--;
        }

        // Recheck after fill or merge
        if (ptr->med_children[i] == NULL) 
        {
            printf("Null child encountered after fill at index %d\n", i);
            return FAILURE;
        }

        printf("calling delete\n");
        SC = deleteMedication(&ptr->med_children[i], Med_ID);
    }

    // If the root node has 0 keys now, collapse the tree
    if (*root && (*root)->med_keys == 0) {
        Medication_node* tmp = *root;
        if (tmp->is_medleaf) {
            free(tmp);
            *root = NULL;
        } else {
            *root = tmp->med_children[0];
            free(tmp);
        }
    }

    return SC;
}







Supplier_node* createSupplier_node(Boolean sup_leaf) 
{
    Supplier_node* node = (Supplier_node*)malloc(sizeof(Supplier_node));
    if (node == NULL) 
    {
        printf("Error: Memory allocation failed for B-Tree node!\n");
        return NULL;
    }
    node->is_supleaf = sup_leaf;
    node->sup_keys = 0;
    for (int i = 0; i < ORDER; i++)
    {
        node->sup_children[i] = NULL;
    }
    return node;
}

void splitSupChild(Supplier_node* parent, int index, Supplier_node* fullChild) 
{
    Supplier_node* newChild = createSupplier_node(fullChild->is_supleaf);
    int mid = (ORDER - 1) / 2;
    // Move second half of keys to new child
    newChild->sup_keys = (ORDER -1)-mid-1;
    int right_keys=(ORDER -1)-mid-1;
    // Move second half of keys to new child
    for(int j = 0 ; j <right_keys ; j++) 
    {
        newChild->sup_data[j].Supplier_ID = fullChild->sup_data[mid + 1 + j].Supplier_ID;
        strcpy(newChild->sup_data[j].Supplier_name, fullChild->sup_data[mid + 1 + j].Supplier_name);
        strcpy(newChild->sup_data[j].contact_info, fullChild->sup_data[mid + 1 + j].contact_info);
        newChild->sup_data[j].count = fullChild->sup_data[mid + 1 + j].count;
        newChild->sup_data[j].quantity_of_med = fullChild->sup_data[mid + 1 + j].quantity_of_med;
        newChild->sup_data[j].turnover = fullChild->sup_data[mid + 1 + j].turnover;
      
    }
    // If it's not a leaf, move children pointers
    if (!fullChild->is_supleaf) 
    {
        for (int j = 0; j <= ORDER / 2; j++) {
            newChild->sup_children[j] = fullChild->sup_children[mid + 1 + j];
        }
    }
    fullChild->sup_keys = mid;
    // Shift parent's children to make room for new child
    for (int j = parent->sup_keys; j > index; j--) 
    {
        parent->sup_children[j + 1] = parent->sup_children[j];
    }
    // Link new child to the parent
    parent->sup_children[index + 1] = newChild;
    // Shift parent's keys to make room for middle key
    for (int j = parent->sup_keys - 1; j >= index; j--) {
        parent->sup_data[j + 1] = parent->sup_data[j];
    }
    // Copy middle key to parent node (key at mid)
    parent->sup_data[index] = fullChild->sup_data[mid];
    // Increase the number of keys in the parent node
    parent->sup_keys++;
}

void sup_insertNonFull(Supplier_node* node,Supplier_Info NewSupplier) 
{
    int i = node->sup_keys - 1;

    // If the node is a leaf, find the correct position and insert
    if (node->is_supleaf) 
    {
        while (i >= 0 && NewSupplier.Supplier_ID < node->sup_data[i].Supplier_ID) 
        {
            node->sup_data[i + 1] = node->sup_data[i];
            i--;
        }

        //node->med_data[i + 1] = med;
        (node)->sup_data[i+1].Supplier_ID=NewSupplier.Supplier_ID;
        strcpy(node->sup_data[i + 1].Supplier_name,NewSupplier.Supplier_name);
        strcpy(node->sup_data[i + 1].contact_info,NewSupplier.contact_info);
        node->sup_data[i + 1].count=NewSupplier.count;
        node->sup_data[i + 1].turnover=NewSupplier.turnover;
        node->sup_data[i + 1].quantity_of_med=NewSupplier.quantity_of_med;
        node->sup_keys++;
    }
    else 
    { 
        // Find the correct child to insert
        while (i >= 0 && NewSupplier.Supplier_ID < node->sup_data[i].Supplier_ID) {
            i--;
        }
        i++;

        // If the child is full, split before insertion
        if (node->sup_children[i]->sup_keys == ORDER - 1)
         {
            splitSupChild(node, i, node->sup_children[i]);

            // Determine which of the two children to insert into
            if (NewSupplier.Supplier_ID > node->sup_data[i].Supplier_ID) 
            {
                i++;
            }
        }
        sup_insertNonFull(node->sup_children[i], NewSupplier);
    }
}

status_code AddNewSupplier(Supplier_node** root, Supplier_Info NewSupplier,Medication_node**med_root) 
{
    status_code SC=SUCCESS;
    int MSC=SUCCESS;
    printf("Entered AddNEwSUpplier()\n");
    if (*root == NULL) 
    {
        *root = createSupplier_node(TRUE);
        if (*root == NULL) 
        {
           SC=FAILURE;
        }   
        else 
        {
          (*root)->sup_data[0].Supplier_ID=NewSupplier.Supplier_ID;
          strcpy((*root)->sup_data[0].Supplier_name,NewSupplier.Supplier_name);
          (*root)->sup_data[0].quantity_of_med=NewSupplier.quantity_of_med;
          strcpy((*root)->sup_data[0].contact_info,NewSupplier.contact_info);
          (*root)->sup_data[0].count=NewSupplier.count;
          (*root)->sup_data[0].turnover=NewSupplier.turnover;
          (*root)->sup_keys = 1;
        }
        
    }

    // If root is full, split it
    else if ((*root)->sup_keys == ORDER - 1) 
    {
        Supplier_node* newRoot = createSupplier_node(FALSE);
        if(!newRoot)
        {
            SC=FAILURE;
        }
        else 
        {
         newRoot->sup_children[0] = *root;
         splitSupChild(newRoot, 0, *root);
         int i = (NewSupplier.Supplier_ID > newRoot->sup_data[0].Supplier_ID) ? 1 : 0;
         sup_insertNonFull(newRoot->sup_children[i], NewSupplier);
         *root = newRoot;
        }
    } 
    else 
    {
        sup_insertNonFull(*root, NewSupplier);
    }

    if(SC==SUCCESS)
    {
        printf("supplier node is added\n");
        printf("now adding the correspoinding medications\n");
        FILE *mfile = fopen(ADD_MEDSUP, "r");  // Open medication file
       if (!mfile) 
       {
        printf("Error opening medication file!\n");
        MSC=FAILURE;
       }
       printf("able to open the file\n");
       char line[256];
       while (fgets(line, sizeof(line), mfile)) 
      {
         Medication_Record med;
         //printf("entered the while loop\n");
        // int supplier_ID;
        int count = sscanf(line, "%d,%29[^,],%d,%d,%f,%10[^,],%d,%29[^,],%d,%29[^,],%f,%d,%d,%d",
        &med.Medication_ID, med.Medication_name, &med.Batch_number, &med.quantity_in_stock,
        &med.Priceperunit, med.Expiration_date, &med.supplier.Supplier_ID, med.supplier.Supplier_name,
        &med.supplier.quantity_of_med, med.supplier.contact_info,&med.supplier.turnover,
        &med.supplier.count,&med.Reorder_level, &med.Total_sales);


        printf("DEBUG: Line read = %s", line);
        printf("DEBUG: sscanf returned %d fields\n", count);
        printf("DEBUG: med.supplier.Supplier_ID = %d, NewSupplier.Supplier_ID = %d\n",
                med.supplier.Supplier_ID, NewSupplier.Supplier_ID);

                 
        if (count == 14 ) 
        {   // Match supplier ID
            //printf("enetred the if condition\n");
            if(NewSupplier.Supplier_ID == med.supplier.Supplier_ID)
            {
                printf(" Match found. Adding medication: %s for Supplier ID: %d\n",
                    med.Medication_name, NewSupplier.Supplier_ID);
              AddNewMedication(med_root,med);
            }
        }
     }
       fclose(mfile); 
    
    }
    return SC;
}

void printSupplier(Supplier_Info sup) 
{
    printf("Supplier ID: %d\n Supplier Name: %s\n contact info : %s\n quantity supplied: %d\n no.of medications supplied: %d\n turnover: %.2f\n",
          sup.Supplier_ID,sup.Supplier_name,sup.contact_info,sup.quantity_of_med
          ,sup.count,sup.turnover);
          printf("\n");
}

void Sup_traverse(Supplier_node* root) 
{
    printf("entered the sup_traverse()\n");
    if (root == NULL) 
    {
        printf("the given root is null\n");
        return;
    }
    int i;
    for (i = 0; i < root->sup_keys; i++) 
    {
        if (!root->is_supleaf) 
        {
            Sup_traverse(root->sup_children[i]);
        }
        //printf(" %d", root->med_data[i].Medication_ID);
        printSupplier(root->sup_data[i]);
    }
    if (!root->is_supleaf) 
    {
       Sup_traverse(root->sup_children[i]);
    }
}

Supplier_node* readSupInfo(char *filename,Medication_node **med_root)
{
  FILE *file = fopen(filename, "r");
  status_code SC=SUCCESS;
  status_code MSC=SUCCESS;
  if (!file) 
  {
      printf(" Error: Unable to open file %s\n", filename);
      return NULL;
  }

  Supplier_node *root = NULL;
  char line[256];  // Buffer to read each line

  while (fgets(line, sizeof(line), file)) 
  {
      Supplier_Info sup;
      printf("DEBUG: Read line -> %s", line); // Debugging

      int count = sscanf(line, "%d,%29[^,],%d,%29[^,],%f,%d" , &sup.Supplier_ID,sup.Supplier_name,&sup.quantity_of_med,sup.contact_info
      ,&sup.turnover,&sup.count);
      if (count == 6) 
      {
          printf("DEBUG: Successfully parsed %d fields.\n", count);
          if (AddNewSupplier(&root,sup,med_root) == FAILURE) 
          {
              printf("!!! Error: Memory allocation failed!\n");
              SC=FAILURE;
              break;
          }
          else
          {
            printf("successfully added a  supplier node into tree\n");
          }
         
      } 
      else 
      {
          printf("!!!Warning: Skipping invalid line: %s\n", line);
          MSC=FAILURE;
      }
  }

  fclose(file);
  if(SC==SUCCESS)
  {
    printf(" ***Successfully loaded medication records from file.\n");
  }
   return root;
}

Supplier_Info* SearchSupplier(Supplier_node *root, int sup_id, Medication_node *med_root,int printflag) 
{
    if (root == NULL) 
    {
        return NULL;
    }

    int i;
    for (i = 0; i < root->sup_keys; i++) 
    {
        if (sup_id == root->sup_data[i].Supplier_ID) 
        {
            if(printflag==1)
            {
              printSupplier(root->sup_data[i]);
              printf("\nMedications supplied by this supplier:\n");
              SearchMedicationSupplier(med_root, sup_id);  // Use your existing function here
            }
              
            return &(root->sup_data[i]);
        } 
        else if (sup_id < root->sup_data[i].Supplier_ID) 
        {
            if (!root->is_supleaf) 
            {
                return SearchSupplier(root->sup_children[i], sup_id, med_root,printflag);
            } 
            else 
            {
                return NULL;
            }
        }
    }

    if (!root->is_supleaf) 
    {
        return SearchSupplier(root->sup_children[i], sup_id, med_root,printflag);
    }

    return NULL;
}

status_code updateSupplier(Supplier_node* root, Medication_node* med_root) 
{
    status_code SC = SUCCESS;
    int sup_ID;
    printf("Enter the Supplier ID to update: ");
    scanf("%d", &sup_ID);

    Supplier_Info* sup = SearchSupplier(root, sup_ID,med_root,0);

    if (sup == NULL) {
        printf(" Supplier not found.\n");
        SC = FAILURE;
    } else {
        // Store old data for debugging if needed
        printf("Enter new supplier name: ");
        scanf(" %s", sup->Supplier_name);
        printf("Enter quantity of medication supplied: ");
        scanf("%d", &sup->quantity_of_med);
        printf("Enter turnover: ");
        scanf("%f", &sup->turnover);
        printf("Enter count of medications supplied: ");
        scanf("%d", &sup->count);
        printf("Enter contact info: ");
        scanf(" %s", sup->contact_info);

        // Sync updates to Medication Tree
        updateMedicationsWithSupplier(med_root, *sup);
        printf(" Supplier info updated and reflected in all associated medications.\n");
    }

    return SC;
}


void mergeSup(Supplier_node* node, int idx) 
{
    Supplier_node* child = node->sup_children[idx];
    Supplier_node* sibling = node->sup_children[idx + 1];

    // Pull the key from current node down into the child
    child->sup_data[ORDER - 2] = node->sup_data[idx];

    // Copy keys from sibling to child
    for (int i = 0; i < sibling->sup_keys; i++) {
        child->sup_data[i + ORDER - 1] = sibling->sup_data[i];
    }

    // Copy children from sibling to child (if not leaf)
    if (!child->is_supleaf) {
        for (int i = 0; i <= sibling->sup_keys; i++) {
            child->sup_children[i + ORDER - 1] = sibling->sup_children[i];
        }
    }

    // Update key count of child
    child->sup_keys += sibling->sup_keys + 1;

    // Shift keys and children in parent
    for (int i = idx + 1; i < node->sup_keys; i++) {
        node->sup_data[i - 1] = node->sup_data[i];
        node->sup_children[i] = node->sup_children[i + 1];
    }

    node->sup_keys--;

    // Remove dangling pointer
    node->sup_children[node->sup_keys + 1] = NULL;

    free(sibling);
}

void SupborrowFromPrev(Supplier_node*node, int idx) 
{
    Supplier_node *child = node->sup_children[idx];
    Supplier_node *sibling = node->sup_children[idx - 1];

    // Shift child’s keys and children to the right
    for (int i = child->sup_keys - 1; i >= 0; i--)
        child->sup_data[i + 1] = child->sup_data[i];

    if (!child->is_supleaf) {
        for (int i = child->sup_keys; i >= 0; i--)
            child->sup_children[i + 1] = child->sup_children[i];
    }

    // Move key from parent to child
    child->sup_data[0] = node->sup_data[idx - 1];

    // Move sibling’s last child if not leaf
    if (!child->is_supleaf)
        child->sup_children[0] = sibling->sup_children[sibling->sup_keys];

    // Move sibling’s last key to parent
    node->sup_data[idx - 1] = sibling->sup_data[sibling->sup_keys - 1];

    child->sup_keys += 1;
    sibling->sup_keys -= 1;
}

void SupborrowFromNext(Supplier_node *node, int idx) 
{
    Supplier_node *child = node->sup_children[idx];
    Supplier_node *sibling = node->sup_children[idx + 1];

    // Move key from parent to child
    child->sup_data[child->sup_keys] = node->sup_data[idx];

    // Move sibling's first child to child if not leaf
    if (!child->is_supleaf)
        child->sup_children[child->sup_keys + 1] = sibling->sup_children[0];

    // Move sibling’s first key up to parent
    node->sup_data[idx] = sibling->sup_data[0];

    // Shift keys and children in sibling
    for (int i = 1; i < sibling->sup_keys; ++i)
        sibling->sup_data[i - 1] = sibling->sup_data[i];

    if (!sibling->is_supleaf) {
        for (int i = 1; i <= sibling->sup_keys; ++i)
            sibling->sup_children[i - 1] = sibling->sup_children[i];
    }

    child->sup_keys += 1;
    sibling->sup_keys -= 1;
}

void Supfill(Supplier_node* node, int idx) 
{
    if (idx > 0 && node->sup_children[idx - 1]->sup_keys > (ORDER-1)/2) 
    {
        SupborrowFromPrev(node, idx);
    }
    else if (idx < node->sup_keys && node->sup_children[idx + 1]->sup_keys > (ORDER-1)/2) 
    {
        SupborrowFromNext(node, idx);
    }
    else 
    {
        if (idx < node->sup_keys) 
        {
            mergeSup(node, idx);
        }
        else 
        {
            mergeSup(node, idx - 1);
        }
    }
}

status_code RemoveAllMedications(Medication_node* root, int Sup_ID) 
{
    printf("Entered into remove()");
    if (root == NULL)
        return FAILURE;

    int i;
    status_code SC = SUCCESS;

    // Step 1: Recurse through all children first (top-down safe traversal)
    if (!root->is_medleaf) {
        for (i = 0; i <= root->med_keys; i++) 
        {
            RemoveAllMedications(root->med_children[i], Sup_ID);
        }
    }

    // Step 2: Collect all medication IDs to be deleted in this node
    int delete_ids[ORDER + 1];
    int count = 0;

    for (i = 0; i < root->med_keys; i++) 
    {
        if (root->med_data[i].supplier.Supplier_ID == Sup_ID) 
        {
            delete_ids[count++] = root->med_data[i].Medication_ID;
        }
    }

    // Step 3: Delete them one-by-one
    for (i = 0; i < count; i++) 
    {
        SC = deleteMedication(&root, delete_ids[i]);
    }

    return SC;
}

Supplier_Info SupgetPredecessor(Supplier_node* node, int idx) 
{
    Supplier_node* cur = node->sup_children[idx];
    while (!cur->is_supleaf)
    {
        cur = cur->sup_children[cur->sup_keys - 1]; 
    }
    return (cur->sup_data[cur->sup_keys - 1]);
}

Supplier_Info SupgetSuccessor(Supplier_node* node, int idx) 
{
    Supplier_node* cur = node->sup_children[idx + 1];
    while (!cur->is_supleaf)
    {
        cur = cur->sup_children[0];
    }
    return cur->sup_data[0];
}

status_code deleteSupplier(Supplier_node** root, int Sup_ID, Medication_node* med_root) 
{
    status_code SC=SUCCESS;
    printf("Entering into the delete supplier");
    if (*root == NULL) 
    {
        printf("Supplier tree is empty\n");
        return FAILURE;
    }

    Supplier_node* ptr = *root;
    int i = 0;

    // Locate the first key >= Sup_ID
    while (i < ptr->sup_keys && Sup_ID > ptr->sup_data[i].Supplier_ID)
        i++;

    if (i < ptr->sup_keys && ptr->sup_data[i].Supplier_ID == Sup_ID) 
    {
        // Remove supplier from medications
        if (med_root != NULL) 
        {
            SC=RemoveAllMedications(med_root, Sup_ID);
        }

        // Case 1: Found in a leaf
        if (ptr->is_supleaf) {
            for (int j = i + 1; j < ptr->sup_keys; j++)
                ptr->sup_data[j - 1] = ptr->sup_data[j];
            ptr->sup_keys--;
        } 
        // Case 2: Internal node
        else {
            if (ptr->sup_children[i] && ptr->sup_children[i]->sup_keys >= (ORDER / 2)) {
                Supplier_Info pred = SupgetPredecessor(ptr, i);
                ptr->sup_data[i] = pred;
                return deleteSupplier(&ptr->sup_children[i], pred.Supplier_ID, med_root);
            } 
            else if (ptr->sup_children[i + 1] && ptr->sup_children[i + 1]->sup_keys >= (ORDER / 2)) {
                Supplier_Info succ = SupgetSuccessor(ptr, i);
                ptr->sup_data[i] = succ;
                return deleteSupplier(&ptr->sup_children[i + 1], succ.Supplier_ID, med_root);
            } 
            else {
                mergeSup(ptr, i);
                return deleteSupplier(&ptr->sup_children[i], Sup_ID, med_root);
            }
        }
    } 
    else {
        // Not found in current node
        if (ptr->is_supleaf) {
            printf("Supplier ID %d not found.\n", Sup_ID);
            return FAILURE;
        }

        Boolean flag = (i == ptr->sup_keys);

        if (ptr->sup_children[i] == NULL) {
            printf("Null child at index %d while searching for Supplier ID %d\n", i, Sup_ID);
            return FAILURE;
        }

        // If child has less than minimum keys, fill it
        if (ptr->sup_children[i]->sup_keys < (ORDER / 2)) {
            Supfill(ptr, i);
            if (flag && i > ptr->sup_keys)
                i--;
        }

        // Safety check again
        if (ptr->sup_children[i] == NULL) {
            printf("Child is NULL after fill at index %d\n", i);
            return FAILURE;
        }

        return deleteSupplier(&ptr->sup_children[i], Sup_ID, med_root);
    }

    // Handle root shrinking
    if ((*root)->sup_keys == 0) {
        Supplier_node* tmp = *root;
        if (tmp->is_supleaf) {
            free(tmp);
            *root = NULL;
        } else {
            *root = tmp->sup_children[0];
            free(tmp);
        }
    }

    return SUCCESS;
}






typedef struct QueueNode 
{
    Medication_Record* data;
    struct QueueNode* next;
} QueueNode;

typedef struct Queue 
{
    QueueNode *front, *rear;
} Queue;

void InitializeQueue(Queue* que)
{
   que->front=que->rear=NULL;
}

Queue* createQueue();
void enqueue(Queue* queue, Medication_Record* node);
Medication_Record* dequeue(Queue* queue);
Boolean isQueueEmpty(Queue* queue);

Queue* createQueue() 
{
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = queue->rear = NULL;
    return queue;
}

void enqueue(Queue* queue, Medication_Record* node)
 {
    QueueNode* temp = (QueueNode*)malloc(sizeof(QueueNode));
    temp->data = node;
    temp->next = NULL;
    if (queue->rear == NULL) 
    {
        queue->front = queue->rear = temp;
        return;
    }
    queue->rear->next = temp;
    queue->rear = temp;
}

Medication_Record* dequeue(Queue* queue) 
{
    if (queue->front == NULL) 
    {
        return NULL;
    }
    QueueNode* temp = queue->front;
    Medication_Record* node = temp->data;
    queue->front = queue->front->next;
    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }
    free(temp);
    return node;
}

Boolean isQueueEmpty(Queue* queue) 
{
    return (queue->front == NULL);
}

void Stock_alerts(Medication_node*root,Queue*alertQue)
{
    if(root==NULL)
    {
        printf("no stock alerts");
    }
    else
    {
        int i;
        for (i = 0; i < root->med_keys; i++) 
        {

            if (!root->is_medleaf) 
            {
                Stock_alerts(root->med_children[i],alertQue);
            }
            if(root->med_data[i].quantity_in_stock<root->med_data[i].Reorder_level)
            {
                Medication_Record* recordCopy = (Medication_Record*)malloc(sizeof(Medication_Record));
                *recordCopy = root->med_data[i];
                printf("Enqueuing: ID = %d, ptr = %p\n", recordCopy->Medication_ID, (void*)recordCopy);
                enqueue(alertQue, recordCopy);
               // printMedication(root->med_data[i]);
            }
        }
        if (!root->is_medleaf) 
        {
            Stock_alerts(root->med_children[i],alertQue);
        }
    }
   
}

void DisplayStockAlerts(Queue* alertQue) 
{
    Medication_Record tempmed;
    if (alertQue->front==NULL) 
    {
        printf("No stock alerts");
    }
    else
    { 
        while (alertQue->front!=NULL) 
        {
         Medication_Record*med = dequeue(alertQue);
         printf("ALERT: Medication with ID %d is below reorder level.\n", med->Medication_ID);
        }
    }
}





typedef struct BatchNode
{
    int batch_keys;
    Medication_Record data[ORDER];             // Sorted by Batch_ID
    struct BatchNode* batch_children[ORDER + 1];        // One more than MAX
    int is_batch_leaf;

}Batch_Node;

Batch_Node* createBatchNode(Boolean batch_leaf) 
{
    Batch_Node* node = (Batch_Node*)malloc(sizeof(Batch_Node));
    if (node == NULL) 
    {
        printf("Error: Memory allocation failed for B-Tree node!\n");
        return NULL;
    }
    node->is_batch_leaf = batch_leaf;
    node->batch_keys = 0;
    for (int i = 0; i < ORDER; i++)
    {
        node->batch_children[i] = NULL;
    }
    return node;
}

void splitChildBatch(Batch_Node* parent, int index,Batch_Node* fullChild) 
{
    Batch_Node* newChild = createBatchNode(fullChild->is_batch_leaf);
    int mid = (ORDER - 1) / 2;
    // Move second half of keys to new child
    newChild->batch_keys = (ORDER -1)-mid-1;
    int right_keys=(ORDER -1)-mid-1;
    // Move second half of keys to new child
    for (int j = 0; j < right_keys; j++) 
    {
        newChild->data[j].Medication_ID = fullChild->data[mid + 1 + j].Medication_ID;
        strcpy(newChild->data[j].Medication_name, fullChild->data[mid + 1 + j].Medication_name);
        newChild->data[j].Batch_number=fullChild->data[mid + 1 + j].Batch_number;
       // newChild->med_data[j].Medication_ID = fullChild->med_data[mid + 1 + j].Medication_ID;
        newChild->data[j].quantity_in_stock = fullChild->data[mid + 1 + j].quantity_in_stock;
        newChild->data[j].Priceperunit = fullChild->data[mid + 1 + j].Priceperunit;
        strcpy(newChild->data[j].Expiration_date, fullChild->data[mid + 1 + j].Expiration_date);
        newChild->data[j].supplier.Supplier_ID = fullChild->data[mid + 1 + j].supplier.Supplier_ID;
        strcpy(newChild->data[j].supplier.Supplier_name, fullChild->data[mid + 1 + j].supplier.Supplier_name);
        newChild->data[j].supplier.quantity_of_med = fullChild->data[mid + 1 + j].supplier.quantity_of_med;
        strcpy(newChild->data[j].supplier.contact_info, fullChild->data[mid + 1 + j].supplier.contact_info);
        newChild->data[j].Reorder_level = fullChild->data[mid + 1 + j].Reorder_level;
        newChild->data[j].Total_sales = fullChild->data[mid + 1 + j].Total_sales;
    }
    // If it's not a leaf, move children pointers
    if (!fullChild->is_batch_leaf) {
        for (int j = 0; j <= ORDER / 2; j++) {
            newChild->batch_children[j] = fullChild->batch_children[mid + 1 + j];
        }
    }
    fullChild->batch_keys = mid;
    // Shift parent's children to make room for new child
    for (int j = parent->batch_keys; j > index; j--) {
        parent->batch_children[j + 1] = parent->batch_children[j];
    }
    // Link new child to the parent
    parent->batch_children[index + 1] = newChild;
    // Shift parent's keys to make room for middle key
    for (int j = parent->batch_keys - 1; j >= index; j--) {
        parent->data[j + 1] = parent->data[j];
    }
    // Copy middle key to parent node (key at mid)
    parent->data[index] = fullChild->data[mid];
    // Increase the number of keys in the parent node
    parent->batch_keys++;
}

void insertNonFullBatch(Batch_Node* node, Medication_Record NewMedication) 
{
    int i = node->batch_keys - 1;

    // If the node is a leaf, find the correct position and insert
    if (node->is_batch_leaf) 
    {
        while (i >= 0 && NewMedication.Batch_number< node->data[i].Batch_number) 
        {
            node->data[i + 1] = node->data[i];
            i--;
        }

        //node->med_data[i + 1] = med;
        (node)->data[i+1].Medication_ID=NewMedication.Medication_ID;
        strcpy(node->data[i + 1].Medication_name,NewMedication.Medication_name);
        node->data[i + 1].Batch_number=NewMedication.Batch_number;
        node->data[i + 1].quantity_in_stock=NewMedication.quantity_in_stock;
        node->data[i + 1].Priceperunit=NewMedication.Priceperunit;
        strcpy(node->data[i + 1].Expiration_date,NewMedication.Expiration_date);
        node->data[i + 1].supplier.Supplier_ID=NewMedication.supplier.Supplier_ID;
        strcpy(node->data[i + 1].supplier.Supplier_name,NewMedication.supplier.Supplier_name);
        node->data[i + 1].supplier.quantity_of_med=NewMedication.supplier.quantity_of_med;
        strcpy(node->data[i + 1].supplier.contact_info,NewMedication.supplier.contact_info);
        node->data[i + 1].Reorder_level=NewMedication.Reorder_level;
        node->data[i + 1].Total_sales=NewMedication.Total_sales;
        node->batch_keys++;
    }
    else 
    { 
        // Find the correct child to insert
        while (i >= 0 && NewMedication.Batch_number < node->data[i].Batch_number) {
            i--;
        }
        i++;

        // If the child is full, split before insertion
        if (node->batch_children[i]->batch_keys == ORDER - 1)
         {
            splitChildBatch(node, i, node->batch_children[i]);

            // Determine which of the two children to insert into
            if (NewMedication.Batch_number > node->data[i].Batch_number) 
            {
                i++;
            }
        }
        insertNonFullBatch(node->batch_children[i], NewMedication);
    }
}

status_code AddNewBatchNode(Batch_Node** root, Medication_Record NewMedication) {
    if (*root == NULL) 
    {
        *root = createBatchNode(TRUE);
        if (*root == NULL) return FAILURE;
        (*root)->data[0].Medication_ID=NewMedication.Medication_ID;
        strcpy((*root)->data[0].Medication_name,NewMedication.Medication_name);
        (*root)->data[0].Batch_number=NewMedication.Batch_number;
        (*root)->data[0].quantity_in_stock=NewMedication.quantity_in_stock;
        (*root)->data[0].Priceperunit=NewMedication.Priceperunit;
        strcpy((*root)->data[0].Expiration_date,NewMedication.Expiration_date);
        (*root)->data[0].supplier.Supplier_ID=NewMedication.supplier.Supplier_ID;
        strcpy((*root)->data[0].supplier.Supplier_name,NewMedication.supplier.Supplier_name);
        (*root)->data[0].supplier.quantity_of_med=NewMedication.supplier.quantity_of_med;
        strcpy((*root)->data[0].supplier.contact_info,NewMedication.supplier.contact_info);
        (*root)->data[0].Reorder_level=NewMedication.Reorder_level;
        (*root)->data[0].Total_sales=NewMedication.Total_sales;
        //(*root)->med_data[0] = med;
        (*root)->batch_keys = 1;
        return SUCCESS;
    }

    // If root is full, split it
    if ((*root)->batch_keys == ORDER - 1) 
    {
        Batch_Node* newRoot = createBatchNode(FALSE);
        newRoot->batch_children[0] = *root;
        splitChildBatch(newRoot, 0, *root);
        int i = (NewMedication.Batch_number > newRoot->data[0].Batch_number) ? 1 : 0;
        insertNonFullBatch(newRoot->batch_children[i], NewMedication);
        *root = newRoot;
    } 
    else 
    {
        insertNonFullBatch(*root, NewMedication);
    }
    return SUCCESS;
}

Batch_Node* readBatchrec(char *filename) 
{
    FILE *file = fopen(filename, "r");
    status_code SC=SUCCESS;
    if (!file) 
    {
        printf(" Error: Unable to open file %s\n", filename);
        return NULL;
    }

    Batch_Node *root = NULL;
    char line[256];  // Buffer to read each line

    while (fgets(line, sizeof(line), file)) 
    {
     // printf("entered while loop\n");
        Medication_Record med;
       // Medication_node *nptr;
        printf("DEBUG: Read line -> %s", line); // Debugging

        int count = sscanf(line, "%d,%29[^,],%d,%d,%f,%10[^,],%d,%29[^,],%d,%29[^,],%f,%d,%d,%d",
                           &med.Medication_ID, med.Medication_name, &med.Batch_number, &med.quantity_in_stock,
                           &med.Priceperunit, med.Expiration_date, &med.supplier.Supplier_ID, med.supplier.Supplier_name,
                           &med.supplier.quantity_of_med, med.supplier.contact_info,&med.supplier.turnover,
                           &med.supplier.count,&med.Reorder_level, &med.Total_sales);
        
        if (count == 14) 
        {

            printf("DEBUG: Successfully parsed %d fields.\n", count);
          // nptr=CreateMedication(med); 
           // printf("created the new node\n");
            if (AddNewBatchNode(&root,med) == FAILURE) 
            {
                printf("!!! Error: Memory allocation failed!\n");
                SC=FAILURE;
                break;
            }
            else
            {
              printf("added a new node to the tree\n");
            }
        } 
        else 
        {
            printf("!!!Warning: Skipping invalid line: %s\n", line);
            SC=FAILURE;
        }
    }

    fclose(file);
    if(SC==FAILURE)
    {
      printf(" ***Successfully loaded medication records from file.\n");
    }  
    return root;
}

void traversebatch(Batch_Node* root) 
{
    if (root == NULL) return;
    int i;
    for (i = 0; i < root->batch_keys; i++) 
    {
        if (!root->is_batch_leaf) {
            traversebatch(root->batch_children[i]);
        }
        //printf(" %d", root->med_data[i].Medication_ID);
        printMedication(root->data[i]);
    }
    if (!root->is_batch_leaf) {
        traversebatch(root->batch_children[i]);
    }
}

Medication_Record* searchBatchNo(Batch_Node* root, int batchID) 
{
    if (root == NULL) 
    {
        return NULL; 
    }

    int low = 0, high = root->batch_keys - 1, mid;
    while (low <= high) 
    {
        mid = (low + high) / 2;
        
        if (root->data[mid].Batch_number == batchID) 
        {
            return (&(root)->data[mid]); // Found the medication
        } 
        else if (batchID < root->data[mid].Batch_number) 
        {
            high = mid - 1; // Search left half
        } 
        else 
        {
            low = mid + 1; // Search right half
        }
    }
    // If the node is a leaf, the medication ID does not exist
    if (root->is_batch_leaf) 
    {
        return NULL;
    }
    return searchBatchNo(root->batch_children[low], batchID);
}




void mergeBatch(Batch_Node* node, int idx) 
{
    Batch_Node* child = node->batch_children[idx];
    
    Batch_Node* sibling = node->batch_children[idx + 1];

    // Pull the key from current node down into the child
    child->data[ORDER - 2] = node->data[idx];

    // Copy keys from sibling to child
    for (int i = 0; i < sibling->batch_keys; i++) 
    {
        child->data[i + ORDER - 1] = sibling->data[i];
    }

    // Copy children from sibling to child (if not leaf)
    if (!child->is_batch_leaf) 
    {
        for (int i = 0; i <= sibling->batch_keys; i++) 
        {
            child->batch_children[i + ORDER - 1] = sibling->batch_children[i];
        }
    }

    // Update key count of child
    child->batch_keys += sibling->batch_keys + 1;

    // Shift keys and children in parent
    for (int i = idx + 1; i < node->batch_keys; i++) 
    {
        node->data[i - 1] = node->data[i];
        node->batch_children[i] = node->batch_children[i + 1];
    }

    node->batch_keys--;

    // Remove dangling pointer
    node->batch_children[node->batch_keys + 1] = NULL;

    free(sibling);
}

void BatchborrowFromPrev(Batch_Node*node, int idx) 
{
    Batch_Node *child = node->batch_children[idx];
    Batch_Node *sibling = node->batch_children[idx - 1];

    // Shift child’s keys and children to the right
    for (int i = child->batch_keys - 1; i >= 0; i--)
        child->data[i + 1] = child->data[i];

    if (!child->is_batch_leaf) 
    {
        for (int i = child->batch_keys; i >= 0; i--)
            child->batch_children[i + 1] = child->batch_children[i];
    }

    // Move key from parent to child
    child->data[0] = node->data[idx - 1];

    // Move sibling’s last child if not leaf
    if (!child->is_batch_leaf)
        child->batch_children[0] = sibling->batch_children[sibling->batch_keys];

    // Move sibling’s last key to parent
    node->data[idx - 1] = sibling->data[sibling->batch_keys - 1];

    child->batch_keys += 1;
    sibling->batch_keys -= 1;
}

void BatchborrowFromNext(Batch_Node *node, int idx) 
{
    Batch_Node *child = node->batch_children[idx];
    Batch_Node *sibling = node->batch_children[idx + 1];

    // Move key from parent to child
    child->data[child->batch_keys] = node->data[idx];

    // Move sibling's first child to child if not leaf
    if (!child->is_batch_leaf)
        child->batch_children[child->batch_keys + 1] = sibling->batch_children[0];

    // Move sibling’s first key up to parent
    node->data[idx] = sibling->data[0];

    // Shift keys and children in sibling
    for (int i = 1; i < sibling->batch_keys; ++i)
        sibling->data[i - 1] = sibling->data[i];

    if (!sibling->is_batch_leaf) 
    {
        for (int i = 1; i <= sibling->batch_keys; ++i)
            sibling->batch_children[i - 1] = sibling->batch_children[i];
    }

    child->batch_keys += 1;
    sibling->batch_keys -= 1;
}

void Batchfill(Batch_Node* node, int idx) 
{
    if (idx > 0 && node->batch_children[idx - 1]->batch_keys > (ORDER-1)/2) 
    {
        BatchborrowFromPrev(node, idx);
    }
    else if (idx < node->batch_keys && node->batch_children[idx + 1]->batch_keys > (ORDER-1)/2) 
    {
        BatchborrowFromNext(node, idx);
    }
    else 
    {
        if (idx < node->batch_keys) 
        {
            mergeBatch(node, idx);
        }
        else 
        {
            mergeBatch(node, idx - 1);
        }
    }
}

status_code RemoveMedications(Medication_node* root, int batch_id) 
{
    //printf("Entered into remove()");
    if (root == NULL)
        return FAILURE;

    int i;
    status_code SC = SUCCESS;

    // Step 1: Recurse through all children first (top-down safe traversal)
    if (!root->is_medleaf) 
    {
        for (i = 0; i <= root->med_keys; i++) 
        {
            RemoveMedications(root->med_children[i], batch_id);
        }
    }

    // Step 2: Collect all medication IDs to be deleted in this node
    int delete_ids[ORDER + 1];
    int count = 0;

    for (i = 0; i < root->med_keys; i++) 
    {
        if (root->med_data[i].Batch_number == batch_id) 
        {
            delete_ids[count++] = root->med_data[i].Medication_ID;
        }
    }

    // Step 3: Delete them one-by-one
    for (i = 0; i < count; i++) 
    {
        SC = deleteMedication(&root, delete_ids[i]);
    }

    return SC;
}

Medication_Record BatchgetPredecessor(Batch_Node* node, int idx) 
{
    Batch_Node* cur = node->batch_children[idx];
    while (!cur->is_batch_leaf)
    {
        cur = cur->batch_children[cur->batch_keys - 1]; 
    }
    return (cur->data[cur->batch_keys - 1]);
}

Medication_Record BatchgetSuccessor(Batch_Node* node, int idx) 
{
    Batch_Node* cur = node->batch_children[idx + 1];
    while (!cur->is_batch_leaf)
    {
        cur = cur->batch_children[0];
    }
    return cur->data[0];
}

status_code deleteBatch(Batch_Node** root, int batch_id, Medication_node* med_root) 
{
    status_code SC=SUCCESS;
    printf("Entering into the delete batch");
    if (*root == NULL) 
    {
        printf("batch tree is empty\n");
        return FAILURE;
    }

    Batch_Node* ptr = *root;
    int i = 0;

    // Locate the first key >= Sup_ID
    while (i < ptr->batch_keys && batch_id > ptr->data[i].Batch_number)
        i++;

    if (i < ptr->batch_keys && ptr->data[i].Batch_number == batch_id) 
    {
        // Remove supplier from medications
        if (med_root != NULL) 
        {
            SC=RemoveMedications(med_root, batch_id);
        }

        // Case 1: Found in a leaf
        if (ptr->is_batch_leaf) 
        {
            for (int j = i + 1; j < ptr->batch_keys; j++)
                ptr->data[j - 1] = ptr->data[j];
            ptr->batch_keys--;
        } 
        // Case 2: Internal node
        else 
        {
            if (ptr->batch_children[i] && ptr->batch_children[i]->batch_keys >= (ORDER / 2)) 
            {
                Medication_Record pred = BatchgetPredecessor(ptr, i);
                ptr->data[i] = pred;
                return deleteBatch(&ptr->batch_children[i], pred.Batch_number, med_root);
            } 
            else if (ptr->batch_children[i + 1] && ptr->batch_children[i + 1]->batch_keys >= (ORDER / 2)) 
            {
                Medication_Record succ = BatchgetSuccessor(ptr, i);
                ptr->data[i] = succ;
                return deleteBatch(&ptr->batch_children[i + 1], succ.Batch_number, med_root);
            } 
            else 
            {
                mergeBatch(ptr, i);
                return deleteBatch(&ptr->batch_children[i], batch_id, med_root);
            }
        }
    } 
    else 
    {
        // Not found in current node
        if (ptr->is_batch_leaf) 
        {
            printf("batch number %d not found.\n", batch_id);
            return FAILURE;
        }

        Boolean flag = (i == ptr->batch_keys);

        if (ptr->batch_children[i] == NULL) 
        {
            printf("Null child at index %d while searching for batch number %d\n", i,batch_id);
            return FAILURE;
        }

        // If child has less than minimum keys, fill it
        if (ptr->batch_children[i]->batch_keys < (ORDER / 2)) 
        {
            Batchfill(ptr, i);
            if (flag && i > ptr->batch_keys)
                i--;
        }

        // Safety check again
        if (ptr->batch_children[i] == NULL) 
        {
            printf("Child is NULL after fill at index %d\n", i);
            return FAILURE;
        }

        return deleteBatch(&ptr->batch_children[i], batch_id, med_root);
    }

    // Handle root shrinking
    if ((*root)->batch_keys == 0) 
    {
        Batch_Node* tmp = *root;
        if (tmp->is_batch_leaf) 
        {
            free(tmp);
            *root = NULL;
        } else 
        {
            *root = tmp->batch_children[0];
            free(tmp);
        }
    }

    return SUCCESS;
}





void searchBatchAndProcessSale(Batch_Node* node, int batch_num, Medication_node* med_root, int* found);

void updateMedicationInMedTree(Medication_node* root, int med_id, int batch, int qty_sold) 
{
    if (root == NULL) return;

    int i;
    for (i = 0; i < root->med_keys; i++) {
        if (!root->is_medleaf)
            updateMedicationInMedTree(root->med_children[i], med_id, batch, qty_sold);

        if (root->med_data[i].Medication_ID == med_id && root->med_data[i].Batch_number == batch) {
            root->med_data[i].quantity_in_stock -= qty_sold;
            root->med_data[i].Total_sales += qty_sold;

            printf("Medication Tree updated (MedID: %d, Batch: %d)\n", med_id, batch);
        }
    }

    if (!root->is_medleaf)
        updateMedicationInMedTree(root->med_children[i], med_id, batch, qty_sold);
}

void sales_track_by_batch(Batch_Node* batch_root, Medication_node* med_root) 
{
    int batch_num;
    printf("Enter Batch Number: ");
    scanf("%d", &batch_num);

    if (batch_root == NULL) {
        printf(" Batch tree is empty.\n");
        return;
    }

    int found = 0;
    searchBatchAndProcessSale(batch_root, batch_num, med_root, &found);

    if (!found)
        printf(" No such batch number %d found in the batch tree.\n", batch_num);
}

void searchBatchAndProcessSale(Batch_Node* node, int batch_num, Medication_node* med_root, int* found) 
{
    if (node == NULL) return;

    int i;
    for (i = 0; i < node->batch_keys; i++) 
    {
        if (!node->is_batch_leaf)
            searchBatchAndProcessSale(node->batch_children[i], batch_num, med_root, found);

        if (node->data[i].Batch_number == batch_num) {
            *found = 1;

            Medication_Record* med = &node->data[i];
            printf("\n📦 Found Medication in Batch %d\n", med->Batch_number);
            printf("   Med ID: %d\n   Name: %s\n   Stock: %d\n   Sales: %d\n",
                   med->Medication_ID, med->Medication_name, med->quantity_in_stock, med->Total_sales);

            int qty_sold;
            printf("Enter quantity sold for Med ID %d: ", med->Medication_ID);
            scanf("%d", &qty_sold);

            if (qty_sold > 0 && qty_sold <= med->quantity_in_stock) 
            {
                // Update in Batch Tree
                med->quantity_in_stock -= qty_sold;
                med->Total_sales += qty_sold;

                // Reflect in Medication Tree
                updateMedicationInMedTree(med_root, med->Medication_ID, med->Batch_number, qty_sold);

                printf(" Sale recorded.\n");
                printf("   New Stock: %d | Total Sales: %d\n", med->quantity_in_stock, med->Total_sales);
            } else 
            {
                printf("Invalid quantity. Available stock: %d\n", med->quantity_in_stock);
            }
        }
    }

    if (!node->is_batch_leaf)
        searchBatchAndProcessSale(node->batch_children[i], batch_num, med_root, found);
}




int dateToInt(const char* dateStr) 
{
    int dd, mm, yyyy;
    sscanf(dateStr, "%2d-%2d-%4d", &dd, &mm, &yyyy);
    return yyyy * 10000 + mm * 100 + dd;
}

int compareByExpiration(const void* a, const void* b) 
{
    Medication_Record* m1 = (Medication_Record*)a;
    Medication_Record* m2 = (Medication_Record*)b;
    int d1 = dateToInt(m1->Expiration_date);
    int d2 = dateToInt(m2->Expiration_date);
    return d1 - d2;
}

Medication_Record* collectMedicationsHelper(Medication_node* root, Medication_Record* array, int* count, int* capacity) 
{
    if (root == NULL) return array;

    for (int i = 0; i < root->med_keys; i++) 
    {
        // Visit left subtree first
        if (!root->is_medleaf) 
        {
            array = collectMedicationsHelper(root->med_children[i], array, count, capacity);
        }

        // Expand array if needed
        if (*count >= *capacity) 
        {
            *capacity *= 2;
            array = realloc(array, (*capacity) * sizeof(Medication_Record));
            if (!array) 
            {
                printf("Memory reallocation failed!\n");
                exit(1);
            }
        }

        array[*count] = root->med_data[i];
        (*count)++;
    }

    // Visit last child
    if (!root->is_medleaf) 
    {
        array = collectMedicationsHelper(root->med_children[root->med_keys], array, count, capacity);
    }

    return array;
}

Medication_Record* collectMedicationsDynamic(Medication_node* root, int* count) 
{
    int capacity = INITIAL_CAPACITY;
    *count = 0;
    Medication_Record* result = malloc(capacity * sizeof(Medication_Record));
    if (!result) 
    {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    return collectMedicationsHelper(root, result, count, &capacity);
}

void sortMedicationsByExpiration(Medication_node* root, const char* date1, const char* date2) 
{
    int d1 = dateToInt(date1);
    int d2 = dateToInt(date2);

    if (d1 > d2) 
    {
        printf("Invalid date range.\n");
        return;
    }

    int count;
    Medication_Record* meds = collectMedicationsDynamic(root, &count);
    qsort(meds, count, sizeof(Medication_Record), compareByExpiration);
    printf("the medications got sorted acc to expiry date\n");

    int found = 0;
    for (int i = 0; i < count; i++) 
    {
        
        int exp = dateToInt(meds[i].Expiration_date);
        if (exp >= d1 && exp <= d2) 
        {
            printMedication(meds[i]);
            found = 0;
        }
    }

    if (!found) printf("No medications in the specified date range.\n");
    free(meds);  // Free allocated memory
}

int getTodayPlusDays(int days) //Get today's date and add 30 days
{
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);

    tm_info->tm_mday += days;
    mktime(tm_info); // Normalize the date

    int dd = tm_info->tm_mday;
    int mm = tm_info->tm_mon + 1; // tm_mon is 0-based
    int yyyy = tm_info->tm_year + 1900;

    return yyyy * 10000 + mm * 100 + dd;
}

void alertExpiringMedications(Medication_node* root) 
{
    int count;
    Medication_Record* meds = collectMedicationsDynamic(root, &count);
    if (!meds) return;

    qsort(meds, count, sizeof(Medication_Record), compareByExpiration);  // Already defined

    int today = getTodayPlusDays(0);
    int cutoff = getTodayPlusDays(30);

    int found = 0;
    printf("\n Medications Expiring Within 30 Days:\n");

    for (int i = 0; i < count; i++) 
    {
        int exp = dateToInt(meds[i].Expiration_date);
        if (exp >= today && exp <= cutoff) {
            printMedication(meds[i]);  // Already defined
            found = 1;
        } else if (exp > cutoff) 
        {
            // Since array is sorted by date, we can break early
            break;
        }
    }

    if (!found) 
    {
        printf(" No medications expiring in the next 30 days.\n");
    }

    free(meds);
}



void resetSupplierCounts(Supplier_node *sup_root) 
{
    if (sup_root == NULL) return;

    for (int i = 0; i < sup_root->sup_keys; i++) 
    {
        sup_root->sup_data[i].count = 0;
    }

    for (int i = 0; i <= sup_root->sup_keys; i++) 
    {
        resetSupplierCounts(sup_root->sup_children[i]);
    }
}

void UpdateSuppliersCount(Medication_node *med_node, Supplier_node *sup_root, Medication_node *med_root) 
{
    if (sup_root == NULL || med_root == NULL||med_node == NULL) return;
    

    for (int i = 0; i < med_node->med_keys; i++) 
    {
        Medication_Record med = med_node->med_data[i];

        Supplier_Info *supplier = SearchSupplier(sup_root, med.supplier.Supplier_ID, med_root, 0); // no print
        if (supplier != NULL) 
        {
            printf("Matching supplier %d for medication %s\n", med.supplier.Supplier_ID, med.Medication_name);
            supplier->count++;
        }
    }

    for (int i = 0; i <= med_node->med_keys; i++) 
    {
        UpdateSuppliersCount(med_node->med_children[i], sup_root, med_root);
    }
}

void collectSuppliersDynamic(Supplier_node* root, Supplier_Info** array, int* count, int* capacity) 
{   // Collect suppliers with dynamic reallocation

    printf("entered the collectsupplierdynamic()\n");
    if (root == NULL)
    {
        printf("root is null\n");
        return;
    } 

    int i;
    for (i = 0; i < root->sup_keys; i++) 
    {
        // Traverse child first (left)
        if (!root->is_supleaf) 
        {
            collectSuppliersDynamic(root->sup_children[i], array, count, capacity);
        }

        // Resize if needed
        if (*count >= *capacity) 
        {
            *capacity *= 2;
            *array = (Supplier_Info*)realloc(*array, (*capacity) * sizeof(Supplier_Info));
            if (*array == NULL) {
                perror("Memory allocation failed");
                exit(EXIT_FAILURE);
            }
        }

        (*array)[(*count)++] = root->sup_data[i];
    }

    // Traverse last child
    if (!root->is_supleaf) 
    {
        collectSuppliersDynamic(root->sup_children[i], array, count, capacity);
    }
}

int compareByCountDesc(const void* a, const void* b) 
{
    // Compare function for sorting by `count` in descending order
    Supplier_Info* s1 = (Supplier_Info*)a;
    Supplier_Info* s2 = (Supplier_Info*)b;
    return s2->count - s1->count;
}

void findAllRounderSuppliers(Supplier_node* root) 
{   // Main function to print top all-rounders
    int capacity = 10;
    int count = 0;
    Supplier_Info* supplierArray = (Supplier_Info*)malloc(capacity * sizeof(Supplier_Info));

    if (supplierArray == NULL) {
        perror("Initial malloc failed");
        return;
    }

    collectSuppliersDynamic(root, &supplierArray, &count, &capacity);

    if (count == 0) {
        printf("No suppliers found.\n");
        free(supplierArray);
        return;
    }

    qsort(supplierArray, count, sizeof(Supplier_Info), compareByCountDesc);

    printf("\n🔝 Best All-Rounder Supplier:\n");
    printf("Supplier ID: %d\nName: %s\nUnique Medications: %d\n\n",
           supplierArray[0].Supplier_ID, supplierArray[0].Supplier_name, supplierArray[0].count);

    printf("Top %d All-Rounder Suppliers:\n", (count < 10) ? count : 10);
    for (int i = 0; i < count && i < 10; i++) {
        printf("%d. ID: %d | Name: %s | Unique Meds: %d\n",
               i + 1,
               supplierArray[i].Supplier_ID,
               supplierArray[i].Supplier_name,
               supplierArray[i].count);
    }

    free(supplierArray);
}






int compareTurnover(const void *a, const void *b)
{
    float t1 = ((Supplier_Info *)a)->turnover;
    float t2 = ((Supplier_Info *)b)->turnover;

    if (t1 < t2) return 1;
    else if (t1 > t2) return -1;
    else return 0;
}

void UpdateSuppliersTurnover(Medication_node *med_node, Supplier_node *sup_root, Medication_node *med_root) 
{
    if (sup_root == NULL || med_node == NULL) return;

    for (int i = 0; i < med_node->med_keys; i++) 
    {
        Medication_Record med = med_node->med_data[i];
        Supplier_Info *supplier = SearchSupplier(sup_root, med.supplier.Supplier_ID, med_root, 0);
        if (supplier != NULL) 
        {
            supplier->turnover += (med.supplier.quantity_of_med * med.Priceperunit);
        }
    }

    if (!med_node->is_medleaf)
    {
        for (int i = 0; i <= med_node->med_keys; i++) 
        {
            UpdateSuppliersTurnover(med_node->med_children[i], sup_root, med_root);
        }
    }
}

void collectSuppliers(Supplier_node *sup_node, Supplier_Info **arr, int *size, int *capacity)
{
    if (sup_node == NULL) return;

    for (int i = 0; i < sup_node->sup_keys; i++) 
    {
        if (*size >= *capacity) 
        {
            *capacity *= 2;
            *arr = realloc(*arr, (*capacity) * sizeof(Supplier_Info));
        }
        (*arr)[*size] = sup_node->sup_data[i];
        (*size)++;
    }

    if (!sup_node->is_supleaf) 
    {
        for (int i = 0; i <= sup_node->sup_keys; i++) 
        {
            collectSuppliers(sup_node->sup_children[i], arr, size, capacity);
        }
    }
}

void PrintTopSuppliers(Supplier_node *sup_root)
{
    int size = 0, capacity = 10;
    Supplier_Info *arr = malloc(capacity * sizeof(Supplier_Info));
    if (!arr) 
    {
        printf("Memory allocation failed!\n");
        return;
    }

    collectSuppliers(sup_root, &arr, &size, &capacity);
    qsort(arr, size, sizeof(Supplier_Info), compareTurnover);

    printf("\nSupplier with Largest Turnover:\n");
    printf("\n");
    if (size > 0)
        printSupplier(arr[0]);

    printf("\nTop 10 Suppliers by Turnover:\n");
    for (int i = 0; i < size && i < 10; i++) 
    {
        printf("\nRank %d:\n", i + 1);
        printSupplier(arr[i]);
    }

    free(arr);
}




void writeMedrec(FILE *file, Medication_node *root) 
{
    if (root == NULL) return;

    int i;
    for (i = 0; i < root->med_keys; i++) 
    {
        if (!root->is_medleaf) 
        {
            writeMedrec(file, root->med_children[i]);
        }

        Medication_Record med = root->med_data[i];
        fprintf(file, "%d,%s,%d,%d,%.2f,%s,%d,%s,%d,%s,%.2f,%d,%d,%d\n",
                med.Medication_ID,
                med.Medication_name,
                med.Batch_number,
                med.quantity_in_stock,
                med.Priceperunit,
                med.Expiration_date,
                med.supplier.Supplier_ID,
                med.supplier.Supplier_name,
                med.supplier.quantity_of_med,
                med.supplier.contact_info,
                med.supplier.turnover,
                med.supplier.count,
                med.Reorder_level,
                med.Total_sales);
    }

    if (!root->is_medleaf) {
        writeMedrec(file, root->med_children[i]);
    }
}

void writeSupplier(FILE *file, Supplier_node *root) 
{
    if (root == NULL) return;

    int i;
    for (i = 0; i < root->sup_keys; i++) {
        if (!root->is_supleaf) {
            writeSupplier(file, root->sup_children[i]);
        }

        Supplier_Info sup = root->sup_data[i];
        fprintf(file, "%d,%s,%d,%s,%.2f,%d\n",
                sup.Supplier_ID,
                sup.Supplier_name,
                sup.quantity_of_med,
                sup.contact_info,
                sup.turnover,
                sup.count);
    }

    if (!root->is_supleaf) {
        writeSupplier(file, root->sup_children[i]);
    }
}

void saveAllDataToFile(Medication_node *med_root, Supplier_node *sup_root, const char *med_file, const char *sup_file) 
{
    FILE *mfile = fopen(med_file, "w");
    if (!mfile) {
        printf(" Error opening medication output file: %s\n", med_file);
        return;
    }

    FILE *sfile = fopen(sup_file, "w");
    if (!sfile) {
        printf(" Error opening supplier output file: %s\n", sup_file);
        fclose(mfile);
        return;
    }

    writeMedrec(mfile, med_root);
    writeSupplier(sfile, sup_root);

    fclose(mfile);
    fclose(sfile);

    printf(" All data successfully saved to %s (medications) and %s (suppliers)\n", med_file, sup_file);
}



int main()

{
  status_code SC;
  int ch=0,choice=1;
  int med_id,sup_id;
  Supplier_Info*ptr;
  char date1[11], date2[11];
  printf("CALLING READMEDREC()----------------------------------\n");
  Medication_node*med_root = readMedrec(FILE_NAME);
  printf("CALLING READMEDREC()----------------------------------\n");
  Medication_node*mptr = readMedrec(ADD_MED);
  printf("CALLING READSUPINFO()----------------------------------\n");
  Supplier_node*sup_root = readSupInfo(SUP_FILE_NAME,&med_root);
  printf("CALLING READSUPINFO()----------------------------------\n");
  Supplier_node*sptr= readSupInfo(ADD_SUP,&med_root);
  printf("CALLING READBATCCHTREE()----------------------------------\n");
  Batch_Node*batch_root =readBatchrec(FILE_NAME);
  Queue que1;
  Queue que2;
  InitializeQueue(&que1);
  InitializeQueue(&que2);
  while(choice==1)
  {
    printf("1. Add New Medication record\n2. Update Medication record\n3. Delete Medication record\n4. Search Medication record\n5. Stock Alerts\n");
    printf("6. Sort Medication records by Expiration Date\n7. Check Expiration Dates\n8. Sales Tracking\n");
    printf("9. Add New Supplier\n10. Update Supplier\n11. Search supplier\n12. Delete supplier\n");
    printf("13. Find all-rounder suppliers\n14. Find suppliers with largest turn-over\n");
    printf("15. med tree traversal\n16. supplier tree traversal\n17. Batch tree traversal\n");
    printf("18. Save the data into files\n");
    printf("\n");
    printf("Enter your choice to access the corresponding  function : \n");
    scanf("%d", &ch);

    switch (ch)
    {
        case 1:
        SC= AddNewMedication(&med_root,mptr->med_data[0]);
        printf("'the medication tree is: \n");
        printf("-------------------------------------------------------------------------\n");
        traverseMed(med_root);
        break;

        case 2:
        SC= updateMedication(med_root);
        printf("'the updated medication tree is: \n");
        printf("-------------------------------------------------------------------------\n");
        traverseMed(med_root);
        break;

        case 3:
        printf("Enter the medication id: ");
        scanf("%d",&med_id);
        SC=deleteMedication(&med_root,med_id);
        printf("'the Modified medication tree is: \n");
        printf("-------------------------------------------------------------------------\n");
        traverseMed(med_root);
        break;

        case 4:
        SC= searchMedication(med_root);
        break;

        case 5:
        Stock_alerts(med_root,&que1);
        printf("the stock alerts are: \n");
        DisplayStockAlerts(&que1);
        break;

        case 6:
        printf("enter date1(DD-MM-YYYY): ");
        scanf(" %s",date1);
        printf("enter date2(DD-MM-YYYY): ");
        scanf(" %s",date2);
        sortMedicationsByExpiration(med_root,date1,date2);
        break;

        case 7:
        alertExpiringMedications(med_root);
        break;

        case 8:
        sales_track_by_batch(batch_root,med_root);
        printf("the modified medication tree is: \n");
        printf("-------------------------------------------------------------------------\n");
        traverseMed(med_root);
        printf("the modified Batch tree is: \n");
        printf("-------------------------------------------------------------------------\n");
        traversebatch(batch_root);
        break;


        case 9:
        SC=AddNewSupplier(&sup_root,sptr->sup_data[0],&med_root);
        printf("'the Supplier tree is: \n");
        printf("-------------------------------------------------------------------------\n");
        Sup_traverse(sup_root);
        printf("'the corresponding medication tree is: \n");
        printf("-------------------------------------------------------------------------\n");
        traverseMed(med_root);
        break;
              
        case 10:
        SC=updateSupplier(sup_root,med_root);
        printf("'the Supplier tree is: \n");
        printf("-------------------------------------------------------------------------\n");
        Sup_traverse(sup_root);
        printf("'the corresponding medication tree is: \n");
        printf("-------------------------------------------------------------------------\n");
        traverseMed(med_root);
        break;

        case 11:
        printf("enter the supplier id: ");
        scanf("%d",&sup_id);
        ptr=SearchSupplier(sup_root,sup_id,med_root,1);
        break;

       /* case 12:
        SC=DeleteSupplierInfo(&sup_head,&head_ptr);
        Print_SupplierProfiles(sup_head);
        Print_MedicationRecords(head_ptr);
        break;*/

        case 13:
        resetSupplierCounts(sup_root);
        UpdateSuppliersCount(med_root,sup_root,med_root);
        //printf("the tree after updating suppliers turn over is: \n");
       // printf("-------------------------------------------------------------------------\n");
       // printf("-------------------------------------------------------------------------\n");
       // Sup_traverse(sup_root);
        findAllRounderSuppliers(sup_root);
        break;

        case 14:
        UpdateSuppliersTurnover(med_root,sup_root,med_root);
       // printf("the supplier tree after updating turnover is: \n");
       // printf("-------------------------------------------------------------------------\n");
       // Sup_traverse(sup_root);
        PrintTopSuppliers(sup_root);
        break;

        case 15:
        printf("the medication tree is: \n");
        printf("-------------------------------------------------------------------------\n");
        traverseMed(med_root);
        break;

        case 16:
        printf("the supplier tree is: \n");
        printf("-------------------------------------------------------------------------\n");
        Sup_traverse(sup_root);
        break;

        case 17:
        printf("the batch tree is: \n");
        printf("-------------------------------------------------------------------------\n");
        traversebatch(batch_root);
        break;

        case 18:
        saveAllDataToFile(med_root,sup_root,FILE_NAME,SUP_FILE_NAME);
        break;

        default:
        printf("INVALID CHOICE!\n");
        break;
   }
    printf("enter 1 if YES\nenter 0 if NO\nDo you want to continue:");
    scanf("%d",&choice);

}
  return 0;

}
