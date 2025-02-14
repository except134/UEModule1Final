// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeBase.h"
#include "SnakeElementBase.h"
#include "Interactable.h"

// Sets default values
ASnakeBase::ASnakeBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ElementSize = 100.0f;
	MovementSpeed = 0.5f;
	LastMoveDirection = EMovementDirection::UP;
}

// Called when the game starts or when spawned
void ASnakeBase::BeginPlay()
{
	Super::BeginPlay();
	
	SetActorTickInterval(MovementSpeed);

	AddSnakeElement(5);
}

// Called every frame
void ASnakeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move();
}

void ASnakeBase::AddSnakeElement(int ElementsNum)
{
	for(int i = 0; i < ElementsNum; ++i) {
		FVector NewLocation(SnakeElements.Num() * ElementSize, 0, 0);
		FTransform NewTransform = FTransform(NewLocation);
		ASnakeElementBase* NewSnakeElem = GetWorld()->SpawnActor<ASnakeElementBase>(SnakeElemetClass, NewTransform);
		NewSnakeElem->SnakeOwner = this;
		int32 ElemIndex = SnakeElements.Add(NewSnakeElem);
		if(ElemIndex == 0) {
			NewSnakeElem->SetFirstElementType();
		}
	}
}

void ASnakeBase::Move()
{
	FVector MovementVector = FVector::ZeroVector;
	float MoveSpeed = ElementSize;

	switch(LastMoveDirection) {
		case EMovementDirection::UP:
			MovementVector.X += MoveSpeed;
		break;
		case EMovementDirection::DOWN:
			MovementVector.X -= MoveSpeed;
		break;
		case EMovementDirection::LEFT:
			MovementVector.Y -= MoveSpeed;
		break;
		case EMovementDirection::RIGHT:
			MovementVector.Y += MoveSpeed;
		break;
	}
	SnakeElements[0]->CollisionOff();

	for(int i = SnakeElements.Num() - 1; i > 0; i--) {
		auto CurrentElement = SnakeElements[i];
		auto PrevElement = SnakeElements[i - 1];
		FVector PrevLocation = PrevElement->GetActorLocation();
		CurrentElement->SetActorLocation(PrevLocation);
	}

	SnakeElements[0]->AddActorWorldOffset(MovementVector);
	SnakeElements[0]->CollisionOn();
}

void ASnakeBase::SnakeElementOverlap(ASnakeElementBase* OverlappedElement, AActor* Other)
{
	if(IsValid(OverlappedElement)) {
		int32 ElementIndex;
		SnakeElements.Find(OverlappedElement, ElementIndex);
		bool bIsFirst = ElementIndex == 0;

		IInteractable* InteractableInterface = Cast<IInteractable>(Other);
		if(InteractableInterface) {
			InteractableInterface->Interact(this, bIsFirst);
		}
	}
}

