目的：
使用swig生成class的python binding

分析：
注册class类型
SWIGINTERN PyObject *Circle_swigregister(PyObject *SWIGUNUSEDPARM(self), PyObject *args) {
  PyObject *obj;
  if (!PyArg_ParseTuple(args,(char *)"O:swigregister", &obj)) return NULL;
  SWIG_TypeNewClientData(SWIGTYPE_p_Circle, SWIG_NewClientData(obj));
  return SWIG_Py_Void();
}


constructor Circle(double r)生成以下代码
SWIGINTERN PyObject *_wrap_new_Circle(PyObject *SWIGUNUSEDPARM(self), PyObject *args) {
  PyObject *resultobj = 0;
  double arg1 ;
  double val1 ;
  int ecode1 = 0 ;
  PyObject * obj0 = 0 ;
  Circle *result = 0 ;
  
  if (!PyArg_ParseTuple(args,(char *)"O:new_Circle",&obj0)) SWIG_fail;
  ecode1 = SWIG_AsVal_double(obj0, &val1);
  if (!SWIG_IsOK(ecode1)) {
    SWIG_exception_fail(SWIG_ArgError(ecode1), "in method '" "new_Circle" "', argument " "1"" of type '" "double""'");
  } 
  arg1 = static_cast< double >(val1);
  result = (Circle *)new Circle(arg1);
  resultobj = SWIG_NewPointerObj(SWIG_as_voidptr(result), SWIGTYPE_p_Circle, SWIG_POINTER_NEW |  0 );
  return resultobj;
fail:
  return NULL;
}

虚函数virtual double area()生成以下代码
SWIGINTERN PyObject *_wrap_Circle_area(PyObject *SWIGUNUSEDPARM(self), PyObject *args) {
  PyObject *resultobj = 0;
  Circle *arg1 = (Circle *) 0 ;
  void *argp1 = 0 ;
  int res1 = 0 ;
  PyObject * obj0 = 0 ;
  double result;
  
  if (!PyArg_ParseTuple(args,(char *)"O:Circle_area",&obj0)) SWIG_fail;
  res1 = SWIG_ConvertPtr(obj0, &argp1,SWIGTYPE_p_Circle, 0 |  0 );
  if (!SWIG_IsOK(res1)) {
    SWIG_exception_fail(SWIG_ArgError(res1), "in method '" "Circle_area" "', argument " "1"" of type '" "Circle *""'"); 
  }
  arg1 = reinterpret_cast< Circle * >(argp1);
  result = (double)(arg1)->area();
  resultobj = SWIG_From_double(static_cast< double >(result));
  return resultobj;
fail:
  return NULL;
}

如何用一个父类Shape::move的函数实现派生类功能Circle::MOve

static void *_p_CircleTo_p_Shape(void *x, int *SWIGUNUSEDPARM(newmemory)) {
    return (void *)((Shape *)  ((Circle *) x));
}
static swig_cast_info _swigc__p_Shape[] = {  {&_swigt__p_Square, _p_SquareTo_p_Shape, 0, 0},  {&_swigt__p_Shape, 0, 0, 0},  {&_swigt__p_Circle, _p_CircleTo_p_Shape, 0, 0},{0, 0, 0, 0}};
static swig_cast_info *swig_cast_initial[] = {
  _swigc__p_Circle,
  _swigc__p_Shape,
  _swigc__p_Square,
  _swigc__p_char,
};
后面俺也没看懂

其中SWIG_ConvertPtr得到this指针
#define SWIG_ConvertPtr(obj, pptr, type, flags)         SWIG_Python_ConvertPtr(obj, pptr, type, flags)
#define SWIG_Python_ConvertPtr(obj, pptr, type, flags)  SWIG_Python_ConvertPtrAndOwn(obj, pptr, type, flags, 0)


/* Convert a pointer value */

SWIGRUNTIME int
SWIG_Python_ConvertPtrAndOwn(PyObject *obj, void **ptr, swig_type_info *ty, int flags, int *own) {
  int res;
  SwigPyObject *sobj;
  int implicit_conv = (flags & SWIG_POINTER_IMPLICIT_CONV) != 0;

  if (!obj)
    return SWIG_ERROR;
  if (obj == Py_None && !implicit_conv) {
    if (ptr)
      *ptr = 0;
    return SWIG_OK;
  }

  res = SWIG_ERROR;

  sobj = SWIG_Python_GetSwigThis(obj);
  if (own)
    *own = 0;
  while (sobj) {
    void *vptr = sobj->ptr;
    if (ty) {
      swig_type_info *to = sobj->ty;
      if (to == ty) {
        /* no type cast needed */
        if (ptr) *ptr = vptr;
        break;
      } else {
        swig_cast_info *tc = SWIG_TypeCheck(to->name,ty);
        if (!tc) {
          sobj = (SwigPyObject *)sobj->next;
        } else {
          if (ptr) {
            int newmemory = 0;
            *ptr = SWIG_TypeCast(tc,vptr,&newmemory);
            if (newmemory == SWIG_CAST_NEW_MEMORY) {
              assert(own); /* badly formed typemap which will lead to a memory leak - it must set and use own to delete *ptr */
              if (own)
                *own = *own | SWIG_CAST_NEW_MEMORY;
            }
          }
          break;
        }
      }
    } else {
      if (ptr) *ptr = vptr;
      break;
    }
  }
  if (sobj) {
    if (own)
      *own = *own | sobj->own;
    if (flags & SWIG_POINTER_DISOWN) {
      sobj->own = 0;
    }
    res = SWIG_OK;
  } else {
    if (implicit_conv) {
      SwigPyClientData *data = ty ? (SwigPyClientData *) ty->clientdata : 0;
      if (data && !data->implicitconv) {
        PyObject *klass = data->klass;
        if (klass) {
          PyObject *impconv;
          data->implicitconv = 1; /* avoid recursion and call 'explicit' constructors*/
          impconv = SWIG_Python_CallFunctor(klass, obj);
          data->implicitconv = 0;
          if (PyErr_Occurred()) {
            PyErr_Clear();
            impconv = 0;
          }
          if (impconv) {
            SwigPyObject *iobj = SWIG_Python_GetSwigThis(impconv);
            if (iobj) {
              void *vptr;
              res = SWIG_Python_ConvertPtrAndOwn((PyObject*)iobj, &vptr, ty, 0, 0);
              if (SWIG_IsOK(res)) {
                if (ptr) {
                  *ptr = vptr;
                  /* transfer the ownership to 'ptr' */
                  iobj->own = 0;
                  res = SWIG_AddCast(res);
                  res = SWIG_AddNewMask(res);
                } else {
                  res = SWIG_AddCast(res);		    
                }
              }
            }
            Py_DECREF(impconv);
          }
        }
      }
    }
    if (!SWIG_IsOK(res) && obj == Py_None) {
      if (ptr)
        *ptr = 0;
      if (PyErr_Occurred())
        PyErr_Clear();
      res = SWIG_OK;
    }
  }
  return res;
}
看代码原理大概就是，维护一个父子类型的树，遍历这个树
如果类型对的上，就返回该类型对应的指针
