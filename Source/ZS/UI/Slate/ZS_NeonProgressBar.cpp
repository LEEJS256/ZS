// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Slate/ZS_NeonProgressBar.h"
#include "UI/Slate/SZS_NeonProgressbar.h"

TSharedRef<SWidget> UZS_NeonProgressBar::RebuildWidget()
{
	MyProgressBar = SNew(SZS_NeonProgressbar)
		.Percent(Percent)
		.BarColor(FSlateColor(BarColor))
		.TrackColor(FSlateColor(TrackColor))
		.GlowIntensity(GlowIntensity)
		.SlantOffset(SlantOffset);

	return MyProgressBar.ToSharedRef();
}

void UZS_NeonProgressBar::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (MyProgressBar.IsValid())
	{
		MyProgressBar->SetPercent(Percent);
		MyProgressBar->SetBarColor(FSlateColor(BarColor));
		MyProgressBar->SetGlowIntensity(GlowIntensity);
		MyProgressBar->SetSlantOffset(SlantOffset);
	}
}

void UZS_NeonProgressBar::SetPercent(float InPercent)
{
	Percent = FMath::Clamp(InPercent, 0.0f, 1.0f);
	if (MyProgressBar.IsValid())
	{
		MyProgressBar->SetPercent(Percent);
	}
}

void UZS_NeonProgressBar::SetSlentOffset(float InPercent)
{
	SlantOffset = InPercent;

	if (MyProgressBar.IsValid())
	{
		MyProgressBar->SetSlantOffset(InPercent);
	}
}

void UZS_NeonProgressBar::SetBarColor(FLinearColor InColor)
{
	BarColor = InColor;
	if (MyProgressBar.IsValid())
	{
		MyProgressBar->SetBarColor(FSlateColor(BarColor));
	}
}

void UZS_NeonProgressBar::SetTrackColor(FLinearColor InColor)
{
	TrackColor = InColor;
	if (MyProgressBar.IsValid())
	{
		MyProgressBar->SetTrackColor(FSlateColor(TrackColor));
	}
}

void UZS_NeonProgressBar::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyProgressBar.Reset();
}

#if WITH_EDITOR
const FText UZS_NeonProgressBar::GetPaletteCategory()
{
	return FText::FromString(TEXT("ZS UI Custom"));
}
#endif
