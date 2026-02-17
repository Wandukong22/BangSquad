#include "Project_Bang_Squad/MapPuzzle/GemStatue.h"
#include "Project_Bang_Squad/MapPuzzle/CenterStatueManager.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"

AGemStatue::AGemStatue()
{
	bReplicates = true;

	StatueMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StatueMesh"));
	RootComponent = StatueMesh;

	HiddenAddonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HiddenAddonMesh"));
	HiddenAddonMesh->SetupAttachment(RootComponent);
	HiddenAddonMesh->SetHiddenInGame(true);
}

void AGemStatue::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGemStatue, bIsActivated);
}

void AGemStatue::SaveActorData(FActorSaveData& OutData)
{
	OutData.BoolData.Add("bIsActivated", bIsActivated);
}

void AGemStatue::LoadActorData(const FActorSaveData& InData)
{
	if (InData.BoolData.Contains("bIsActivated"))
	{
		bIsActivated = InData.BoolData["bIsActivated"];

		if (bIsActivated)
		{
			if (HiddenAddonMesh)
			{
				HiddenAddonMesh->SetHiddenInGame(false);
			}
		}
	}
}

void AGemStatue::EndPlay(const EEndPlayReason::Type EndPlayReason)
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

void AGemStatue::BeginPlay()
{
	Super::BeginPlay();

	if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
	{
		if (FActorSaveData* SavedData = GI->GetDataFromInstance(PuzzleID))
		{
			LoadActorData(*SavedData);
		}
	}

	if (bIsActivated) return;

	CurrentGemCount = 0;

	for (AActor* Gem : TargetGems)
	{
		if (IsValid(Gem))
		{
			Gem->OnDestroyed.AddDynamic(this, &AGemStatue::CheckGemStatus);
			CurrentGemCount++;
		}
	}

	if (CurrentGemCount == 0)
	{
		CheckGemStatus(nullptr);
	}
}

void AGemStatue::CheckGemStatus(AActor* DestroyedGem)
{
	if (!HasAuthority() || bIsActivated) return;

	if (DestroyedGem) CurrentGemCount--;

	if (CurrentGemCount <= 0)
	{
		bIsActivated = true;
		OnRep_IsActivated();

		if (CenterStatue) CenterStatue->ActivateRightGoblet();
	}
}

void AGemStatue::OnRep_IsActivated()
{
	if (HiddenAddonMesh) HiddenAddonMesh->SetHiddenInGame(false);
}