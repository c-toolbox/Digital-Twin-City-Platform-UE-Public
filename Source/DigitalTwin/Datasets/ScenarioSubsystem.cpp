#include "DigitalTwin/Datasets/ScenarioSubsystem.h"

#include "JsonObjectConverter.h"


namespace Utils
{
	TArray<FString> GetDirectories(IFileManager& FileManager, const FString& FilePath) {
		TArray<FString> FoundDirectories;
		FString SearchPath = FilePath / "*";
		FileManager.FindFiles(FoundDirectories, *SearchPath, false, true);
		return FoundDirectories;
	}


	void FromJSON(TArray<TSharedPtr<FJsonValue>> ScenarioSubSectionJson, TArray<FScenarioSubSection> &Out)
	{
		for(const auto& Sections : ScenarioSubSectionJson ) {
			FScenarioSubSection SubSection;
			auto SectionsAsObject = Sections->AsObject();
			SubSection.Title = SectionsAsObject->GetStringField("Title");
			auto FilenamesStr = SectionsAsObject->GetStringField("Filenames");
			TArray<FString> FilenameArray;
			FilenamesStr.ParseIntoArray(FilenameArray,TEXT(","));
			for (auto& FileName : FilenameArray) {
			  FileName.TrimStartAndEndInline();
			}
			SubSection.FilenameArray = FilenameArray;
			SubSection.Filenames = SectionsAsObject->GetStringField("Filenames");
			//TODO Remove this
			//SubSection.Comment = SectionsAsObject->GetStringField("Comment");
			//TODO Handle this in blueprint
			SubSection.Text1 = SectionsAsObject->GetStringField("Text1");
			SubSection.Text2 = SectionsAsObject->GetStringField("Text2");
			//TODO Handle this in blueprint
			SubSection.Legend1 = SectionsAsObject->GetStringField("Legend1");
			SubSection.Legend2 = SectionsAsObject->GetStringField("Legend2");
			
			//TODO Handle this in blueprint
			SubSection.Default = SectionsAsObject->GetBoolField("Default");
			SubSection.IsIdle = SectionsAsObject->GetBoolField("Idle");
			
			//TODO Handle this in blueprint
			auto PositionString = SectionsAsObject->GetStringField("Position");
			if(!PositionString.IsEmpty()) {
				TArray<FString> OutString;
				PositionString.ParseIntoArray(OutString,TEXT(","),true);
				
				if(OutString.Num() == 2) {
					SubSection.SectionType = ScenarioSubSectionType::PointLayer;
					double lat = FCString::Atod(*OutString[0]);
					double lon = FCString::Atod(*OutString[1]);
					SubSection.Lat_Lon = FVector2D(lat,lon);
				} else {
					SubSection.Lat_Lon = {};
				}
			} else {
				SubSection.SectionType = ScenarioSubSectionType::Layer;
				SubSection.Lat_Lon = {};
			}
			
			Out.Push(SubSection);
		}
	}
	
	void FromJSON(TArray<TSharedPtr<FJsonValue>> ScenarioSectionJson, TArray<FScenarioSection> &Out)
	{
		
		for(const auto& Sections : ScenarioSectionJson ) {
			FScenarioSection Section;
			auto SectionsAsObject = Sections->AsObject();
			auto Label1 =  SectionsAsObject->GetStringField("Label1");
			Section.Label1 = Label1;
			auto SubSection = SectionsAsObject->GetArrayField("SectionObject");
			TArray<FScenarioSubSection> SubSections;
			FromJSON(SubSection,SubSections);
			Section.SectionObject = SubSections;
			Section.SectionContent.Add(Label1,SubSections);
			Out.Push(Section);
		}
	}
	
	void FromJSON(TSharedPtr<FJsonObject> ScenarioJson, FScenario &Out)
	{
		auto SectionsJson = ScenarioJson->GetArrayField("Sections");
		auto Title = ScenarioJson->GetStringField("Title");
		TArray<FScenarioSection> Section;
		FromJSON(SectionsJson,Section);
		Out.Sections = Section;
		Out.Title = Title;
	}

}

void UScenarioSubsystem::ParseScenarios(FString DatasetRootDir, TArray<FString> ScenarioFiles, Language type)
{
	for (const FString& ScenarioFile : ScenarioFiles) {

		FString JsonString;
		FString FolderExtension;

		if (type == Language::Swedish)
			FolderExtension = "sv_SE/";

		if (type == Language::English)
			FolderExtension = "en_GB/";

		if (FFileHelper::LoadFileToString(JsonString, *(DatasetRootDir / FolderExtension + ScenarioFile))) {

			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
			TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

			FScenario Scenario;

			if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid()) {
				Utils::FromJSON(JsonObject, Scenario);

				//TODO Change to other name
				//Scenario.Title = ScenarioFile;

				if (type == Language::Swedish)
					SV_Scenarios_.Push(Scenario);

				if (type == Language::English)
					EN_Scenarios_.Push(Scenario);
			}
		}
	}
}

void UScenarioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FString ScenarioRootDir = FPaths::ProjectDir() + TEXT("Scenarios");
	if (!FPaths::DirectoryExists(ScenarioRootDir)) {
	  UE_LOG(LogTemp, Warning, TEXT("Could not find Scenario root dir: %s, creating it..."), *ScenarioRootDir);
	  IFileManager::Get().MakeDirectory(*ScenarioRootDir);
	}

	UWorld* World = GetWorld();
	if (World && World->WorldType == EWorldType::Game || World->WorldType == EWorldType::GamePreview || World->WorldType == EWorldType::PIE) {
		IFileManager& FileManager = IFileManager::Get();
		TArray<FString> CatalogDirectories = Utils::GetDirectories(FileManager, ScenarioRootDir);

		//Find all directories in the Catalog and assume they are RasterDatasets:
		//TODO This is a hack !
		// Needs to be refactored !
		{
			TArray<FString> ScenarioFiles;
			FString SearchString = "sv_SE/*.json";
			FileManager.FindFiles(ScenarioFiles, *(ScenarioRootDir / SearchString), true, false);
			ParseScenarios(ScenarioRootDir, ScenarioFiles,Language::Swedish);
		}
		{
			TArray<FString> ScenarioFiles;
			FString SearchString = "en_GB/*.json";
			FileManager.FindFiles(ScenarioFiles, *(ScenarioRootDir / SearchString), true, false);
			ParseScenarios(ScenarioRootDir, ScenarioFiles,Language::English);
		}		
	}
}

void UScenarioSubsystem::Deinitialize()
{
	Super::Deinitialize();

}

TArray<FScenario> UScenarioSubsystem::GetScenarios(Language Lang)
{
	switch (Lang) {
	case Language::English: {
		return EN_Scenarios_;
		break;
	}
	case Language::Swedish: {
		return SV_Scenarios_;
		break;
	}
	}
	return {};
}

TArray<FString> UScenarioSubsystem::GetSectionsForScenarios(FScenarioSection& Section)
{
	TArray<FString> Keys;
	Section.SectionContent.GetKeys(Keys);
	return Keys; 
}

TArray<FString> UScenarioSubsystem::GetSubSectionsForScenarios(FScenarioSection& SubSection)
{
	TArray<FString> Keys;
	SubSection.SectionContent.GetKeys(Keys);
	return Keys; 
}