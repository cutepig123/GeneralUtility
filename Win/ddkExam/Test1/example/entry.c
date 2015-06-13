/**********************************************************************
 * 
 *  Toby Opferman
 *
 *  Driver Example
 *
 *  This example is for educational purposes only.  I license this source
 *  out for use in learning how to write a device driver.
 *
 *     Driver Entry Point
 **********************************************************************/
//#define _X86_


#include <wdm.h>
#include "example.h"

 
    
VOID Example_Unload(PDRIVER_OBJECT  DriverObject);    
NTSTATUS DriverEntry(PDRIVER_OBJECT  pDriverObject, PUNICODE_STRING  pRegistryPath); 

/* 
 * These compiler directives tell the Operating System how to load the
 * driver into memory. The "INIT" section is discardable as you only
 * need the driver entry upon initialization, then it can be discarded.
 *
 */
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, Example_Unload)

/**********************************************************************
 * 
 *  DriverEntry
 *
 *    This is the default entry point for drivers.  The parameters
 *    are a driver object and the registry path.
 *
 **********************************************************************/
NTSTATUS DriverEntry(PDRIVER_OBJECT  pDriverObject, PUNICODE_STRING  pRegistryPath)
{
    NTSTATUS NtStatus = STATUS_SUCCESS;
    UINT uiIndex = 0;
    PDEVICE_OBJECT pDeviceObject = NULL;
    UNICODE_STRING usDriverName, usDosDeviceName;

    KdPrint(("DriverEntry Called \r\n"));

    RtlInitUnicodeString(&usDriverName, L"\\Device\\Example");
    RtlInitUnicodeString(&usDosDeviceName, L"\\DosDevices\\Example"); 

    NtStatus = IoCreateDevice(pDriverObject, 0, &usDriverName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);

    if(NtStatus == STATUS_SUCCESS)
    {

        /*
         * The "MajorFunction" is a list of function pointers for entry points into the driver.
         * You can set them all to point to 1 function, then have a switch statement for all
         * IRP_MJ_*** functions or you can set specific function pointers for each entry
         * into the driver.
         *
         */
        for(uiIndex = 0; uiIndex < IRP_MJ_MAXIMUM_FUNCTION; uiIndex++)
             pDriverObject->MajorFunction[uiIndex] = Example_UnSupportedFunction;
    
        pDriverObject->MajorFunction[IRP_MJ_CLOSE]             = Example_Close;
        pDriverObject->MajorFunction[IRP_MJ_CREATE]            = Example_Create;
        pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]    = Example_IoControl;
        pDriverObject->MajorFunction[IRP_MJ_READ]              = Example_Read;
        pDriverObject->MajorFunction[IRP_MJ_WRITE]             = USE_WRITE_FUNCTION;
    
        /* 
         * Required to unload the driver dynamically.  If this function is missing
         * the driver cannot be dynamically unloaded.
         */
        pDriverObject->DriverUnload =  Example_Unload; 

        /* 
         * Setting the flags on the device driver object determine what type of I/O
         * you wish to use.
         *   
         *  Direct I/O - MdlAddress describes the Virtual Address list.  This is then
         *                 used to lock the pages in memory.
         *
         *                 PROS: Fast, Pages are not copied.
         *                 CONS: Uses resources, needs to lock pages into memory.
         *
         *  Buffered I/o - SystemBuffer is then used by the driver to access the data.  The I/O
         *                   manager will copy the data given by the user mode driver into this buffer
         *                   on behalf of the driver.
         *
         *                   CONS: Slower operation (Use on smaller data sets)
         *                         Uses resources, allocates Non-paged memory
         *                         Large allocations may not work since it would
         *                         require allocating large sequential non-paged memory.
         *                   PROS: Easier to use, driver simply accesses the buffer
         *                         Usermode buffer is not locked in memory
         *
         *
         *  Neither Buffered or Direct - This is when you simply read the buffer directly using the user-mode address.
         *                     Simply omit DO_DIRECT_IO and DO_BUFFERED_IO to perform this action.
         *
         *                     PROS: No copying or locking pages occurs.
         *
         *                     CONS: You *MUST* be in the context of the user-mode thread that made the request.
         *                           being in another process space you the page tables would not point to
         *                           the same location.
         *                           You have to perform some checking and probeing in order to verify
         *                           when you can read/write from the pages.
         *                           You cannot access a user mode address unless it's locked into memory
         *                           at >= DPC level.
         *                           The usermode process could also change the access rights of the
         *                           buffer while the driver is trying to read it!
         *
         *
         *  If your driver services lower level drivers you will need to set this field to the same type of
         *  I/O.
         *
         *  The flags for Read/Write is:
         *      DO_BUFFERED_IO, DO_DIRECT_IO, Specify neither flag for "Neither".
         *
         *  The flags (defined in the IOCTL itself) for Control I/O is:
         *     METHOD_NEITHER, METHOD_BUFFERED, METHOD_IN_DIRECT or METHOD_OUT_DIRECT 
         *
         *  From MSDN:
         *    For IRP_MJ_READ and IRP_MJ_WRITE requests, drivers specify the I/O method by using flags in each 
         *    DEVICE_OBJECT structure. For more information, see Initializing a Device Object.
         *
         *    For IRP_MJ_DEVICE_CONTROL and IRP_MJ_INTERNAL_DEVICE_CONTROL requests, the I/O method is determined 
         *    by the TransferType value that is contained in each IOCTL value. For more information, see Defining
         *    I/O Control Codes.
         */
         
        pDeviceObject->Flags |= IO_TYPE;
    
        /*
         * We are not required to clear this flag in the DriverEntry as the I/O Manager will
         * clear it for us, but we will anyway.  Creating a device in any other location we
         * would need to clear it.
         */
    
        pDeviceObject->Flags &= (~DO_DEVICE_INITIALIZING);
    
        /*
         * Create a Symbolic Link to the device. Example -> \Device\Example
         */
    
        IoCreateSymbolicLink(&usDosDeviceName, &usDriverName);
    }


    return NtStatus;
}


/**********************************************************************
 * 
 *  Example_Unload
 *
 *    This is an optional unload function which is called when the
 *    driver is unloaded.
 *
 **********************************************************************/
VOID Example_Unload(PDRIVER_OBJECT  DriverObject)
{    
    
    UNICODE_STRING usDosDeviceName;
    
    KdPrint(("Example_Unload Called \r\n"));
    
    RtlInitUnicodeString(&usDosDeviceName, L"\\DosDevices\\Example");
    IoDeleteSymbolicLink(&usDosDeviceName);

    IoDeleteDevice(DriverObject->DeviceObject);
}


