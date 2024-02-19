#include <iostream>
#include <string>
#include <fstream>
#include <vector>
using namespace std;

struct node_like{
    string subject;
    string object;
    node_like *next;
    node_like():subject(""),object(""),next(nullptr){}
    node_like(string &s, string &o, node_like *nl):subject(s),object(o),next(nl){}
};

struct node_taken{
    string name;
    node_taken *next;
    node_taken():name(""),next(nullptr){}
    node_taken(string s, node_taken *nt):name(s),next(nt){}
};

void print_likes_list2(node_like* head);

node_like* is_subject_in_likes(node_like * &head, const string & str)//takes a subject and checks if it is in the likes list, if so, it returns the node of its last occurrence, and nullptr otherwise
{
    node_like *temp = head;
    node_like *last_occurrence = nullptr;
    while(temp != nullptr)
    {
        if(temp->subject == str)
        {
            last_occurrence = temp;
        }
        temp = temp->next;
    }
    return last_occurrence;
}


void add_to_end_likes(node_like* & head, string s, string o) // this adds new subject to the end of the list
{
    if (head == nullptr) // in case list is empty
    {
        head = new node_like(s, o, nullptr);
    }
    else
    {
        node_like *last_occurrence = is_subject_in_likes(head, s);  // is_in_likes function finds the last occurrence of the subject, and add the node after it
        if (last_occurrence != nullptr)  // if the subject is in the list
        {
            last_occurrence->next = new node_like(s, o, last_occurrence->next);
        }
        else // subject is not in the list, so add it to the end of the list
        {
            node_like *temp = head;
            while (temp->next != nullptr)
            {
                temp = temp->next;
            }
            temp->next = new node_like(s, o, nullptr);
        }
    }
}

void print_likes_list(node_like* head)
{
    cout << "Likes list: ";
    node_like *temp = head;
    while(temp != nullptr)
    {
        cout << "(" << temp->subject << ", " << temp->object << ")";
        temp = temp->next;
        if(temp != nullptr)
        {
            cout << " ";
        }
    }
    cout << endl;
}

void print_taken_list(node_taken * head)
{
    cout << "Taken list: " ;
    node_taken *temp = head;
    while(temp != nullptr)
    {
        cout << temp->name;
        temp = temp->next;
        if(temp != nullptr)
        {
            cout << " ";
        }
    }
    cout << endl;
}

// add to the taken list, if not exist
void add_to_taken(node_taken* &head, const string & str) // addition is sorted
{
    if (head == nullptr|| head->name > str)
    {
        head = new node_taken(str, head);
        cout << "Added to taken list: " << str << endl;
        return;
    }
    node_taken *temp = head;
    while(temp->next != nullptr && temp->next->name < str)
    {
        temp = temp->next;
    }
    if(temp->name == str)
    {
        cout << "Already in the taken list: " << str << endl;
        return;
    }
    temp->next = new node_taken(str, temp->next);
    cout << "Added to taken list: " << str << endl;
}


