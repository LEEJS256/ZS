// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class ZS_API SZS_NeonProgressbar : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SZS_NeonProgressbar)
			: _Percent(1.0f)
			  , _BarColor(FLinearColor(1.0f, 0.17f, 0.33f, 1.0f)) // Neon Red (#ff2d55)
			  , _TrackColor(FLinearColor(0.05f, 0.06f, 0.09f, 0.8f))
			  , _GlowIntensity(1.5f)
		      ,_SlantOffset(6.0f)
		{
		}

		SLATE_ATTRIBUTE(float, Percent)
		SLATE_ATTRIBUTE(FSlateColor, BarColor)
		SLATE_ATTRIBUTE(FSlateColor, TrackColor)
		SLATE_ATTRIBUTE(float, GlowIntensity)
		SLATE_ATTRIBUTE(float, SlantOffset)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	void SetPercent(TAttribute<float> InPercent);
	void SetBarColor(TAttribute<FSlateColor> InBarColor);
	void SetTrackColor(TAttribute<FSlateColor> InTrackColor);
	void SetGlowIntensity(TAttribute<float> InGlowIntensity);
	void SetSlantOffset(TAttribute<float> InSlantOffset);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	                      FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
	                      bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

private:
	TAttribute<float> Percent;
	TAttribute<FSlateColor> BarColor;
	TAttribute<FSlateColor> TrackColor; //빈테두리
	TAttribute<float> GlowIntensity; // 발광정도
	TAttribute<float> SlantOffset; //기울기 

	const FSlateBrush* WhiteBrush = nullptr;
};
