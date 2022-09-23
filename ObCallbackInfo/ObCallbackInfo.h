#pragma once

#include <ntifs.h>

typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemModuleInformation = 11,
	SystemBasicInformation = 0,
	SystemPerformanceInformation = 2,
	SystemTimeOfDayInformation = 3,
	SystemProcessInformation = 5,
	SystemProcessorPerformanceInformation = 8,
	SystemInterruptInformation = 23,
	SystemExceptionInformation = 33,
	SystemRegistryQuotaInformation = 37,
	SystemLookasideInformation = 45
} SYSTEM_INFORMATION_CLASS;

typedef struct _OBJECT_TYPE {
	LIST_ENTRY TypeList;
	UNICODE_STRING Name;
	VOID* DefaultObject;
	UCHAR Index;
	unsigned __int32 TotalNumberOfObjects;
	unsigned __int32 TotalNumberOfHandles;
	unsigned __int32 HighWaterNumberOfObjects;
	unsigned __int32 HighWaterNumberOfHandles;
	char TypeInfo[0x78];
	EX_PUSH_LOCK TypeLock;
	unsigned __int32 Key;
	LIST_ENTRY CallbackList; // A linked list of CALLBACK_ENTRY_ITEM
}OBJECT_TYPE, * POBJECT_TYPE;

typedef struct _CALLBACK_ENTRY_ITEM {
	LIST_ENTRY EntryItemList;
	OB_OPERATION Operations;
	struct _CALLBACK_ENTRY* CallbackEntry;
	POBJECT_TYPE ObjectType;
	POB_PRE_OPERATION_CALLBACK PreOperation;
	POB_POST_OPERATION_CALLBACK PostOperation;
	__int64 unk;
}CALLBACK_ENTRY_ITEM, * PCALLBACK_ENTRY_ITEM;

typedef struct _CALLBACK_ENTRY {
	__int16 Version;
	char buffer1[6];
	POB_OPERATION_REGISTRATION RegistrationContext;
	__int16 AltitudeLength1;
	__int16 AltitudeLength2;
	char buffer2[4];
	WCHAR* AltitudeString;
	CALLBACK_ENTRY_ITEM Items;
}CALLBACK_ENTRY, * PCALLBACK_ENTRY;

typedef struct _SYSTEM_MODULE_ENTRY
{
	HANDLE Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR FullPathName[256];
} SYSTEM_MODULE_ENTRY, * PSYSTEM_MODULE_ENTRY;

typedef struct _SYSTEM_MODULE_INFORMATION
{
	ULONG Count;
	SYSTEM_MODULE_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

typedef NTSTATUS(*pZwQuerySystemInformation)(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	ULONG* ReturnLength);
pZwQuerySystemInformation _ZwQuerySystemInformation;

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath);
NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject);
void registeredObjectCallbacksInfo(OBJECT_TYPE ob);

