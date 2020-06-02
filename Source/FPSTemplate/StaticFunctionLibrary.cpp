#include "FPSTemplate.h"
#include "StaticFunctionLibrary.h"
#include "Items/Firearm.h"

#define TeamColor FLinearColor(0.f, 1.f, 0.f, 1.f)
#define EnemyColor FLinearColor(1.f, 0.2f, 0.f, 1.f)

#define PlayerConfigSection TEXT("Player")


TSubclassOf<AItem> UStaticFunctionLibrary::GetItemClass(const UObject * WorldContextObject, const FName & ItemName)
{
	UBaseGameInstance * GameInstance = GetBaseGameInstance(WorldContextObject);
	if (GameInstance)
	{
		TArray<TSubclassOf<AItem>> Items;
		GameInstance->GetItems(Items);
		for (const TSubclassOf<AItem> & ItemClass : Items)
		{
			if (ItemClass->GetFName() == ItemName)
			{
				return ItemClass;
			}
		}
	}

	return NULL;
}

AItem * UStaticFunctionLibrary::GetItemCDO(TSubclassOf<AItem> ItemClass)
{
	return ItemClass.GetDefaultObject();
}



FText UStaticFunctionLibrary::ClassNameToDisplayName(FString ClassName)
{
	ClassName.RemoveFromStart(TEXT("BP_"));
	ClassName.RemoveFromEnd(TEXT("_C"));
	ClassName.Replace(TEXT("_"), TEXT(" "));

	return FText::FromString(ClassName);
}

FLinearColor UStaticFunctionLibrary::GetPlayerColor(ABasePlayerState * OwnerPS, ABasePlayerState * OtherPS)
{
	if (OwnerPS && OwnerPS != OtherPS)
	{
		return OwnerPS->IsEnemyFor(OtherPS) ? EnemyColor : TeamColor;
	}

	return FLinearColor::White;
}

AFPSGameMode * UStaticFunctionLibrary::GetGameModeCDO(UObject * WorldContextObject)
{
	AGameState * GameState = Cast<AGameState>(UGameplayStatics::GetGameState(WorldContextObject));
	if (GameState)
	{
		const AFPSGameMode * GameMode = Cast<AFPSGameMode>(GameState->GetDefaultGameMode());
		if (GameMode)
		{
			return (AFPSGameMode*)GameMode;
		}
	}

	return NULL;
}

USaveGame * UStaticFunctionLibrary::LoadOrCreateSaveGame(const FString & SlotName, TSubclassOf<USaveGame> SaveGameClass, int32 UserIndex)
{
	USaveGame * SaveGame = UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex);
	if (SaveGame)
	{
		return SaveGame;
	}

	return UGameplayStatics::CreateSaveGameObject(SaveGameClass);
}

ABaseWorldSettings * UStaticFunctionLibrary::GetWorldSettings(UObject * WorldContextObject)
{
	UWorld * World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		return Cast<ABaseWorldSettings>(World->GetWorldSettings());
	}

	return NULL;
}

UBaseGameInstance * UStaticFunctionLibrary::GetBaseGameInstance(const UObject * WorldContextObject)
{
	UWorld * World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		return Cast<UBaseGameInstance>(World->GetGameInstance());
	}

	return NULL;
}

UObject * UStaticFunctionLibrary::GetDefaultObject(TSubclassOf<UObject> InClass)
{
	return InClass.GetDefaultObject();
}

float UStaticFunctionLibrary::GetConsoleVariableFloat(const FString & VarName)
{
	const auto CVar = IConsoleManager::Get().FindConsoleVariable(*VarName);
	if (CVar)
	{
		return CVar->GetFloat();
	}

	return 0.f;
}

int32 UStaticFunctionLibrary::GetConsoleVariableInt(const FString & VarName)
{
	const auto CVar = IConsoleManager::Get().FindConsoleVariable(*VarName);
	if (CVar)
	{
		return CVar->GetInt();
	}

	return 0;
}

FString UStaticFunctionLibrary::GetConsoleVariableString(const FString & VarName)
{
	const auto CVar = IConsoleManager::Get().FindConsoleVariable(*VarName);
	if (CVar)
	{
		return CVar->GetString();
	}

	return FString();
}

void UStaticFunctionLibrary::GetScreenResolution(int32 & OutX, int32 & OutY)
{
	UGameUserSettings * UserSettings = GEngine->GetGameUserSettings();
	if (UserSettings)
	{
		const FIntPoint Point = UserSettings->GetScreenResolution();
		OutX = Point.X;
		OutY = Point.Y;
	}
}

void UStaticFunctionLibrary::SetScreenResolution(int32 X, int32 Y)
{
	UGameUserSettings * UserSettings = GEngine->GetGameUserSettings();
	if (UserSettings)
	{
		UserSettings->SetScreenResolution(FIntPoint(X, Y));
		UserSettings->RequestResolutionChange(X, Y, UserSettings->GetFullscreenMode());
		UserSettings->SaveConfig();
	}
}

uint8 UStaticFunctionLibrary::GetFullscreenMode()
{
	UGameUserSettings * UserSettings = GEngine->GetGameUserSettings();
	if (UserSettings)
	{
		return (uint8)UserSettings->GetFullscreenMode();
	}

	return EWindowMode::Fullscreen;
}

void UStaticFunctionLibrary::SetFullscreenMode(uint8 Mode)
{
	UGameUserSettings * UserSettings = GEngine->GetGameUserSettings();
	if (UserSettings)
	{
		const EWindowMode::Type WindowMode = (EWindowMode::Type)Mode;
		const FIntPoint Point = UserSettings->GetScreenResolution();
		UserSettings->SetFullscreenMode(WindowMode);
		UserSettings->RequestResolutionChange(Point.X, Point.Y, WindowMode);
		UserSettings->SaveConfig();
	}
}

FString UStaticFunctionLibrary::GetPlayerName()
{
	if (GConfig)
	{
		FString PlayerName;
		GConfig->GetString(PlayerConfigSection, TEXT("PlayerName"), PlayerName, GGameIni);
		return PlayerName;
	}

	return "NoName";
}

void UStaticFunctionLibrary::SetPlayerName(APlayerController * PC, const FString & NewName)
{
	if (!NewName.IsEmpty() && GConfig)
	{
		GConfig->SetString(PlayerConfigSection, TEXT("PlayerName"), *NewName, GGameIni);
		GConfig->Flush(false, GGameIni);
		PC->SetName(NewName);
	}
}