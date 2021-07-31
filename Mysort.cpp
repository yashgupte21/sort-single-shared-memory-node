#include <iostream>
//For string functions
#include <cstring>
#include <string>
//For files
#include <fstream>
#include <bits/stdc++.h>

//Quicksort
#include "quicksort.h"
#include "minheap.h"

//Mergesort
//#include "mergesort.h"

#include <thread>
#include <chrono>
#include <cstdio>
#include <vector>
#include <queue>
#include <numeric>

using namespace std;

//Define constants
#define MB (1024*1024)
#define KB (1024)
#define LINE_LENGTH 100

#define DEFAULT_NUM_THREADS 48

long long  GB  = 1024*1024*1024;
// Setting memory used by threads to 6 gb so that overall memory usage does not exceed 8 GB considering stack, code text segments etc.,
long long  MEM_SIZE  = 8*GB; 

//Initialize total read, write, sort, merge times.
double total_write_time = 0;
double total_read_time = 0;
double total_sort_time = 0;
double total_merge_time = 0;

//Class that represents main file
class Main_File
{
    char **data; //data is an array of some K number of lines
    long long  dataLines; // no. of lines that the data have
    fstream fileHandler;    
    std::string fileName;
    int openType;
    long long totalSize;
    long long  totalNumLines;
    public:
    Main_File(std::string file_name,long long total_file_size,int open_type)
    {
        totalSize = total_file_size;
        fileName = file_name;
        totalNumLines = (long long)total_file_size/LINE_LENGTH;
        openType = open_type;
    }
    int File_Open()
    {
        if(openType == 0) // if file mode is in read mode 
        {
            fileHandler.open(fileName,ios::out | ios::in);
        }
        else  // else file mode is in write mode
        {
            fileHandler.open(fileName,ios::out);
        }    
        return 0;    
    }
    int File_Close()
    {
        fileHandler.close();

        return 0;
    }
    char** File_Read(long long  number_Lines,long long startLine,vector<double> &time_spent, int index)
    {
        chrono::high_resolution_clock::time_point start_read = chrono::high_resolution_clock::now();
        ifstream infile;
        infile.open(fileName,ios::binary);
        infile.seekg(startLine * LINE_LENGTH,ios_base::beg);
        char **array;
        array = (char**)malloc(number_Lines * sizeof(char*));
        for (long long  i = 0; i < number_Lines; i++)
        {
            char* line = (char*)malloc(LINE_LENGTH);
            infile.read(line,LINE_LENGTH);
            array[i] = line;
        }
        infile.close();
        chrono::high_resolution_clock::time_point end_read = chrono::high_resolution_clock::now();
        chrono::duration<double> total_read_time_span = chrono::duration_cast<chrono::duration<double>>(end_read - start_read);
        time_spent[index] = total_read_time_span.count();
        return array;
    }

    //File_Write data to files
    void File_Write(char** writedata,long long  number_Lines,vector<double> &time_spent, int index)
    {
        chrono::high_resolution_clock::time_point start_write = chrono::high_resolution_clock::now();
        ofstream outfile(fileName,ios::binary);
        for(long long  i = 0; i < number_Lines; ++i)
        {
            outfile.write(writedata[i],LINE_LENGTH);
        }
        for (long long  i = 0; i < number_Lines; i++)
        {
            delete[] writedata[i];
        }
        delete[] writedata;
        chrono::high_resolution_clock::time_point end_write = chrono::high_resolution_clock::now();
        chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(end_write - start_write);
        time_spent[index] = time_span.count();

    }
    long long  getTotalFS()
    {
        return totalSize;
    }
    long long  getTotalNumberLines()
    {
        return totalNumLines;
    }
    char** getData()
    {
        return data;
    }
    void setData(char** input_data)
    {
        data = input_data;
    }
    void setNumberDataLines(long long  num_of_data_lines)
    {
        dataLines = num_of_data_lines;
    }
    string getFileName()
    {
        return fileName;
    }
};

