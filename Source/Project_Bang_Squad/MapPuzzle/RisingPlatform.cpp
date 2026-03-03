#include "Project_Bang_Squad/MapPuzzle/RisingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Curves/CurveFloat.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraShakeBase.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"

ARisingPlatform::ARisingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetMobility(EComponentMobility::Movable);
}

void ARisingPlatform::SaveActorData(FActorSaveData& OutData)
{
	OutData.BoolData.Add("bHasArrived", bHasArrived);
}

void ARisingPlatform::LoadActorData(const FActorSaveData& InData)
{
	if (InData.BoolData.Contains("bHasArrived"))
	{
		bHasArrived = InData.BoolData["bHasArrived"];

		if (bHasArrived)
		{
			OnRep_HasArrived();
		}
	}
}

void ARisingPlatform::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	EndLocation = StartLocation + FVector(0, 0, RiseHeight);

	if (RiseCurve)
	{
		float MinTime, MaxTime;
		RiseCurve->GetTimeRange(MinTime, MaxTime);
		MaxCurveTime = MaxTime;
	}
	
	//저장된 데이터 확인
	if (HasAuthority())
	{
		if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
		{
			if (FActorSaveData* SavedData = GI->GetDataFromInstance(PuzzleID))
			{
				LoadActorData(*SavedData);
			}
		}
	}
}

void ARisingPlatform::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
		{
			FActorSaveData NewData;
			SaveActorData(NewData);
			GI->SaveDataToInstance(PuzzleID, NewData);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void ARisingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRising && RiseCurve && !bHasArrived)
	{
		CurrentCurveTime += DeltaTime;

		float Alpha = RiseCurve->GetFloatValue(CurrentCurveTime);
		FVector NewLoc = FMath::Lerp(StartLocation, EndLocation, Alpha);
		SetActorLocation(NewLoc);

		if (CurrentCurveTime >= MaxCurveTime)
		{
			bIsRising = false;
			bHasArrived = true;
			
			SetActorLocation(EndLocation);

			if (ArrivalCameraShake)
			{
				APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
				if (PC)
				{
					PC->ClientStartCameraShake(ArrivalCameraShake, 1.0f);
				}
			}

			// 2. 사운드 재생 (나중에 주석 풀고 사용)
			/*
			if (ArrivalSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, ArrivalSound, GetActorLocation());
			}
			*/
		}
	}
}

void ARisingPlatform::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARisingPlatform, bHasArrived);
}

void ARisingPlatform::OnRep_HasArrived()
{
	if (bHasArrived)
	{
		bIsRising = false;
		SetActorLocation(EndLocation);
	}
}

void ARisingPlatform::Multicast_TriggerRise_Implementation()
{
	if (bHasArrived) return;
	
	bIsRising = true;
	CurrentCurveTime = 0.0f;
}
