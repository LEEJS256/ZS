// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/ZS_MinimapSubsystem.h"

void UZS_MinimapSubsystem::RegisterIconComponent(UZS_MinimapIcon_Component* Comp)
{
	if (!Comp) return;

	ActiveIconComponents.Add(Comp);
	OnIconRegistered.Broadcast(Comp);
}

void UZS_MinimapSubsystem::UnregisterIconComponent(UZS_MinimapIcon_Component* Comp)
{
	if (!Comp) return;

	ActiveIconComponents.Remove(Comp);
	OnIconUnregistered.Broadcast(Comp);
}