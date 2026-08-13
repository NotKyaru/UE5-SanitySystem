# UE5 Sanity System.

![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)
![Unreal Engine 5](https://img.shields.io/badge/Unreal%20Engine-5-black?logo=unrealengine)

A sanity & perception system for UE5 horror games. Drop it in, wire up a few things, and your character starts losing their mind when stuff gets dark, enemies get close, or you walk into cursed zones.

Built in C++, runs off events (no per-frame polling), and doesn't fight your existing setup.

## Why I made this

Every sanity system tutorial I found hardcodes thresholds in the drain logic and ties everything to `Tick()`. Works fine for a YouTube demo, falls apart once you have three different things draining sanity at once. This one keeps a single component as the source of truth — everything else just asks it to change the value, and it handles the rest.

## What you get

- **`SanityComponent`** — owns the sanity value and tier state. Nothing else touches it directly.
- **Four tiers via Gameplay Tags** — `Stable → Uneasy → Critical → Breaking`. No magic numbers in your BP graphs.
- **Hysteresis so it doesn't flicker** — sanity has to actually recover past a threshold before the tier bumps back up.
- **Events, not polling** — `OnSanityChanged` and `OnSanityTierChanged` fire only when something changes.
- **Drop-in actors** — `SanityDrainVolume`, `SanityRestoreZone`, `SanityEnemyProximity`. Place them, done.
- **Post-process controller** — listens to tier changes and blends visual distortion.
- **Blueprints work too** — everything's exposed, you don't need to touch C++ after setup.

## Quick setup

1. Copy `Source/SanitySystem` into your project's `Source/` folder.
2. Add `SanitySystem` to your `.Build.cs`.
3. Put a `SanityComponent` on your character.
4. Place a `SanityDrainVolume` or `SanityEnemyProximity` somewhere.
5. Bind to `OnSanityTierChanged` wherever you need it.

```cpp
SanityComponent->OnSanityTierChanged.AddDynamic(this, &AMyCharacter::HandleSanityTierChanged);

void AMyCharacter::HandleSanityTierChanged(FGameplayTag OldTier, FGameplayTag NewTier)
{
    UE_LOG(LogTemp, Log, TEXT("Sanity tier changed: %s -> %s"), *OldTier.ToString(), *NewTier.ToString());
}
```

Blueprints? See [docs/blueprint-integration.md](docs/blueprint-integration.md).

## File layout

```
Source/SanitySystem/
├── Public/
│   ├── SanityComponent.h            # source of truth
│   ├── SanityConfig.h                # thresholds live here
│   ├── SanityGameplayTags.h          # tier tags
│   ├── SanityDrainVolume.h
│   ├── SanityRestoreZone.h
│   ├── SanityEnemyProximity.h
│   └── SanityPostProcessController.h
└── Private/
    └── (matching .cpp files)
```

Everything calls `SanityComponent::ModifySanity(float Delta, FGameplayTag Reason)`. One function, one place to debug.

## Make it work in 5 minutes

No demo level in the repo yet, but you can spin one up yourself real quick:

- [ ] New C++ UE5 project (ThirdPerson template's fine).
- [ ] Drop `Source/SanitySystem` into `Source/`.
- [ ] Add `SanitySystem` to `.Build.cs`.
- [ ] Right-click Content Browser → Data Asset → `SanityConfig`. Leave defaults or tweak.
- [ ] On your character:
  - Add `SanityComponent`, assign the config.
  - Add `SanityPostProcessController`.
- [ ] Make a post-process material:
  - New Material → `M_SanityPP`.
  - Material Domain → Post Process.
  - Add four scalar params: `VignetteIntensity`, `ChromaticAberration`, `DesaturationAmount`, `NoiseGrainIntensity`.
  - Make a Material Instance `MI_SanityPP`, assign it to the controller.
- [ ] Drag a `SanityDrainVolume` and a `SanityRestoreZone` into the level.
- [ ] Hit Play. Walk into the drain zone, watch sanity drop. Step into the restore zone, watch it climb. Post-process should scale with each tier.

That's it. Once this works, swap in your own art and audio.

## What you need

- UE5.x, C++ enabled
- GameplayTags plugin on (it ships with the engine, just make sure it's enabled)

## What's still half-baked

- No demo level in the repo — the checklist above is your DIY version for now.
- No audio hooks — bind to `OnSanityTierChanged` yourself if you want sound cues.
- Hysteresis margin is still on the component, not in the config asset. Could move it there later.

It works, but it's not "shipped product" polished. Issues and PRs welcome if you want to help finish it out.

## License

MIT. Use it, modify it, sell something with it — just keep the copyright notice. See [LICENSE](LICENSE).
