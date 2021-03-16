#pragma once

#include "Items/Item.h"
#include "Components/ActorComponent.h"
#include "LoadoutBuilderComponent.generated.h"

#define NumItemSlots 7



// Struct used to store the loadout of a specific loadout slot
USTRUCT(BlueprintType)
struct FLoadoutSlotStruct
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = Loadout)
		FName ItemClassName;

	UPROPERTY(BlueprintReadOnly, Category = Loadout)
		FName PatternMaterialName;

	UPROPERTY(BlueprintReadOnly, Category = Loadout)
		FName ProjectileClassName;

	UPROPERTY(BlueprintReadOnly, Category = Loadout)
		FName SightClassName;

	UPROPERTY(BlueprintReadOnly, Category = Loadout)
		bool bHasSuppressor = false;

	UPROPERTY(BlueprintReadOnly, Category = Loadout)
		TArray<FName> AttachmentComponentNames;

	FLoadoutSlotStruct() {}

	FLoadoutSlotStruct(const FName & InItemClassName)
	{
		ItemClassName = InItemClassName;
	}
};

// Struct used to store a loadout
USTRUCT(BlueprintType)
struct FLoadoutStruct
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Loadout)
		TArray<FLoadoutSlotStruct> ItemSlots;

	//UPROPERTY(BlueprintReadOnly, Category = Loadout)
		//EPerkEnum Perk;

	FLoadoutStruct()
	{
		ItemSlots.SetNum(NumItemSlots);
		// By default every loadout has a knife
		ItemSlots[uint8(EItemSlotEnum::Secondary) - 1].ItemClassName = FName(TEXT("BP_Battlehawk_C"));
		ItemSlots[uint8(EItemSlotEnum::Knife) - 1].ItemClassName = FName(TEXT("BP_M9_C"));
	}
};

// Component used to safely construct loadouts
UCLASS(ClassGroup = (Custom))
class FPSTEMPLATE_API ULoadoutBuilderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = Loadout)
		FLoadoutStruct Loadout;

	UPROPERTY(BlueprintReadWrite, Category = Loadout)
		uint8 CurrentItemSlot;

	ULoadoutBuilderComponent();

	UFUNCTION(BlueprintCallable, Category = Loadout)
		void ClearLoadout();

	void SetLoadoutChecked(const FLoadoutStruct & NewLoadout);

	UFUNCTION(BlueprintPure, Category = Loadout)
		FLoadoutSlotStruct & GetCurrentSlot();

	// Item
	// Return whether item can be equipped in CurrentItemSlot
	UFUNCTION(BlueprintPure, Category = Loadout)
		virtual bool IsItemFromCurrentSlot(const FName & ItemName) const;

	UFUNCTION(BlueprintPure, Category = Loadout)
		virtual bool CanEquipItem(const FName & ItemName) const;

	UFUNCTION(BlueprintCallable, Category = Loadout)
		void EquipItem(const FName & ItemName);


	
	
};
