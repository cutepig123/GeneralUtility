/**********************************************************************
 * 
 *  Toby Opferman
 *
 *  Driver Example
 *
 *  This example is for educational purposes only.  I license this source
 *  out for use in learning how to write a device driver.
 *
 *     Driver Shared Header File
 **********************************************************************/




#ifndef __EXAMPLE_H__
#define __EXAMPLE_H__

typedef unsigned int UINT;
typedef char * PCHAR;

/* #define __USE_DIRECT__ */
/* #define __USE_BUFFERED__ */

NTSTATUS Example_Create(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Example_Close(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Example_IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Example_Read(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Example_WriteBufferedIO(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Example_WriteDirectIO(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Example_WriteNeither(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS Example_UnSupportedFunction(PDEVICE_OBJECT DeviceObject, PIRP Irp);

#ifdef __USE_DIRECT__
#define IO_TYPE DO_DIRECT_IO
#define USE_WRITE_FUNCTION  Example_WriteDirectIO
#endif
 
#ifdef __USE_BUFFERED__
#define IO_TYPE DO_BUFFERED_IO
#define USE_WRITE_FUNCTION  Example_WriteBufferedIO
#endif

#ifndef IO_TYPE
#define IO_TYPE 0
#define USE_WRITE_FUNCTION  Example_WriteNeither
#endif

#endif






