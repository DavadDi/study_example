int add(int x, int y){
    return x + y;
}

#include<Python.h>

static PyObject* W_add(PyObject* self, PyObject* args){
    int x;
    int y;
    if(!PyArg_ParseTuple(args, "i|i", &x, &y)){
        return NULL;
    } else {
        return Py_BuildValue("i", add(x, y));
    }
}

static PyMethodDef ExtendMethods[] = {
    {"add", W_add, METH_VARARGS, "a function from C"},
    {NULL, NULL, 0, NULL},
};

PyMODINIT_FUNC initadd(){
    Py_InitModule("add", ExtendMethods);
}
