# sort-single-shared-memory-node
Sort data files on single shared memory node deployed on Chameleon Cloud platform

Internal and External Sorting has been performed on 4 files with a Shared Memory program (Mysort.cpp)
and  Linux sort command line utility.

Datasets of 1gb , 4gb, 16gb and 64gb have been generated using gensort utility.

Gensort can be installed from this link http://www.ordinal.com/gensort.html

Generating datasets using gensort :
Use the 64 bit version of gensort to generate data

	./64/gensort -a [FILE_SIZE] [FILENAME_PATH]

Example 1gb file : ./64/gensort -a 10000000 1gb.txt

--- Shared Memory Program Mysort.cpp ----

Mysort.cpp program has to be compiled by running the Makefile

	make -f Makefile

This Makefile consists of the following command 

	g++ -std=c++11 -g -pthread quicksort.cpp Mysort.cpp minheap.cpp -o ms.out

After executing the above Makefile we will get the output object file ms.out

My program takes two arguments , the generated file name and the number of threads

	./ms.out -F [FILENAME_PATH] -t [NUM_OF_THREADS] | tee -o [LOG_FILENAME]

For 1gb and 4gb files I have implemented 2 and 4 number of threads 
And for 16gb and 64gb files I have implemented 24 and 48 number of threads 
The above command generated sorted-data at the same file location

Example : ./ms.out -F 1gb.txt -t 4

---- Linux sort implementation --- 

The linux sort command has to be executed along with some parameters like the key, buffer size , parallel number of threads and the output file location
Time command is used to keep track of the time required to executed linux sort command

	time sort [FILENAME] -k1 -S8G --parallel=[NUME_OF_THREADS] -o [OUTPUT_FILENAME] | tee -a [LOG_FILENAME]

Example : time sort 116gb.txt -k1 -S8G --parallel=48 -o sorted_data_64gb48t.txt | tee -a log64gb48t.log

--- Valsort execution --
Now the above generated sorted-data files will be validated using valsort utility

	./64/valsort [FILENAME]

Example : ./64/valsort sorted-data

The above command will validate sorted data

--- Repository contents --

Logs folder - contains log files and screenshots for the best combination of file size and number of threads.
 



  


