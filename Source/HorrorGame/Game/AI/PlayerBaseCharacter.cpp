// Fill out your copyright notice in the Description page of Project Settings.


#include "Bot/PlayerBaseCharacter.h"

// Sets default values
APlayerBaseCharacter::APlayerBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

