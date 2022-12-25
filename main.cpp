#include <cstdio>
#include <iostream>

#include <Python.h>
#include <iostream>

int main() {

    FILE *fp;
    char path[1035];

    // Call a Python program and pass data through stdin and stdout
    fp = popen("./PythonNlp/Nlp.py", "w");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit(1);
    }

    // Write data to the Python program's stdin
    fprintf(fp, "This is a sentence.\n");
    fflush(fp);

    // Read data from the Python program's stdout
    while (fgets(path, sizeof(path), fp) != NULL) {
        printf("%s", path);
    }

    pclose(fp);

    // //using 'popen'

    // PyObject *pName, *pModule, *pFunc;
    // PyObject *pArgs, *pValue;
    // int i;

    // // Initialize the Python interpreter
    // Py_Initialize();

    // // Import the Python module
    // pName = PyUnicode_New

    return 0;

}