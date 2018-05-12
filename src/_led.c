#include <Python.h>

#include <rc/led.h>

static char module_docstring[] =
  "This module provides an interface for the LEDs.";

static PyObject *ledError;

// led functions
static PyObject *led_set(PyObject *self, PyObject *args);
static PyObject *led_get(PyObject *self, PyObject *args);
static PyObject *led_blink(PyObject *self, PyObject *args);

static PyMethodDef module_methods[] = {
  {"set",
   (PyCFunction)led_set,
   METH_VARARGS,
   "set led"}
  ,
  {"get",
   (PyCFunction)led_get,
   METH_VARARGS,
   "get led"}
  ,
  {"blink",
   (PyCFunction)led_blink,
   METH_VARARGS,
   "blink led"}
  ,
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef module = {
  PyModuleDef_HEAD_INIT,
  "_led",   /* name of module */
  module_docstring, /* module documentation, may be NULL */
  -1,       /* size of per-interpreter state of the module,
         or -1 if the module keeps state in global variables. */
  module_methods
};

/* python functions */

PyMODINIT_FUNC PyInit__led(void)
{
  PyObject *m;

  /* create module */
  m = PyModule_Create(&module);
  if (m == NULL)
    return NULL;

  /* create exception */
  ledError = PyErr_NewException("led.error", NULL, NULL);
  Py_INCREF(ledError);
  PyModule_AddObject(m, "error", ledError);

  // no initialization required for LEDs, they init themselves as used

  return m;
}

static
PyObject *led_set(PyObject *self,
      PyObject *args)
{

  /* parse arguments */
  rc_led_t led, state;
  if (!PyArg_ParseTuple(args, "Ii", &led, &state)) {
    PyErr_SetString(ledError, "Invalid arguments");
    return NULL;
  }

  /* set led */
  if ((state = rc_led_set(led, state)) < 0) {
    //printf("state = %d\n", state);
    PyErr_SetString(ledError, "Failed");
    return NULL;
  }

  /* Build the output tuple */
  PyObject *ret = Py_BuildValue("");

  return ret;
}

static
PyObject *led_get(PyObject *self,
      PyObject *args)
{

  /* parse arguments */
  rc_led_t led;
  if (!PyArg_ParseTuple(args, "I", &led)) {
    PyErr_SetString(ledError, "Invalid argument");
    return NULL;
  }

  /* set led */
  int state = rc_led_get(led);
  if (state < 0) {
    PyErr_SetString(ledError, "Failed");
    return NULL;
  }

  /* Build the output tuple */
  PyObject *ret = Py_BuildValue("i", state);

  return ret;
}

static
PyObject *led_blink(PyObject *self,
        PyObject *args)
{

  /* parse arguments */
  rc_led_t led;
  float hz, duration;
  if (!PyArg_ParseTuple(args, "Iff", &led, &hz, &duration)) {
    PyErr_SetString(ledError, "Invalid argument");
    return NULL;
  }

  /* blink led */
  if (rc_led_blink(led, hz, duration) < 0) {
    PyErr_SetString(ledError, "Failed");
    return NULL;
  }

  /* Build the output tuple */
  PyObject *ret = Py_BuildValue("");

  return ret;
}