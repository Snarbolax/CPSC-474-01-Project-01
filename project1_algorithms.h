#include <algorithm>
#include <ctype.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "project1_helper.h"

// prints a matrix showing the LC values of a given event matrix
// 4th argument (int matrix_op) determines whether it prints an LC matrix or event matrix
// int matrix_op = 0, prints LC matrix, meant for algorithm_calculate()
// int matrix_op = 1, prints event matrix, meant for algorithm_verify()
void print_matrix(process proc_list[], int row_n, int column_m, int matrix_op, std::ofstream& fout){
    if (matrix_op == 0){
        std::cout << "\nLogical Clock (Value) Matrix";
        fout << "\nLogical Clock (Value) Matrix";
    }
    else if(matrix_op == 1){
        std::cout << std::endl << "Event Matrix";
        fout << "\n" << "Event Matrix";
    }
    std::cout << "\n============================\n\n";
    fout << "\n============================\n\n";

    for(int i = 0; i < row_n; ++i){
        std::cout << "p" << proc_list[i].process_count << "   ";
        fout << "p" << proc_list[i].process_count << "   ";

        for(int j = 0; j < column_m; ++j){
            if(matrix_op == 0){
                std::cout << proc_list[i].LC_values[j] << " ";
                fout << proc_list[i].LC_values[j] << " ";
            }
            else if(matrix_op == 1){
                std::cout << proc_list[i].events[j] << " ";
                fout << proc_list[i].events[j] << " ";
            }
        }
        std::cout << std::endl;
        fout << "\n";
    }
    std::cout << "\n============================\n";
    fout << "\n============================\n";
}

send_event send_list[10]; // assume there are less than 10 send events. Holds unreceived send events encountered.
int send_list_count = 0; // count of send events in send_list

