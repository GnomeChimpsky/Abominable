# Renderer source and build

`abom-renderer.c` is the source for the optional native 2560x1440 renderer
shipped as `abom_renderer_2560x1440.dll`. It is loaded by DxWrapper and applies
signature-checked runtime hooks; it does not modify `abomb.exe` on disk.

The validated release-candidate binary was built with the LLVM-MinGW i686 GCC
driver. Set the original build timestamp and output the original artifact name
before renaming it; both values are embedded in the PE image and are required
to reproduce the published hash byte for byte:

```powershell
$env:SOURCE_DATE_EPOCH = '1793916248'
i686-w64-mingw32-gcc.exe -shared -s -std=gnu99 `
  -DABOM_PRODUCTION_WIDESCREEN=1 `
  -DABOM_EXPERIMENTAL_WIDESCREEN=1 `
  -DABOM_EXPERIMENTAL_TERRAIN_COPY=1 `
  -DABOM_EXPERIMENTAL_TERRAIN_REFRESH=1 `
  -DABOM_EXPERIMENTAL_HUD_ANCHORS=1 `
  -DABOM_EXPERIMENTAL_SECONDARY_UI=1 `
  -DABOM_UI_PROBE=1 `
  -DABOM_MODEL_RASTER_PROBE=1 `
  -DABOM_MODEL_SUPERSAMPLE_PROBE=1 `
  -DABOM_WIDESCREEN_WIDTH=2560 `
  -DABOM_WIDESCREEN_HEIGHT=1440 `
  -DABOM_WIDESCREEN_PRESENT_ADJUST=3840 `
  .\patches\abom-renderer.c `
  -o .\abom_highres_2560x1440_ui16_modelss2x_scannerrestore_probe.dll `
  -lddraw -ldxguid
Move-Item `
  .\abom_highres_2560x1440_ui16_modelss2x_scannerrestore_probe.dll `
  .\abom_renderer_2560x1440.dll
Remove-Item Env:SOURCE_DATE_EPOCH
```

Expected SHA-256:

```text
884ACF9C822B4ECC3BF4E406319CF465DFAA0E584FE863DDA5611F593D6EFA6E
```

The enabled probe names reflect the reconstruction history. The bounded model
and UI logging remains useful for this release candidate and does not change
gameplay state. The generated log can be removed after the game closes.

`dxwrapper-mode-aware-native-resolution.patch` is the separate patch applied
to DxWrapper at upstream commit `785a68cda7dc2ddfb8e16ff821fe59003ff34c91`.
