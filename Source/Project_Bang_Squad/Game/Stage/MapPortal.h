// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Project_Bang_Squad/Core/BSGameTypes.h"
#include "MapPortal.generated.h"

class UBoxComponent;
class UWidgetComponent;
class UTextRenderComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EPortalShape : uint8
{
	Sphere,
	Box
};

UCLASS()
class PROJECT_BANG_SQUAD_API AMapPortal : public AActor
{
	GENERATED_BODY()

public:
	AMapPortal();

	UFUNCTION()
	void ActivatePortal();

	void SaveAllPuzzles();

protected:
	virtual void BeginPlay() override;

	//렌더링되는 거리
	UPROPERTY(EditAnywhere, Category = "BS|Widget")
	float MaxDrawDistance = 1000.f;
	
	//처음부터 활성화인지
	UPROPERTY(EditAnywhere, Category = "BS|Map")
	bool bIsStartActive = true;
	
	//원형 구역
	UPROPERTY(EditAnywhere, Category = "BS|Components")
	TObjectPtr<USphereComponent> TriggerSphere;

	//Mesh
	UPROPERTY(VisibleAnywhere, Category = "BS|Components")
	TObjectPtr<UStaticMeshComponent> PortalMesh;

	// 루트 교체용 (쉐이프를 자식으로 두기 위함)
	UPROPERTY(VisibleAnywhere, Category = "BS|Components")
	TObjectPtr<USceneComponent> DefaultSceneRoot;

	// 박스 형태 추가
	UPROPERTY(EditAnywhere, Category = "BS|Components")
	TObjectPtr<UBoxComponent> TriggerBox;

	// 형태 선택 변수
	UPROPERTY(EditAnywhere, Category = "BS|Map")
	EPortalShape PortalShape = EPortalShape::Sphere;

	// 에디터에서 값 변경 시 바로 반영
	virtual void OnConstruction(const FTransform& Transform) override;

	//카운트다운 표시용 텍스트
	//UPROPERTY(VisibleAnywhere, Category = "BS|Components")
	//TObjectPtr<UTextRenderComponent> CountdownText;

	UPROPERTY(EditAnywhere, Category = "BS|Map")
	EStageIndex TargetStageIndex = EStageIndex::None;

	UPROPERTY(EditAnywhere, Category = "BS|Map")
	EStageSection TargetSection = EStageSection::Main;

	//Overlap
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, Category = "BS|Components")
	TObjectPtr<UWidgetComponent> PortalWidgetComp;

	FTimerHandle CheckDistanceTimerHandle;
	void CheckWidgetDistance();
private:
	//구역 안의 플레이어
	TSet<AActor*> OverlappingPlayers;

	FTimerHandle TravelTimerHandle;

	//카운트다운 갱신용 타이머
	FTimerHandle CountdownUpdateTimer;
	int32 RemainingTime;

	void StartCountdown();
	void CancelCountdown();
	void ProcessLevelTransition();
	void UpdateCountdownText();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateUI(int32 CurrentPlayerCount, int32 MaxPlayers, int32 CurrentTime);
};
