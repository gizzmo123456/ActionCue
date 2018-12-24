// Ashley M Sands (gizzmo123456) 2018

#include "BaseAudioActor.h"
#include "Classes/Sound/SoundWave.h"

// Sets default values
ABaseAudioActor::ABaseAudioActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseAudioActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseAudioActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

