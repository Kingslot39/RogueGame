#include "LevelTrigger.h"

#include "EnemySpawner.h"
#include "ThegameCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALevelTrigger::ALevelTrigger()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	LevelTriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Level Trigger Area"));
	LevelTriggerMesh->SetupAttachment(RootComponent);
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Volume"));
	RootComponent = TriggerVolume;

	MapName.Add(FName("woodlands_RuinsLight"));
	MapName.Add(FName("Woodlands_Mountain"));
	MapName.Add(FName("Woodlands_MountainDry"));
	MapName.Add(FName("woodlands_RuinsDark"));
	MapName.Add(FName("Woodlands_MountainNight"));


}

void ALevelTrigger::StartLevelStreaming(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bSweep, const FHitResult& Hit)
{
	if(Cast<AThegameCharacter>(OtherActor))
	{
		int32 RandomIndex = FMath::RandRange(0, MapName.Num() -1);
		FName SelectedMap = MapName[RandomIndex];
		UGameplayStatics::OpenLevel(this, SelectedMap);
	}


}

// Called when the game starts or when spawned
void ALevelTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ALevelTrigger::StartLevelStreaming);
	if(LevelTriggerClass)
	{
		UNiagaraComponent* SpawnedEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LevelTriggerClass, GetActorLocation(), GetActorRotation());
	}
	
}

// Called every frame
void ALevelTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
