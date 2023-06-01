// Fill out your copyright notice in the Description page of Project Settings.


#include "Board.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "DirectorPiezas.h"
#include "PiezaCruz.h"
#include "PiezaDispersa.h"
#include "PiezaTetramino.h"
#include "PiezaLineaPunteada.h"
#include "Pieza3Bloques.h"
#include <utility>
#include "Pieza.h"

#include "Kismet/GameplayStatics.h"
// Sets default values
FVector Location(0.0, 0.0,0.0);
ABoard::ABoard()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABoard::BeginPlay()
{
	Super::BeginPlay();
    flagPermitirMovimiento = false;
    flagInicioFuncion = true;
    estrategias = 0;
    /*CurrentPiece->puntuacion = 0;*/
    PiezaCruzBuilder = GetWorld()->SpawnActor<APiezaCruz>(APiezaCruz::StaticClass());
    PiezaDispersaBuilder = GetWorld()->SpawnActor<APiezaDispersa>(APiezaDispersa::StaticClass());
    PiezaTetraminoBuilder = GetWorld()->SpawnActor<APiezaTetramino>(APiezaTetramino::StaticClass()); 
    PiezaLineaPunteadaBuilder = GetWorld()->SpawnActor<APiezaLineaPunteada>(APiezaLineaPunteada::StaticClass());
    Pieza3BloquesBuilder = GetWorld()->SpawnActor<APieza3Bloques>(APieza3Bloques::StaticClass());
    Director = GetWorld()->SpawnActor<ADirectorPiezas>(ADirectorPiezas::StaticClass());
    DirectorPS = GetWorld()->SpawnActor<ADirectorPiezas>(ADirectorPiezas::StaticClass());
    //CREACION DE FACTORY DE NIVELES
    FabricaEscenas = GetWorld()->SpawnActor<AFactoryNivelesConcreta>(AFactoryNivelesConcreta::StaticClass());
    if (vectorPiezas.Num() == 0) {
        llenarVector();
    }
    for (TActorIterator<APieza> it(GetWorld()); it; ++it)
    {
        
        if (it->GetFName() == TEXT("DissmissPieces"))
        {
            it->Dismiss(0);
            it->Destroy();
        }
    }
    EstrategiaBasica = GetWorld() -> SpawnActor<AEstrategiaBasica>(AEstrategiaBasica::StaticClass());
    EstDeslizarIzquierda = GetWorld()->SpawnActor<AEstDeslizarIzquierdo>(AEstDeslizarIzquierdo::StaticClass());
    EstDeslizarDerecha = GetWorld()->SpawnActor<AEstDeslizarDerecho>(AEstDeslizarDerecho::StaticClass());

    cambiarEstrategia(EstrategiaBasica);
}

// Called every frame
void ABoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    SetearPosicionInicial();
    if (bGameOver)
    {
        return;
    }

    switch (Status)
    {
    case PS_NOT_INITED:
        NewPiece();
        CoolLeft = CoolDown;
        Status = PS_MOVING;
        break;
    case PS_MOVING:
        CoolLeft -= DeltaTime;
        if (CoolLeft <= 0.0f)
        {
            MoveDown();
        }
        break;
    case PS_GOT_BOTTOM:
        CoolLeft -= DeltaTime;
        if (CoolLeft <= 0.0f)
        {
            if (CurrentPiece)
            {
                CurrentPiece->Dismiss(0);
                CurrentPiece->Destroy();
            }
            CurrentPiece = nullptr;
            NewPiece();
            CoolLeft = CoolDown;
            Status = PS_MOVING;
        }
        break;
    default:
        break;
    }
}

// Called to bind functionality to input
void ABoard::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("Rotate", IE_Pressed, this, &ABoard::Rotate);
    PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ABoard::MoveLeft);
    PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ABoard::MoveRight);
    PlayerInputComponent->BindAction("MoveDownToEnd", IE_Pressed, this, &ABoard::MoveDownToEnd);
    PlayerInputComponent->BindAction("CambiarNivel1", IE_Pressed, this, &ABoard::CambiarNivel1);
    PlayerInputComponent->BindAction("CambiarNivel2", IE_Pressed, this, &ABoard::CambiarNivel2);
    PlayerInputComponent->BindAction("CambiarNivel3", IE_Pressed, this, &ABoard::CambiarNivel3);
    PlayerInputComponent->BindAction("cambiarEstrategia", IE_Pressed, this, &ABoard::CambiarEstrategia);
    /*PlayerInputComponent->BindAction("CambiarNivel3", IE_Pressed, this, &ABoard::CambiarNivel1);*/
  

}
void ABoard::CambiarNivel1() {
    FabricaEscenas->cargarNivel("MapaTetris");
}
void ABoard::CambiarNivel2() {
    FabricaEscenas->cargarNivel("MapaTetris2");
}
void ABoard::CambiarNivel3() {
    FabricaEscenas->cargarNivel("MapaTetris3");
}