// calculate the Lamport logical clock values for the events at each of the N processes
// each run of a program must terminate within 60 minutes if N=3 and M=4.
void algorithm_calculate(process proc_list[], int proc_num, int event_num){
    for(int i = 0; i < proc_num; ++i){ // row corresponding to processor number
        for (int j = 0; j < event_num; ++j){ // column corresponding to event number
            char char_pointer;
            char_pointer = proc_list[i].events[j][0]; // points to the first char of the j'th event in process i

            if(j == 0){ // if first event ...
                if(char_pointer == 's'){ // is a send event
                    char_pointer = proc_list[i].events[j][1]; // points to the second char of the j'th event in process i
                    // "If a is the first event and is an internal or send event, then LC(a) = 1"
                    proc_list[i].LC_values[j] = 1;
                    bool event_acknowledged = false;

                    for(int z = 0; z < send_list_count; ++z){
                        if(char_pointer == send_list[z].send_num){
                            event_acknowledged = true; // if char_pointer is pointing to an unreceived / already-processed send event
                        }
                    }
                    if(event_acknowledged == false){ // if the send event has not been received or processed
                        send_event temp_send;
                        temp_send.send_num = char_pointer;
                        temp_send.LC_value = 1;
                        send_list[send_list_count] = temp_send;
                        ++send_list_count;
                        algorithm_calculate(proc_list, proc_num, event_num);  // recursively call function to proceed with updated LC values
                    }
                }
                else if(char_pointer == 'r'){ // is a receive event
                    char_pointer = proc_list[i].events[j][1];

                    for(int z = 0; z < send_list_count; ++z){
                        if(char_pointer == send_list[z].send_num){ // if receive event is associated with a discovered send event
                            // "... LC(a) = k + 1 where k is the LC-value of the send event corresponding to a (that has occurred at a process other than P)"
                            proc_list[i].LC_values[j] = send_list[z].LC_value + 1;
                        }
                    }
                }
                else if(isalpha(char_pointer)){
                    char_pointer = proc_list[i].events[j][0];

                    if(char_pointer == 'N'){ // is null event
                        for(int z = j; z < event_num; ++z){
                            proc_list[i].LC_values[z] = 0; // null event and everything following null event in current process has a LC value of 0
                        }
                    }
                    else if(char_pointer != 'N'){ // is internal event
                        // "If a is the first event and is an internal or send event, then LC(a) = 1"
                        proc_list[i].LC_values[j] = 1;
                    }
                }
            }
            else{ // if not first event ...
                if(char_pointer == 's'){ // is send event
                    char_pointer = proc_list[i].events[j][1];
                    // "... LC(a) = k + 1 where k is the LC-value of the event just before a at process P."
                    proc_list[i].LC_values[j] = proc_list[i].LC_values[j-1] + 1;
                    bool event_acknowledged = false;

                    for(int z = 0; z < send_list_count; ++z){
                        if(char_pointer == send_list[z].send_num){
                            if(send_list[z].LC_value != proc_list[i].LC_values[j]){
                                send_list[z].LC_value = proc_list[i].LC_values[j-1] + 1; // update LC value
                                algorithm_calculate(proc_list, proc_num, event_num);
                            }
                            event_acknowledged = true;
                        }
                    }
                    if(event_acknowledged == false){
                        send_event temp_send;
                        temp_send.send_num = char_pointer;
                        temp_send.LC_value = proc_list[i].LC_values[j-1] + 1;
                        send_list[send_list_count] = temp_send;
                        ++send_list_count;
                        algorithm_calculate(proc_list, proc_num, event_num);
                    }
                }
                else if(char_pointer == 'r'){ // is receive event
                    char_pointer = proc_list[i].events[j][1];

                    for(int z = 0; z < send_list_count; ++z){
                        if(char_pointer == send_list[z].send_num){ // if receive finds a send event to follow
                            // "LC(a) = max{ k, LC(b) } + 1"
                            proc_list[i].LC_values[j] = std::max(proc_list[i].LC_values[j-1], send_list[z].LC_value) + 1;
                        }
                    }
                }
                else if(isalpha(char_pointer)){
                    if(char_pointer == 'N'){ // is null event
                        for(int z = j; z < event_num; ++z){
                            proc_list[i].LC_values[z] = 0;
                        }
                    }
                    else if(char_pointer != 'N'){ // is internal event
                        // "... LC(a) = k + 1 where k is the LC-value of the event just before a at process P"
                        proc_list[i].LC_values[j] = proc_list[i].LC_values[j-1] + 1;
                    }
                }
            }
        }
    }
}

// checks whether each process is in asecending order, there's an initiator in the LC matrix, and if all events properly execute in relative time
bool correct_check(process proc_list[], int row_n, int column_m, std::vector<int> accounted_event){
    sort(accounted_event.begin(), accounted_event.end()); // ascending order, from left to right

    for (int i = 1; i <= accounted_event.size(); ++i){
        if (i == 1){ // first accounted event
            if (accounted_event.at(i - 1) != 1){ // initiator does not exist
                return false; // incorrect execution, no initiator(s)
            }
        }
        else{ // not first accounted event
            if ((accounted_event.at(i - 1) != i || accounted_event.at(i - 2) != (i - 1))){ // accounted_event should contain an ascending order of unique and uninterrupted natural numbers, from 1 to the size of accounted_event
                return false; // incorrect execution, gap in chain of natural numbers.
            }
        }
    }

    for (int i = 0; i < row_n; ++i){
        std::vector<int> curr_row;

        for (int j = 0; j < column_m; ++j){
            int value = proc_list[i].LC_values[j]; // LC value of current event in a particular process

            if (value != 0){ // ignore NULL
                curr_row.push_back(value);

                for (int z = 0; z < curr_row.size(); ++z){
                    if (curr_row.at(z) > value){ // while ignoring 0s, current process has an imperfect ascending order of LC values / events
                        return false; // incorrect execution, process's chain of (non-null) events is not in ascending order
                    }
                }
            }
        }
    }

    return true; // the LC matrix survived evaluation-hell :)))
}

