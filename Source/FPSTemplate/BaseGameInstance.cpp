#include "FPSTemplate.h"
#include "BaseGameInstance.h"

UBaseGameInstance::UBaseGameInstance()
{
	LevelLibrary = UObjectLibrary::CreateLibrary(UWorld::StaticClass(), false, GIsEditor);
	LevelLibrary->AddToRoot();
	LevelLibrary->LoadAssetDataFromPath(TEXT("/Game/FPSTemplate/Maps"));

	ItemLibrary = UObjectLibrary::CreateLibrary(AItem::StaticClass(), true, GIsEditor);
	ItemLibrary->AddToRoot();
	ItemLibrary->LoadBlueprintsFromPath(TEXT("/Game/FPSTemplate/Items"));



	ProjectileLibrary = UObjectLibrary::CreateLibrary(ABaseProjectile::StaticClass(), true, GIsEditor);
	ProjectileLibrary->AddToRoot();
	ProjectileLibrary->LoadBlueprintsFromPath(TEXT("/Game/FPSTemplate/Projectiles"));

	NumTickets = 50;
}

void UBaseGameInstance::GetLevels(TArray<FName> & OutLevels)
{
	TArray<FAssetData> AssetDatas;
	LevelLibrary->GetAssetDataList(AssetDatas);

	for (int32 i = 0; i < AssetDatas.Num(); ++i)
	{
		FAssetData & AssetData = AssetDatas[i];
		OutLevels.Add(AssetData.AssetName);
	}

	//GetAssetsFromLibrary<UWorld>(LevelLibrary, OutLevels);
}

void UBaseGameInstance::GetItems(TArray<TSubclassOf<AItem>> & OutItems)
{
	GetBlueprintClassesFromLibrary<AItem>(ItemLibrary, OutItems);

	for (int32 i = 0; i < OutItems.Num(); ++i)
	{
		AItem * ItemCDO = Cast<AItem>(OutItems[i]->GetDefaultObject());
		if (ItemCDO->ActorHasTag(NAME_HideItem))
			OutItems.RemoveAt(i--);
	}
}

TSubclassOf<AItem> UBaseGameInstance::GetItem(const FName & ItemName)
{
	return GetBlueprintClassFromLibrary<AItem>(ItemLibrary, ItemName);
}


void UBaseGameInstance::GetProjectiles(TArray<TSubclassOf<ABaseProjectile>> & OutProjectiles)
{
	GetBlueprintClassesFromLibrary<ABaseProjectile>(ProjectileLibrary, OutProjectiles);
}

TSubclassOf<ABaseProjectile> UBaseGameInstance::GetProjectile(const FName & ProjectileName)
{
	return GetBlueprintClassFromLibrary<ABaseProjectile>(ProjectileLibrary, ProjectileName);
}

void UBaseGameInstance::GetItemPatternMaterials(TSubclassOf<AItem> ItemClass, TArray<FName> & OutMaterials)
{
	TArray<FAssetData> AssetDatas;
	GetItemPatternMaterialDatas(ItemClass, AssetDatas);

	for (int32 i = 0; i < AssetDatas.Num(); ++i)
	{
		FAssetData & AssetData = AssetDatas[i];
		OutMaterials.Add(AssetData.AssetName);
	}
}

void UBaseGameInstance::GetItemPatternMaterialDatas(TSubclassOf<AItem> ItemClass, TArray<FAssetData>& OutDatas)
{
	AItem * ItemCDO = ItemClass.GetDefaultObject();
	if (!ItemCDO) return;

	UMaterialInterface * DefaultMaterial = ItemCDO->Mesh->GetMaterial(ItemCDO->PatternMaterialSlot);
	if (!DefaultMaterial) return;

	// Create Library
	UObjectLibrary * MaterialLibrary = UObjectLibrary::CreateLibrary(UMaterialInterface::StaticClass(), false, GIsEditor);

	TArray<FString> AssetDirectories;

	FAssetData ClassAssetData(*ItemClass);
	AssetDirectories.Add(ClassAssetData.PackagePath.ToString() + TEXT("/Materials"));

	FAssetData MaterialAssetData(DefaultMaterial);
	AssetDirectories.Add(MaterialAssetData.PackagePath.ToString());

	MaterialLibrary->LoadAssetDataFromPaths(AssetDirectories);

	TArray<FAssetData> AssetDatas;
	MaterialLibrary->GetAssetDataList(AssetDatas);

	for (int32 i = 0; i < AssetDatas.Num(); ++i)
	{
		FAssetData & AssetData = AssetDatas[i];
		if (AssetData.AssetName != MaterialAssetData.AssetName)
		{
			OutDatas.Add(AssetData);
		}
	}
}

UMaterialInterface * UBaseGameInstance::GetItemPatternMaterial(TSubclassOf<AItem> ItemClass, const FName & MaterialName)
{
	TArray<FAssetData> AssetDatas;
	GetItemPatternMaterialDatas(ItemClass, AssetDatas);

	for (int32 i = 0; i < AssetDatas.Num(); ++i)
	{
		FAssetData & AssetData = AssetDatas[i];
		if (AssetData.AssetName == MaterialName)
		{
			return Cast<UMaterialInterface>(StreamableManager.LoadSynchronous(AssetData.ToStringReference()));
		}
	}

	return NULL;
}