void ABoard::cambiarEstrategia(AActor* estrategiaPosicion)
{
    EstrategiaActual = Cast<IEstrategiasInterfaz>(estrategiaPosicion);
    //Log Error if the cast failed if (!BattleStrategy)
    if (!estrategiaPosicion)
    {
        GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("NO SE PUDO HACER CASTING!!!")));

    }
}

void ABoard::CambiarEstrategia()
{   
    estrategias++;
    if (estrategias > 2)
        estrategias = 0;
    switch (estrategias)
    {case 0:
        cambiarEstrategia(EstrategiaBasica);
        GEngine->AddOnScreenDebugMessage(1, 15.f, FColor::Green, FString::Printf(TEXT("SE CAMBIO ESTRATEGIA BASICA"), estrategias));
        break;
    case 1:
        cambiarEstrategia(EstDeslizarDerecha);
        GEngine->AddOnScreenDebugMessage(1, 15.f, FColor::Green, FString::Printf(TEXT("SE CAMBIO ESTRATEGIA DESLIZAR IZQUIERDA...."), estrategias));
        break;
    case 2:
        cambiarEstrategia(EstDeslizarIzquierda);
        GEngine->AddOnScreenDebugMessage(1, 15.f, FColor::Green, FString::Printf(TEXT("SE CAMBIO ESTRATEGIA DESLIZAR DERECHA..."), estrategias));
        break;
    }
    /*estrategias++;
    if (estrategias > 2)
        estrategias = 0;*/
    /*GEngine->AddOnScreenDebugMessage(1, 15.f, FColor::Green, FString::Printf(TEXT("SE CAMBIO ESTRATEGIA A: %d"), estrategias));*/
}

void ABoard::SetearPosicionInicial()
{
    if (!EstrategiaActual)
    {
    GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, FString::Printf(TEXT("NO HAY ESTRATEGIA!!")));
    return; 
    }
    flags = EstrategiaActual->PosicionarPieza(CurrentPiece, flagPermitirMovimiento,flagInicioFuncion);
    flagPermitirMovimiento = flags.first;
    flagInicioFuncion = flags.second;
    /*switch (estrategias) {
    case 0:
        if (CurrentPiece) {
            if (flagInicioFuncion == true) {
                Location = FVector(0.0, 5, 175);
                CurrentPiece->SetActorLocation(Location);
                flagInicioFuncion = false;
            }
            flagPermitirMovimiento = true;

        }
        break;
    case 1:
        if (CurrentPiece) {
            if (flagInicioFuncion == true) {
                Location = FVector(0.0, -200, 300);
                CurrentPiece->SetActorLocation(Location);
                flagInicioFuncion = false;
            }
            FVector MoverLocalizacionY(0.0, 5, 0);
            FVector MoverLocalizacionZ(0.0, 0, -1);
            auto locActor = CurrentPiece->GetActorLocation();
            float auxY = locActor.Y;
            float auxZ = locActor.Z;
            if (auxY < 5.0f && flagPermitirMovimiento==false) {
                auto nuevaLocY = CurrentPiece->GetActorLocation() + MoverLocalizacionY;
                CurrentPiece->SetActorLocation(nuevaLocY);
            }
            if (auxZ > 175.0f && flagPermitirMovimiento == false) {
                auto nuevaLocZ = CurrentPiece->GetActorLocation() + MoverLocalizacionZ;
                CurrentPiece->SetActorLocation(nuevaLocZ);
            }
            if (auxY >= 5.0f && auxZ <= 175.0f) {
                flagPermitirMovimiento = true;
            }

        }
        break;
    case 2:
        if (CurrentPiece) {
            if (flagInicioFuncion == true) {
                Location = FVector(0.0, 115, 285);
                CurrentPiece->SetActorLocation(Location);
                flagInicioFuncion = false;
            }
            FVector MoverLocalizacionY(0.0, -5, 0);
            FVector MoverLocalizacionZ(0.0, 0, -1);
            auto locActor = CurrentPiece->GetActorLocation();
            float auxY = locActor.Y;
            float auxZ = locActor.Z;
            if (auxY > 5.0f && flagPermitirMovimiento == false) {
                auto nuevaLocY = CurrentPiece->GetActorLocation() + MoverLocalizacionY;
                CurrentPiece->SetActorLocation(nuevaLocY);
            }
            if (auxZ > 175.0f && flagPermitirMovimiento == false) {
                auto nuevaLocZ = CurrentPiece->GetActorLocation() + MoverLocalizacionZ;
                CurrentPiece->SetActorLocation(nuevaLocZ);
            }
            if (auxY <= 5.0f && auxZ<=175.0f) {
                flagPermitirMovimiento = true;
            }

        }
        break;
    }*/
    
    
}

