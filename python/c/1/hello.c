#include <Python.h>

static PyObject *helloError;

static PyObject *
hello_system(PyObject *self, PyObject *args)
{
	const char *command;
	int sts;

	if (!PyArg_ParseTuple(args, "s", &command))
		return NULL;
	sts = system(command);
	if (sts < 0) {
		PyErr_SetString(helloError, "System command failed");
		return NULL;
	}
	return PyLong_FromLong(sts);
}



static PyMethodDef helloMethods[] = {
{"system",  hello_system, METH_VARARGS,
"Execute a shell command."},
{NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef hellomodule = {
	PyModuleDef_HEAD_INIT,
	"hello",   /* name of module */
	"hello_doc", /* module documentation, may be NULL */
	-1,       /* size of per-interpreter state of the module,
			  or -1 if the module keeps state in global variables. */
	helloMethods
};

PyMODINIT_FUNC
PyInit_hello(void)
{
	PyObject *m;

	m = PyModule_Create(&hellomodule);
	if (m == NULL)
		return NULL;

	helloError = PyErr_NewException("hello.error", NULL, NULL);
	Py_INCREF(helloError);
	PyModule_AddObject(m, "error", helloError);
	return m;
}

