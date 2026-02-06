// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "../Lobby/JobSelectWidget.h"
#include "PlayerRow.generated.h"

class UQTEWidget;

UENUM(BlueprintType)
enum class ERowMode : uint8
{
	Lobby,
	Stage
};

UCLASS()
class PROJECT_BANG_SQUAD_API UPlayerRow : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UImage* Img_Profile;

	//프로필 테두리
	UPROPERTY(meta = (BindWidget))
	UImage* Img_ProfileFrame;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Name;

	//   로비 전용
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_ReadyState;

	//   스테이지 전용
	UPROPERTY(meta = (BindWidget))
	UProgressBar* PB_HpBar;
	//HP바 테두리
	UPROPERTY(meta = (BindWidget, OptionalWidget = true))
	UImage* Img_HpBarFrame;
	UPROPERTY(meta = (BindWidget))
	UOverlay* Overlay_Death;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_RespawnTime;

	UPROPERTY(meta = (BindWidget))
	UQTEWidget* QTEWidget;

public:
	class APlayerState* GetTargetPlayerState() const { return TargetPlayerState.Get(); }
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;
	
	//모드 설정
	UFUNCTION(BlueprintCallable)
	void SetWidgetMode(ERowMode NewMode);

	//데이터 연결
	void SetTargetPlayerState(class APlayerState* InPlayerState);

	//정보 갱신(로비)
	void UpdateLobbyInfo(bool bIsReady, EJobType JobType);

	//정보 갱신(스테이지)
	//void UpdateStageInfo();

protected:
	void UpdateProfileImage(EJobType JobType);

private:
	ERowMode CurrentMode = ERowMode::Lobby;
	TWeakObjectPtr<class APlayerState> TargetPlayerState;

	UPROPERTY()
	class UMaterialInstanceDynamic* ProfileMaterial;

	//폰이 새로 생겼을 때
	UFUNCTION()
	void OnPawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

	//체력 변경 시
	UFUNCTION()
	void UpdateHealthUI(float CurrentHealth, float MaxHealth);

	//부활 시간 변경 시
	UFUNCTION()
	void UpdateDeathState(float NewRespawnTime);

	UFUNCTION()
	void HandleOnDead();

	//1초마다 남은 시간 숫자만 줄여줌
	void RefreshRespawnTimer();

	FTimerHandle RespawnTimerHandle;
};
