// Fill out your copyright notice in the Description page of Project Settings.


#include "Bot/AngelBot.h"

#include "Bot/PlayerBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AAngelBot::AAngelBot()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f,540.0f,0.0f);
}

// Called when the game starts or when spawned
void AAngelBot::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAngelBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAngelBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AAngelBot::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation,int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible,int32* UserData) const
{
	FName NameAILineOfSight = FName(TEXT("PawnLineOfSight"));
	FHitResult HitResult;

	APlayerBaseCharacter* BasePlayer = Cast<APlayerBaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	FVector TargetLocation = BasePlayer->GetMesh()->GetSocketLocation(TargetBone);
	const bool bHitSocket = GetWorld()->LineTraceSingleByChannel(HitResult,ObserverLocation, TargetLocation,ECC_Visibility,FCollisionQueryParams(NameAILineOfSight, true,IgnoreActor));
	
	/*
	const bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult,ObserverLocation, GetActorLocation(),FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic)),FCollisionQueryParams(NameAILineOfSight, true,IgnoreActor));

	NumberOfLoSChecksPerformed++;
	if(bHit==false || (IsValid(HitResult.GetActor()) && HitResult.GetActor()->IsOwnedBy(this)))
	{
		OutSeenLocation = GetActorLocation();
		OutSightStrength = 1;
		return true;
	}
	APlayerBaseCharacter* Chr = Cast<APlayerBaseCharacter>(HitResult.GetActor());
	FVector SightTargetLocation =Chr->GetMesh()->GetSocketLocation(TargetBone);
	const bool bHitSocket = GetWorld()->LineTraceSingleByObjectType(HitResult,ObserverLocation, SightTargetLocation,FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic)),FCollisionQueryParams(NameAILineOfSight, true,IgnoreActor));
*/
	//NumberOfLoSChecksPerformed++;
	if(bHitSocket || (IsValid(HitResult.GetActor()))) /*&& HitResult.GetActor()->IsOwnedBy(this))*/
	{
		OutSeenLocation = TargetLocation;
		OutSightStrength = 1;
		return true;
	}
	OutSightStrength = 0;
	return false;
}