void ABoard::Rotate()
{
    if (flagPermitirMovimiento == true) {
        if (CurrentPiece && Status != PS_GOT_BOTTOM)
        {
            CurrentPiece->TestRotate();
        }
    }
}


void ABoard::MoveLeft()
{
    if (flagPermitirMovimiento == true) {
        if (CurrentPiece)
        {
            CurrentPiece->MoveLeft();
            if (Status == PS_GOT_BOTTOM)
            {
                MoveDownToEnd();
            }
        }
    }
}

void ABoard::MoveRight()
{
    if (flagPermitirMovimiento == true) {
        if (CurrentPiece)
        {
            CurrentPiece->MoveRight();
            if (Status == PS_GOT_BOTTOM)
            {
                MoveDownToEnd();
            }
        }
    }
}

void ABoard::MoveDown()
{   
    if (flagPermitirMovimiento == true) {
        if (CurrentPiece)
        {
            if (!CurrentPiece->MoveDown())
            {
                Status = PS_GOT_BOTTOM;
            }
            CoolLeft = CoolDown;
        }
    }
}
void ABoard::llenarVector() {//Llenar la bolsa donde se encuentran las piezas
    int32 RandomInt;
    bool flagRepetido;
    //While:Repetiremos el proceso hasta tener 3 numeros diferentes sin repetir.
    while (vectorPiezas.Num() < 4) {
        RandomInt = FMath::RandRange(0, 3); 
        flagRepetido = false;       //Seteamos el Flag como falso para verificar si se repite un numero
        if (vectorPiezas.Num() != 0) {
            for (const int32& Value : vectorPiezas) //Iteramos entre todos los valores actuales del Array
            {
                if (Value == RandomInt) {       //Si councide un valor, flag=true
                    flagRepetido = true;
                    break;                      //Basta que se repita un valor para ya dejar de iterar en el for.
                }
            }
            if (flagRepetido == false) {    //Si despues de iterar todo el for, no hay repetidos, entonces agregamos a Array.
                vectorPiezas.Add(RandomInt);
            }
        }
        else        //Si el vector se encuentra vacio, entonces directamente agregamos un valor aleatorio dentro del rango
        {
            vectorPiezas.Add(RandomInt);
        }   
    }
}
void ABoard::NewPiece()
{
    CheckLine();
    flagPermitirMovimiento = false;

    UE_LOG(LogTemp, Warning, TEXT("GENERANDO NUEVA PIEZA!"));
    if (vectorPiezas.Num() == 0) {
        llenarVector();
    }
    int selectorBuilders;
    selectorBuilders = vectorPiezas.Last();
    vectorPiezas.Pop();
    if (vectorPiezas.Num() == 0) {
        llenarVector();
    }
    //Asignamos el ultimo valor del vector a CurrentPiece Mediante el director.
    //Luego, eliminamos ese valor del vector, por tanto, el antepenultimo se asigna a piezasiguiente (es el nuevo ultimo)
    //Despues de eliminar un elemento del vector, siempre revisamos si se quedo vacio para volver a llenarlo
    switch (selectorBuilders)
    {
    case 0:
        Director->AsignarConstructorDePieza(PiezaTetraminoBuilder);
        break;
    case 1:
        Director->AsignarConstructorDePieza(Pieza3BloquesBuilder);
        break;
    case 2:
        Director->AsignarConstructorDePieza(PiezaLineaPunteadaBuilder);
        break;
    case 3:
        Director->AsignarConstructorDePieza(PiezaCruzBuilder);
        break;
    default:
        Director->AsignarConstructorDePieza(PiezaCruzBuilder);
        break;
    }
    selectorBuilders = vectorPiezas.Last();
   
    Director->ConstruirPieza();
    CurrentPiece = Director->GetPieza();
    flagInicioFuncion = true;
    /*switch (estrategias) {
    case 0:
        Location=FVector(0.0, 5, 175);
        CurrentPiece->SetActorLocation(Location);
        break;
    case 1:
        break;
    case 2:
        break;
    case 3:
        Location= FVector(0.0, -115, 285);
        CurrentPiece->SetActorLocation(Location);
        break;
    }*/
    
    
    

    /*FVector Location1(0.0, 5, 175);
    CurrentPiece->SetActorLocation(Location1);*/
    
    bGameOver = CheckGameOver();
    if (bGameOver)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game Over!!!!!!!!"));
    }
}

