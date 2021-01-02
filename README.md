# m-project
gcc -Wall EMModel.c -o EMModel

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define TIME_MAX 80
#define FILE_PROC_STAT "/proc/stat"
#define CSV_FILE_NAME "csvFile_NameC.csv"

/**
* This method is used to access the proc/stat file (that gives cpu statistics) and
* store its informations in several variable given as parameters
* @param p_Cuser : store time in user mode
* @param p_Cnice : store time in user mode with low priority
* @param p_Csystem : store time in system mode
* @param p_Cidle : store time in the idle task
*/
void getCPUCycles(long *p_Cuser, int *p_Cnice, long *p_Csystem, long *p_Cidle)
{
    FILE* file = fopen(FILE_PROC_STAT, "r"); //pointer to proc file
    if (file != NULL)
	{//if file exists
        fscanf(file,"%*s %li %li %li %d", &*p_Cuser, &*p_Csystem, &*p_Cidle, &*p_Cnice);

        fclose(file);
    }else
	{ //if file doesn't exist or is corrupted
        printf("error reading file");
        exit(1);
    }
}
/**
* This method, called first in the execution, is used to calculate a first measure of cpu cycles
* with the informations retrieved by getCPUCycles method.
* @param p_Cbusyt_1 Calculate the busy time of the cpu : the sum of time spend in user and system mode
* @param p_Ctotal_1 Calculate the busy time but add the time spend in the idle task
*
*  other parameters are described in getCPUCycles method
*/
void calculateVar_1(long *p_Cbusyt_1, long *p_Ctotalt_1, long *p_Cuser, int *p_Cnice, long *p_Csystem, long *p_Cidle)
{
    getCPUCycles(p_Cuser, p_Cnice, p_Csystem, p_Cidle);

    //calculate CPU cycles
    *p_Cbusyt_1 = *p_Cuser + *p_Cnice + *p_Csystem;
    *p_Ctotalt_1 = *p_Cbusyt_1 + *p_Cidle;
}

/**
* This method is used to calculate, first a ratio (p_u) between the 2 measure
* and then, with this ratio, the energy consumed by the cpu
* @param p_u calculate the ratio between time spend
* @param p_P calculate the consumption in cpu
*  other parameters are described above
*/
void calculateP(long *p_Cbusyt, long *p_Cbusyt_1, long *p_Ctotalt, long *p_Ctotalt_1, float *p_P, float *p_u){
    float x_Cbusyt = (*p_Cbusyt - *p_Cbusyt_1);
    float y_Ctotalt = (*p_Ctotalt - *p_Ctotalt_1);
    float z = x_Cbusyt/y_Ctotalt;
    *p_u = z; // This ratio is needed to calculate p_P

    if (*p_u <= 0.5)
	{
        *p_P = 3.4495*(*p_u) + 3.8563; // based on mathematical formula
    }
    else
	{
        *p_P = 1.4584*(*p_u) + 4.7788;
    }
}

/**
* This method, called at the end of the execution, is used to calculate a new measure of cpu cycles,
* it does the same as calculateVar_1 except it calls calculateP to have real consomation of the cpu
* @param p_Cbusyt same as p_Cbusyt_1 but with more recent values
* @param p_Ctotal same as p_Ctotal_1 but with more recent values
* @params p_P and p_u are calculated with calculateP method
*/
void calculateVar(long *p_Cbusyt, long *p_Cbusyt_1, long *p_Ctotalt, long *p_Ctotalt_1, long *p_Cuser, int *p_Cnice, long *p_Csystem, long *p_Cidle, float *p_P, float *p_u)
{
    getCPUCycles(p_Cuser, p_Cnice, p_Csystem, p_Cidle);
    //calculate the final CPU cycles
    *p_Cbusyt = *p_Cuser + *p_Cnice + *p_Csystem;
    *p_Ctotalt = *p_Cbusyt + *p_Cidle;

    calculateP(p_Cbusyt, p_Cbusyt_1, p_Ctotalt, p_Ctotalt_1, p_P, p_u);
}

