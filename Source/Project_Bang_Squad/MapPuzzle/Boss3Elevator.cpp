#include "Boss3Elevator.h"
#include "Components/StaticMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "Misc/OutputDeviceNull.h" 

ABoss3Elevator::ABoss3Elevator()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	ElevatorTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ElevatorTimeline"));
}

void ABoss3Elevator::BeginPlay()
{
	Super::BeginPlay();

	// 1. 목표 위치 계산 (현재 위치 + Z축 높이)
	StartLocation = GetActorLocation();
	EndLocation = StartLocation + FVector(0.0f, 0.0f, RiseHeight);

	// 2. 타임라인 설정
	if (RiseCurve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("HandleTimelineProgress"));

		FOnTimelineEvent FinishedFunction;
		FinishedFunction.BindUFunction(this, FName("OnTimelineFinished"));

		ElevatorTimeline->AddInterpFloat(RiseCurve, ProgressFunction);
		ElevatorTimeline->SetTimelineFinishedFunc(FinishedFunction);

		// 반복 재생 안 함
		ElevatorTimeline->SetLooping(false);
	}
}

void ABoss3Elevator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ElevatorTimeline)
	{
		ElevatorTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, nullptr);
	}
}

void ABoss3Elevator::ActivateElevator()
{
	if (bIsActivated) return;

	bIsActivated = true;

	// [1] 엘리베이터 상승 시작
	if (RiseCurve)
	{
		ElevatorTimeline->PlayFromStart();
	}

	// [2] 연결된 기계들(BP) 강제 작동 (리플렉션 사용)
	if (TargetMachines.Num() > 0)
	{
		// 블루프린트 함수 호출을 위한 빈 출력 장치
		FOutputDeviceNull Ar;

		for (AActor* Machine : TargetMachines)
		{
			if (IsValid(Machine))
			{
				// "Activate"라는 이름의 커스텀 이벤트를 찾아서 실행
				Machine->CallFunctionByNameWithArguments(TEXT("Activate"), Ar, nullptr, true);
			}
		}
	}
}

void ABoss3Elevator::HandleTimelineProgress(float Value)
{
	FVector NewLoc = FMath::Lerp(StartLocation, EndLocation, Value);
	SetActorLocation(NewLoc);
}

void ABoss3Elevator::OnTimelineFinished()
{
	if (TargetMachines.Num() > 0)
	{
		FOutputDeviceNull Ar; // 리플렉션 호출용 빈 껍데기

		for (AActor* Machine : TargetMachines)
		{
			if (IsValid(Machine))
			{
				// BP의 "Deactivate" 커스텀 이벤트를 강제로 실행
				Machine->CallFunctionByNameWithArguments(TEXT("Deactivate"), Ar, nullptr, true);
			}
		}
	}
}