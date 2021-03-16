#pragma once

#include "BaseGameInstance.h"
#include "BaseWorldSettings.h"
#include "Game/FPSGameMode.h"
#include "Player/BasePlayerState.h"
#include "Items/Item.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StaticFunctionLibrary.generated.h"

// Library for static functions, partially exposed to blueprints
UCLASS()
class FPSTEMPLATE_API UStaticFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	

	// Get item class from item class name
	UFUNCTION(BlueprintCallable, Category = StaticFunctionLibrary, Meta = (WorldContext = WorldContextObject))
		static TSubclassOf<AItem> GetItemClass(const UObject * WorldContextObject, const FName & ItemName);

	// Get item class default object for item class name
	UFUNCTION(BlueprintPure, Category = StaticFunctionLibrary)
		static AItem * GetItemCDO(TSubclassOf<AItem> ItemClass);

	

	// Convert the name of a class to a text without prefixes and underscores
	UFUNCTION(BlueprintPure, Category = StaticFunctionLibrary)
		static FText ClassNameToDisplayName(FString ClassName);

	// Return color to paint a player in
	UFUNCTION(BlueprintPure, Category = StaticFunctionLibrary)
		static FLinearColor GetPlayerColor(ABasePlayerState * OwnerPS, ABasePlayerState * OtherPS);

	// Return class default object of game mode class
	UFUNCTION(BlueprintPure, Category = StaticFunctionLibrary, Meta = (WorldContext = WorldContextObject))
		static AFPSGameMode * GetGameModeCDO(UObject * WorldContextObject);

	// Return a valid save game object, creates a new one if neccessary
	UFUNCTION(BlueprintCallable, Category = StaticFunctionLibrary)
		static USaveGame * LoadOrCreateSaveGame(const FString & SlotName, TSubclassOf<USaveGame> SaveGameClass, int32 UserIndex);

	UFUNCTION(BlueprintPure, Category = StaticFunctionLibrary, Meta = (WorldContext = WorldContextObject))
		static ABaseWorldSettings * GetWorldSettings(UObject * WorldContextObject);

	UFUNCTION(BlueprintPure, Category = StaticFunctionLibrary, Meta = (WorldContext = WorldContextObject))
		static UBaseGameInstance * GetBaseGameInstance(const UObject * WorldContextObject);

	// Return the class default object of any class
	UFUNCTION(BlueprintCallable, Category = StaticFunctionLibrary)
		static UObject * GetDefaultObject(TSubclassOf<UObject> InClass);

	// Console
	UFUNCTION(BlueprintCallable, Category = StaticFunctionLibrary)
		static float GetConsoleVariableFloat(const FString & VarName);

	UFUNCTION(BlueprintCallable, Category = StaticFunctionLibrary)
		static int32 GetConsoleVariableInt(const FString & VarName);

	UFUNCTION(BlueprintCallable, Category = StaticFunctionLibrary)
		static FString GetConsoleVariableString(const FString & VarName);

	// Game user settings
	UFUNCTION(BlueprintPure, Category = StaticFunctionLibrary)
		static void GetScreenResolution(int32 & OutX, int32 & OutY);

	UFUNCTION(BlueprintCallable, Category = StaticFunctionLibrary)
		static void SetScreenResolution(int32 X, int32 Y);

	UFUNCTION(BlueprintPure, Category = StaticFunctionLibrary)
		static uint8 GetFullscreenMode();

	UFUNCTION(BlueprintCallable, Category = StaticFunctionLibrary)
		static void SetFullscreenMode(uint8 Mode);

	// Config
	UFUNCTION(BlueprintPure, Category = StaticFunctionLibrary)
		static FString GetPlayerName();

	UFUNCTION(BlueprintCallable, Category = StaticFunctionLibrary)
		static void SetPlayerName(APlayerController * PC, const FString & NewName);
	
};
