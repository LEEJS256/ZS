#include "SZS_GlowIcon.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Text/STextBlock.h"

void SZS_GlowIcon::Construct(const FArguments& InArgs)
{
    IconText = InArgs._IconText;
    IconColor = InArgs._IconColor;
    BackgroundColor = InArgs._BackgroundColor;
    GlowIntensity = InArgs._GlowIntensity;
    BoxSize = InArgs._BoxSize;

    WhiteBrush = FCoreStyle::Get().GetBrush("WhiteBrush");

    ChildSlot
    .HAlign(HAlign_Center)
    .VAlign(VAlign_Center)
    [
        SAssignNew(TextBlock, STextBlock)
        .Text(IconText)
        .ColorAndOpacity(IconColor)
        .Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
    ];
}

void SZS_GlowIcon::SetIconText(TAttribute<FText> InText)
{
    IconText = InText;
    if (TextBlock.IsValid())
    {
        TextBlock->SetText(IconText);
    }
}

void SZS_GlowIcon::SetIconColor(TAttribute<FSlateColor> InColor)
{
    IconColor = InColor;
    if (TextBlock.IsValid())
    {
        TextBlock->SetColorAndOpacity(IconColor);
    }
    Invalidate(EInvalidateWidgetReason::Paint);
}

void SZS_GlowIcon::SetGlowIntensity(TAttribute<float> InIntensity)
{
    GlowIntensity = InIntensity;
    Invalidate(EInvalidateWidgetReason::Paint);
}

FVector2D SZS_GlowIcon::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
    return BoxSize;
}

int32 SZS_GlowIcon::OnPaint(
    const FPaintArgs& Args,
    const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect,
    FSlateWindowElementList& OutDrawElements,
    int32 LayerId,
    const FWidgetStyle& InWidgetStyle,
    bool bParentEnabled) const
{
    const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
    const FLinearColor TintColor = IconColor.Get().GetColor(InWidgetStyle);
    const FLinearColor BgColor = BackgroundColor.Get().GetColor(InWidgetStyle);

    // 1. 반투명 배경
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(FVector2f(LocalSize.X, LocalSize.Y), FSlateLayoutTransform()),
        WhiteBrush,
        ESlateDrawEffect::None,
        BgColor
    );

    // 2. 다중 패스 네온 번짐 (Outer Glow)
    const int32 GlowSteps = 2;
    for (int32 i = GlowSteps; i >= 1; --i)
    {
        float Spread = (float)i * 1.5f * (GlowIntensity.Get() * 0.5f);
        float Alpha = 0.25f / (float)i;

        TArray<FVector2f> GlowBorder;
        GlowBorder.Add(FVector2f(-Spread, -Spread));
        GlowBorder.Add(FVector2f(LocalSize.X + Spread, -Spread));
        GlowBorder.Add(FVector2f(LocalSize.X + Spread, LocalSize.Y + Spread));
        GlowBorder.Add(FVector2f(-Spread, LocalSize.Y + Spread));
        GlowBorder.Add(FVector2f(-Spread, -Spread));

        FSlateDrawElement::MakeLines(
            OutDrawElements,
            LayerId + 1,
            AllottedGeometry.ToPaintGeometry(),
            GlowBorder,
            ESlateDrawEffect::None,
            FLinearColor(TintColor.R, TintColor.G, TintColor.B, Alpha),
            true,
            1.5f + (float)i * 1.0f
        );
    }

    // 3. 선명한 외곽 테두리
    TArray<FVector2f> SharpBorder;
    SharpBorder.Add(FVector2f(0.0f, 0.0f));
    SharpBorder.Add(FVector2f(LocalSize.X, 0.0f));
    SharpBorder.Add(FVector2f(LocalSize.X, LocalSize.Y));
    SharpBorder.Add(FVector2f(0.0f, LocalSize.Y));
    SharpBorder.Add(FVector2f(0.0f, 0.0f));

    FSlateDrawElement::MakeLines(
        OutDrawElements,
        LayerId + 2,
        AllottedGeometry.ToPaintGeometry(),
        SharpBorder,
        ESlateDrawEffect::None,
        TintColor * GlowIntensity.Get(),
        true,
        1.5f
    );

    // 4. 내부 텍스트 위젯 페인트
    return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId + 3, InWidgetStyle, bParentEnabled);
}