bool is_taken(node_taken *head, const string & str)
{
    node_taken *temp = head;
    while(temp != nullptr)
    {
        if(temp->name == str)
        {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

bool is_match_candidate(node_like * head_like, const string & subject, const string & object)
{
    node_like *temp = head_like;
    while(temp != nullptr)
    {
        if(temp->subject == object && temp->object == subject)
        {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

bool is_redundant(node_like * head_like, const string & subject, const string & object)
{
    node_like *temp = head_like;
    while(temp != nullptr)
    {
        if(temp->subject == subject && temp->object == object)
        {
            return true;
        }
        temp = temp->next;
    }
    return false;
}


void remove_from_likes(node_like* & head, const string & str)
{
    node_like *temp = head;
    node_like *prev = nullptr;

    while(temp != nullptr)
    {
        if (temp->subject == str || temp->object == str)
        {
            if (prev == nullptr) // If the node to be deleted is the head node
            {
                head = temp->next;
            }
            else
            {
                prev->next = temp->next;
            }

            node_like *to_be_deleted = temp;
            temp = temp->next;
            delete to_be_deleted;
        }
        else
        {
            prev = temp;
            temp = temp->next;
        }
    }
}

void read_file_into_lists(ifstream & file, node_like* & head_like, node_taken* & head_taken)
{
    string subject, verb, object;
    while(file >> subject >> verb >> object)
    {
        cout<< "Read: " << subject << " " << verb << " " << object << endl;
        if (is_taken(head_taken, subject) || is_taken(head_taken, object))
        {
            cout<< "Either " << subject << " or " << object << " is already taken, so the like relation is not added." << endl;
            print_likes_list(head_like);
            continue;
        }
        if (subject == object)
        {
            cout << "Subject and object can not be the same, the relation is not added." << endl;
            print_likes_list(head_like);
            continue;
        }
        if (is_match_candidate(head_like, subject, object))
        {
            cout << "Match found: " << subject << " likes " << object << " and " << object << " likes " << subject << endl;
            add_to_taken(head_taken, subject);
            add_to_taken(head_taken, object);
            // as a result of match, we need to remove the like relation from the list
            cout << "Any node that has " << subject << " or " << object << " in its subject or object will be removed from the likes list." << endl;
            remove_from_likes(head_like, subject);
            remove_from_likes(head_like, object);
            print_likes_list(head_like);
            continue;
        }
        if (is_redundant(head_like, subject, object))
        {
            cout << "The relation is redundant, so it is not added." << endl;
            print_likes_list(head_like);
            continue;
        }
        cout << "It is safe to add the relation to the likes list." << endl;
        add_to_end_likes(head_like, subject, object);
        print_likes_list(head_like);
    }
}

void open_file(ifstream & input_file, string file_name)
{
    input_file.open(file_name,ios_base::in);
    while (!input_file.is_open())
    {
        input_file.clear();
        cout << "Could not open the file. Please enter a valid file name:"<<endl;
        cin>>file_name;
        input_file.open(file_name,ios_base::in);
    }
}

void deleteList(node_like* & head)
{
    node_like* current = head;
    while (current != nullptr)
    {
        node_like* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
}

void deleteList(node_taken* & head)  // overloaded function
{
    node_taken* current = head;
    while (current != nullptr)
    {
        node_taken* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
}


int main() {

    node_like *head_like = nullptr;
    node_taken *head_taken= nullptr;

    string file_name;
    cout << "Please enter the file name:"<<endl;
    cin >> file_name;
    ifstream file;
    open_file(file,file_name);
    read_file_into_lists(file, head_like, head_taken);
    file.close();
    //print_likes_list(head_like);
    print_taken_list(head_taken);

    // clearing both lists
    deleteList(head_like);
    deleteList(head_taken);
    cout << "Programs ends successfully." << endl;
    return 0;
}


void print_likes_list2(node_like* head)
{
    if (head == nullptr)
    {
        //cout << "Likes list is empty." << endl;
        cout << endl;
        return;
    }

    string current_subject = head->subject;
    vector<string> current_objects = {head->object};

    node_like *temp = head->next;
    while(temp != nullptr)
    {
        if (temp->subject == current_subject)
        {
            current_objects.push_back(temp->object);
        }
        else
        {
            cout << "{" << current_subject << ": ";
            for (int i = 0; i < current_objects.size(); i++)
            {
                cout << current_objects[i];
                if (i != current_objects.size() - 1)
                {
                    cout << ", ";
                }
            }
            cout << "} " ;

            current_subject = temp->subject;
            current_objects = {temp->object};
        }
        temp = temp->next;
    }

    // Print the objects for the last subject
    cout << "{" << current_subject << ": ";
    for (int i = 0; i < current_objects.size(); i++)
    {
        cout << current_objects[i];
        if (i != current_objects.size() - 1)
        {
            cout << ", ";
        }
    }
    cout << "} " ;

    cout << endl;
}