#include <minwindef.h>
#include <subauth.h>
#include <winnt.h>

// unfortunately, some structures (like InLoadOrderLinks) are not exposed by
// standard windows headers. I have to redefine the following types
typedef struct _PEB_LDR_DATA {
  ULONG Length;
  BOOLEAN Initialized;
  HANDLE SsHandle;
  LIST_ENTRY InLoadOrderModuleList;
  LIST_ENTRY InMemoryOrderModuleList;
  LIST_ENTRY InInitializationOrderModuleList;
  PVOID EntryInProgress;
  BOOLEAN ShutdownInProgress;
  HANDLE ShutdownThreadId;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _API_SET_NAMESPACE {
  ULONG Version;
  ULONG Size;
  ULONG Flags;
  ULONG Count;
  ULONG EntryOffset;
  ULONG HashOffset;
  ULONG HashFactor;
} API_SET_NAMESPACE, *PAPI_SET_NAMESPACE;

typedef struct _RTL_USER_PROCESS_PARAMETERS {
  BYTE Reserved1[16];
  PVOID Reserved2[10];
  UNICODE_STRING ImagePathName;
  UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

typedef struct _PEB {
  BOOLEAN InheritedAddressSpace;
  BOOLEAN ReadImageFileExecOptions;
  BOOLEAN BeingDebugged;
  union {
    BOOLEAN BitField;
    struct {
      BOOLEAN ImageUsesLargePages : 1;
      BOOLEAN IsProtectedProcess : 1;
      BOOLEAN IsLegacyProcess : 1;
      BOOLEAN IsImageDynamicallyRelocated : 1;
      BOOLEAN SkipPatchingUser32Forwarders : 1;
      BOOLEAN SpareBits : 3;
    };
  };
  HANDLE Mutant;

  PVOID ImageBaseAddress;
  PPEB_LDR_DATA Ldr;
  PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
  PVOID SubSystemData;
  PVOID ProcessHeap;
  PRTL_CRITICAL_SECTION FastPebLock;
  PVOID IFEOKey;
  PSLIST_HEADER AtlThunkSListPtr;
  union {
    ULONG CrossProcessFlags;
    struct {
      ULONG ProcessInJob : 1;
      ULONG ProcessInitializing : 1;
      ULONG ProcessUsingVEH : 1;
      ULONG ProcessUsingVCH : 1;
      ULONG ProcessUsingFTH : 1;
      ULONG ProcessPreviouslyThrottled : 1;
      ULONG ProcessCurrentlyThrottled : 1;
      ULONG ProcessImagesHotPatched : 1;
      ULONG ReservedBits0 : 24;
    };
  };
  union {
    PVOID KernelCallbackTable;
    PVOID UserSharedInfoPtr;
  };
  ULONG SystemReserved;
  ULONG AtlThunkSListPtr32;
  PAPI_SET_NAMESPACE ApiSetMap;
  ULONG TlsExpansionCounter;
  PVOID TlsBitmap;
  ULONG TlsBitmapBits[2];
  PVOID ReadOnlySharedMemoryBase;
  PVOID SharedData;
  PVOID *ReadOnlyStaticServerData;
  PVOID AnsiCodePageData;
  PVOID OemCodePageData;
  PVOID UnicodeCaseTableData;
  ULONG NumberOfProcessors;
  ULONG NtGlobalFlag;
  ULARGE_INTEGER CriticalSectionTimeout;
  SIZE_T HeapSegmentReserve;
  SIZE_T HeapSegmentCommit;
  SIZE_T HeapDeCommitTotalFreeThreshold;
  SIZE_T HeapDeCommitFreeBlockThreshold;
  ULONG NumberOfHeaps;
  ULONG MaximumNumberOfHeaps;
  PVOID *ProcessHeaps;
  PVOID GdiSharedHandleTable;
  PVOID ProcessStarterHelper;
  ULONG GdiDCAttributeList;
  PRTL_CRITICAL_SECTION LoaderLock;
  ULONG OSMajorVersion;
  ULONG OSMinorVersion;
  USHORT OSBuildNumber;
} PEB, *PPEB;

typedef struct _LDR_DATA_TABLE_ENTRY {
  LIST_ENTRY InLoadOrderLinks;
  LIST_ENTRY InMemoryOrderLinks;
  union {
    LIST_ENTRY InInitializationOrderLinks;
    LIST_ENTRY InProgressLinks;
  };
  PVOID DllBase;
  PVOID EntryPoint;
  ULONG SizeOfImage;
  UNICODE_STRING FullDllName;
  UNICODE_STRING BaseDllName;
  ULONG Flags;
  WORD LoadCount;
  WORD TlsIndex;
  union {
    LIST_ENTRY HashLinks;
    struct {
      PVOID SectionPointer;
      ULONG CheckSum;
    };
  };
  union {
    ULONG TimeDateStamp;
    PVOID LoadedImports;
  };
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _TEB {
  PVOID Reserved1[12];
  PPEB ProcessEnvironmentBlock;
  PVOID Reserved2[399];
  BYTE Reserved3[1952];
  PVOID TlsSlots[64];
  BYTE Reserved4[8];
  PVOID Reserved5[26];
  PVOID ReservedForOle;
  PVOID Reserved6[4];
  PVOID TlsExpansionSlots;
} TEB, *PTEB;
