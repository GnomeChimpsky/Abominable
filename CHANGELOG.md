# Changelog

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
