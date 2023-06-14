// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LootTiers.generated.h"

USTRUCT(BlueprintType)
struct FLootItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LootTiers|Spawning")
	TSubclassOf<AActor> LootClass;

	UPROPERTY(EditAnywhere, Category = "LootTiers|Spawning")
	float ChanceToSpawn = 0.f;

	UPROPERTY(EditAnywhere, Category = "LootTiers|Spawning")
	int32 MaxNumberToSpawn = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LootTiers|Spawning")
	bool bLootLevelOverride = true;
};

/**
 * 
 */
UCLASS()
class AURA_API ULootTiers : public UDataAsset
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	TArray<FLootItem> GetLootItems();
	
	UPROPERTY(EditDefaultsOnly, Category = "LootTiers|Spawning")
	TArray<FLootItem> LootItems;
};