void create_event_matrix(process proc_list[], std::vector<int> internal_vector, std::vector<int> receive_vector, std::vector<int> send_vector, int row_n, int column_m){
    int char_index = 96; // initialzed as the int value of character literal 'a'. intended range is 97-122 or 'a'-'z'.

    std::vector<int> receive_vector_copy = receive_vector; // create copy because the original will be affected by erase()
    std::vector<int> send_vector_copy = send_vector; // create copy because the original will be affected by erase()

    sort(receive_vector_copy.begin(), receive_vector_copy.end()); // ascending order, from left to right
    sort(send_vector_copy.begin(), send_vector_copy.end()); // ascending order, from left to right

    for (int i = 0; i < column_m; i++){
        for (int j = 0; j < row_n; j++){
            int value = proc_list[j].LC_values[i];

            if (value == 0){ // is null event
                proc_list[j].events[i] = "NULL";
            }
            else if (value != 0){ // not null event
                bool found_internal = (std::find(internal_vector.begin(), internal_vector.end(), (value)) != internal_vector.end());
                bool found_receive = (std::find(receive_vector.begin(), receive_vector.end(), (value)) != receive_vector.end());
                bool found_send = (std::find(send_vector.begin(), send_vector.end(), (value)) != send_vector.end());

                if (found_internal){
                    if (found_internal && found_receive){ // is both internal and receive, receive > internal
                        proc_list[j].events[i] = "r" + std::to_string(std::distance(receive_vector_copy.begin(), std::find(receive_vector_copy.begin(), receive_vector_copy.end(), value)) + 1);
                        receive_vector.erase(std::find(receive_vector.begin(), receive_vector.end(), (value))); // remove receive event so find() can search for other receive events
                    }
                    else if (found_internal && found_send){ // is both internal and send, send > internal
                        proc_list[j].events[i] = "s" + std::to_string(std::distance(send_vector_copy.begin(), std::find(send_vector_copy.begin(), send_vector_copy.end(), value)) + 1);
                        send_vector.erase(std::find(send_vector.begin(), send_vector.end(), (value))); // remove send event so find() can search for other send events
                    }
                    else{ // is actually just an internal event
                        ++char_index; // next int value of ASCII char.
                        proc_list[j].events[i] = static_cast<char>(char_index); //type-cast the int value as a char
                    }
                }
                else if (found_receive){
                    proc_list[j].events[i] = "r" + std::to_string(std::distance(receive_vector_copy.begin(), std::find(receive_vector_copy.begin(), receive_vector_copy.end(), value)) + 1);
                    receive_vector.erase(std::find(receive_vector.begin(), receive_vector.end(), (value)));
                }
                else if (found_send){
                    proc_list[j].events[i] = "s" + std::to_string(std::distance(send_vector_copy.begin(), std::find(send_vector_copy.begin(), send_vector_copy.end(), value)) + 1);
                    send_vector.erase(std::find(send_vector.begin(), send_vector.end(), (value)));
                }
            }
        }
    }
}

