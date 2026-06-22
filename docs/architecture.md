# System Architecture

## Design Goals

- **Drop-in component** — no game-wide dependencies. Works in any UE5 project.
- **No GAS required** — self-contained. GAS integration is a future optional extension.
- **Designer-friendly** — all tunable values live in a `USanityConfig` DataAsset.
- **Extensible** — new drain/restore sources just call `ModifySanity()`. No subclassing needed.

---

## Data Flow

```
[Environment Actors]          [USanityComponent]        [Visual Feedback]

ASanityDrainVolume  ──────►  ModifySanity(delta)  ──►  OnSanityChanged
USanityEnemyProximity ────►  (clamps to 0-100)    ──►  OnSanityThresholdReached
ASanityRestoreZone  ──────►  updates SanityValue  ──►  USanityPostProcessController
                                                   ──►  (drives PP Material params)
```

---

## Thread Safety

All sanity modifications happen on the game thread (Tick / Overlap callbacks).  
No async concerns in the current implementation.

---

## Extending the System

To add a new drain source (e.g., a cursed item the player is holding):

1. Get a reference to `USanityComponent` on the player.
2. Call `ModifySanity(-DrainAmount, FName("CursedItem"))` on tick or event.
3. That's it — the component handles threshold checks and delegate broadcasting automatically.

You do **not** need to subclass anything.
