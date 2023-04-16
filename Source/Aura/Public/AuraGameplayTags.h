// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * AuraGameplayTags
 *
 * Singleton containing native Gameplay Tags
 */

struct FAuraGameplayTags
{
public:
 static const FAuraGameplayTags& Get() { return GameplayTags;}
 static void InitializeNativeGameplayTags();
protected:

private:
 static FAuraGameplayTags GameplayTags;
};
