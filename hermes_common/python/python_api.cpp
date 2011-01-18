#include <stdexcept>

#include "python_api.h"
#include "python_engine_api.h"

static int python_count=0;

Python::Python()
{
    this->_init(-1, NULL);
}

Python::Python(int argc, char* argv[])
{
    this->_init(argc, argv);
}

PyMODINIT_FUNC initpython_engine(void); /*proto*/

void Python::_init(int argc, char* argv[])
{
    python_count++;
    if (python_count == 1) {
        Py_Initialize();
        if (argc >= 0)
            PySys_SetArgv(argc, argv);
        // This initializes the Python module using Python C / API:
        initpython_engine();
        // This imports the module using Cython functionality, so that we can
        // use the Cython api functions
        if (import_python_engine())
            throw std::runtime_error("python_engine failed to import.");
    }
    this->_namespace = namespace_create();
}

Python::~Python()
{
    // Free the namespace. This frees all the dictionary items, so if there
    // are some numpy arrays (or your classes) in the namespace, they will be
    // deallocated at this time.
    Py_DECREF(this->_namespace);

    // The code below would free the interpreter if this was the last instance
    // using it. However, it is currently disabled, because the numpy package
    // segfaults when imported again; also the PYTHONPATH is set only once if
    // python_count is never decreased (which is what we want).
    /*python_count--;
    if (python_count == 0) {
        Py_Finalize();
    }
    */
}

void Python::print_namespace()
{
    namespace_print(_namespace);
}

void Python::exec(const std::string &text)
{
    run_cmd(text.c_str(), this->_namespace);
}

void Python::push(const char *name, PyObject *o)
{
    namespace_push(this->_namespace, name, o);
    // namespace_push() is a regular Cython function and
    // as such, it increfs the object "o" before storing it in the namespace,
    // but we want to steal the reference, so we decref it here (there is still
    // at least one reference stored in the dictionary this->_namespace, so
    // it's safe). This is so that
    //     this->push("i", c2py_int(5));
    // doesn't leak (c2py_int() creates a python reference and push() destroys
    // this python reference)
    Py_DECREF(o);
}

PyObject *Python::pull(const char *name)
{
    PyObject *tmp = namespace_pull(this->_namespace, name);
    // namespace_pull() is a regular Cython function and
    // as such, it increfs the result before returning it, but we only want to
    // borrow a reference, so we decref it here (there is still at least one
    // reference stored in the dictionary this->_namespace, so it's safe)
    // This is so that
    //     int i = py2c_int(this->pull("i"));
    // doesn't leak (pull() borrows the reference, py2c_int() doesn't do
    // anything with the reference, so no leak nor segfault happens)
    Py_DECREF(tmp);
    return tmp;
}

void Python::push_int(const std::string &name, int i)
{
    this->push(name.c_str(), c2py_int(i));
}

int Python::pull_int(const std::string &name)
{
    return py2c_int(this->pull(name.c_str()));
}
