// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ZS_MinimapIcon.h"

#include "Components/Image.h"

void UZS_MinimapIcon::SetIconTexture(UTexture2D* Texture)
{
	if (IconImage && Texture)
	{
		IconImage->SetBrushFromTexture(Texture);
	}
}

void UZS_MinimapIcon::SetIconRotation(float AngleDegree)
{
	SetRenderTransformAngle(AngleDegree);
}