class Arguments   // handles arguments for files and threads
{
    int num_of_Threads;
    string fileName;
    bool fileNameExists;
    bool threadFlagPresent;
    public:
    Arguments(int argc, char* argv[])
    {
        threadFlagPresent = false;

        for (int i = 0; i < argc; i++)
        {
            if(i+1 != argc){
                if(strcmp(argv[i],"-t") == 0)
                {
                    num_of_Threads = std::stoi(argv[i+1]);
                    threadFlagPresent = true;
                }
                else if(strcmp(argv[i],"-F") == 0)
                {
                    fileName = argv[i+1];
                    fileNameExists = true;
                }
            }
        }
        if(threadFlagPresent == false)
        {
            num_of_Threads = DEFAULT_NUM_THREADS;
        }
    }
    int Checkfile()
    {
        if(fileNameExists == false)
        {
            return 0;
        }
        return 1;
    }
    int getNumThreads()
    {
        return num_of_Threads;
    } 
    string getFileName()
    {
        return fileName;
    }
};

// Function that performs sorting operation
void sort_funcn(char** data,long long  num_lines,vector<double>&sort_time_spent,int sort_index) 
{
    chrono::high_resolution_clock::time_point start_sort = chrono::high_resolution_clock::now();
    QuickSort(data, 0, num_lines - 1);  
    //MergeSort(data, 0 , num_lines-1);
    chrono::high_resolution_clock::time_point end_sort = chrono::high_resolution_clock::now();
    chrono::duration<double> sort_time_span = chrono::duration_cast<chrono::duration<double>>(end_sort - start_sort);
    sort_time_spent[sort_index] = sort_time_span.count();          
}

//Function that performs sort and write operation
void sort_write(char** data,long long  num_lines,long long  file_size, string outfilename, vector<double>&write_time_spent, int write_index, vector<double>&sort_time_spent, int sort_index)
{
    sort_funcn(data,num_lines,sort_time_spent,sort_index);
    Main_File *outfile = new Main_File(outfilename,file_size, 1); //NOTE: Actually file_size will be input file size by number of threads. also write to file 
    outfile->setNumberDataLines((long long)(file_size/LINE_LENGTH));
    outfile->File_Write(data,num_lines,write_time_spent,write_index);
}

//This needs list of files and output file name
class File_Merger
{
    string outfile;
    vector<string> infilenames;
    public:
    File_Merger(vector<string> ifile,string ofile)
    {
        infilenames = ifile;
        outfile = ofile;
    }
    void mergefiles()
    {

        chrono::high_resolution_clock::time_point start_merge = chrono::high_resolution_clock::now();

        ifstream infile[infilenames.size()];
        ofstream ofile;
        int num_input_files = infilenames.size();
        int file_num = 0;
        int line_length = 100;
        //Input files
        for (size_t i = 0; i < num_input_files; i++)
        {
            infile[i].open(infilenames.at(i),ios::binary);
        }
        //Output file
        ofile.open(outfile,ios::binary);
        MinHeapNode harr[num_input_files];
        priority_queue<MinHeapNode, vector<MinHeapNode>, comp> prior_qu;

        int i = 0;
        for (i = 0; i < num_input_files; i++)
        {
           
            char* line = (char*)malloc(line_length);
            chrono::high_resolution_clock::time_point time1 = chrono::high_resolution_clock::now();            
            infile[i].read(line,line_length);
            chrono::high_resolution_clock::time_point time2 = chrono::high_resolution_clock::now();
            chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(time2 - time1);
            total_read_time += time_span.count();
            harr[i].element = line;
            harr[i].i = i;    
            prior_qu.push(harr[i]);
        }

        int count = 0;
    
        while (count != num_input_files)
        {
            // get min element and store it in the output file 
            MinHeapNode root = prior_qu.top();

            prior_qu.pop();
            chrono::high_resolution_clock::time_point write_1 = chrono::high_resolution_clock::now();            
            ofile.write(root.element,line_length); 
            
            chrono::high_resolution_clock::time_point write_2 = chrono::high_resolution_clock::now();
            chrono::duration<double> write_time_span = chrono::duration_cast<chrono::duration<double>>(write_2 - write_1);
            total_write_time += write_time_span.count();
            delete root.element;
            char* line = (char*)malloc(line_length);
            i = root.i;
            if(infile[i].peek() == EOF)
            {
                count++;
                if (remove(infilenames[i].data()) != 0)
                {
                    string msg = infilenames[i] + " Main_File deletion failed";
                    cout<<msg<<endl;         
                }
                continue;
            }
            chrono::high_resolution_clock::time_point read_1 = chrono::high_resolution_clock::now();
            infile[i].read(line,line_length);
            chrono::high_resolution_clock::time_point read_2 = chrono::high_resolution_clock::now();
            chrono::duration<double> total_read_time_span = chrono::duration_cast<chrono::duration<double>>(read_2 - read_1);
            total_read_time += total_read_time_span.count();
            
            root.element = line;
            prior_qu.push(root);
        }
        chrono::high_resolution_clock::time_point end_merge = chrono::high_resolution_clock::now();
        chrono::duration<double> merge_time_span = chrono::duration_cast<chrono::duration<double>>(end_merge - start_merge);
        total_merge_time = merge_time_span.count();
    }
};

