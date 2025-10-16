// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"

void AFPSHUD::DrawHUD() {
	Super::DrawHUD();
	if (CrosshairTex) {
		FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
		FVector2D CrosshairDrawPosition(Center.X - (CrosshairTex->GetSurfaceWidth() * 0.5f), Center.Y - (CrosshairTex->GetSurfaceHeight() * 0.5f));
		FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
		TileItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(TileItem);
	}

}
