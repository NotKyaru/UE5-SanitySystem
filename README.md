# UE5 Sanity & Perception System

A modular, component-based Sanity system for Unreal Engine 5 horror games.  
Drives post-process visual feedback based on environmental stimuli. No GAS dependency required.

![UE5](https://img.shields.io/badge/Unreal%20Engine-5.3%2B-black?logo=unrealengine) ![C++](https://img.shields.io/badge/language-C%2B%2B17-blue) ![Blueprint](https://img.shields.io/badge/Blueprint-Compatible-purple) ![License](https://img.shields.io/badge/license-MIT-green)

---

## Demo

> Recording in progress — GIF and video will be added once test level is finalized.

---

## Overview

This system tracks the player's sanity as a float (0–100) and responds to horror stimuli placed in the environment. As sanity degrades, the world visually deteriorates through a layered post-process material driven entirely by the component's output.

Designed as a drop-in `UActorComponent` — attach it to any `ACharacter` subclass and wire up the environment triggers. No engine modifications required.

---

## Repository Structure

```
UE5-SanitySystem/
├── Source/
│   └── SanitySystem/
│       ├── Public/
│       │   ├── SanityComponent.h              # Core component — attach to PlayerCharacter
│       │   ├── SanityConfig.h                 # DataAsset: designer-tunable thresholds
│       │   ├── SanityDrainVolume.h            # Overlap trigger (darkness zones, cursed areas)
│       │   ├── SanityEnemyProximity.h         # Radius-based drain, scales with distance
│       │   ├── SanityRestoreZone.h            # Safe room — slow sanity restoration
│       │   └── SanityPostProcessController.h  # Drives PP material scalar parameters
│       └── Private/
│           ├── SanityComponent.cpp
│           ├── SanityDrainVolume.cpp
│           ├── SanityEnemyProximity.cpp
│           ├── SanityRestoreZone.cpp
│           └── SanityPostProcessController.cpp
└── docs/
    ├── architecture.md                        # System design and data flow
    └── blueprint-integration.md              # Step-by-step Blueprint wiring guide
```

---

## System Layers

### Layer 1 — `USanityComponent`

The source of truth. Holds `SanityValue` (0–100) and exposes:

- `ModifySanity(float Delta, FName Source)` — called by any modifier actor or component
- `OnSanityChanged` — broadcasts on every value change
- `OnSanityThresholdReached` — broadcasts once per threshold crossing (resets on recovery)
- Thresholds configured at **75 / 50 / 25 / 0** via `USanityConfig` DataAsset

### Layer 2 — Modifier Sources

| Class | Trigger | Behavior |
|---|---|---|
| `ASanityDrainVolume` | Box overlap | Drains sanity per second while inside |
| `USanityEnemyProximity` | Tick + radius check | Drain scales linearly with proximity; optional line-of-sight check |
| `ASanityRestoreZone` | Box overlap | Restores sanity per second while inside |

### Layer 3 — Post-Process Feedback

| Sanity Range | Visual Effect |
|---|---|
| 100 → 75 | Vignette creeps in |
| 75 → 50 | Chromatic aberration onset |
| 50 → 25 | Desaturation and noise grain |
| 25 → 0 | Full distortion; hallucination actors spawn |

All effects are driven by scalar parameters on a single Post-Process Material. No hardcoded values.

---

## Integration

### C++

```cpp
// PlayerCharacter.h
#include "SanityComponent.h"

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sanity")
USanityComponent* SanityComponent;

// PlayerCharacter.cpp — constructor
SanityComponent = CreateDefaultSubobject<USanityComponent>(TEXT("SanityComponent"));
```

### Blueprint

See [`docs/blueprint-integration.md`](docs/blueprint-integration.md) for the full wiring guide, including PostProcess Material setup and level actor placement.

---

## Configuration

All thresholds and rates are defined in a `USanityConfig` DataAsset. Designers can adjust values without recompiling.

```cpp
// USanityConfig (DataAsset)
float Threshold_Uneasy    = 75.f;   // Stage 1: unease
float Threshold_Disturbed = 50.f;   // Stage 2: disturbed
float Threshold_Breaking  = 25.f;   // Stage 3: breaking
float Threshold_Broken    = 0.f;    // Stage 4: full breakdown
```

---

## Technical Notes

- **Engine version:** Unreal Engine 5.3+. Compatible with 5.1+ with minor adjustments.
- **C++ / Blueprint split:** ~70% C++ (component logic, modifiers) / 30% Blueprint (PostProcess Material, test level wiring)
- **GAS:** Not required. An optional GAS attribute set wrapper is listed as a planned extension.
- **Thread safety:** All sanity modifications occur on the game thread via Tick and overlap callbacks.

---

## Planned Extensions

- [ ] MetaSounds layer — audio graph reacting to sanity thresholds
- [ ] Hallucination actor pool (object pooling, not runtime spawning)
- [ ] Save/Load support for persistent sanity state across levels
- [ ] Optional GAS integration via attribute set wrapper
- [ ] Multiplayer replication

---

## License

MIT — free to use in personal and commercial projects.
