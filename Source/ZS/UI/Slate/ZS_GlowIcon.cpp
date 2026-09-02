#include "ZS_GlowIcon.h"
#include "SZS_GlowIcon.h"

TSharedRef<SWidget> UZS_GlowIcon::RebuildWidget()
{
	MyGlowIcon = SNew(SZS_GlowIcon)
		.IconText(IconText)
		.IconColor(FSlateColor(IconColor))
		.BackgroundColor(FSlateColor(BackgroundColor))
		.GlowIntensity(GlowIntensity)
		.BoxSize(BoxSize);

	return MyGlowIcon.ToSharedRef();
}

void UZS_GlowIcon::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (MyGlowIcon.IsValid())
	{
		MyGlowIcon->SetIconText(IconText);
		MyGlowIcon->SetIconColor(FSlateColor(IconColor));
		MyGlowIcon->SetGlowIntensity(GlowIntensity);
	}
}

void UZS_GlowIcon::SetIconText(FText InText)
{
	IconText = InText;
	if (MyGlowIcon.IsValid())
	{
		MyGlowIcon->SetIconText(IconText);
	}
}

void UZS_GlowIcon::SetIconColor(FLinearColor InColor)
{
	IconColor = InColor;
	if (MyGlowIcon.IsValid())
	{
		MyGlowIcon->SetIconColor(FSlateColor(IconColor));
	}
}

void UZS_GlowIcon::SetGlowIntensity(float InIntensity)
{
	GlowIntensity = InIntensity;
	if (MyGlowIcon.IsValid())
	{
		MyGlowIcon->SetGlowIntensity(InIntensity);
	}
}

void UZS_GlowIcon::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyGlowIcon.Reset();
}

#if WITH_EDITOR
const FText UZS_GlowIcon::GetPaletteCategory()
{
	return FText::FromString(TEXT("ZS UI Custom"));
}
#endif