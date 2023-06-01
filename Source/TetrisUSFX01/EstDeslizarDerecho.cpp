// Fill out your copyright notice in the Description page of Project Settings.


#include "EstDeslizarDerecho.h"

// Sets default values
AEstDeslizarDerecho::AEstDeslizarDerecho()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEstDeslizarDerecho::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void AEstDeslizarDerecho::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

std::pair<bool, bool> AEstDeslizarDerecho::PosicionarPieza(APieza* pieza, bool flagMov, bool flagInicFun)
{
    if (pieza) {
        auto Location = FVector(0, 0, 0);
        if (flagInicFun == true) {
            Location = FVector(0.0, 115, 285);
            pieza->SetActorLocation(Location);
            flagInicFun = false;
        }
        FVector MoverLocalizacionY(0.0, -5, 0);
        FVector MoverLocalizacionZ(0.0, 0, -1);
        auto locActor = pieza->GetActorLocation();
        float auxY = locActor.Y;
        float auxZ = locActor.Z;
        if (auxY > 5.0f && flagMov == false) {
            auto nuevaLocY = pieza->GetActorLocation() + MoverLocalizacionY;
            pieza->SetActorLocation(nuevaLocY);
        }
        if (auxZ > 175.0f && flagMov == false) {
            auto nuevaLocZ = pieza->GetActorLocation() + MoverLocalizacionZ;
            pieza->SetActorLocation(nuevaLocZ);
        }
        if (auxY <= 5.0f && auxZ <= 175.0f) {
            flagMov = true;
        }

    }
    return std::make_pair(flagMov, flagInicFun);

}

