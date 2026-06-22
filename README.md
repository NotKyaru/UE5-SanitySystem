# UE5 Sanity & Perception System

> A modular, component-based Sanity system for Unreal Engine 5 horror games.  
> Drives post-process visual feedback based on environmental stimuli — no full GAS dependency required.

![UE5](https://img.shields.io/badge/Unreal%20Engine-5.3%2B-black?logo=unrealengine) ![C++](https://img.shields.io/badge/C%2B%2B-17-blue) ![Blueprint](https://img.shields.io/badge/Blueprint-Compatible-purple) ![License](https://img.shields.io/badge/license-MIT-green)

---

## 📽️ Demo

> _GIF / video coming soon — recording in progress._

---

## 🧠 What It Does

This system tracks the player's **sanity** as a float (0–100) and reacts to horror stimuli in the environment. As sanity degrades, the world visually deteriorates through a layered post-process material driven entirely by the component's output.

It is designed as a **drop-in Actor Component** — attach it to any `ACharacter` subclass and wire up the environment triggers.

---

## 🏗️ Architecture

```
UE5-SanitySystem/
├── Source/
│   └── SanitySystem/
│       ├── Public/
│       │   ├── SanityComponent.h          ← Core component (attach to PlayerCharacter)
│       │   ├── SanityConfig.h             ← DataAsset: designer-tunable thresholds
│       │   ├── SanityDrainVolume.h        ← Overlap trigger (darkness zones, cursed areas)
│       │   ├── SanityEnemyProximity.h     ← Radius-based drain scaled by distance
│       │   ├── SanityRestoreZone.h        ← Safe room — slow sanity restore
│       │   └── SanityPostProcessController.h  ← Drives PP material scalar params
│       └── Private/
│           ├── SanityComponent.cpp
│           ├── SanityDrainVolume.cpp
│           ├── SanityEnemyProximity.cpp
│           ├── SanityRestoreZone.cpp
│           └── SanityPostProcessController.cpp
└── docs/
    ├── architecture.md                    ← System design notes
    └── blueprint-integration.md           ← How to wire this in Blueprint
```

---

## ⚙️ System Layers

### Layer 1 — `USanityComponent`
The core. Holds `SanityValue` (0–100) and exposes:
- `ModifySanity(float Delta, FName Source)` — call from any modifier
- Delegates: `OnSanityChanged`, `OnSanityThresholdReached`
- Thresholds broadcast at **75 / 50 / 25 / 0** — configurable via `USanityConfig`

### Layer 2 — Modifier Sources
| Class | Trigger | Behavior |
|---|---|---|
| `USanityDrainVolume` | Box overlap | Drains while inside |
| `USanityEnemyProximity` | Tick radius | Drain scales with distance |
| `USanityRestoreZone` | Box overlap | Slowly restores sanity |

### Layer 3 — Visual Feedback (Post-Process)
| Sanity Range | Effect |
|---|---|
| 100 → 75 | Subtle vignette creep |
| 75 → 50 | Chromatic aberration onset |
| 50 → 25 | Desaturation + noise grain |
| 25 → 0 | Hallucination meshes spawn, full distortion |

All effects driven by scalar parameters on a single Post-Process Material — no hardcoded values.

---

## 🔌 Integration

### C++
```cpp
// In your PlayerCharacter.h
#include "SanityComponent.h"

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sanity")
USanityComponent* SanityComponent;

// In PlayerCharacter.cpp constructor
SanityComponent = CreateDefaultSubobject<USanityComponent>(TEXT("SanityComponent"));
```

### Blueprint
See [`docs/blueprint-integration.md`](docs/blueprint-integration.md) for full wiring guide.

---

## 🎛️ Configuration

All thresholds and drain rates are defined in a `USanityConfig` DataAsset — no recompile needed for designer tweaks.

```cpp
// SanityConfig.h (DataAsset)
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Thresholds")
float Threshold_Uneasy = 75.f;

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Thresholds")
float Threshold_Disturbed = 50.f;

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Thresholds")
float Threshold_Breaking = 25.f;

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Thresholds")
float Threshold_Broken = 0.f;
```

---

## 🛠️ UE5 Version

Built and tested on **Unreal Engine 5.3+**. Should be compatible with 5.1+ with minor adjustments.

**C++ / Blueprint split:** ~70% C++ (component logic, modifiers) / 30% Blueprint (PostProcess material, test level wiring)

---

## 🔭 What I'd Extend Next

- [ ] Sound design layer — MetaSounds graph reacting to sanity thresholds
- [ ] Hallucination actor pool (pooled, not spawned on demand)
- [ ] Save/Load support for persistent sanity across levels
- [ ] GAS integration path (optional attribute set wrapper)
- [ ] Multiplayer replication support

---

## 📄 License

MIT — free to use in personal and commercial projects.
