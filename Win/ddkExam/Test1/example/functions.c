/**********************************************************************
 * 
 *  Toby Opferman
 *
 *  Driver Example
 *
 *  This example is for educational purposes only.  I license this source
 *  out for use in learning how to write a device driver.
 *
 *     Driver Functionality
 **********************************************************************/

//#define _X86_ 

#include <wdm.h>
#include "example.h"


/**********************************************************************
 * Internal Functions
 **********************************************************************/
BOOLEAN Example_IsStringTerminated(PCHAR pString, UINT uiLength);


#pragma alloc_text(PAGE, Example_Create) 
#pragma alloc_text(PAGE, Example_Close) 
#pragma alloc_text(PAGE, Example_IoControl) 
#pragma alloc_text(PAGE, Example_Read)
#pragma alloc_text(PAGE, Example_WriteDirectIO)
#pragma alloc_text(PAGE, Example_WriteBufferedIO)
#pragma alloc_text(PAGE, Example_WriteNeither)
#pragma alloc_text(PAGE, Example_UnSupportedFunction)
#pragma alloc_text(PAGE, Example_IsStringTerminated)




/**********************************************************************
 * 
 *  Example_Create
 *
 *    This is called when an instance of this driver is created (CreateFile)
 *
 **********************************************************************/
NTSTATUS Example_Create(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_SUCCESS;
    DbgPrint("Example_Create Called \r\n");

    return NtStatus;
}

/**********************************************************************
 * 
 *  Example_Close
 *
 *    This is called when an instance of this driver is closed (CloseHandle)
 *
 **********************************************************************/
NTSTATUS Example_Close(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_SUCCESS;
    DbgPrint("Example_Close Called \r\n");

    return NtStatus;
}



/**********************************************************************
 * 
 *  Example_IoControl
 *
 *    This is called when an IOCTL is issued on the device handle (DeviceIoControl)
 *
 **********************************************************************/
NTSTATUS Example_IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_SUCCESS;
    DbgPrint("Example_IoControl Called \r\n");

    return NtStatus;
}



/**********************************************************************
 * 
 *  Example_Read
 *
 *    This is called when a read is issued on the device handle (ReadFile/ReadFileEx)
 *
 **********************************************************************/
NTSTATUS Example_Read(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_SUCCESS;
    DbgPrint("Example_Read Called \r\n");

    return NtStatus;
}



/**********************************************************************
 * 
 *  Example_WriteDirectIO
 *
 *    This is called when a write is issued on the device handle (WriteFile/WriteFileEx)
 *
 *    This version uses Direct I/O
 *
 **********************************************************************/
NTSTATUS Example_WriteDirectIO(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_SUCCESS;
    PIO_STACK_LOCATION pIoStackIrp = NULL;
    PCHAR pWriteDataBuffer;

    DbgPrint("Example_WriteDirectIO Called \r\n");
    
    /*
     * Each time the IRP is passed down the driver stack a new stack location is added
     * specifying certain parameters for the IRP to the driver.
     */
    pIoStackIrp = IoGetCurrentIrpStackLocation(Irp);
    
    if(pIoStackIrp)
    {
        pWriteDataBuffer = MmGetSystemAddressForMdlSafe(Irp->MdlAddress, NormalPagePriority);
    
        if(pWriteDataBuffer)
        {                             
            /*
             * We need to verify that the string is NULL terminated. Bad things can happen
             * if we access memory not valid while in the Kernel.
             */
           if(Example_IsStringTerminated(pWriteDataBuffer, pIoStackIrp->Parameters.Write.Length))
           {
                DbgPrint(pWriteDataBuffer);
           }
        }
    }

    return NtStatus;
}

/**********************************************************************
 * 
 *  Example_WriteBufferedIO
 *
 *    This is called when a write is issued on the device handle (WriteFile/WriteFileEx)
 *
 *    This version uses Buffered I/O
 *
 **********************************************************************/
NTSTATUS Example_WriteBufferedIO(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_SUCCESS;
    PIO_STACK_LOCATION pIoStackIrp = NULL;
    PCHAR pWriteDataBuffer;

    DbgPrint("Example_WriteBufferedIO Called \r\n");
    
    /*
     * Each time the IRP is passed down the driver stack a new stack location is added
     * specifying certain parameters for the IRP to the driver.
     */
    pIoStackIrp = IoGetCurrentIrpStackLocation(Irp);
    
    if(pIoStackIrp)
    {
        pWriteDataBuffer = (PCHAR)Irp->AssociatedIrp.SystemBuffer;
    
        if(pWriteDataBuffer)
        {                             
            /*
             * We need to verify that the string is NULL terminated. Bad things can happen
             * if we access memory not valid while in the Kernel.
             */
           if(Example_IsStringTerminated(pWriteDataBuffer, pIoStackIrp->Parameters.Write.Length))
           {
                DbgPrint(pWriteDataBuffer);
           }
        }
    }

    return NtStatus;
}

/**********************************************************************
 * 
 *  Example_WriteNeither
 *
 *    This is called when a write is issued on the device handle (WriteFile/WriteFileEx)
 *
 *    This version uses Neither buffered or direct I/O.  User mode memory is
 *    read directly.
 *
 **********************************************************************/
NTSTATUS Example_WriteNeither(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_SUCCESS;
    PIO_STACK_LOCATION pIoStackIrp = NULL;
    PCHAR pWriteDataBuffer;

    DbgPrint("Example_WriteNeither Called \r\n");
    
    /*
     * Each time the IRP is passed down the driver stack a new stack location is added
     * specifying certain parameters for the IRP to the driver.
     */
    pIoStackIrp = IoGetCurrentIrpStackLocation(Irp);
    
    if(pIoStackIrp)
    {
        /*
         * We need this in an exception handler or else we could trap.
         */
        __try {
        
                //ProbeForRead(Irp->UserBuffer, pIoStackIrp->Parameters.Write.Length, TYPE_ALIGNMENT(char));
                pWriteDataBuffer = Irp->UserBuffer;
            
                if(pWriteDataBuffer)
                {                             
                    /*
                     * We need to verify that the string is NULL terminated. Bad things can happen
                     * if we access memory not valid while in the Kernel.
                     */
                   if(Example_IsStringTerminated(pWriteDataBuffer, pIoStackIrp->Parameters.Write.Length))
                   {
                        DbgPrint(pWriteDataBuffer);
                   }
                }

        } __except( EXCEPTION_EXECUTE_HANDLER ) {

              NtStatus = GetExceptionCode();     
        }

    }

    return NtStatus;
}
                       

/**********************************************************************
 * 
 *  Example_UnSupportedFunction
 *
 *    This is called when a major function is issued that isn't supported.
 *
 **********************************************************************/
NTSTATUS Example_UnSupportedFunction(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS NtStatus = STATUS_NOT_SUPPORTED;
    DbgPrint("Example_UnSupportedFunction Called \r\n");

    return NtStatus;
}


/**********************************************************************
 * 
 *  Example_IsStringTerminated
 *
 *    Simple function to determine a string is NULL terminated.
 *
 **** We could validate also the characters in the string are printable! ***
 *
 **********************************************************************/
BOOLEAN Example_IsStringTerminated(PCHAR pString, UINT uiLength)
{
    BOOLEAN bStringIsTerminated = FALSE;
    UINT uiIndex = 0;

    while(uiIndex < uiLength && bStringIsTerminated == FALSE)
    {
        if(pString[uiIndex] == '\0')
        {
            bStringIsTerminated = TRUE;
        }
        else
        {
           uiIndex++;
        }
    }

    return bStringIsTerminated;
}


