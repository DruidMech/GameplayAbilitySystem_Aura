// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

class UGameplayAbility;
UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	EnterName,
	Taken
};

USTRUCT()
struct FSavedActor
{
	GENERATED_BODY()

	UPROPERTY()
	FName ActorName = FName();

	UPROPERTY()
	FTransform Transform = FTransform();

	// Serialized variables from the Actor - only those marked with SaveGame specifier
	UPROPERTY()
	TArray<uint8> Bytes;
};

inline bool operator==(const FSavedActor& Left, const FSavedActor& Right)
{
	return Left.ActorName == Right.ActorName;
}

USTRUCT()
struct FSavedMap
{
	GENERATED_BODY()

	UPROPERTY()
	FString MapAssetName = FString();

	UPROPERTY()
	TArray<FSavedActor> SavedActors;
};

USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ClassDefaults")
	TSubclassOf<UGameplayAbility> GameplayAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityStatus = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilitySlot = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityType = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 AbilityLevel = 1;
};

inline bool operator==(const FSavedAbility& Left, const FSavedAbility& Right)
{
	return Left.AbilityTag.MatchesTagExact(Right.AbilityTag);
}

/**
 * 
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	
	UPROPERTY()
	FString SlotName = FString();

	UPROPERTY()
	int32 SlotIndex = 0;

	UPROPERTY()
	FString PlayerName = FString("Default Name");

	UPROPERTY()
	FString MapName = FString("Default Map Name");

	UPROPERTY()
	FString MapAssetName = FString("Default Map Asset Name");

	UPROPERTY()
	FName PlayerStartTag;

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = Vacant;

	UPROPERTY()
	bool bFirstTimeLoadIn = true;

	/* Player */

	UPROPERTY()
	int32 PlayerLevel = 1;

	UPROPERTY()
	int32 XP = 0;

	UPROPERTY()
	int32 SpellPoints = 0;

	UPROPERTY()
	int32 AttributePoints = 0;

	UPROPERTY()
	float Strength = 0;

	UPROPERTY()
	float Intelligence = 0;

	UPROPERTY()
	float Resilience = 0;

	UPROPERTY()
	float Vigor = 0;
	
	/* Abilities */

	UPROPERTY()
	TArray<FSavedAbility> SavedAbilities;

	UPROPERTY()
	TArray<FSavedMap> SavedMaps;

	FSavedMap GetSavedMapWithMapName(const FString& InMapName);
	bool HasMap(const FString& InMapName);
};