void ABoard::CheckLine()
{
    auto MoveDownFromLine = [this](int z) {
        Location=FVector (0.0f, 0.0f, 5.0 * z + 100.0);
        FRotator Rotation(0.0f, 0.0f, 0.0f);
        TArray<struct FOverlapResult> OutOverlaps;
        FCollisionShape CollisionShape;
        FVector Extent(4.5f, 49.5f, 95.0 + 4.5 - 5.0 * z);
        CollisionShape.SetBox(Extent);
        DrawDebugBox(GetWorld(), Location, Extent, FColor::Purple, false, 1, 0, 1);
        FCollisionQueryParams Params;
        FCollisionResponseParams ResponseParam;
        if (GetWorld()->OverlapMultiByChannel(OutOverlaps,
            Location, Rotation.Quaternion(), ECollisionChannel::ECC_WorldDynamic,
            CollisionShape, Params, ResponseParam))
        {
            for (auto&& Result : OutOverlaps)
            {
                FVector NewLocation = Result.GetActor()->GetActorLocation();
                NewLocation.Z -= 10.0f;
                Result.GetActor()->SetActorLocation(NewLocation);
            }
        }
    };

    int z = 0;
    while (z < 20)
    {
        Location=FVector(0.0f, 0.0f, 10.0f * z + 5.0f);
        FRotator Rotation(0.0f, 0.0f, 0.0f);
        TArray<struct FOverlapResult> OutOverlaps;
        FCollisionShape CollisionShape;
        CollisionShape.SetBox(FVector(4.5f, 49.0f, 4.5f));
        //DrawDebugBox(GetWorld(), Location, FVector(4.5f, 10.0f, 4.5f), FColor::Purple, false, 1, 0, 1);
        FCollisionQueryParams Params;
        FCollisionResponseParams ResponseParam;
        bool b = GetWorld()->OverlapMultiByChannel(OutOverlaps,Location, Rotation.Quaternion(), ECollisionChannel::ECC_WorldDynamic, CollisionShape, Params, ResponseParam);
        if (!b || OutOverlaps.Num() < 10)
        {
            ++z;
            continue;
        }
        else // this line is full, remove the line
        {
            UE_LOG(LogTemp, Warning, TEXT("Find FULL LINE at z=%d"), z);
            for (auto&& Result : OutOverlaps)
            {
                /*Result.GetActor()->Destroy();*/
                AActor* actorPrueba=nullptr;
                actorPrueba = Result.GetActor();
                
                ((ABlock*)actorPrueba)->reducirVida();
                //actorPrueba->Destroy();  //OJO esto tenemos que eliminarlo una vez corregido linea completa
            }
            MoveDownFromLine(z);

            /*if (LineRemoveSoundCue)
            {
                UGameplayStatics::PlaySoundAtLocation(GetWorld(), LineRemoveSoundCue, GetActorLocation(), GetActorRotation());
            }*/
        }
    }
}

void ABoard::MoveDownToEnd()
{
    if (flagPermitirMovimiento == true) {
        if (!CurrentPiece)
        {
            return;
        }

        while (CurrentPiece->MoveDown())
        {
        }

        /*if (MoveToEndSoundCue)
        {
            UGameplayStatics::PlaySoundAtLocation(GetWorld(), MoveToEndSoundCue, GetActorLocation(), GetActorRotation());
        }*/

        switch (Status)
        {
        case PS_MOVING:
            Status = PS_GOT_BOTTOM;
            CoolLeft = CoolDown;
            break;
        case PS_GOT_BOTTOM:
            break;
        default:
            UE_LOG(LogTemp, Warning, TEXT("Wrong status for MoveDownToEnd"));
            break;
        }
    }
}

bool ABoard::CheckGameOver()
{
    if (!CurrentPiece)
    {
        UE_LOG(LogTemp, Warning, TEXT("NoPieces"));
        return true;
    }

    return CurrentPiece->CheckWillCollision([](FVector OldVector) { return OldVector; });
}
