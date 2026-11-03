# Abominable

Abominable is a drop-in Windows 10/11 compatibility and windowing pack for
*Abomination: The Nemesis Project*. It translates the game's DirectDraw output
to Direct3D 9 with [DxWrapper](https://github.com/elishacloud/dxwrapper).

No game files are included or modified. Extract the release into a legitimate
game installation, beside `abomb.exe`.

## What it provides

- A reliable bordered window using the game's original 4:3 rendering.
- Aspect-preserving, desktop-sized borderless output with black bars as needed.
- Optional native 16:9 mission rendering at 854x480.
- No permanent desktop-resolution change in either supported mode.
- Reversible 32-bit registry setup for installations whose original registry
  entry is missing or points to the wrong folder.

The two compatibility modes use presentation scaling. The optional native
widescreen mode expands mission rendering to 854x480; menus remain at their
original resolution. Abominable deliberately does not impose an
FPS cap: testing confirmed that DxWrapper can cap presentation, but did not
establish that a cap is safe for the game's simulation timing.

## Install and play

1. Download the release ZIP and extract **its contents** into the game folder.
   `Abominable.ps1`, `ddraw.dll`, `dxwrapper.dll`, and `abomb.exe` must end up in
   the same folder.
2. If the game was installed normally and already starts, use either play
   shortcut immediately.
3. If the game opens only the System Analyzer, fails to start, or was copied
   from another PC, run `1 - Set up registry.cmd`. Approve the administrator
   prompt, then run the game's `ReBenchmark.exe` once and accept its prompt.
4. Start the game with one of:

   - `Play - Bordered.cmd` -- normal desktop window frame.
   - `Play - Borderless 4x3.cmd` -- fills the desktop while preserving 4:3.
   - `Play - Native Widescreen 854x480.cmd` -- expands mission rendering to
     16:9 in a bordered window.

The last selected mode is stored in `dxwrapper.ini`. Starting `abomb.exe`
directly afterward uses that mode too.

The game directory must be writable when switching modes. If an older install
under `Program Files (x86)` reports access denied, move or reinstall the game in
a user-writable games folder instead of running the game as administrator.

## Registry setup and rollback

The original game expects this key in the **32-bit view** of HKLM:

```text
HKLM\SOFTWARE\Hothouse Creations - Abomination\Abomination: The Nemesis Project
```

`1 - Set up registry.cmd` changes only its `PATH` value to the folder containing
the script. Before the first change it records whether the key existed and
backs up every existing value to `.abominable-registry-backup.json`.

`Restore registry.cmd` restores that snapshot exactly. If the key did not exist
before setup, restoration removes the key, including settings generated after
setup. The restore script asks for confirmation in that case. Keep the backup
file until you are sure you no longer need rollback.

Both registry operations require administrator permission. They refuse to run
while `abomb.exe` or `ReBenchmark.exe` is active.

## Removing Abominable

1. Close the game.
2. Optionally run `Restore registry.cmd` if Abominable changed the registry.
3. Delete the files and `Profiles` folder listed in `ABOMINABLE-MANIFEST.txt`.

Do not delete `abomb.exe`, `ReBenchmark.exe`, `Data`, saves, or any other game
content. Abominable never patches the executable.

## Display behavior

| Mode | Output | Desktop mode | Aspect ratio |
| --- | --- | --- | --- |
| Bordered | Game-sized window with frame | Unchanged | Original 4:3 |
| Borderless 4:3 | Scaled to desktop, centered | Unchanged | Preserved; bars on widescreen displays |
| Native widescreen | 854x480 mission window | Unchanged | Native 16:9 |

Native widescreen is a release candidate. Terrain and background rendering
were validated in gameplay, including the corrected expanded row pitch. The
renderer centers the loading artwork and animated bar, expands map-edge
warnings to the complete viewport, preserves the established HUD anchors,
places quick-action popups beside characters in the added play area, and
centers the pause interface, interaction regions, hover overlays, and briefing
content. The packaged production build completed startup, menu restoration,
854x480 mission entry, sustained gameplay, interface interaction checks, and a
clean exit. Movies, music, input, and gameplay paths are left unchanged by the
renderer hook.

An additional controlled live-mission timing check sampled the engine's
internal game-state counter under both uncapped presentation (about 100 FPS on
the test system) and a 60 FPS wrapper cap. Both passes advanced at approximately
15.9–16.0 internal ticks per second. This supports the uncapped default for
smoother presentation and scrolling: the core scheduled tick did not accelerate
with render FPS. A follow-up probe of the engine's actual animation-frame
selector found that active mission-entity records selected new sprite frames
approximately 16–17 times per second. Character animation is therefore
effectively about 16 FPS even while the completed image is presented near
100 FPS; intervening presentation frames repeat the current sprite. The pack
still avoids forcing a particular presentation cap because the higher output
rate provides smoother presentation and may benefit camera/input paths without
altering the measured gameplay or animation cadence.

Avoid Windows compatibility settings that force 640x480 resolution, and avoid
combining this pack with another local `ddraw.dll` wrapper. Multi-monitor,
unusual DPI, HDR, and non-NVIDIA systems have not yet been validated as broadly
as the primary Windows 11 test system.

## Audio and input

The supported profiles wrap DirectDraw only. DirectSound and DirectInput
translation are disabled, so audio and input continue through the game's
original Windows paths. Manual testing confirmed menus, options, and gameplay
input in the wrapped test copy. Audio was not independently validated, and this
pack does not claim to repair audio devices, CD-audio behavior, force feedback,
or unusual mouse issues.

If input fails only after switching focus, try returning to bordered mode and
remove third-party overlays before reporting it. Include `dxwrapper-abomb.log`
with reports; it is created beside the executable and can be deleted safely
after the game closes.

## Validation status

The baseline was tested on Windows 11 build 26200 at a 2560x1440 desktop with an
NVIDIA GeForce RTX 3080. The bordered profile reached menus and gameplay. The
borderless profile produced desktop-sized output with a centered 4:3 image.
Normal non-debugged playback remained responsive and the DxWrapper log showed
expected device resets without errors.

The release profiles differ from the recorded test profiles only by hiding
DxWrapper's diagnostic FPS counter; the windowing and scaling settings are
unchanged.

Windows 10 is a compatibility target of this pack, but has not yet received the
same hands-on validation. Higher internal resolutions and exhaustive timing
validation of every gameplay subsystem remain outside this release candidate's
scope.

Run `Validate package.cmd` after extraction to check the wrapper binaries,
profiles, required files, and accidental inclusion of known game files.

## Credits and license

DxWrapper is by Elisha Riedlinger and contributors. Its unmodified binary
distribution license is included as `LICENSE-dxwrapper.txt`. Abominable's
scripts and documentation are provided under the MIT License; see `LICENSE`.
