// --------------------------------------------------------------------------------
// Copyright 2021, Linköping University and Universeum AB
//
// Authors:
//   Patric Ljung, patric.ljung@liu.se
//
// Package: AppDesign
// --------------------------------------------------------------------------------

#include "AppDesignSubsystem.h"

DEFINE_LOG_CATEGORY(LogAppDesign);

UAppDesignSubsystem::UAppDesignSubsystem()
{
    UE_LOG(LogAppDesign, Log, TEXT("AppDesign Subsystem Constructor"));
}

UAppDesignSubsystem::~UAppDesignSubsystem()
{
    UE_LOG(LogAppDesign, Log, TEXT("AppDesign Subsystem Destructor"));
}

void UAppDesignSubsystem::Initialize(FSubsystemCollectionBase & Collection)
{
    UE_LOG(LogAppDesign, Log, TEXT("AppDesign Subsystem initialized"));

    Settings = GetDefault<UAppDesignSettings>();
    DefaultFont = Settings->DefaultFont;

    InitializeFonts();
}

void UAppDesignSubsystem::Deinitialize()
{
    if (AppFontsTable)
    {
        AppFontsTable->OnDataTableChanged().Remove(FontsTableDelegateHandle);
        AppFontsTable = 0;
    }

    UE_LOG(LogAppDesign, Log, TEXT("AppDesign Subsystem deinitialized"));
}

bool UAppDesignSubsystem::FindFont(FName Key, FSlateFontInfo & Font) const
{
    if (!AppFontsTable)
    {
        UE_LOG(LogAppDesign, Warning, TEXT("AppDesign: Missing AppFontsTable, check configuration"));

        return false;
    }

    uint8 * const * RowPtr = AppFontsTable->GetRowMap().Find(Key);

    if (RowPtr)
    {
        FFontEntry * Entry = reinterpret_cast<FFontEntry *>(*RowPtr);
        if (Entry)
        {
            Font = Entry->Font;
            return true;
        }
    }

    UE_LOG(LogAppDesign, Warning, TEXT("AppDesign: Missing font for Key '%s'"),
           *Key.ToString());

    Font = DefaultFont;
    return false;
}


/*
 --------------------------------------------------------------------------------
   INTERNAL FUNCTIONS / PROTECTED / PRIVATE
 --------------------------------------------------------------------------------
*/

template <typename Type>
bool VerifyStructType(const FName & StructName)
{
    return StructName.IsEqual(Type::StaticStruct()->GetFName());
}

bool UAppDesignSubsystem::InitializeFonts()
{
    UE_LOG(LogAppDesign, Log, TEXT("AppDesign Subsystem initializing fonts"));

    // Try load Object and verify it's a Data Table
    FString FontsTablePath(Settings->AppFontsTable.ToString());
    UObject * FontsTableObject = Settings->AppFontsTable.TryLoad();

    if (FontsTableObject)
    {
        AppFontsTable = Cast<UDataTable>(FontsTableObject);

        if (!IsValid(AppFontsTable))
        {
            UE_LOG(LogAppDesign, Error, TEXT("Loaded object for AppFontsTable is not a DataTable, Path '%s'"),
                   *FontsTablePath);
            AppFontsTable = 0;
            return false;
        }
    }
    else
    {
        UE_LOG(LogAppDesign, Error, TEXT("Failed to load AppFontsTable '%s'"),
               *FontsTablePath);
        AppFontsTable = 0;
        return false;
    }

#if WITH_EDITOR
    // Verify it has the correct Row Struct
    if (!VerifyStructType<FFontEntry>(AppFontsTable->GetRowStructName()))
    {
        UE_LOG(LogAppDesign, Error,
               TEXT("Row struct '%s' of AppFontsTable is incorrect, expecting '%s'"),
               *AppFontsTable->GetRowStructName().ToString(),
               *FFontEntry::StaticStruct()->GetName());
        AppFontsTable = 0;
        return false;
    }
#endif

    // Bind event for DataTable updated
    auto & Dispatcher = AppFontsTable->OnDataTableChanged();
    FontsTableDelegateHandle = Dispatcher.AddUObject(this, &UAppDesignSubsystem::OnFontsTableChanged);

    return true;
}

void UAppDesignSubsystem::OnFontsTableChanged()
{
    if (IsPendingKillOrUnreachable() || HasAnyFlags(RF_BeginDestroyed))
    {
        // Receiving this while being torn down, don't propagate.
        return;
    }

    OnFontsChanged.Broadcast();
}
