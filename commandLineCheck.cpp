#include "commandLineCheck.h"
/*
bool checkParameters(int , char** )
{
    if(argvCheck() && enumCheck() && directoryCheck())
    {
        return true;
    }
    else()
    {
        return false;
    }
}

bool argvCheck(){

    int port, threads, buffers;
    for(size_t i = 1; i < 4; i++)
    {
        if(atoi(argv[i]))
        {
            if(i == 1)
            {
                port = atoi(argv[1]);
            }
            else if(i == 2)
            {
                threads = atoi(argv[2]);
            }
            else
            {
                buffers = atoi(argv[3]);
            }
        }
        else()
        {
            return false;
        }
    }

    return true;
}

bool enumCheck()
{
    if(strncmp(argv[4]), "FIFO", 4)) == 0)
    {
        return true;
    }
    else if(strncmp(argv[4]), "ANY", 3)) == 0)
    {
        return true;
    }
    else if(strncmp(argv[4]), "SJF", 3)) == 0)
    {
        return true;
    }
    else if(strncmp(argv[4]), "SRJF", 4)) == 0)
    {
        return true;
    }

    return false;

}
*/

/* Dan: I made a couple of changes to this.
 * Basically, it closes dir if the directory successfully opens
 * and then it returns true.
 *
 * In addition, directoryCheck takes a string as a parameter
 */
bool directoryCheck(char *path)
{
    DIR* dir = opendir(path);
    if (dir) 
    {
        closedir(dir);
	return true;
    }
    return false;
    
}