class SortingController
{
    int type;
    Main_File *inputFile;
    Arguments* arguments;
    public:
    SortingController(Main_File* input,Arguments* options)
    {
        //Type will be set according to this
        inputFile = input;
        if(inputFile->getTotalFS() > MEM_SIZE)
        {
            type = 1 ; // External Sorting
        }
        else
        {
            type = 0 ; //Internal sorting
        }
        arguments = options;
    }

    int execute()
    {
        inputFile->File_Open();
        //Step 1: File_Read the data into memory
        long long  num_lines = inputFile->getTotalNumberLines(); 
        long long  file_size = inputFile->getTotalFS();
        switch (type)
        {
            case 0: 
            {
                // case 0 for file sizes less than 8gb withh only one thread for Internal sorting
                vector<double> read_time(1,0);
                vector<double> write_time(1,0);
                vector<double> sort_time(1,0);
                char** data = inputFile->File_Read(num_lines,0,std::ref(read_time),0);
                total_read_time = read_time[0];
                string outfilename = "sorted-data";
                int index=0;
                sort_write(data,num_lines,file_size,outfilename,std::ref(write_time),index,std::ref(sort_time),index);
                    
                total_write_time = write_time[0];
                total_sort_time = sort_time[0];
            }
            break;
            case 1:
            {
                // case 1 for file sizes greater than 8gb and multiple threads for External sorting
                //Decide the size of each file operated by each thread
                //Load data for each thread
                //sort all threads individually and 
                //for each thread, write output to temporary file
                int total_num_threads = arguments->getNumThreads();

                //Chunk size in bytes
                long long  chunk_size = (long long )(MEM_SIZE/(total_num_threads*LINE_LENGTH))*LINE_LENGTH;
                long long  num_lines_per_chunk = (long long )chunk_size/LINE_LENGTH;
                long long  lines_sorted_per_iteration = num_lines_per_chunk*total_num_threads;

                //Number of iterations
                int num_iterations = (int)(file_size/(LINE_LENGTH*lines_sorted_per_iteration));
                num_iterations += 1;

                //Add these leftover lines to the last chunk
                long long  leftover_lines = num_lines;

                //Position
                long long  pos = 0;

                //Create thread array
                thread myThreads[total_num_threads];

                int iteration = 0;
                vector<string> temp_files;
                int leftoverrun = false;
                vector<double> read_time(total_num_threads,0);
                vector<double> write_time(total_num_threads,0);
                vector<double> sort_time(total_num_threads,0);
                while(iteration < num_iterations)
                {
                    fill(read_time.begin(),read_time.end(),0);    
                    fill(write_time.begin(),write_time.end(),0);    
                    fill(sort_time.begin(),sort_time.end(),0);    
                    int numThreadsRunning = 0;
                    for (int i = 0; i < total_num_threads; i++)
                    {
                        if(leftover_lines > 0)
                        {
                            char **data;                                        
                            //File_Read num lines per chunk from the input file
                            if(leftover_lines>num_lines_per_chunk)
                            {
                                data = inputFile->File_Read(num_lines_per_chunk,pos,std::ref(read_time),i);
                            }
                            else
                            {
                                data = inputFile->File_Read(leftover_lines,pos,std::ref(read_time),i);
                                leftoverrun = true;
                            }
                            string outfilename = "file_"+to_string(iteration)+"_"+ to_string(i);  
                            temp_files.push_back(outfilename);
                            long long  file_size = num_lines_per_chunk*LINE_LENGTH;  
                            if(leftoverrun == false)
                            {
                                myThreads[i] = thread(sort_write,data,num_lines_per_chunk,file_size,outfilename,std::ref(write_time),i,std::ref(sort_time),i);                                    
                                numThreadsRunning++;
                            }
                            else
                            {
                                myThreads[i] = thread(sort_write,data,leftover_lines,file_size,outfilename,std::ref(write_time),i,std::ref(sort_time),i);
                                numThreadsRunning++;
                            }
                            pos+=num_lines_per_chunk;
                            leftover_lines -=num_lines_per_chunk;
                            }
                            else
                            {
                                break;
                            }
                        }

                        for (size_t i = 0; i < numThreadsRunning; i++)
                        {
                            myThreads[i].join();
                        }
                        //Merge all read, write and sort times 
                        double iteration_read_time = accumulate(read_time.begin(),read_time.end(),0.0);
                        double iteration_write_time = accumulate(write_time.begin(),write_time.end(),0.0);
                        double iteration_sort_time = accumulate(sort_time.begin(),sort_time.end(),0.0);
                        // cout<<"iteration read time is : "<<iteration_read_time<<endl;
                        // cout<<"iteration write time is : "<<iteration_write_time<<endl;
                        // cout<<"iteration sort time is : "<<iteration_sort_time<<endl;
                        // cout<<"Num Threads in this iteration are : "<<numThreadsRunning<<endl;
                        total_read_time += (iteration_read_time/numThreadsRunning);
                        total_write_time += (iteration_write_time/numThreadsRunning);
                        total_sort_time += (iteration_sort_time/numThreadsRunning);
                        iteration++;
                        //leftover_lines -= lines_sorted_per_iteration;        
                    }
                    if (remove(inputFile->getFileName().data()) != 0)
                    {
                		perror("Main_File deletion failed");         
                    }
                    //call MinHeap opr
                    string outfile = "sorted-data";
                    File_Merger* fm = new File_Merger(temp_files,outfile);
                    fm->mergefiles();
                }
            break;
            
            default:
                break;
        }

        return 0;
    }

};

