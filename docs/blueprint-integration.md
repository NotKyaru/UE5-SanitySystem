# Blueprint Integration Guide

## Step 1 — Add the Component

1. Open your `BP_PlayerCharacter`.
2. Click **Add Component** → search for **Sanity Component**.
3. In the Details panel, assign a `SanityConfig` DataAsset (create one in the Content Browser: right-click → Miscellaneous → Data Asset → `USanityConfig`).
4. Add **Sanity PostProcess Controller** component to the same character.

---

## Step 2 — Create the Post-Process Material

1. Create a new Material in the Content Browser.
2. Set **Material Domain** → `Post Process`.
3. Add the following **Scalar Parameter** nodes:
   - `SanityNormalized` (0–1)
   - `VignetteIntensity` (0–1)
   - `ChromaticAberration` (0–1)
   - `DesaturationAmount` (0–1)
   - `NoiseGrainIntensity` (0–1)
4. Wire them to your desired effects (vignette mask, scene fringe, desaturation node, etc.).
5. Create a **Material Instance Dynamic** from this material in your Character BP's `BeginPlay`, and assign it to the `SanityPostProcessController`'s `SanityMaterial` slot.

---

## Step 3 — Place Level Actors

- Drag `BP_SanityDrainVolume` into dark areas or cursed zones.
- Attach `USanityEnemyProximity` to your enemy Blueprints.
- Drag `BP_SanityRestoreZone` into safe rooms or lit areas.

---

## Step 4 — Listen to Events (Optional)

In your Character BP, after BeginPlay, bind to:
- `OnSanityChanged` — update a HUD widget, trigger animation
- `OnSanityThresholdReached` — trigger cinematic events, spawn hallucinations, play VO

```
Event BeginPlay
  → Get SanityComponent
  → Bind Event to OnSanityThresholdReached
      → Switch on Threshold value
          25 → Play "Breaking" sound cue
           0 → Trigger hallucination sequence
```