bool algorithm_verify(process proc_list[], int proc_num, int event_num, std::ofstream& fout){
    std::vector<int> internal_vector; // vector of potential LC internal events
    std::vector<int> receive_vector; // vector of potential LC receive events
    std::vector<int> send_vector; // vector of potential LC send events

    std::vector<int> accounted_event; // events that are already acknowledged / noticed

    for (int i = 0; i < event_num; ++i){ // events / columns / horizontal
        std::vector<int> curr_column; // vector of LC events in current column

        for (int j = 0; j < proc_num; ++j){ // processes / rows / vertical
            std::vector<int> curr_row; // vector of LC events in current row
            int value = proc_list[j].LC_values[i]; // LC value of current event in a particular process

            curr_column.push_back(value);

            if (value != 0){ // event is not NULL
                if ((value == 1) && !(std::find(accounted_event.begin(), accounted_event.end(), value) != accounted_event.end())){ // event is an initiator. event is not accounted for
                    accounted_event.push_back(value); // event is now accounted for
                }

                if ((value != 1 && !(std::find(accounted_event.begin(), accounted_event.end(), (value - 1)) != accounted_event.end())) || (std::find(curr_column.begin(), curr_column.end(), (value - 1)) != curr_column.end())){ // event preceding in LC value is not found or acknowledged
                    for(int z = 0; z < event_num; ++z){
                        int checking = proc_list[j].LC_values[z];

                        curr_row.push_back(checking); // populate curr_row with LC values of all events at the process
                    }

                    if(i == 0){ // first events
                        receive_vector.push_back(value); // gap detected / first event is a receive event
                        send_vector.push_back(value - 1); // LC value of its respective send event is 1 less than the LC value of the receive event

                        if (!(std::find(accounted_event.begin(), accounted_event.end(), value) != accounted_event.end())){
                            accounted_event.push_back(value);
                        }
                    }
                    else if (i != 0){ // not first events
                        if (!(curr_row.at(i - 1) == (value - 1))){ // if LC value of preceding event in curr_row vector doesn't equal to LC value of preceding event for value
                            receive_vector.push_back(value); // gap detected. Value is a receive event
                            send_vector.push_back(value - 1); // Preceding event is a candidate for a send event.

                            if (!(std::find(accounted_event.begin(), accounted_event.end(), value) != accounted_event.end())){ // receive event isn't accounted for
                                accounted_event.push_back(value); // receive event is accounted
                            }
                        }
                        else{
                            internal_vector.push_back(value); // event is internal

                            if (!(std::find(accounted_event.begin(), accounted_event.end(), value) != accounted_event.end())){
                                accounted_event.push_back(value);
                            }
                        }
                    }
                }
                else{ // event preceding in LC value is found or acknowledged
                    internal_vector.push_back(value); // event is an internal event candidate

                    if (!(std::find(accounted_event.begin(), accounted_event.end(), value) != accounted_event.end())){
                        accounted_event.push_back(value);
                    }
                }
            }
        }
    }

    bool correct = correct_check(proc_list, proc_num, event_num, accounted_event);

    if (correct == false){
        std::cout << std::endl << "INCORRECT\n";
        fout << "\nINCORRECT\n";
    }
    else if (correct == true){
        create_event_matrix(proc_list, internal_vector, receive_vector, send_vector, proc_num, event_num);
    }

    return correct;
}

// prompts user for the algo to run
// 1 = calculate a matrix's Lamport LC value, 2 = Verify the execution correctness of all processes in an LC matrix, 3 = exit program
void algo_select(process process_list[], int row_n, int column_m, std::ofstream& fout){
    std::string input_buffer;
    int input_option = 0;

    while(input_option != 1 && input_option!= 2 && input_option!= 3){
        std::cout << "\nSelect the algorithm for the matrix to be used as an input for.\n\t1. Calculate (Lamport LC Values)\n\t2. Verify (Execution Correctness)\n\t3. Exit program\n";
        fout << "\nSelect the algorithm for the matrix to be used as an input for.\n\t1. Calculate (Lamport LC Values)\n\t2. Verify (Execution Correctness)\n\t3. Exit program\n";

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

    if(input_option == 1){
        algorithm_calculate(process_list, row_n, column_m);
        print_matrix(process_list, row_n, column_m, 0, fout);
    }
    else if(input_option == 2){
        bool correct = algorithm_verify(process_list, row_n, column_m, fout);
        if (correct == true){
            print_matrix(process_list, row_n, column_m, 1, fout);
        }
    }
    else if(input_option == 3){
        std::cout << "\nProgram closed.\n";
        fout << "\nProgram closed.\n";
        fout.close();
        exit(0);
    }
}