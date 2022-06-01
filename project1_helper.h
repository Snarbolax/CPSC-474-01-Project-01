#include <ctype.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

// represents a process (or router in the context of the project)
struct process{
    int process_count; // process number
    int event_count; // number of events in the process

    // Assume there are less than 25 events.
    std::string events[25] = { "NULL" }; // holds strings of events.
                                         // initialized all elements to "NULL" to be consistent with LC_values
    int LC_values[25] = { 0 }; // holds logical clock values for each event.
                               // initialize all LC values to 0 to avoid bug in algorithm_verify() where
                               // curr_column.push_back(value) when value is empty (i.e. if elements of LC_values[] aren't initialized to a value)
                               // will just push the value of the event above it (if available [in the same column]) onto curr_column again.
};

// represents a send event (or sent message in the context of the project)
struct send_event{
    char send_num; // holds the digit of the send event
    int LC_value; // holds the logical clock value of the send event. Meant to be used by receive events
};

// prints print_message to output file-stream fout if status is true. Caller should have received status from its own caller, if it is not the initiator
// commented out since it is not necessary for this project iteration
/* void print_switch(std::ofstream& fout, std::string print_message, bool status){
    if (status == true){
        fout << print_message;
    }
} */

// prints the correct input syntax guide for populating an event matrix
void print_events_syntax(std::ofstream& fout){
    std::cout << "\nCorrect Input Syntax for Event Matrix\n";
    std::cout << "=====================================\n";
    std::cout << "1. internal - a single character, a letter from the English alphabet other than s and r\n";
    std::cout << "2. send - a two-character string, the first character is s followed by a digit in the range 1-9\n";
    std::cout << "3. receive - a two-character string, the first character is r followed by a digit in the range 1-9\n";
    std::cout << "4. null - type \"NULL\"\n";
    std::cout << "=====================================\n";
    std::cout << "\tnote: The above rules are case-sensitive.\n";

    fout << "\nCorrect Input Syntax for Event Matrix\n";
    fout << "=====================================\n";
    fout << "1. internal - a single character, a letter from the English alphabet other than s and r\n";
    fout << "2. send - a two-character string, the first character is s followed by a digit in the range 1-9\n";
    fout << "3. receive - a two-character string, the first character is r followed by a digit in the range 1-9\n";
    fout << "4. null - type \"NULL\"\n";
    fout << "=====================================\n";
    fout << "\tnote: The above rules are case-sensitive.\n";
}

// prints assumptions in the context of this project
void print_assumptions(std::ofstream& fout){
    std::cout << "\nAssumptions\n";
    std::cout << "===========\n";
    std::cout << "1. The network is fully connected.\n";
    std::cout << "2. A broadcast is a send event with more than two receive events.\n";
    std::cout << "3. Each send event is followed by one or more receive events.\n";
    std::cout << "4. A process has only one receive event associated with a send event executed at another process.\n";
    std::cout << "5. There is no receive event without a send event that has happened at a different process earlier in the execution.\n";
    std::cout << "6. There are 1 < n < 6 processes possible per matrix.\n";
    std::cout << "7. There are less than 25 maximum events possible per process.\n";
    std::cout << "8. There are at most 9 send events possible per matrix.\n";
    std::cout << "===========\n";

    fout << "\nAssumptions\n";
    fout << "===========\n";
    fout << "1. The network is fully connected.\n";
    fout << "2. A broadcast is a send event with more than two receive events.\n";
    fout << "3. Each send event is followed by one or more receive events.\n";
    fout << "4. A process has only one receive event associated with a send event executed at another process.\n";
    fout << "5. There is no receive event without a send event that has happened at a different process earlier in the execution.\n";
    fout << "6. There are 1 < n < 6 processes possible per matrix.\n";
    fout << "7. There are less than 25 maximum events possible per process.\n";
    fout << "8. There are at most 9 send events possible per matrix.\n";
    fout << "===========\n";
}

// prompts user for input preference
// 1 = read user input, 2 = file input, 3 = abort process / exit program
int selection_menu(std::ofstream& fout){
    std::string input_buffer; // string in case user messes up and doesn't input an int
    int input_option = 0;

    while(input_option != 1 && input_option!= 2 && input_option!= 3){
        std::cout << "\nSelect input preference.\n\t1. Read user input (via terminal)\n\t2. File input\n\t3. Exit program\n";
        fout << "\nSelect input preference.\n\t1. Read user input (via terminal)\n\t2. File input\n\t3. Exit program\n";

        std::cin >> input_buffer;
        fout << input_buffer << "\n";
        std::cin.ignore();

        if (std::isdigit(std::atoi(input_buffer.c_str())) == 0){ // input is actually an int
            input_option = std::atoi(input_buffer.c_str());
        }

        if(input_option != 1 && input_option!= 2 && input_option!= 3){
            std::cout << "Invalid choice. Please enter another selection.\n";
            fout << "Invalid choice. Please enter another selection.\n";
        }
    }
    if(input_option == 3){
        std::cout << "\nProgram closed.\n";
        fout << "\nProgram closed.\n";
        fout.close();
        exit(0);
    }

    return input_option;
}