/**
* This method is used to store all the informations calculated in a CSV file
* which is created in the curent directory
* @param date_time contains the date when the measures are made
*/
void recordInfo(char *date_time, long *p_Cuser, int *p_Cnice, long *p_Csystem, long *p_Cidle, long *p_Cbusyt, long *p_Cbusyt_1, long *p_Ctotalt, long *p_Ctotalt_1, float *p_u, float *p_P){
    FILE *p_file;
    char filename[20] = CSV_FILE_NAME;

    if(fopen(filename, "r"))
	{ //if file already exists
        p_file = fopen(filename, "a"); //writes at the end of the file
        fprintf(p_file, "%s,%li,%d,%li,%li,%li,%li,%li,%li,%f,%f\n", date_time, *p_Cuser, *p_Cnice, *p_Csystem, *p_Cidle, *p_Cbusyt, *p_Cbusyt_1, *p_Ctotalt, *p_Ctotalt_1, *p_u, *p_P);
        fclose(p_file);
    }
    else
	{
        p_file = fopen(filename, "w"); //creates an empty file for writing
        fprintf(p_file, "Time,Cuser,Cnice,Csystem,Cidle,Cbusyt,Cbusyt_1,Ctotalt,Ctotalt_1,u,P\n"); // create headers in the new file
        fprintf(p_file, "%s,%li,%d,%li,%li,%li,%li,%li,%li,%f,%f\n", date_time, *p_Cuser, *p_Cnice, *p_Csystem, *p_Cidle, *p_Cbusyt, *p_Cbusyt_1, *p_Ctotalt, *p_Ctotalt_1, *p_u, *p_P);
        fclose(p_file);
    }
}

/**
* This method displays Help Menu
*/
void helpMenu(){
	printf ("Welcome to HELP MENU\n");
}

/**
* This method displays values on terminal
*/
void printCmd(int Pflag, int uflag, float P, float u, long Cuser, int Cnice, long Csystem, long Cidle){
	if(Pflag == 1 && uflag == 1)
	{
		printf("P %f, u %f", P, u);
	}
	else if(Pflag == 1)
	{
		printf("P %f", P);
	}
	else if(uflag == 1)
	{
		printf("u %f", u);
	}
	else
	{
		printf("%f %f %li %li %li %d\n", P, u, Cuser, Csystem, Cidle, Cnice);
	}
}

/**
* isprint declaration. checks whether the passed character is printable
* @param c : character to be checked
*/
int isprint(int c);

/**
* Main function
* @params argc and **argv are used in helpMenu function
*/
int main(int argc, char **argv)
{
    //VARIABLES
    float P=0; //energy consumption of the cpu
    float *p_P=&P;//pointer to P
    //the components of the formula: u= (Cbusy[t]-Cbusy[t-1])/(Ctotal[t]-Ctotal[t-1])
    float u=0;
    float *p_u=&u;//pointer to u

    //Cbusy=Cuser+Cnice+Csystem
    long Cbusyt=0;
    long *p_Cbusyt=&Cbusyt;//pointer to Cbusyt
    long Cbusyt_1=0;
    long *p_Cbusyt_1=&Cbusyt_1;//pointer to Cbusyt_1

    //Ctotal=Cbusy+Cidle
    long Ctotalt=0;
    long *p_Ctotalt=&Ctotalt;//pointer to Ctotalt
    long Ctotalt_1=0;
    long *p_Ctotalt_1=&Ctotalt_1;//pointer to Ctotalt_1

    long Cuser=0;//Time spent in user mode
    long *p_Cuser=&Cuser;//pointer to Cuser
    int Cnice=0;//Time spent in user mode with low priority
    int *p_Cnice=&Cnice;//pointer to Cnice
    long Csystem=0;//Time spent in system mode
    long *p_Csystem=&Csystem;//pointer to Csystem

    //This value should be USER_HZ times the second entry in the /proc/uptime pseudo-file
    long Cidle=0;//Time spent in the idle task
    long *p_Cidle=&Cidle;//pointer to Cidle

    //Help menu
    int opt=0;
    
    int Pflag=0;
    int uflag=0;
    
    opterr=0;
    
/**
* WHILE LOOP - Check and display some options. Args take into account : -h -u -p -up
*/
    while ((opt = getopt(argc, argv, "huP")) != -1) //while there are remaining options
        switch (opt)
        {
        case 'h':
            helpMenu();
            exit(1);
            break;
        case 'u':
            uflag=1;
            break;
        case 'P':
            Pflag=1;
            break;
        case '?':
            if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
            return 1;
		default:
			abort();
		}

/**
 * INFINITE LOOP - Main program
*/
    while(1)
	{
        calculateVar_1(p_Cbusyt_1, p_Ctotalt_1, p_Cuser, p_Cnice, p_Csystem, p_Cidle);
        sleep(1); //needed to have 2 different measure
        calculateVar(p_Cbusyt, p_Cbusyt_1, p_Ctotalt, p_Ctotalt_1, p_Cuser, p_Cnice, p_Csystem, p_Cidle, p_P, p_u);
        printCmd(Pflag, uflag, P, u, Cuser, Cnice, Csystem, Cidle);
		
        time_t rawtime = time(&rawtime);
        struct tm *p_tm = localtime(&rawtime);
        char date_time[TIME_MAX];
        strftime(date_time, TIME_MAX, "%X", p_tm);//convert the date according to local settings

        recordInfo(date_time, p_Cuser, p_Cnice, p_Csystem, p_Cidle, p_Cbusyt, p_Cbusyt_1, p_Ctotalt, p_Ctotalt_1, p_u, p_P);
    }
    return 0;
}

