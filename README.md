# Fall 2021 CPSC 474-01 Project 01 - Lamport's Logical Clock

* Group Member(s)
	* __Kenneth Doan__
		* [snarbolax@csu.fullerton.edu](snarbolax@csu.fullerton.edu)

* Language(s)
	* C++

* Professor
	* Prof. Doina Bein, CSU Fullerton

### Project Description
> Several routers need to send encrypted messages to one another but they are located on different time zones. We need to identify the Lamport logical clock timestamp for each message that was sent and received by these routers.
</br></br>
In this project you will design and implement two algorithms related to Lamport’s logical clock. One is to calculate the logical clock values for the events at each of the N processes. Another one is to detect whether an execution is correct given the logical clock values for events at all processes.  For simplification, we assume that the network is fully connected so there are no intermediate nodes for any transmission. You will design two separate algorithms, describe the algorithms using clear pseudocode and implement your algorithms in C/C++/Java, compile, test it, and submit BOTH the report (as a PDF file) and the programs.

### How to Compile the Code
1. Navigate to the directory that contains __main.cpp__, __algorithm_calculate.h__, and __helper.h__ with your OS's terminal / command-line.
2. Type *one* of the following into the terminal / command-line and press enter:
	* > clang main.cpp -o project01.exe

	* > g++ main.cpp -o project01.exe

### How to Run the Code
1. Double-click on "__project01.exe__".
2. Follow the prompts that will print on the terminal / command-line.

or

1. Navigate to the directory that contains __project01.exe__ with your OS's terminal / command-line.
2. Type the following into the terminal / command-line and press enter:
	* > project01.exe
		* You can specify a different output file to use by including the name of the output file--extension included--as the second parameter of the program execution line.
			* the output file has to be in the same directory of the program
			* e.g. "project01.exe insert_user_custom_output_file_here.txt", without the quotation marks
			* you are still able to specify a different output file if you execute the program by double-clicking on it.

Both methods of execution will use "__output.txt__" as the default output file. If the user attempts to use a file that does not exist (in the same directory as the program) as the non-default output file, a file with the specified name will be created.

### Notes for Program Execution
* Input Files Format
	* null events must be represented as "NULL", in all caps for event matrices
		* null events must be represented as "0", for LC matrices
	* each event that belongs to a particular process should be separated by white space
	* each process (with their own events) should be on their own line
	* each process should have the same number of maximum events
		* For event matrices, fill in and lead the last event of each process with null events (i.e. "*NULL*") for processes that have less events than the process with the most events
		* For LC matrices, fill in and lead the last event of each process with "0"s for processes that have less events than the process with the most events
	* several input files have already been included as examples and for demonstrations
		* __calc1.txt__ - meant for the calculate algorithm, sample matrix from Project 01 info, N = 3
		* __calc2.txt__ - meant for the calculate algorithm, N = 5
		* __verify1.txt__ - meant for the verify algorithm, sample matrix from Project 01 info, the LC matrix of *calc1.txt*, N = 3
		* __verify2.txt__ - meant for the verify algorithm, the LC matrix of *calc2.txt*, N = 5
* input files must be in the same directory as the executable file
* when prompted for the names of input files, include the extension of the file
	* e.g. "*calc1.txt*", without the quotation marks

### Miscellaneous Info
#### Assumptions
1.  The network is fully connected.
2.  A broadcast is a send event with more than two receive events.
3.  Each send event is followed by one or more receive events.
4.  A process has only one receive event associated with a send event executed at another process.
5.  There is no receive event without a send event that has happened at a different process earlier in the execution.
6.  There are 1 < n < 6 processes possible per matrix.
7.  There are less than 25 maximum events possible per process.
8.  There are at most 9 send events possible per matrix.
#### Correct Input Syntax for Event Matrix
1. **internal** - a single character, a letter from the English alphabet other than s and r
2. **send** - a two-character string, the first character is s followed by a digit in the range 1-9
3. **receive** - a two-character string, the first character is r followed by a digit in the range 1-9
4. **null** - enter "NULL"

note: The above rules are case-sensitive.

### Known Bugs
1. The LC Matrix or Event Matrix is not printed after running an algorithm.
	* This bug occurs when the user inputs a number less than the actual amount of processes in a file, when prompted for number of processes.
		* This is because the *process_list[]* process array is initialized with the specified number of processes, and the *print_matrix()* function will go out bounds while printing the elements of the matrix if *process_array[]* has not reserved enough memory for the actual number of processes.
	* The bug can be fixed if:
		* *process_list[]* is converted into a process vector
		* *print_matrix()* doesn't recognize user error and iterates based on the user's input for process count instead
		* *f_open()* does not update *int row_n* to be larger than the user-specified number of processes
2. Multiple send events of the same digit (e.g. more than s1, s2, s3, ... .etc) appear in the event matrix created by the verify algorithm
	* This bug occurs due to how *create_event_matrix()* computes and labels these numbers

### Possible Improvements
* Optimization Related
	* convert *int* variables into *size_t* variables
	* convert *process_list[]* into a process vector
	* redo / tweak *create_event_matrix()* to address a known bug
* Clutter Related
	* make output (files) optional
	* make a distinction between system output and user input
	* turn the input / *int* verification codeblock into a function