//Returns file size of a given file. This is a helper function
long long fileSize(std::string fileName)
{
    FILE *file_ptr = NULL;
    file_ptr = fopen(fileName.c_str(),"r");
    fseek(file_ptr,0,SEEK_END);
    long long size = ftell(file_ptr);
    fclose(file_ptr);
    return size;
}

int main(int argc,char *argv[])
{

    double main_routine_time = 0;
    chrono::high_resolution_clock::time_point main_start = chrono::high_resolution_clock::now();

    //Just have options and controller objects together
    //here
    Arguments *arguments = new Arguments(argc,argv);
    if(!arguments->Checkfile())
    {
        std::cout<<"Wrong usage of command"<<std::endl;
        std::cout<<"Usage: ./ms.out -F [filename] -t [number of threads]"<<std::endl;
        exit(0);
    };

    long long file_size = fileSize(arguments->getFileName());
    Main_File *fh = new Main_File(arguments->getFileName(),file_size,0); // read mode

    SortingController* ctrlr = new SortingController(fh,arguments);
    ctrlr->execute();


    cout<<"Total time for READ : "<<total_read_time<<endl;
    cout<<"Total time for WRITE: "<<total_write_time<<endl;
    cout<<"Total time for SORT: "<<total_sort_time<<endl;
    cout<<"Total time for MERGE: "<<total_merge_time<<endl;

    cout<<"File Size :  "<<file_size<<endl;
    double write_speed = 0;
    double read_speed = 0;
    
    if(file_size > MEM_SIZE)
    {
        write_speed =  (file_size*2)/(total_write_time*MB);
        read_speed = (file_size*2)/(total_read_time*MB);    
    }
    else
    {
        write_speed =  (file_size)/(total_write_time*MB);
        read_speed = (file_size)/(total_read_time*MB);
    }

    double sort_speed = (file_size)/(total_sort_time*MB);

    cout<<"File_Read speed : "<<read_speed<<" MBPS"<<endl;
    cout<<"File_Write speed : "<<write_speed<<" MBPS"<<endl;
    cout<<"Sorting speed : "<<sort_speed<<" MBPS"<<endl;

    chrono::high_resolution_clock::time_point main_end = chrono::high_resolution_clock::now();
    chrono::duration<double> main_time_span = chrono::duration_cast<chrono::duration<double>>(main_end - main_start);
    main_routine_time = main_time_span.count();

    double mysort_speed = (file_size)/(main_routine_time*MB);
    cout<<"Total main routine time : "<<main_routine_time<<endl;
    cout<<"MySort speed is  : "<<mysort_speed<<" MBPS"<<endl;
    
    return 0;
}

