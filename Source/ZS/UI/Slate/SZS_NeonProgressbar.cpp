// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Slate/SZS_NeonProgressbar.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SZS_NeonProgressbar::Construct(const FArguments& InArgs)
{
	Percent = InArgs._Percent;
	BarColor = InArgs._BarColor;
	TrackColor = InArgs._TrackColor;
	GlowIntensity = InArgs._GlowIntensity;
	SlantOffset = InArgs._SlantOffset;
	
	WhiteBrush = FCoreStyle::Get().GetBrush("WhiteBrush");
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION



void SZS_NeonProgressbar::SetPercent(TAttribute<float> InPercent)
{
	Percent = InPercent;
	Invalidate(EInvalidateWidgetReason::Paint);
}

void SZS_NeonProgressbar::SetBarColor(TAttribute<FSlateColor> InBarColor)
{
	BarColor = InBarColor;
	Invalidate(EInvalidateWidgetReason::Paint);
}

void SZS_NeonProgressbar::SetTrackColor(TAttribute<FSlateColor> InTrackColor)
{
	TrackColor = InTrackColor;
	Invalidate(EInvalidateWidgetReason::Paint);
}

void SZS_NeonProgressbar::SetGlowIntensity(TAttribute<float> InGlowIntensity)
{
	GlowIntensity = InGlowIntensity;
	Invalidate(EInvalidateWidgetReason::Paint);
}

void SZS_NeonProgressbar::SetSlantOffset(TAttribute<float> InSlantOffset)
{
	SlantOffset = InSlantOffset;
	Invalidate(EInvalidateWidgetReason::Paint);
}


int32 SZS_NeonProgressbar::OnPaint(
    const FPaintArgs& Args,
    const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect,
    FSlateWindowElementList& OutDrawElements,
    int32 LayerId,
    const FWidgetStyle& InWidgetStyle,
    bool bParentEnabled) const
{
  const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
    const float CurrentPercent = FMath::Clamp(Percent.Get(), 0.0f, 1.0f);
    const FLinearColor BaseBarColor = BarColor.Get().GetColor(InWidgetStyle);
    const FLinearColor TintTrackColor = TrackColor.Get().GetColor(InWidgetStyle);

    // 하드코딩 대신 속성 값 직접 사용 (0이면 수직 직사각형)
    const float CurrentSlant = SlantOffset.Get();
    const FSlateRenderTransform& RenderTransform = AllottedGeometry.GetAccumulatedRenderTransform();
    const FSlateResourceHandle& BrushResource = WhiteBrush ? WhiteBrush->GetRenderingResource() : FSlateResourceHandle();

    // 1. 트랙 배경
    TArray<FSlateVertex> TrackVertices;
    TArray<SlateIndex> TrackIndices;
    const FColor TrackColor8 = TintTrackColor.ToFColor(true);

    TrackVertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(RenderTransform, FVector2f(0.0f, 0.0f), FVector2f(0.0f, 0.0f), TrackColor8));
    TrackVertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(RenderTransform, FVector2f(LocalSize.X, 0.0f), FVector2f(1.0f, 0.0f), TrackColor8));
    TrackVertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(RenderTransform, FVector2f(LocalSize.X - CurrentSlant, LocalSize.Y), FVector2f(1.0f, 1.0f), TrackColor8));
    TrackVertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(RenderTransform, FVector2f(-CurrentSlant, LocalSize.Y), FVector2f(0.0f, 1.0f), TrackColor8));

    TrackIndices.Add(0); TrackIndices.Add(1); TrackIndices.Add(2);
    TrackIndices.Add(0); TrackIndices.Add(2); TrackIndices.Add(3);

    FSlateDrawElement::MakeCustomVerts(
        OutDrawElements, LayerId, BrushResource, TrackVertices, TrackIndices, nullptr, 0, 0
    );

    // 2. 게이지 채우기
    if (CurrentPercent > 0.0f)
    {
        const float FillWidth = LocalSize.X * CurrentPercent;
        TArray<FSlateVertex> FillVertices;
        TArray<SlateIndex> FillIndices;

        const FColor LeftColor = (BaseBarColor * 0.35f).ToFColor(true);
        const FColor RightColor = (BaseBarColor * GlowIntensity.Get()).ToFColor(true);

        FillVertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(RenderTransform, FVector2f(0.0f, 0.0f), FVector2f(0.0f, 0.0f), LeftColor));
        FillVertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(RenderTransform, FVector2f(FillWidth, 0.0f), FVector2f(1.0f, 0.0f), RightColor));
        FillVertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(RenderTransform, FVector2f(FillWidth - CurrentSlant, LocalSize.Y), FVector2f(1.0f, 1.0f), RightColor));
        FillVertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(RenderTransform, FVector2f(-CurrentSlant, LocalSize.Y), FVector2f(0.0f, 1.0f), LeftColor));

        FillIndices.Add(0); FillIndices.Add(1); FillIndices.Add(2);
        FillIndices.Add(0); FillIndices.Add(2); FillIndices.Add(3);

        FSlateDrawElement::MakeCustomVerts(
            OutDrawElements, LayerId + 1, BrushResource, FillVertices, FillIndices, nullptr, 0, 0
        );

        // 끝단 캡 라인
        TArray<FVector2f> CapPoints;
        CapPoints.Add(FVector2f(FillWidth, 0.0f));
        CapPoints.Add(FVector2f(FillWidth - CurrentSlant, LocalSize.Y));

        FSlateDrawElement::MakeLines(
            OutDrawElements, LayerId + 2, AllottedGeometry.ToPaintGeometry(), CapPoints, ESlateDrawEffect::None, FLinearColor::White, true, 2.0f
        );
    }

    // 3. 다중 외곽 글로우
    const int32 GlowSteps = 2;
    for (int32 i = GlowSteps; i >= 1; --i)
    {
        float Spread = (float)i * 1.5f;
        float Alpha = 0.25f / (float)i;

        TArray<FVector2f> SlantedGlow;
        SlantedGlow.Add(FVector2f(-Spread, -Spread));
        SlantedGlow.Add(FVector2f(LocalSize.X + Spread, -Spread));
        SlantedGlow.Add(FVector2f(LocalSize.X - CurrentSlant + Spread, LocalSize.Y + Spread));
        SlantedGlow.Add(FVector2f(-CurrentSlant - Spread, LocalSize.Y + Spread));
        SlantedGlow.Add(FVector2f(-Spread, -Spread));

        FSlateDrawElement::MakeLines(
            OutDrawElements, LayerId + 3, AllottedGeometry.ToPaintGeometry(), SlantedGlow, ESlateDrawEffect::None,
            FLinearColor(BaseBarColor.R, BaseBarColor.G, BaseBarColor.B, Alpha), true, 1.5f + (float)i * 1.2f
        );
    }

    // 4. 외곽 테두리
    TArray<FVector2f> SharpFrame;
    SharpFrame.Add(FVector2f(0.0f, 0.0f));
    SharpFrame.Add(FVector2f(LocalSize.X, 0.0f));
    SharpFrame.Add(FVector2f(LocalSize.X - CurrentSlant, LocalSize.Y));
    SharpFrame.Add(FVector2f(-CurrentSlant, LocalSize.Y));
    SharpFrame.Add(FVector2f(0.0f, 0.0f));

    FSlateDrawElement::MakeLines(
        OutDrawElements, LayerId + 4, AllottedGeometry.ToPaintGeometry(), SharpFrame, ESlateDrawEffect::None, BaseBarColor, true, 1.2f
    );

    return LayerId + 5;
}


FVector2D SZS_NeonProgressbar::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return FVector2D(100.0f, 20.0f);
}
