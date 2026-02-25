#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "BossPlatform.generated.h"

UCLASS()
class PROJECT_BANG_SQUAD_API ABossPlatform : public AActor
{
	GENERATED_BODY()

public:
	ABossPlatform();
	virtual void BeginPlay() override;

	// 명령 함수
	void SetWarning(bool bActive);
	void MoveDown(bool bPermanent); // 하강 (0.5초 + 진동)
	void MoveUp(); // 상승 (1.0초 + 진동)

	// 상태 확인
	bool IsWalkable() const { return !bIsDown && !bIsPermanentlyDown; }
	int32 GetPlayerCount() const { return OverlappingPlayers.Num(); }
	int32 PlatformID = -1;

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerBox;

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* MoveTimeline;

	UPROPERTY(EditDefaultsOnly, Category = "Timeline")
	UCurveFloat* MovementCurve; // 0~1 값 (1초 길이 기준)

	// [수정] 물리적 진동 강도 (cm 단위)
	UPROPERTY(EditAnywhere, Category = "Effect")
	float ShakeIntensity = 5.0f;

	UPROPERTY()
	UMaterialInstanceDynamic* DynMaterial;

	bool bIsDown = false;
	bool bIsPermanentlyDown = false;
	FVector InitialLocation;
	float DownDepth = -500.0f;

	TSet<AActor*> OverlappingPlayers;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void HandleProgress(float Value);

	UFUNCTION()
	void OnMoveFinished(); // 이동 끝났을 때 정위치 보정용
};