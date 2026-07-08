// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraCombatSet.h"

#include "GameplayEffectExtension.h"
#include "LyraGameplayTags.h"
#include "AbilitySystem/Attributes/LyraAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraCombatSet)

class FLifetimeProperty;


ULyraCombatSet::ULyraCombatSet()
	: BaseDamage(0.0f)
	, BaseHeal(0.0f)
	, Mana(100.0f)
	, MaxMana(100.0f)
{
}

void ULyraCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ULyraCombatSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULyraCombatSet, BaseHeal, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULyraCombatSet, Mana, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULyraCombatSet, MaxMana, COND_OwnerOnly, REPNOTIFY_Always);
}

void ULyraCombatSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULyraCombatSet, Mana, OldValue);
}

void ULyraCombatSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULyraCombatSet, MaxMana, OldValue);
}

void ULyraCombatSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetManaAttribute())
	{
		// Do not allow Mana to go negative or above max Mana.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		// Do not allow max Mana to drop below 1.
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}

bool ULyraCombatSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		if (Data.EvaluatedData.Magnitude < 0.0f)
		{
#if !UE_BUILD_SHIPPING
			// Check GodMode cheat, unlimited health is checked below
			if (Data.Target.HasMatchingGameplayTag(LyraGameplayTags::Cheat_GodMode))
			{
				// Do not take away any mana.
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}
#endif
			
		}
	}
	return true;
}

void ULyraCombatSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void ULyraCombatSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void ULyraCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULyraCombatSet, BaseDamage, OldValue);
}

void ULyraCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULyraCombatSet, BaseHeal, OldValue);
}

