#include "FPSTemplate.h"
#include "LoadoutBuilderComponent.h"
#include "Items/Item.h"
#include "Items/Firearm.h"
#include "StaticFunctionLibrary.h"
#include "Projectiles/BaseProjectile.h"

#define GET_GAME_INSTANCE() UBaseGameInstance * GameInstance = UStaticFunctionLibrary::GetBaseGameInstance(this); if (!GameInstance) return false;

ULoadoutBuilderComponent::ULoadoutBuilderComponent()
{
	
}

void ULoadoutBuilderComponent::ClearLoadout()
{
	Loadout = FLoadoutStruct();
}

void ULoadoutBuilderComponent::SetLoadoutChecked(const FLoadoutStruct & NewLoadout)
{
	ClearLoadout();
	uint8 OldSlot = CurrentItemSlot;
	CurrentItemSlot = 0;


	for (const FLoadoutSlotStruct & Slot : NewLoadout.ItemSlots)
	{
		EquipItem(Slot.ItemClassName);
		

		CurrentItemSlot++;
	}

	CurrentItemSlot = OldSlot;
}

FLoadoutSlotStruct & ULoadoutBuilderComponent::GetCurrentSlot()
{
	return Loadout.ItemSlots[CurrentItemSlot];
}

bool ULoadoutBuilderComponent::IsItemFromCurrentSlot(const FName & ItemName) const
{
	GET_GAME_INSTANCE()

	const AItem * ItemCDO = GameInstance->GetItem(ItemName).GetDefaultObject();
	if (ItemCDO)
	{
		// Check if item is in correct slot in loadout
		if (ItemCDO->ItemSlot == EItemSlotEnum::Grenade)
		{
			if (CurrentItemSlot < uint8(EItemSlotEnum::Grenade) - 1)
			{
				return false;
			}
		}
		else if (uint8(ItemCDO->ItemSlot) - 1 != CurrentItemSlot)
		{
			return false;
		}
	}
	else if (CurrentItemSlot == uint8(EItemSlotEnum::Knife) - 1)
	{
		
		return false;
	}

	return true;
}

bool ULoadoutBuilderComponent::CanEquipItem(const FName & ItemName) const
{
	return IsItemFromCurrentSlot(ItemName);
}

void ULoadoutBuilderComponent::EquipItem(const FName & ItemName)
{
	if (CanEquipItem(ItemName))
	{
		Loadout.ItemSlots[CurrentItemSlot] = FLoadoutSlotStruct(ItemName);
	}
}