// prompts user for # of processes and # of maximum events at a process
void specify_mDimensions(int input_option, int &row_n, int&column_m, std::ofstream& fout){
    std::string input_buffer;
    // row_n must be between 1 and 6
    while(row_n <= 1 || row_n >= 6){ // should be optional if there's support for process_list as a <process> vector.
        std::cout << "\nInput the number of processes (i.e. rows): ";
        fout << "\nInput the number of processes (i.e. rows): ";

        std::cin >> input_buffer;
        fout << input_buffer << "\n";
        std::cin.ignore();

        if (std::isdigit(std::atoi(input_buffer.c_str())) == 0){ // input is actually an int
            row_n = std::atoi(input_buffer.c_str());
        }

        if(row_n <= 1 || row_n >= 6){
            std::cout << "Invalid specification. Number of processes must be between 1 and 6.\n";
            fout << "Invalid specification. Number of processes must be between 1 and 6.\n";
        }
    }
    if(input_option == 1){
        // column_m must be less than 25 and at least 0
        while (!((column_m < 25) && (column_m > 0))){
            std::cout << "\nInput the maximum number of events at a process (i.e. columns): ";
            fout << "\nInput the maximum number of events at a process (i.e. columns): ";

            std::cin >> input_buffer;
            fout << input_buffer << "\n";
            std::cin.ignore();

            if (std::isdigit(std::atoi(input_buffer.c_str())) == 0){ // input is actually an int
                column_m = std::atoi(input_buffer.c_str());
            }

            if (!((column_m < 25) && (column_m > 0))){
                std::cout << "Invalid specification. Number of maximum events at a process must be a non-negative int less than 25.\n";
                fout << "Invalid specification. Number of maximum events at a process must be a non-negative int less than 25.\n";
            }
        }
    }
}

// prompts user to populate the matrix of the dimensions they've specified
void input_matrix(process process_list[], int row_n, int column_m, std::ofstream& fout){
    for (int i = 0; i < row_n; ++i){
        bool event_null_detected = false; // checks to automatically populate remaining events in row to null events 
        bool LC_null_detected = false;    // checks to automatically populate remaining events in row to null events
                                          // both bool variables are made obsolete and redundant after initializing
                                          // all elements in events[] and LC_values in struct process to "NULL" and "0", respectively.
                                          // both are still kept in, just to be safe.
        process temp_proc; // temporary process object to define and insert into process_list
        temp_proc.process_count = i;

        std::cout << "\np" << i << "   \n";
        std::cout << "========================\n\n";

        fout << "\np" << i << "   \n";
        fout << "========================\n\n";
        for(int j = 0; j < column_m; ++j){ // prompt user for events to populate each process
            if (event_null_detected == false && LC_null_detected == false){
                std::cout << "Input event " << j << " at p" << i << ": ";
                fout << "Input event " << j << " at p" << i << ": ";

                std::cin >> temp_proc.events[j];
                fout << temp_proc.events[j] << "\n";
                temp_proc.LC_values[j] = std::atoi(temp_proc.events[j].c_str()); // used for determining an event's LC value for verify

                if (temp_proc.events[j] == "NULL"){
                    event_null_detected = true;
                }
                else if(temp_proc.events[j] == "0"){
                    LC_null_detected = true;
                }
            }
            else{
                if (event_null_detected == true){
                    temp_proc.events[j] = "NULL";
                }
                else if(LC_null_detected == true){
                    temp_proc.LC_values[j] = 0;
                }
            }
        }
        process_list[i] = temp_proc;
    }
}

// reads matrix info from user-specified file
void f_open(process process_list[], int &row_n, int &column_b, std::ofstream& fout){
    std::string file = ""; // holds the name of the file

    std::string str; // holds an entire line from a file stream
    std::string token; // holds an event token to be processed into an event for a process.

    int proc_count = 0; // updates row_n in case of user error when inputting # of processes. Also updates a process's process number
                        // Only helps if user overstates # of processes. Doesn't help if user understates # of processes in specify_mDimensions().
    int event_count = 0; // updates column_b. Also used as index to iterate through a process's array of events and LC values

    int max_events = 0; // holds the maximum possible events per process, dependent on the process with the most events

    std::cout << "Enter the full name of a file to be analyzed (file must be in the same directory as executable) and hit enter: ";  //prompts user to enter file name to be parsed
    fout << "Enter the full name of a file to be analyzed (file must be in the same directory as executable) and hit enter: ";

    std::cin >> file;                                         //file name entered is inputted to string
    fout << file << "\n";
    std::ifstream fin(file);                                 //creates file object to open file to be parsed

    if(fin.is_open()){                                       // tests if file is open, outputs confirmation if it is
        std::cout << "\nInput file \"" << file << "\" opened succesfully\n\n";
        fout << "\nInput file \"" << file << "\" opened succesfully\n\n";

        while(getline(fin,str)){  //stores characters of a file into str, on a line by line basis
            event_count = 0; // reset event count for new process
            process temp_proc; // temporary process to populate with info and to insert into process_list

            temp_proc.process_count = proc_count; // update current temporary process with proc_count+1 process number

            std::stringstream str_copy(str); // creates of copy of str (i.e. the current line) to split into tokens

            while(str_copy >> token){ // separates alphanum strings delimited by whitespace and newlines
                temp_proc.events[event_count] = token; // insert the event into the current process's event array
                temp_proc.LC_values[event_count] = std::atoi(token.c_str()); // used for determining an event's LC value for verify
                ++event_count; // event processed for current process
            }
            temp_proc.event_count = event_count;
            if (event_count > max_events){
                max_events = event_count;
            }
            process_list[proc_count] = temp_proc; //add the process to the process array
            ++proc_count; // process processed
        }
        row_n = proc_count;
        column_b = max_events;
    }
    else{
        std::cout << "\nInput file \"" << file << "\" could not be opened\n\n";      // if file is not able to open, outputs this to user
        fout << "\nInput file \"" << file << "\" could not be opened\n\n";
        fin.close();
        fout.close();
        exit(EXIT_FAILURE);
    }

    if(fin.is_open())
        fin.close();                                      //closes input file
}