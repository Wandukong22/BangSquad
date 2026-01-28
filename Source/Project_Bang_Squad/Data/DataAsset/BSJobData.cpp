// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Data/DataAsset/BSJobData.h"

const FJobInfo& UBSJobData::GetJobInfo(EJobType InJobType) const
{
	for (const FJobInfo& Info : Jobs)
	{
		if (Info.JobType == InJobType)
		{
			return Info;
		}
	}

	static FJobInfo EmptyInfo;
	return EmptyInfo;
}

TSubclassOf<ACharacter> UBSJobData::GetCharacterClass(EJobType InJobType) const
{
	return GetJobInfo(InJobType).CharacterClass;
}

FLinearColor UBSJobData::GetJobColor(EJobType InJobType) const
{
	return GetJobInfo(InJobType).JobColor;
}

UTexture2D* UBSJobData::GetJobIcon(EJobType InJobType) const
{
	return GetJobInfo(InJobType).Icon;
}
