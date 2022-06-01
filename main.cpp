#include <ctype.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "project1_algorithms.h"

void output_option(std::string& file_name){
    std::string input_buffer;
    int input_option = 0;

    while(input_option != 1 && input_option!= 2){
        std::cout << "\nWould you like to use an output file that is not \"output.txt\"?\n\t1. Yes\n\t2. No\n";

        std::cin >> input_buffer;
        std::cin.ignore();

        if (std::isdigit(std::atoi(input_buffer.c_str())) == 0){ // input is actually an int
            input_option = std::atoi(input_buffer.c_str());
        }

        if(input_option != 1 && input_option!= 2){
            std::cout << "Invalid choice. Please enter another selection.\n";
        }
    }

    if(input_option == 1){
        std::cout << "\nPlease enter the full name (i.e. extension included) of the non-default output file: ";

        std::cin >> input_buffer;
        std::cin.ignore();

        if (input_buffer == "output.txt"){
            std::cout << "No change detected. Preference for output file has not been changed.\n";
        }
        else{
            file_name = input_buffer;
            std::cout << "Output file has been set to \"" << input_buffer << "\".\n";
        }
    }
}

// asks user to specify the # of row_n processes and column_m events per process for an event matrix
// prompts user to populate the event matrix of the given (i.e. row_n x column_m) dimensions
int main(int argc, const char* argv[]){
    std::string file_name = "output.txt";
    if (argc > 1){
        file_name = *(argv+1);
    }
    else if (argc == 1){
        output_option(file_name);
    }
    std::ofstream fout(file_name.c_str());

    int row_n = 0; // 1 < row_n < 6
    int column_m = -1; // column_m < 25
    
    int input_option = 0;

    print_assumptions(fout);
    input_option = selection_menu(fout); // 1 = read user input, 2 = file input, 3 = abort process / exit program

    specify_mDimensions(input_option, row_n, column_m, fout);

    process process_list[row_n]; // array of processes to calculate and verify

    if(input_option == 1){
        print_events_syntax(fout);
        input_matrix(process_list, row_n, column_m, fout);
    }
    else if(input_option == 2){
        f_open(process_list, row_n, column_m, fout);
    }

    algo_select(process_list, row_n, column_m, fout);

    std::cout << "\nExiting program ...\n";
    fout << "\nExiting program ...\n";
    fout.close();
    system("pause");
    return 0;   //end of main function
}