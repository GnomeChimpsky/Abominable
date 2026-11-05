# Changelog

## 1.2.0-rc1 - 2026-11-05

- Add an opt-in 2560x1440 native widescreen mission renderer, presented at the
  current desktop resolution.
- Use a documented DxWrapper source patch that recalculates aspect geometry
  when the game switches between 4:3 menus/movies and widescreen missions.
- Preserve arbitrary logical mission modes under native-resolution scaling;
  this prevents native mission startup from being rejected as an unavailable
  physical monitor mode.
- Render terrain and the widened playfield at 2560x1440 while preserving the
  original menu and movie aspect ratio.
- Add model-only 2x supersampling for sharper polygonal characters without
  changing selection, input, simulation, or animation timing.
- Correct high-resolution loading, HUD, quick-action, pause, briefing,
  map-edge, and scanner placement and interaction.
- Keep the scanner fullscreen and stable by pairing every expanded frame with
  restoration of an isolated 640x480 source snapshot.
- Preserve native widescreen state when loading a second mission in the same
  process.
- Publish the renderer source, reproducible build command, binary hashes, and
  the exact DxWrapper source patch used by the package.

## 1.1.0-rc3 - 2026-11-02

- Centered the animated loading bar with the widescreen loading artwork.
- Expanded map-edge warning flashes across the complete 854-pixel viewport.
- Corrected quick-action popup placement in the added right-side play area.
- Centered the pause interface, its mouse confinement and hit regions, and all
  hover/selection overlays.
- Centered Review Briefing content and its clipping region.
- Preserved the validated terrain, loading, HUD, movie, music, and gameplay
  behavior from the earlier renderer candidates.
- Validated the exact packaged production DLL through startup and a live
  mission smoke test with diagnostic hooks and logging disabled.

## 1.1.0-rc2 - 2026-11-01

- Added an optional native 16:9 mission renderer at 854x480.
- Corrected the terrain-copy source and destination pitch calculations so the
  expanded viewport draws without background corruption.
- Centered the fixed mission-loading artwork in the widescreen viewport.
- Right-anchored the bottom-right status meters while retaining the native HUD
  artwork size and original left/top anchors.
- Preserved the original bordered and borderless 4:3 modes as fallbacks.
- Kept gameplay uncapped at its existing cadence; high-frame-rate simulation
  work remains experimental and is not enabled in this candidate.

## 1.0.0 - 2026-10-31

- Added the validated DxWrapper Dd7to9 compatibility path.
- Added uncapped bordered and aspect-preserving borderless 4:3 modes.
- Added reversible, path-relative 32-bit HKLM registry setup.
- Added package validation and end-user operating notes.
