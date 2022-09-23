#include "ObCallbackInfo.h"

#pragma warning(disable : 4100)

UNICODE_STRING sZwQuerySystemInformation = RTL_CONSTANT_STRING(L"ZwQuerySystemInformation");

void registeredObjectCallbacksInfo(OBJECT_TYPE ob) // each OBJECT_TYPE contains a pointer to a doubly linked list of callbacks
{
	PSYSTEM_MODULE_INFORMATION pModInfo;
	ULONG bytes = 0;
	// Get current module info size and allocate memory for struct
	NTSTATUS status = _ZwQuerySystemInformation(SystemModuleInformation, 0, bytes, &bytes); 
	pModInfo = (PSYSTEM_MODULE_INFORMATION)ExAllocatePool2(POOL_FLAG_NON_PAGED, bytes, 'tag');
	if (pModInfo == 0)
	{
		DbgPrintEx(0, 0, "Failed to alloc mem");
		return;
	}
	// Fill struct
	status = _ZwQuerySystemInformation(0x0B, pModInfo, bytes, 0);
	if (status != STATUS_SUCCESS)
	{
		DbgPrintEx(0, 0, "Failed to query sys info : %x", status);
		ExFreePool(pModInfo);
		return;
	}

	PLIST_ENTRY pHead = ob.CallbackList.Flink;
	PLIST_ENTRY pEntry = pHead;
	
	// Iterate doubly linked list
	while (pEntry->Flink != pHead)
	{
		PCALLBACK_ENTRY_ITEM cItem = (PCALLBACK_ENTRY_ITEM)CONTAINING_RECORD(pEntry, CALLBACK_ENTRY_ITEM, EntryItemList);

		for (size_t i = 0; i < pModInfo->Count; i++)
		{
	
			// Determine which kernel module is responsible for each callback + extra callback info
			if ((ULONG64)cItem->PreOperation > (ULONG64)pModInfo->Module[i].ImageBase
				&& (ULONG64)cItem->PreOperation < ((ULONG64)pModInfo->Module[i].ImageBase + pModInfo->Module[i].ImageSize))
			{
				DbgPrintEx(0, 0, "PreOperation %p : PostOperation %p : Altitude %S : Module %s",
					cItem->PreOperation,
					cItem->PostOperation,
					cItem->CallbackEntry->AltitudeString,
					pModInfo->Module[i].FullPathName);
			}

		}

		pEntry = pEntry->Flink;
	}

	ExFreePool(pModInfo);
	return;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	DbgPrintEx(0, 0, "Driver has been loaded");
	pDriverObject->DriverUnload = UnloadDriver;

	_ZwQuerySystemInformation = (pZwQuerySystemInformation)MmGetSystemRoutineAddress(&sZwQuerySystemInformation);
	if (_ZwQuerySystemInformation == 0)
	{
		DbgPrintEx(0, 0, "Failed to get system routine address");
		return STATUS_SUCCESS;
	}

	OBJECT_TYPE procOb = **PsProcessType;
	OBJECT_TYPE threadOb = **PsThreadType;
	DbgPrintEx(0, 0, "----------------- Process Callbacks -----------------");
	registeredObjectCallbacksInfo(procOb);
	DbgPrintEx(0, 0, "----------------- Thread Callbacks -----------------");
	registeredObjectCallbacksInfo(threadOb);

	return STATUS_SUCCESS;
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	DbgPrintEx(0, 0, "Driver has been unloaded");
	return STATUS_SUCCESS;
}