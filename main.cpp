#include <cstdio>
#include <iostream>

#include <Python.h>

int main() {

    // //using 'popen'

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

   //using Python.h

  PyObject *pName, *pModule, *pFunc;
  PyObject *pArgs, *pValue;
  int i;

  // Initialize the Python interpreter
  Py_Initialize();

  PyRun_SimpleString("import sys");
  PyRun_SimpleString("sys.path.append('/home/dptemp/AI/PythonNlp/')");
   
  pModule = PyImport_ImportModule("Nlp");
  

  std::cout << pModule << std::endl;

//   // Import the Python module
//   pName = PyUnicode_FromString("./PythonNlp/Nlp.py");
//   pModule = PyImport_Import(pName);
  //Py_DECREF(pName);

  std::cout << "Result2: " << std::endl;

  if (pModule != NULL) {
    // Get the Python function
    pFunc = PyObject_GetAttrString(pModule, "tokenize");
    if (pFunc && PyCallable_Check(pFunc)) {
      // Prepare the function arguments
    //   pArgs = PyTuple_New("I will win, you will lose");
      //PyTuple_SetItem(pArgs, 0, PyLong_FromLong(10));

      pArgs = PyTuple_New(1);
      PyTuple_SetItem(pArgs, 0, PyUnicode_FromString("I will win, you will lose, I will earn a lot of money"));


      // Call the Python function
      pValue = PyObject_CallObject(pFunc, pArgs);
      Py_DECREF(pArgs);

      if (pValue != NULL) {
        // Print the returned value
        std::cout << "Result: " << PyLong_AsLong(pValue) << std::endl;
        Py_DECREF(pValue);
      } else {
        Py_DECREF(pFunc);
        Py_DECREF(pModule);
        PyErr_Print();
        std::cout << "Call failed" << std::endl;
        return 1;
      }
    } else {
      if (PyErr_Occurred()) {
        PyErr_Print();
      }
      std::cout << "Cannot find function" << std::endl;
    }
    Py_XDECREF(pFunc);
    Py_DECREF(pModule);
  } else {
    PyErr_Print();
    std::cout << "Failed to load module" << std::endl;
    return 1;
  }

  // Clean up
  Py_Finalize();
  return 0;

    return 0;

}