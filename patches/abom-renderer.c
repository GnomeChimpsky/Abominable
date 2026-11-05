#define COBJMACROS
#include <windows.h>
#include <ddraw.h>
#include <stdarg.h>
#include <stdio.h>

#ifndef ABOM_EXPERIMENTAL_WIDESCREEN
#define ABOM_EXPERIMENTAL_WIDESCREEN 0
#endif

/* Adds argument attribution to the already-installed pixel-copy probe.  This
 * is observational only and is enabled solely for the next disposable V8 run. */
#ifndef ABOM_BACKGROUND_PROBE
#define ABOM_BACKGROUND_PROBE 0
#endif

#ifndef ABOM_WIDESCREEN_WIDTH
#define ABOM_WIDESCREEN_WIDTH 854
#endif
#ifndef ABOM_WIDESCREEN_HEIGHT
#define ABOM_WIDESCREEN_HEIGHT 480
#endif

#define ABOM_VIEWPORT_X_DELTA (ABOM_WIDESCREEN_WIDTH - 640)
#define ABOM_VIEWPORT_Y_DELTA (ABOM_WIDESCREEN_HEIGHT - 480)
#define ABOM_UI_X_OFFSET (ABOM_VIEWPORT_X_DELTA / 2)
#define ABOM_UI_Y_OFFSET (ABOM_VIEWPORT_Y_DELTA / 2)
#define ABOM_QUICK_ACTION_MAX_X (ABOM_WIDESCREEN_WIDTH - 66)
#define ABOM_QUICK_ACTION_MAX_Y (ABOM_WIDESCREEN_HEIGHT - 66)
#define ABOM_QUICK_ACTION_RIGHT_BOUND (ABOM_WIDESCREEN_WIDTH - 144)
#define ABOM_QUICK_ACTION_BOTTOM_BOUND (ABOM_WIDESCREEN_HEIGHT - 101)
#define ABOM_LOADING_POINTER_DELTA \
    ((ABOM_UI_Y_OFFSET * ABOM_WIDESCREEN_WIDTH + ABOM_UI_X_OFFSET) * 2)

/* FUN_0059F950 passes row adjustments measured from the original 640-pixel
 * copier.  A wider build must match destinationPitch - (640 * 2), not the
 * destination pitch itself.  Keep this separately overridable because some
 * DirectDraw implementations may pad a surface beyond width * 2. */
#ifndef ABOM_WIDESCREEN_PRESENT_ADJUST
#define ABOM_WIDESCREEN_PRESENT_ADJUST 428
#endif

#define ABOM_STRINGIFY_INNER(value) #value
#define ABOM_STRINGIFY(value) ABOM_STRINGIFY_INNER(value)

/* Replaces only the fixed 640x480 terrain-window copy reached from
 * FUN_0052C0A0.  Normal modes retain FUN_00422ADA unchanged. */
#ifndef ABOM_EXPERIMENTAL_TERRAIN_COPY
#define ABOM_EXPERIMENTAL_TERRAIN_COPY 0
#endif

/* High-resolution diagnostic only.  The normal mission renderer suppresses
 * its full terrain restore while DAT_0071FE54 is nonzero and relies on the
 * 640x480 dirty-region system thereafter.  At a 1920x1080 internal viewport
 * that leaves most of a newly recreated surface black and reveals it only as
 * dirty rectangles are touched.  Bypass that one suppression test so the
 * already pitch-safe terrain copy runs at the start of every world frame.
 * The separate DAT_00693864 guard and all ordinary builds remain unchanged. */
#ifndef ABOM_EXPERIMENTAL_TERRAIN_REFRESH
#define ABOM_EXPERIMENTAL_TERRAIN_REFRESH 0
#endif

/* V24 proved PAGE_GUARD unsafe for the wrapped DirectDraw surface chain. */
#ifndef ABOM_GUARD_WRITE_PROBE
#define ABOM_GUARD_WRITE_PROBE 0
#endif

/* Lean delivery build: install only the signature-checked widescreen
 * patches and suppress diagnostic logging/dumps and observation detours. */
#ifndef ABOM_PRODUCTION_WIDESCREEN
#define ABOM_PRODUCTION_WIDESCREEN 0
#endif

#ifndef ABOM_TERRAIN_SOURCE_DUMP
#define ABOM_TERRAIN_SOURCE_DUMP 0
#endif

/* Caller-scoped interface reflow for the exact 854x480 mission mode.  The
 * fixed 640-wide loading artwork and pause interface are centered, the
 * loading bar destination follows its artwork, and only FUN_00557310's
 * bottom-right status/meter group is right-anchored. */
#ifndef ABOM_EXPERIMENTAL_HUD_ANCHORS
#define ABOM_EXPERIMENTAL_HUD_ANCHORS 0
#endif

/* Work-in-progress caller families.  Kept separate so the validated loading
 * and HUD fixes can be tested without incomplete pause/edge reflow. */
#ifndef ABOM_EXPERIMENTAL_SECONDARY_UI
#define ABOM_EXPERIMENTAL_SECONDARY_UI 0
#endif

#ifndef ABOM_UI_PROBE
#define ABOM_UI_PROBE 0
#endif

/* Read-only wall-clock sampling of the engine's internal tick/FPS globals.
 * This is kept out of release builds and never changes game timing. */
#ifndef ABOM_TIMING_PROBE
#define ABOM_TIMING_PROBE 0
#endif

#ifndef ABOM_ANIMATION_PROBE
#define ABOM_ANIMATION_PROBE 0
#endif

/* Disposable presentation-only experiment.  Keep the authoritative engine at
 * its measured ~16 Hz cadence, retain two completed RGB565 frames, and write
 * three synthesized frames between them.  This deliberately adds one engine
 * frame of presentation latency; it never calls back into simulation, input,
 * audio, camera, or renderer code. */
#ifndef ABOM_FRAME_INTERPOLATION_PROBE
#define ABOM_FRAME_INTERPOLATION_PROBE 0
#endif

/* Disposable high-rate renderer experiment.  While the original 50 ms
 * master-loop deadline remains authoritative, redraw and present the current
 * mission state at intermediate clock quanta without calling the fixed-step
 * camera controller or simulation. */
#ifndef ABOM_RENDER_ONLY_PROBE
#define ABOM_RENDER_ONLY_PROBE 0
#endif

/* Observation-only trace of the polygon/model triangle raster boundary.
 * The probe replaces only the direct call at 0x0059E60E, records the live
 * target, pitch, projection scale and transformed triangle coordinates, then
 * tail-jumps to the original rasterizer.  It never changes renderer state. */
#ifndef ABOM_MODEL_RASTER_PROBE
#define ABOM_MODEL_RASTER_PROBE 0
#endif

/* Disposable 2x supersampling experiment for renderer mode 0.  The complete
 * model draw is redirected to an isolated 1280x960 RGB565 scratch buffer,
 * centered at (640,480), then only its occupied bounds are reduced back into
 * the engine's original 640x480 intermediate. */
#ifndef ABOM_MODEL_SUPERSAMPLE_PROBE
#define ABOM_MODEL_SUPERSAMPLE_PROBE 0
#endif

/* Diagnostic-only interception.  It never modifies DirectDraw arguments, return
 * values, pixels, timing, or the game's executable. */

typedef HRESULT (WINAPI *DirectDrawCreateFn)(GUID *, LPDIRECTDRAW *, IUnknown *);
typedef HRESULT (WINAPI *CreateSurfaceFn)(LPDIRECTDRAW, LPDDSURFACEDESC, LPDIRECTDRAWSURFACE *, IUnknown *);
typedef HRESULT (WINAPI *SetCooperativeLevelFn)(LPDIRECTDRAW, HWND, DWORD);
typedef HRESULT (WINAPI *SetDisplayModeFn)(LPDIRECTDRAW, DWORD, DWORD, DWORD);
typedef HRESULT (WINAPI *BltFn)(LPDIRECTDRAWSURFACE, LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX);
typedef HRESULT (WINAPI *BltFastFn)(LPDIRECTDRAWSURFACE, DWORD, DWORD, LPDIRECTDRAWSURFACE, LPRECT, DWORD);
typedef HRESULT (WINAPI *FlipFn)(LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE, DWORD);
typedef HRESULT (WINAPI *LockFn)(LPDIRECTDRAWSURFACE, LPRECT, LPDDSURFACEDESC, DWORD, HANDLE);
typedef HRESULT (WINAPI *UnlockFn)(LPDIRECTDRAWSURFACE, LPVOID);
typedef HRESULT (WINAPI *GetDCFn)(LPDIRECTDRAWSURFACE, HDC *);

static DirectDrawCreateFn g_realDirectDrawCreate;
static CreateSurfaceFn g_realCreateSurface;
static SetCooperativeLevelFn g_realSetCooperativeLevel;
static SetDisplayModeFn g_realSetDisplayMode;
static BltFn g_realBlt;
static BltFastFn g_realBltFast;
static FlipFn g_realFlip;
static LockFn g_realLock;
static UnlockFn g_realUnlock;
static GetDCFn g_realGetDC;
static CRITICAL_SECTION g_logLock;
static char g_logPath[MAX_PATH];
static LONG g_initialized;
static void log_line(const char *format, ...);

#if ABOM_MODEL_RASTER_PROBE
static void *g_modelRasterOriginal;
static volatile LONG g_modelRasterSamples;
#endif
#if ABOM_MODEL_SUPERSAMPLE_PROBE
#define ABOM_MODEL_SS_WIDTH 1280
#define ABOM_MODEL_SS_HEIGHT 960
#define ABOM_MODEL_SS_PITCH (ABOM_MODEL_SS_WIDTH * 2)
#define ABOM_MODEL_SS_CENTER_X (ABOM_MODEL_SS_WIDTH / 2)
#define ABOM_MODEL_SS_CENTER_Y (ABOM_MODEL_SS_HEIGHT / 2)
#define ABOM_MODEL_KEY 0xf81f
static void *g_modelRenderOriginal;
static WORD *g_modelSupersamplePixels;
static WORD *g_modelLegacyTarget;
static volatile LONG g_modelSupersampleActive;
static LONG g_modelSupersampleMinX;
static LONG g_modelSupersampleMinY;
static LONG g_modelSupersampleMaxX;
static LONG g_modelSupersampleMaxY;
#endif
#if ABOM_ANIMATION_PROBE
static void *g_animationFrameOriginal;
static volatile LONG g_animationChanges[768];
static volatile LONG g_animationLastFrame[768];
static volatile LONG g_animationType[768];
static volatile LONG g_animationAsset[768];
#endif
static void *g_fullCopyCallers[16];
static void *g_lockCallers[128];
static void *g_unlockCallers[128];
static void *g_getDCCallers[16];
static void *g_bltFastCallers[64];
static void *g_spriteBlitterCallers[256];
static void *g_spriteBlitterTrampoline;
static void *g_compositorCallers[64];
static void *g_compositorTrampoline;
static void *g_objectBlitterCallers[128];
static void *g_objectBlitterTrampoline;
static void *g_pixelCopyCallers[512];
static void *g_pixelCopyTrampoline;
static volatile LONG g_latestPresentPitch;
#if ABOM_EXPERIMENTAL_HUD_ANCHORS
static void *g_hudSpriteOriginal;
static void *g_hudClipOriginal;
static void *g_edgeLineOriginal;
static void *g_pauseBriefingOriginal;
static void *g_overlayUnlockOriginal;
static void *g_overlayWorldFrameOriginal;
static volatile LONG g_pauseBoundsShifted;
static volatile LONG g_overlayRasterScaleArmed;
static volatile LONG g_overlayUnlockScaleCount;
static volatile LONG g_overlaySourceRestoreCount;
static WORD *g_overlayLegacySnapshot;
static volatile LONG g_overlayLegacySnapshotValid;
static volatile LONG g_overlaySourceRestorePending;
/* Set by the scanner's final 640x480 sprite draw and consumed by the
 * high-resolution presentation copy.  Keep this outside ABOM_UI_PROBE: the
 * presentation boundary is the reliable point at which the complete scanner
 * (background, processed map, grid and markers) is available for expansion. */
static volatile LONG g_overlayFrameActive;
static volatile LONG g_overlayPresentScaleCount;
#if ABOM_UI_PROBE
static void *g_pauseMouseXOriginal;
static void *g_overlayCoordinatesOriginal;
static volatile LONG g_pauseMouseRawX __attribute__((used));
static volatile LONG g_pauseMouseAdjustedX __attribute__((used));
static volatile LONG g_pauseControllerState __attribute__((used)) = -1;
static volatile LONG g_quickActionProbeLogged;
static volatile LONG g_pauseInputProbeLogged;
static volatile LONG g_overlayCaptureArmed;
static volatile LONG g_overlayCaptureDone;
#endif
#endif
#if ABOM_BACKGROUND_PROBE
typedef struct PixelCopyGeometry {
    void *caller;
    int width;
    int height;
    int sourceStride;
    int destinationStride;
} PixelCopyGeometry;
static PixelCopyGeometry g_pixelCopyGeometries[256];
static PixelCopyGeometry g_opaqueCopyGeometries[256];
static void *g_opaqueCopyTrampoline;
static volatile LONG g_backgroundProbeLive;
static void *g_surfaceDispatchCallers[128];
static void *g_surfaceDispatchTrampoline;
static void *g_surfaceUnlockCallers[128];
static void *g_surfaceUnlockTrampoline;
static void *g_surfaceLockHelperCallers[256];
static void *g_surfaceLockHelperTrampoline;
typedef struct LiveSpriteGeometry {
    void *caller;
    const USHORT *sprite;
    int width;
    int height;
    int flags;
} LiveSpriteGeometry;
static LiveSpriteGeometry g_liveSpriteGeometries[384];
typedef struct GuardWrite {
    void *page;
    void *instruction;
    void *address;
} GuardWrite;
static GuardWrite g_guardWrites[256];
static PVOID g_guardHandler;
static LPDIRECTDRAWSURFACE g_guardSurface;
static BYTE *g_guardBase;
static SIZE_T g_guardBytes;
static DWORD g_guardProtect;
static volatile LONG g_guardActive;
static void *g_liveLockOrigins[128];
static void *g_liveLockPixels[8];

/* Keep this optional: importing the API directly prevents some proxy loaders
 * from accepting the diagnostic DLL before DllMain can create its log. */
typedef USHORT (WINAPI *CaptureStackBackTraceProc)(ULONG, ULONG, PVOID *, PULONG);
#endif

#if ABOM_BACKGROUND_PROBE
__attribute__((used, noinline)) static void log_surface_dispatch_once(void *caller, const void *clip,
                                                                        int x, int y, const USHORT *sprite,
                                                                        int route)
{
    int index;
    int width = sprite ? sprite[0] : -1;
    int height = sprite ? sprite[1] : -1;
    for (index = 0; index < (int)(sizeof(g_surfaceDispatchCallers) / sizeof(g_surfaceDispatchCallers[0])); ++index) {
        if (g_surfaceDispatchCallers[index] == caller) return;
        if (g_surfaceDispatchCallers[index] == NULL) {
            g_surfaceDispatchCallers[index] = caller;
            log_line("SurfaceDispatchCaller=%p clip=%p at=(%d,%d) sprite=%p size=%dx%d route=%d",
                     caller, clip, x, y, sprite, width, height, route);
            return;
        }
    }
}

/* FUN_005A0210 is the renderer's common DirectDraw-surface unlock helper.
 * Logging its callers only after the second mission mode request identifies
 * routines which have completed a write to a live mission surface, while
 * leaving the unlock itself and every pixel untouched. */
__attribute__((used, noinline)) static void log_surface_unlock_caller_once(void *caller)
{
    int index;
    if (InterlockedCompareExchange(&g_backgroundProbeLive, 0, 0) == 0) return;
    for (index = 0; index < (int)(sizeof(g_surfaceUnlockCallers) / sizeof(g_surfaceUnlockCallers[0])); ++index) {
        if (g_surfaceUnlockCallers[index] == caller) return;
        if (g_surfaceUnlockCallers[index] == NULL) {
            g_surfaceUnlockCallers[index] = caller;
            log_line("SurfaceUnlockCaller=%p", caller);
            return;
        }
    }
}

/* The direct caller of FUN_005A00E0 identifies the rendering subsystem that
 * acquired either of the live DirectDraw surfaces.  The helper's `this` and
 * selector are observation-only; both are preserved unchanged. */
__attribute__((used, noinline)) static void log_surface_lock_helper_caller_once(void *caller, void *self, int selector)
{
    int index;
    if (InterlockedCompareExchange(&g_backgroundProbeLive, 0, 0) == 0) return;
    for (index = 0; index < (int)(sizeof(g_surfaceLockHelperCallers) / sizeof(g_surfaceLockHelperCallers[0])); ++index) {
        if (g_surfaceLockHelperCallers[index] == caller) return;
        if (g_surfaceLockHelperCallers[index] == NULL) {
            g_surfaceLockHelperCallers[index] = caller;
            log_line("SurfaceLockHelperCaller=%p self=%p selector=%d", caller, self, selector);
            return;
        }
    }
}
#endif
static void *g_worldRendererCallers[64];
static void *g_worldRendererTrampoline;
static void *g_cameraCallers[64];
static void *g_cameraTrampoline;
static void *g_modeRequestCallers[128];
static void *g_modeRequestTrampoline;
static unsigned int g_presentationStates[32];
static void *g_presentationCallers[128];
static void *g_presentationTrampoline;
#if ABOM_EXPERIMENTAL_WIDESCREEN
static BOOL g_widescreenPresentPatchInstalled;
static void *g_adaCopyOriginal;
static void *g_bceCopyOriginal;
static void *g_adaWidthSuccess;
static void *g_adaWidthFallback;
static void *g_bceWidthSuccess;
static void *g_bceWidthFallback;
static void *g_adaHeightSuccess;
static void *g_adaHeightFallback;
static void *g_bceHeightSuccess;
static void *g_bceHeightFallback;
#endif
#if ABOM_EXPERIMENTAL_TERRAIN_COPY
typedef void (__cdecl *FixedTerrainCopyFn)(const void *, int, void *, int);
static FixedTerrainCopyFn g_fixedTerrainCopy;
static BOOL g_widescreenTerrainPatchInstalled;
static volatile LONG g_terrainCopyLogged;
#if ABOM_EXPERIMENTAL_TERRAIN_REFRESH
static volatile LONG g_highResLive;
static volatile LONG g_liveTerrainCopies;
static volatile LONG g_livePresentCopies;
#if ABOM_FRAME_INTERPOLATION_PROBE
static CRITICAL_SECTION g_interpolationLock;
static WORD *g_interpolationPrevious;
static WORD *g_interpolationCurrent;
static BYTE *g_interpolationDestination;
static int g_interpolationDestinationPitch;
static volatile LONG g_interpolationPairReady;
static volatile LONG g_interpolationFrameSerial;
static volatile LONG g_interpolationLastPhase;
static volatile DWORD g_interpolationFrameStart;
#endif
#endif

static BOOL dump_terrain_source_bmp(const char *path, const BYTE *source,
                                    int sourcePitch, int width, int height)
{
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    HANDLE file;
    BYTE *outputRow;
    DWORD written;
    int outputPitch;
    int y;

    outputPitch = (width * 3 + 3) & ~3;
    outputRow = (BYTE *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, outputPitch);
    if (!outputRow) return FALSE;

    ZeroMemory(&fileHeader, sizeof(fileHeader));
    ZeroMemory(&infoHeader, sizeof(infoHeader));
    fileHeader.bfType = 0x4d42;
    fileHeader.bfOffBits = sizeof(fileHeader) + sizeof(infoHeader);
    fileHeader.bfSize = fileHeader.bfOffBits + outputPitch * height;
    infoHeader.biSize = sizeof(infoHeader);
    infoHeader.biWidth = width;
    infoHeader.biHeight = height;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 24;
    infoHeader.biCompression = BI_RGB;
    infoHeader.biSizeImage = outputPitch * height;

    file = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        HeapFree(GetProcessHeap(), 0, outputRow);
        return FALSE;
    }
    if (!WriteFile(file, &fileHeader, sizeof(fileHeader), &written, NULL) ||
        written != sizeof(fileHeader) ||
        !WriteFile(file, &infoHeader, sizeof(infoHeader), &written, NULL) ||
        written != sizeof(infoHeader)) {
        CloseHandle(file);
        HeapFree(GetProcessHeap(), 0, outputRow);
        return FALSE;
    }

    for (y = height - 1; y >= 0; --y) {
        const WORD *sourcePixel = (const WORD *)(source + y * sourcePitch);
        int x;
        ZeroMemory(outputRow, outputPitch);
        for (x = 0; x < width; ++x) {
            WORD pixel = sourcePixel[x];
            outputRow[x * 3 + 0] = (BYTE)((pixel & 0x001f) * 255 / 31);
            outputRow[x * 3 + 1] = (BYTE)(((pixel >> 5) & 0x003f) * 255 / 63);
            outputRow[x * 3 + 2] = (BYTE)(((pixel >> 11) & 0x001f) * 255 / 31);
        }
        if (!WriteFile(file, outputRow, outputPitch, &written, NULL) ||
            written != (DWORD)outputPitch) {
            CloseHandle(file);
            HeapFree(GetProcessHeap(), 0, outputRow);
            return FALSE;
        }
    }
    CloseHandle(file);
    HeapFree(GetProcessHeap(), 0, outputRow);
    return TRUE;
}
#endif

static void log_line(const char *format, ...)
{
#if ABOM_PRODUCTION_WIDESCREEN && !ABOM_UI_PROBE && !ABOM_TIMING_PROBE && !ABOM_MODEL_RASTER_PROBE
    (void)format;
    return;
#else
    FILE *stream;
    va_list args;
    char text[1024];
    SYSTEMTIME time;
    if (!g_initialized) return;
    va_start(args, format);
    _vsnprintf(text, sizeof(text) - 1, format, args);
    va_end(args);
    text[sizeof(text) - 1] = '\0';
    GetLocalTime(&time);
    EnterCriticalSection(&g_logLock);
    stream = fopen(g_logPath, "a");
    if (stream) {
        fprintf(stream, "%02u:%02u:%02u.%03u %s\n", time.wHour, time.wMinute,
                time.wSecond, time.wMilliseconds, text);
        fclose(stream);
    }
    LeaveCriticalSection(&g_logLock);
#endif
}

#if ABOM_MODEL_RASTER_PROBE
#if ABOM_MODEL_SUPERSAMPLE_PROBE
__attribute__((used, noinline)) static void begin_model_supersample(void *renderer)
{
    BYTE *map;
    if (!renderer || !g_modelSupersamplePixels ||
        *(LONG *)((BYTE *)renderer + 0x5084) != 0) {
        InterlockedExchange(&g_modelSupersampleActive, 0);
        return;
    }
    map = *(BYTE **)0x006df068;
    if (!map || !*(WORD **)(map + 0x18)) {
        InterlockedExchange(&g_modelSupersampleActive, 0);
        return;
    }
    g_modelLegacyTarget = *(WORD **)(map + 0x18);
    g_modelSupersampleMinX = 640;
    g_modelSupersampleMinY = 480;
    g_modelSupersampleMaxX = -1;
    g_modelSupersampleMaxY = -1;
    InterlockedExchange(&g_modelSupersampleActive, 1);
}

__attribute__((used, noinline)) static void prepare_model_supersample_triangle(void)
{
    LONG *coordinates[6] = {
        (LONG *)0x00642bb0, (LONG *)0x00642bb4,
        (LONG *)0x00642bc0, (LONG *)0x00642bc4,
        (LONG *)0x00642bd0, (LONG *)0x00642bd4
    };
    int vertex;
    if (InterlockedCompareExchange(&g_modelSupersampleActive, 0, 0) == 0)
        return;
    for (vertex = 0; vertex < 3; ++vertex) {
        LONG x = *coordinates[vertex * 2];
        LONG y = *coordinates[vertex * 2 + 1];
        if (x < 0 || x >= 640 || y < 0 || y >= 480) {
            InterlockedExchange(&g_modelSupersampleActive, -1);
            return;
        }
        if (x < g_modelSupersampleMinX) g_modelSupersampleMinX = x;
        if (x > g_modelSupersampleMaxX) g_modelSupersampleMaxX = x;
        if (y < g_modelSupersampleMinY) g_modelSupersampleMinY = y;
        if (y > g_modelSupersampleMaxY) g_modelSupersampleMaxY = y;
        *coordinates[vertex * 2] = x * 2;
        *coordinates[vertex * 2 + 1] = y * 2;
    }
    *(WORD **)0x006438c0 = g_modelSupersamplePixels;
    *(LONG *)0x006438c8 = ABOM_MODEL_SS_PITCH;
}

__attribute__((used, noinline)) static void finish_model_supersample_triangle(void)
{
    if (InterlockedCompareExchange(&g_modelSupersampleActive, 0, 0) == 1) {
        *(WORD **)0x006438c0 = g_modelLegacyTarget;
        *(LONG *)0x006438c8 = 1280;
    }
}

static WORD average_rgb565_samples(const WORD samples[4], int *coverage)
{
    unsigned int red = 0, green = 0, blue = 0;
    int count = 0;
    int index;
    for (index = 0; index < 4; ++index) {
        WORD pixel = samples[index];
        if (pixel == ABOM_MODEL_KEY) continue;
        red += (pixel >> 11) & 0x1f;
        green += (pixel >> 5) & 0x3f;
        blue += pixel & 0x1f;
        ++count;
    }
    *coverage = count;
    if (!count) return ABOM_MODEL_KEY;
    red = (red + count / 2) / count;
    green = (green + count / 2) / count;
    blue = (blue + count / 2) / count;
    return (WORD)((red << 11) | (green << 5) | blue);
}

__attribute__((used, noinline)) static void end_model_supersample(void)
{
    LONG state = InterlockedExchange(&g_modelSupersampleActive, 0);
    int minX, minY, maxX, maxY, x, y;
    if (state != 1 || !g_modelLegacyTarget || g_modelSupersampleMaxX < 0 ||
        g_modelSupersampleMaxY < 0)
        return;
    minX = g_modelSupersampleMinX > 0 ? g_modelSupersampleMinX - 1 : 0;
    minY = g_modelSupersampleMinY > 0 ? g_modelSupersampleMinY - 1 : 0;
    maxX = g_modelSupersampleMaxX < 639 ? g_modelSupersampleMaxX + 1 : 639;
    maxY = g_modelSupersampleMaxY < 479 ? g_modelSupersampleMaxY + 1 : 479;
    for (y = minY; y <= maxY; ++y) {
        WORD *destination = g_modelLegacyTarget + y * 640;
        WORD *source0 = g_modelSupersamplePixels + (y * 2) * ABOM_MODEL_SS_WIDTH;
        WORD *source1 = source0 + ABOM_MODEL_SS_WIDTH;
        for (x = minX; x <= maxX; ++x) {
            int sourceX = x * 2;
            WORD samples[4] = {
                source0[sourceX], source0[sourceX + 1],
                source1[sourceX], source1[sourceX + 1]
            };
            int coverage;
            WORD color = average_rgb565_samples(samples, &coverage);
            /* Preserve the engine's binary magenta color key.  Ordered
             * coverage retains subpixel edge information without baking a
             * magenta halo into opaque RGB565 edge texels. */
            if (coverage == 4 ||
                (coverage && (((x & 1) | ((y & 1) << 1)) < coverage)))
                destination[x] = color;
            else
                destination[x] = ABOM_MODEL_KEY;
            source0[sourceX] = source0[sourceX + 1] = ABOM_MODEL_KEY;
            source1[sourceX] = source1[sourceX + 1] = ABOM_MODEL_KEY;
        }
    }
}

__attribute__((naked)) static void hooked_model_render(void)
{
    __asm__ volatile (
        "pushfl\n\t"
        "pushal\n\t"
        "pushl 24(%esp)\n\t"
        "call _begin_model_supersample\n\t"
        "addl $4, %esp\n\t"
        "popal\n\t"
        "popfl\n\t"
        "call *_g_modelRenderOriginal\n\t"
        "pushfl\n\t"
        "pushal\n\t"
        "call _end_model_supersample\n\t"
        "popal\n\t"
        "popfl\n\t"
        "ret\n\t"
    );
}
#endif

/* Called immediately before FUN_00401159 from FUN_0059E270.  ESI still holds
 * the renderer manager, and the six triangle coordinates have already been
 * translated for the selected target.  Keep the sample deliberately small so
 * the first live mission frame is useful without materially extending it. */
__attribute__((used, noinline)) static void log_model_raster_sample(void *renderer)
{
    LONG sample;
    BYTE *manager = (BYTE *)renderer;
    DWORD scaleBits;
    if (!manager || *(volatile LONG *)0x0065b140 != 7 ||
        *(volatile LONG *)0x00644eec != ABOM_WIDESCREEN_WIDTH ||
        *(volatile LONG *)0x00644ef0 != ABOM_WIDESCREEN_HEIGHT)
        return;
    sample = InterlockedIncrement(&g_modelRasterSamples);
    if (sample > 64) return;
    CopyMemory(&scaleBits, (const void *)0x00609064, sizeof(scaleBits));
    log_line("ModelRaster sample=%ld manager=%p mode=%ld target=%p pitch=%ld "
             "viewport=%ldx%ld offset=(%ld,%ld) scaleBits=%08lx "
             "tri=(%ld,%ld)(%ld,%ld)(%ld,%ld) batches=%ld "
             "clip=(%ld,%ld,%ld,%ld)",
             sample, manager, *(LONG *)(manager + 0x5084),
             *(void **)0x006438c0, *(LONG *)0x006438c8,
             *(LONG *)0x00644eec, *(LONG *)0x00644ef0,
             *(LONG *)(manager + 0x5068), *(LONG *)(manager + 0x506c),
             (unsigned long)scaleBits,
             *(LONG *)0x00642bb0, *(LONG *)0x00642bb4,
             *(LONG *)0x00642bc0, *(LONG *)0x00642bc4,
             *(LONG *)0x00642bd0, *(LONG *)0x00642bd4,
             *(LONG *)(manager + 0x50b4),
             *(LONG *)0x00644dd0, *(LONG *)0x00644dd4,
             *(LONG *)0x00644dd8, *(LONG *)0x00644ddc);
}

__attribute__((naked)) static void hooked_model_raster_dispatch(void)
{
    __asm__ volatile (
        "pushfl\n\t"
        "pushal\n\t"
        "pushl 4(%esp)\n\t"
        "call _log_model_raster_sample\n\t"
        "addl $4, %esp\n\t"
#if ABOM_MODEL_SUPERSAMPLE_PROBE
        "call _prepare_model_supersample_triangle\n\t"
#endif
        "popal\n\t"
        "popfl\n\t"
#if ABOM_MODEL_SUPERSAMPLE_PROBE
        "call *_g_modelRasterOriginal\n\t"
        "pushfl\n\t"
        "pushal\n\t"
        "call _finish_model_supersample_triangle\n\t"
        "popal\n\t"
        "popfl\n\t"
        "ret\n\t"
#else
        "movl _g_modelRasterOriginal, %eax\n\t"
        "jmp *%eax\n\t"
#endif
    );
}

static void hook_model_raster_dispatch(void)
{
    BYTE *base = (BYTE *)GetModuleHandleA(NULL);
    BYTE *call = base + 0x19e60e;
    BYTE *expectedTarget = base + 0x1159;
    INT32 existingRelative;
    INT32 replacementRelative;
    BYTE *actualTarget;
    DWORD oldProtect;
    if (call[0] != 0xe8) {
        log_line("model-raster call signature mismatch at %p", call);
        return;
    }
    CopyMemory(&existingRelative, call + 1, sizeof(existingRelative));
    actualTarget = call + 5 + existingRelative;
    if (actualTarget != expectedTarget) {
        log_line("model-raster target mismatch at %p: got %p expected %p",
                 call, actualTarget, expectedTarget);
        return;
    }
    if (!VirtualProtect(call, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        log_line("model-raster call protection failed: %lu", GetLastError());
        return;
    }
    g_modelRasterOriginal = expectedTarget;
    replacementRelative = (INT32)((BYTE *)hooked_model_raster_dispatch -
                                  (call + 5));
    CopyMemory(call + 1, &replacementRelative, sizeof(replacementRelative));
    FlushInstructionCache(GetCurrentProcess(), call, 5);
    VirtualProtect(call, 5, oldProtect, &oldProtect);
    log_line("installed observation-only model-raster call probe at %p target=%p",
             call, expectedTarget);
}

#if ABOM_MODEL_SUPERSAMPLE_PROBE
static void hook_model_render(void)
{
    BYTE *base = (BYTE *)GetModuleHandleA(NULL);
    BYTE *call = base + 0x19a0d4;
    BYTE *expectedTarget = base + 0x19e270;
    INT32 existingRelative;
    INT32 replacementRelative;
    BYTE *actualTarget;
    DWORD oldProtect;
    if (call[0] != 0xe8) {
        log_line("model-render call signature mismatch at %p", call);
        return;
    }
    CopyMemory(&existingRelative, call + 1, sizeof(existingRelative));
    actualTarget = call + 5 + existingRelative;
    if (actualTarget != expectedTarget) {
        log_line("model-render target mismatch at %p: got %p expected %p",
                 call, actualTarget, expectedTarget);
        return;
    }
    g_modelSupersamplePixels = (WORD *)VirtualAlloc(
        NULL, ABOM_MODEL_SS_WIDTH * ABOM_MODEL_SS_HEIGHT * sizeof(WORD),
        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!g_modelSupersamplePixels) {
        log_line("model supersample allocation failed: %lu", GetLastError());
        return;
    }
    {
        SIZE_T index;
        SIZE_T count = (SIZE_T)ABOM_MODEL_SS_WIDTH * ABOM_MODEL_SS_HEIGHT;
        for (index = 0; index < count; ++index)
            g_modelSupersamplePixels[index] = ABOM_MODEL_KEY;
    }
    if (!VirtualProtect(call, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        log_line("model-render call protection failed: %lu", GetLastError());
        VirtualFree(g_modelSupersamplePixels, 0, MEM_RELEASE);
        g_modelSupersamplePixels = NULL;
        return;
    }
    g_modelRenderOriginal = expectedTarget;
    replacementRelative = (INT32)((BYTE *)hooked_model_render - (call + 5));
    CopyMemory(call + 1, &replacementRelative, sizeof(replacementRelative));
    FlushInstructionCache(GetCurrentProcess(), call, 5);
    VirtualProtect(call, 5, oldProtect, &oldProtect);
    log_line("installed 2x model supersample wrapper at %p target=%p scratch=%p",
             call, expectedTarget, g_modelSupersamplePixels);
}
#endif
#endif

#if ABOM_TIMING_PROBE
static volatile LONG g_cameraProbeCalls;
static volatile LONG g_cameraProbeChanges;
static volatile LONG g_cameraProbeInitialized;
static volatile LONG g_cameraProbeLastX;
static volatile LONG g_cameraProbeLastY;
static volatile LONG g_renderProbeStartTick;
static volatile LONG g_renderProbeLatencyTotal;
static volatile LONG g_renderProbeLatencyCount;
static volatile LONG g_renderProbeLatencyMaximum;
static volatile LONG g_masterDispatchCalls;
static volatile LONG g_masterDispatchDurationTotal;
static volatile LONG g_masterDispatchDurationCount;
static volatile LONG g_masterDispatchDurationMaximum;
static volatile LONG g_lockstepWaitDurationTotal;
static volatile LONG g_lockstepWaitDurationCount;
static volatile LONG g_lockstepWaitDurationMaximum;
static volatile LONG g_sleepDurationTotal;
static volatile LONG g_sleepDurationCount;
static volatile LONG g_sleepDurationMaximum;
static volatile LONG g_waitDurationTotal;
static volatile LONG g_waitDurationCount;
static volatile LONG g_waitDurationMaximum;
static volatile LONG g_msgWaitDurationTotal;
static volatile LONG g_msgWaitDurationCount;
static volatile LONG g_msgWaitDurationMaximum;
static volatile LONG g_statusDurationTotal;
static volatile LONG g_statusDurationCount;
static volatile LONG g_statusDurationMaximum;
static volatile LONG g_masterDispatchActive;
static volatile LONG g_masterDispatchThreadId;
static volatile LONG g_dispatchSleepDurationTotal;
static volatile LONG g_dispatchSleepDurationCount;
static volatile LONG g_dispatchSleepDurationMaximum;
static volatile LONG g_dispatchSleepRequestedTotal;
static volatile LONG g_dispatchGetTickCalls;
static volatile LONG g_dispatchTimeGetTimeCalls;
static volatile LONG g_dispatchGetTickFrameCounter;
static volatile LONG g_dispatchGetTickInitCounter;
static volatile LONG g_dispatchGetTickPacketStampCounter;
static volatile LONG g_dispatchGetTickPacketPollCounter;
static volatile LONG g_dispatchGetTickOtherCounter;
static void *volatile g_dispatchGetTickUnknownCaller;
static volatile LONG g_renderOnlyProbeCalls;
static volatile LONG g_renderOnlyProbeActive;
static volatile LONG g_renderOnlyProbeLastTick;
__attribute__((used, noinline)) static void record_animation_frame(void *renderer, int index, int frame)
{
#if ABOM_ANIMATION_PROBE
    BYTE *record;
    LONG tag;
    LONG previous;
    if (*(volatile LONG *)0x00644eec != 854 ||
        *(volatile LONG *)0x00644ef0 != 480 ||
        !renderer || index < 0 || index >= 768) return;
    record = (BYTE *)renderer + index * 0x19c;
    tag = (LONG)(frame + 1);
    previous = InterlockedExchange(&g_animationLastFrame[index], tag);
    g_animationType[index] = (LONG)*(BYTE *)(record + 0xe0);
    g_animationAsset[index] = *(LONG *)(record + 0xfc);
    if (previous != tag) InterlockedIncrement(&g_animationChanges[index]);
#else
    (void)renderer;
    (void)index;
    (void)frame;
#endif
}

static DWORD WINAPI timing_probe_thread(LPVOID unused)
{
    DWORD previousTime = GetTickCount();
    DWORD previousTick = *(volatile DWORD *)0x006df0c8;
    (void)unused;
    for (;;) {
        DWORD now;
        DWORD tick;
        DWORD elapsed;
        DWORD tickDelta;
        LONG width;
        LONG height;
        float fps;
        Sleep(1000);
        now = GetTickCount();
        tick = *(volatile DWORD *)0x006df0c8;
        width = *(volatile LONG *)0x00644eec;
        height = *(volatile LONG *)0x00644ef0;
        fps = *(volatile float *)0x006df0d0;
        elapsed = now - previousTime;
        tickDelta = tick - previousTick;
        log_line("TimingSample viewport=%ldx%ld fps=%.3f tick=%lu delta=%lu elapsed=%lu rate=%.3f",
                 width, height, (double)fps, (unsigned long)tick,
                 (unsigned long)tickDelta, (unsigned long)elapsed,
                 elapsed ? ((double)tickDelta * 1000.0 / (double)elapsed) : 0.0);
        log_line("CameraCadence calls=%ld coordinateChanges=%ld",
                 InterlockedExchange(&g_cameraProbeCalls, 0),
                 InterlockedExchange(&g_cameraProbeChanges, 0));
        log_line("MasterDispatch calls=%ld",
                 InterlockedExchange(&g_masterDispatchCalls, 0));
        {
            LONG total = InterlockedExchange(&g_masterDispatchDurationTotal, 0);
            LONG count = InterlockedExchange(&g_masterDispatchDurationCount, 0);
            LONG maximum = InterlockedExchange(&g_masterDispatchDurationMaximum, 0);
            log_line("MasterDispatchDuration samples=%ld averageMs=%.3f maximumMs=%ld",
                     count, count ? (double)total / (double)count : 0.0, maximum);
        }
        {
            LONG total = InterlockedExchange(&g_lockstepWaitDurationTotal, 0);
            LONG count = InterlockedExchange(&g_lockstepWaitDurationCount, 0);
            LONG maximum = InterlockedExchange(&g_lockstepWaitDurationMaximum, 0);
            log_line("LockstepWaitDuration samples=%ld averageMs=%.3f maximumMs=%ld",
                     count, count ? (double)total / (double)count : 0.0, maximum);
        }
        {
            LONG total = InterlockedExchange(&g_sleepDurationTotal, 0);
            LONG count = InterlockedExchange(&g_sleepDurationCount, 0);
            LONG maximum = InterlockedExchange(&g_sleepDurationMaximum, 0);
            log_line("SleepDuration samples=%ld averageMs=%.3f maximumMs=%ld",
                     count, count ? (double)total / (double)count : 0.0, maximum);
        }
        {
            LONG total = InterlockedExchange(&g_waitDurationTotal, 0);
            LONG count = InterlockedExchange(&g_waitDurationCount, 0);
            LONG maximum = InterlockedExchange(&g_waitDurationMaximum, 0);
            log_line("WaitForSingleObjectDuration samples=%ld averageMs=%.3f maximumMs=%ld",
                     count, count ? (double)total / (double)count : 0.0, maximum);
        }
        {
            LONG total = InterlockedExchange(&g_msgWaitDurationTotal, 0);
            LONG count = InterlockedExchange(&g_msgWaitDurationCount, 0);
            LONG maximum = InterlockedExchange(&g_msgWaitDurationMaximum, 0);
            log_line("MsgWaitDuration samples=%ld averageMs=%.3f maximumMs=%ld",
                     count, count ? (double)total / (double)count : 0.0, maximum);
        }
        {
            LONG total = InterlockedExchange(&g_statusDurationTotal, 0);
            LONG count = InterlockedExchange(&g_statusDurationCount, 0);
            LONG maximum = InterlockedExchange(&g_statusDurationMaximum, 0);
            log_line("PostRenderStatusDuration samples=%ld averageMs=%.3f maximumMs=%ld",
                     count, count ? (double)total / (double)count : 0.0, maximum);
        }
        {
            LONG total = InterlockedExchange(&g_dispatchSleepDurationTotal, 0);
            LONG count = InterlockedExchange(&g_dispatchSleepDurationCount, 0);
            LONG maximum = InterlockedExchange(&g_dispatchSleepDurationMaximum, 0);
            LONG requested = InterlockedExchange(&g_dispatchSleepRequestedTotal, 0);
            log_line("DispatchSleep samples=%ld requestedMs=%ld averageMs=%.3f maximumMs=%ld",
                     count, requested,
                     count ? (double)total / (double)count : 0.0, maximum);
            log_line("DispatchClockReads GetTickCount=%ld timeGetTime=%ld",
                     InterlockedExchange(&g_dispatchGetTickCalls, 0),
                     InterlockedExchange(&g_dispatchTimeGetTimeCalls, 0));
            log_line("DispatchGetTickCallers frame=%ld init=%ld packetStamp=%ld packetPoll=%ld other=%ld",
                     InterlockedExchange(&g_dispatchGetTickFrameCounter, 0),
                     InterlockedExchange(&g_dispatchGetTickInitCounter, 0),
                     InterlockedExchange(&g_dispatchGetTickPacketStampCounter, 0),
                     InterlockedExchange(&g_dispatchGetTickPacketPollCounter, 0),
                     InterlockedExchange(&g_dispatchGetTickOtherCounter, 0));
            {
                BYTE *base = (BYTE *)GetModuleHandleA(NULL);
                BYTE *caller = (BYTE *)InterlockedCompareExchangePointer(
                    &g_dispatchGetTickUnknownCaller, NULL, NULL);
                log_line("DispatchGetTickUnknownCaller address=%p rva=0x%08lx",
                         caller, caller && base ? (unsigned long)(caller - base) : 0UL);
                log_line("RenderOnlyPasses calls=%ld",
                         InterlockedExchange(&g_renderOnlyProbeCalls, 0));
            }
        }
        {
            LONG total = InterlockedExchange(&g_renderProbeLatencyTotal, 0);
            LONG count = InterlockedExchange(&g_renderProbeLatencyCount, 0);
            LONG maximum = InterlockedExchange(&g_renderProbeLatencyMaximum, 0);
            log_line("RenderLatency samples=%ld averageMs=%.3f maximumMs=%ld",
                     count, count ? (double)total / (double)count : 0.0, maximum);
        }
#if ABOM_ANIMATION_PROBE
        {
            int index;
            int logged = 0;
            LONG total = 0;
            for (index = 0; index < 768; ++index) {
                LONG changes = InterlockedExchange(&g_animationChanges[index], 0);
                if (changes <= 0) continue;
                total += changes;
                if (logged < 24) {
                    log_line("AnimationTransitions index=%d changes=%ld frame=%ld type=%ld asset=%ld",
                             index, changes, g_animationLastFrame[index] - 1,
                             g_animationType[index], g_animationAsset[index]);
                    ++logged;
                }
            }
            log_line("AnimationSummary changes=%ld active=%d", total, logged);
        }
#endif
        previousTime = now;
        previousTick = tick;
    }
}
#endif

#if ABOM_TIMING_PROBE
static void (__cdecl *g_masterDispatchOriginal)(void);
static void (__attribute__((thiscall)) *g_lockstepWaitOriginal)(void *self);
static int (__attribute__((thiscall)) *g_statusOriginal)(void *self);

static void record_timing_duration(volatile LONG *total, volatile LONG *count,
                                   volatile LONG *maximum, LONG duration)
{
    LONG previous = InterlockedCompareExchange(maximum, 0, 0);
    InterlockedExchangeAdd(total, duration);
    InterlockedIncrement(count);
    while (duration > previous) {
        LONG observed = InterlockedCompareExchange(maximum, duration, previous);
        if (observed == previous) break;
        previous = observed;
    }
}

static void __cdecl hooked_master_dispatch_call(void)
{
    DWORD start = GetTickCount();
    LONG duration;
    LONG maximum;
    InterlockedIncrement(&g_masterDispatchCalls);
    InterlockedExchange(&g_masterDispatchThreadId, (LONG)GetCurrentThreadId());
    InterlockedIncrement(&g_masterDispatchActive);
    g_masterDispatchOriginal();
    InterlockedDecrement(&g_masterDispatchActive);
    duration = (LONG)(GetTickCount() - start);
    InterlockedExchangeAdd(&g_masterDispatchDurationTotal, duration);
    InterlockedIncrement(&g_masterDispatchDurationCount);
    maximum = InterlockedCompareExchange(&g_masterDispatchDurationMaximum, 0, 0);
    while (duration > maximum) {
        LONG observed = InterlockedCompareExchange(&g_masterDispatchDurationMaximum,
                                                   duration, maximum);
        if (observed == maximum) break;
        maximum = observed;
    }
}

static void hook_master_dispatch_call(void)
{
    BYTE *base = (BYTE *)GetModuleHandleA(NULL);
    BYTE *callSite = base ? base + 0x125c69 : NULL;
    const BYTE expected[5] = {0xe8, 0xa2, 0xa6, 0xfe, 0xff};
    DWORD oldProtect;
    INT32 relative;
    if (!callSite || memcmp(callSite, expected, sizeof(expected)) != 0) {
        log_line("master dispatch call signature mismatch at %p", callSite);
        return;
    }
    g_masterDispatchOriginal = (void (__cdecl *)(void))(base + 0x110310);
    if (!VirtualProtect(callSite, sizeof(expected), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        log_line("master dispatch call patch protection failed: %lu", GetLastError());
        return;
    }
    relative = (INT32)((BYTE *)hooked_master_dispatch_call - (callSite + 5));
    callSite[0] = 0xe8;
    CopyMemory(callSite + 1, &relative, sizeof(relative));
    FlushInstructionCache(GetCurrentProcess(), callSite, sizeof(expected));
    VirtualProtect(callSite, sizeof(expected), oldProtect, &oldProtect);
    log_line("installed read-only master dispatch counter at %p", callSite);
}

static void __attribute__((fastcall)) hooked_lockstep_wait_call(void *self)
{
    DWORD start = GetTickCount();
    LONG duration;
    LONG maximum;
    g_lockstepWaitOriginal(self);
    duration = (LONG)(GetTickCount() - start);
    InterlockedExchangeAdd(&g_lockstepWaitDurationTotal, duration);
    InterlockedIncrement(&g_lockstepWaitDurationCount);
    maximum = InterlockedCompareExchange(&g_lockstepWaitDurationMaximum, 0, 0);
    while (duration > maximum) {
        LONG observed = InterlockedCompareExchange(&g_lockstepWaitDurationMaximum,
                                                   duration, maximum);
        if (observed == maximum) break;
        maximum = observed;
    }
}

static void hook_lockstep_wait_call(void)
{
    BYTE *base = (BYTE *)GetModuleHandleA(NULL);
    BYTE *callSite = base ? base + 0x11203a : NULL;
    const BYTE expected[5] = {0xe8, 0xe1, 0x67, 0x0d, 0x00};
    DWORD oldProtect;
    INT32 relative;
    if (!callSite || memcmp(callSite, expected, sizeof(expected)) != 0) {
        log_line("lockstep wait call signature mismatch at %p", callSite);
        return;
    }
    g_lockstepWaitOriginal =
        (void (__attribute__((thiscall)) *)(void *))(base + 0x1e8820);
    if (!VirtualProtect(callSite, sizeof(expected), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        log_line("lockstep wait patch protection failed: %lu", GetLastError());
        return;
    }
    relative = (INT32)((BYTE *)hooked_lockstep_wait_call - (callSite + 5));
    callSite[0] = 0xe8;
    CopyMemory(callSite + 1, &relative, sizeof(relative));
    FlushInstructionCache(GetCurrentProcess(), callSite, sizeof(expected));
    VirtualProtect(callSite, sizeof(expected), oldProtect, &oldProtect);
    log_line("installed read-only lockstep wait counter at %p", callSite);
}

static int __attribute__((fastcall)) hooked_status_call(void *self)
{
    DWORD start = GetTickCount();
    int result = g_statusOriginal(self);
    record_timing_duration(&g_statusDurationTotal, &g_statusDurationCount,
                           &g_statusDurationMaximum,
                           (LONG)(GetTickCount() - start));
    return result;
}

static void hook_status_call(void)
{
    BYTE *base = (BYTE *)GetModuleHandleA(NULL);
    BYTE *callSite = base ? base + 0x1120c8 : NULL;
    const BYTE expected[5] = {0xe8, 0x43, 0x0c, 0x0a, 0x00};
    DWORD oldProtect;
    INT32 relative;
    if (!callSite || memcmp(callSite, expected, sizeof(expected)) != 0) {
        log_line("post-render status call signature mismatch at %p", callSite);
        return;
    }
    g_statusOriginal =
        (int (__attribute__((thiscall)) *)(void *))(base + 0x1b2d10);
    if (!VirtualProtect(callSite, sizeof(expected), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        log_line("post-render status patch protection failed: %lu", GetLastError());
        return;
    }
    relative = (INT32)((BYTE *)hooked_status_call - (callSite + 5));
    callSite[0] = 0xe8;
    CopyMemory(callSite + 1, &relative, sizeof(relative));
    FlushInstructionCache(GetCurrentProcess(), callSite, sizeof(expected));
    VirtualProtect(callSite, sizeof(expected), oldProtect, &oldProtect);
    log_line("installed read-only post-render status timer at %p", callSite);
}
#endif

static void log_rect(const char *label, const RECT *rect)
{
    if (rect) log_line("%s=(%ld,%ld)-(%ld,%ld)", label, rect->left, rect->top, rect->right, rect->bottom);
    else log_line("%s=<full surface>", label);
}

static void log_full_copy_caller_once(void *caller)
{
    int index;
    for (index = 0; index < (int)(sizeof(g_fullCopyCallers) / sizeof(g_fullCopyCallers[0])); ++index) {
        if (g_fullCopyCallers[index] == caller) return;
        if (g_fullCopyCallers[index] == NULL) {
            g_fullCopyCallers[index] = caller;
            log_line("  fullCopyCaller=%p", caller);
            return;
        }
    }
}

static void log_lock_caller_once(void *caller, const RECT *rect, DWORD flags)
{
    int index;
#if ABOM_BACKGROUND_PROBE
    if (InterlockedCompareExchange(&g_backgroundProbeLive, 0, 0) == 0) return;
#endif
    for (index = 0; index < (int)(sizeof(g_lockCallers) / sizeof(g_lockCallers[0])); ++index) {
        if (g_lockCallers[index] == caller) return;
        if (g_lockCallers[index] == NULL) {
            g_lockCallers[index] = caller;
            log_line("Lock caller=%p flags=0x%08lx", caller, flags);
            log_rect("  lockRect", rect);
            return;
        }
    }
}

static void log_unlock_caller_once(void *caller)
{
    int index;
#if ABOM_BACKGROUND_PROBE
    if (InterlockedCompareExchange(&g_backgroundProbeLive, 0, 0) == 0) return;
#endif
    for (index = 0; index < (int)(sizeof(g_unlockCallers) / sizeof(g_unlockCallers[0])); ++index) {
        if (g_unlockCallers[index] == caller) return;
        if (g_unlockCallers[index] == NULL) {
            g_unlockCallers[index] = caller;
            log_line("Unlock caller=%p", caller);
            return;
        }
    }
}

/* Capture the caller above FUN_005A00E0 from the already-safe DirectDraw
 * vtable wrapper. This attributes retained-buffer users without patching game
 * code or stopping the process. */
#if ABOM_BACKGROUND_PROBE
static void log_live_lock_stack_once(LPDIRECTDRAWSURFACE surface, const DDSURFACEDESC *desc, DWORD flags)
{
    static CaptureStackBackTraceProc captureStack;
    static int captureStackResolved;
    void *frames[6];
    USHORT count;
    void *origin;
    int index;
    if (!desc || InterlockedCompareExchange(&g_backgroundProbeLive, 0, 0) == 0) return;
    for (index = 0; index < (int)(sizeof(g_liveLockPixels) / sizeof(g_liveLockPixels[0])); ++index) {
        if (g_liveLockPixels[index] == desc->lpSurface) break;
        if (g_liveLockPixels[index] == NULL) {
            g_liveLockPixels[index] = desc->lpSurface;
            log_line("LiveSurfaceLock surface=%p pixels=%p pitch=%ld width=%lu height=%lu flags=0x%08lx",
                     surface, desc->lpSurface, desc->lPitch, desc->dwWidth, desc->dwHeight, flags);
            break;
        }
    }
    if (!captureStackResolved) {
        captureStack = (CaptureStackBackTraceProc)GetProcAddress(GetModuleHandleA("kernel32.dll"), "RtlCaptureStackBackTrace");
        captureStackResolved = 1;
    }
    if (!captureStack) return;
    count = captureStack(1, 6, frames, NULL);
    origin = count > 1 ? frames[1] : NULL;
    if (!origin) return;
    for (index = 0; index < (int)(sizeof(g_liveLockOrigins) / sizeof(g_liveLockOrigins[0])); ++index) {
        if (g_liveLockOrigins[index] == origin) return;
        if (g_liveLockOrigins[index] == NULL) {
            g_liveLockOrigins[index] = origin;
            log_line("LiveLockStack origin=%p immediate=%p next=%p depth=%u",
                     origin, count > 0 ? frames[0] : NULL, count > 2 ? frames[2] : NULL, count);
            return;
        }
    }
}
#endif

static void log_get_dc_caller_once(void *caller)
{
    int index;
    for (index = 0; index < (int)(sizeof(g_getDCCallers) / sizeof(g_getDCCallers[0])); ++index) {
        if (g_getDCCallers[index] == caller) return;
        if (g_getDCCallers[index] == NULL) {
            g_getDCCallers[index] = caller;
            log_line("GetDC caller=%p", caller);
            return;
        }
    }
}

static void log_blt_fast_caller_once(void *caller, DWORD x, DWORD y, const RECT *rect)
{
    int index;
    for (index = 0; index < (int)(sizeof(g_bltFastCallers) / sizeof(g_bltFastCallers[0])); ++index) {
        if (g_bltFastCallers[index] == caller) return;
        if (g_bltFastCallers[index] == NULL) {
            g_bltFastCallers[index] = caller;
            log_line("  BltFastCaller=%p at=(%lu,%lu)", caller, x, y);
            log_rect("  callerSrcRect", rect);
            return;
        }
    }
}

/* FUN_0058d980 is the 16-bit software sprite blitter identified in the
 * read-only Ghidra project.  The temporary detour records the caller's return
 * address once, then immediately resumes the unmodified function through a
 * copied prologue.  It does not inspect or change its four arguments. */
__attribute__((used, noinline)) static void log_sprite_blitter_caller_once(void *caller, int x, int y,
                                                                             void *sprite, int flags)
{
    int index;
#if ABOM_UI_PROBE
    if ((ULONG_PTR)caller == 0x0054ea06 && sprite) {
        const USHORT *words = (const USHORT *)sprite;
        InterlockedExchange(&g_overlayFrameActive, 1);
        if (InterlockedCompareExchange(&g_overlayCaptureDone, 0, 0) == 0)
            InterlockedExchange(&g_overlayCaptureArmed, 1);
        log_line("OverlaySprite at=(%d,%d) sprite=%p header=%04x,%04x,%04x,%04x,%04x,%04x,%04x,%04x",
                 x, y, sprite, words[0], words[1], words[2], words[3],
                 words[4], words[5], words[6], words[7]);
    }
    if ((ULONG_PTR)caller == 0x0054d773 &&
        InterlockedCompareExchange(&g_quickActionProbeLogged, 1, 0) == 0) {
        BYTE *ui = *(BYTE **)0x006df07c;
        BYTE *input = *(BYTE **)0x006df058;
        log_line("QuickActionState draw=(%d,%d) input=(%ld,%ld) cursor=(%ld,%ld) saved=(%ld,%ld) hotspot=(%ld,%ld) state=%ld",
                 x, y,
                 input ? *(LONG *)(input + 0x78c) : -1,
                 input ? *(LONG *)(input + 0x790) : -1,
                 ui ? *(LONG *)(ui + 0x87d4) : -1,
                 ui ? *(LONG *)(ui + 0x87d8) : -1,
                 ui ? *(LONG *)(ui + 0x8c70) : -1,
                 ui ? *(LONG *)(ui + 0x8c74) : -1,
                 ui ? *(LONG *)(ui + 0x8820) : -1,
                 ui ? *(LONG *)(ui + 0x8824) : -1,
                 ui ? *(LONG *)(ui + 0x0c) : -1);
        if (ui) {
            for (index = 0; index < 8; ++index) {
                BYTE *slot = ui + index * 0x106c;
                LONG slotX = *(LONG *)(slot + 0x330);
                LONG slotY = *(LONG *)(slot + 0x334);
                LONG slotId = *(LONG *)(slot + 0x344);
                if (slotX || slotY || slotId)
                    log_line("QuickActionSlot index=%d xy=(%ld,%ld) id=%ld anchor=(%ld,%ld)",
                             index, slotX, slotY, slotId,
                             *(LONG *)(slot + 0x318), *(LONG *)(slot + 0x31c));
            }
        }
    }
#endif
    for (index = 0; index < (int)(sizeof(g_spriteBlitterCallers) / sizeof(g_spriteBlitterCallers[0])); ++index) {
        if (g_spriteBlitterCallers[index] == caller) return;
        if (g_spriteBlitterCallers[index] == NULL) {
            g_spriteBlitterCallers[index] = caller;
            log_line("SpriteBlitterCaller=%p at=(%d,%d) sprite=%p flags=0x%08x",
                     caller, x, y, sprite, (unsigned int)flags);
            return;
        }
    }
}

#if ABOM_BACKGROUND_PROBE
/* Bounded live-only sampling of the clipped 16-bit raster dispatcher.  Its
 * sprite header is read before the game receives it; no metadata or pixels are
 * changed.  Grouping by caller/source/dimensions makes terrain tile families
 * visible without recording every frame or movement coordinate. */
__attribute__((used, noinline)) static void log_live_sprite_geometry_once(void *caller, int x, int y,
                                                                            const USHORT *sprite, int flags)
{
    int index;
    int width, height;
    if (InterlockedCompareExchange(&g_backgroundProbeLive, 0, 0) == 0 || !sprite) return;
    width = (int)sprite[0];
    height = (int)sprite[1];
#if ABOM_UI_PROBE
    if ((ULONG_PTR)caller == 0x0054d773 &&
        InterlockedCompareExchange(&g_quickActionProbeLogged, 1, 0) == 0) {
        BYTE *ui = *(BYTE **)0x006df07c;
        BYTE *input = *(BYTE **)0x006df058;
        log_line("QuickActionState draw=(%d,%d) input=(%ld,%ld) cursor=(%ld,%ld) saved=(%ld,%ld) hotspot=(%ld,%ld) state=%ld",
                 x, y,
                 input ? *(LONG *)(input + 0x78c) : -1,
                 input ? *(LONG *)(input + 0x790) : -1,
                 ui ? *(LONG *)(ui + 0x87d4) : -1,
                 ui ? *(LONG *)(ui + 0x87d8) : -1,
                 ui ? *(LONG *)(ui + 0x8c70) : -1,
                 ui ? *(LONG *)(ui + 0x8c74) : -1,
                 ui ? *(LONG *)(ui + 0x8820) : -1,
                 ui ? *(LONG *)(ui + 0x8824) : -1,
                 ui ? *(LONG *)(ui + 0x0c) : -1);
        if (ui) {
            for (index = 0; index < 8; ++index) {
                BYTE *slot = ui + index * 0x106c;
                LONG slotX = *(LONG *)(slot + 0x330);
                LONG slotY = *(LONG *)(slot + 0x334);
                LONG slotId = *(LONG *)(slot + 0x344);
                if (slotX || slotY || slotId)
                    log_line("QuickActionSlot index=%d xy=(%ld,%ld) id=%ld anchor=(%ld,%ld)",
                             index, slotX, slotY, slotId,
                             *(LONG *)(slot + 0x318), *(LONG *)(slot + 0x31c));
            }
        }
    }
    if ((ULONG_PTR)caller >= 0x005dfdd7 && (ULONG_PTR)caller <= 0x005e0992 &&
        InterlockedCompareExchange(&g_pauseInputProbeLogged, 1, 0) == 0) {
        BYTE *input = *(BYTE **)0x006df058;
        log_line("PauseInput state=%ld input=(%ld,%ld) viewport=%ldx%ld draw=(%d,%d) caller=%p",
                 g_pauseControllerState,
                 input ? *(LONG *)(input + 0x78c) : -1,
                 input ? *(LONG *)(input + 0x790) : -1,
                 *(LONG *)0x00644eec, *(LONG *)0x00644ef0, x, y, caller);
    }
#endif
    for (index = 0; index < (int)(sizeof(g_liveSpriteGeometries) / sizeof(g_liveSpriteGeometries[0])); ++index) {
        LiveSpriteGeometry *entry = &g_liveSpriteGeometries[index];
        if (entry->sprite == sprite && entry->caller == caller && entry->width == width &&
            entry->height == height && entry->flags == flags) return;
        if (entry->sprite == NULL) {
            entry->caller = caller;
            entry->sprite = sprite;
            entry->width = width;
            entry->height = height;
            entry->flags = flags;
            log_line("LiveSpriteGeometry caller=%p at=(%d,%d) sprite=%p size=%dx%d flags=0x%08x",
                     caller, x, y, sprite, width, height, (unsigned int)flags);
            return;
        }
    }
}

static LONG CALLBACK guarded_surface_exception(EXCEPTION_POINTERS *info)
{
    BYTE *address, *instruction, *module;
    LONG index;
    if (!g_guardActive || !info || !info->ExceptionRecord || !info->ContextRecord ||
        info->ExceptionRecord->ExceptionCode != STATUS_GUARD_PAGE_VIOLATION ||
        info->ExceptionRecord->NumberParameters < 2 || info->ExceptionRecord->ExceptionInformation[0] != 1)
        return EXCEPTION_CONTINUE_SEARCH;
    address = (BYTE *)info->ExceptionRecord->ExceptionInformation[1];
    instruction = (BYTE *)info->ContextRecord->Eip;
    module = (BYTE *)GetModuleHandleA(NULL);
    if (address < g_guardBase || address >= g_guardBase + g_guardBytes ||
        instruction < module || instruction >= module + 0x00400000)
        return EXCEPTION_CONTINUE_SEARCH;
    index = InterlockedIncrement(&g_guardActive) - 2;
    if (index >= 0 && index < (LONG)(sizeof(g_guardWrites) / sizeof(g_guardWrites[0]))) {
        g_guardWrites[index].page = (void *)((ULONG_PTR)address & ~(ULONG_PTR)0xfff);
        g_guardWrites[index].instruction = instruction;
        g_guardWrites[index].address = address;
    }
    return EXCEPTION_CONTINUE_EXECUTION;
}

static void arm_surface_write_guard(LPDIRECTDRAWSURFACE surface, const DDSURFACEDESC *desc, DWORD flags)
{
    SYSTEM_INFO si; BYTE *base; SIZE_T bytes; DWORD oldProtect;
    if (!desc || !desc->lpSurface || flags != 0x801 || !InterlockedCompareExchange(&g_backgroundProbeLive,0,0)) return;
    if (InterlockedCompareExchange(&g_guardActive,1,0) != 0) return;
    GetSystemInfo(&si);
    base = (BYTE *)((ULONG_PTR)desc->lpSurface & ~((ULONG_PTR)si.dwPageSize - 1));
    bytes = (SIZE_T)((BYTE *)desc->lpSurface + desc->lPitch * desc->dwHeight - base);
    if (!VirtualProtect(base,bytes,PAGE_READWRITE|PAGE_GUARD,&oldProtect)) { g_guardActive=0; log_line("GuardArmFailed error=%lu",GetLastError()); return; }
    g_guardSurface=surface; g_guardBase=base; g_guardBytes=bytes; g_guardProtect=oldProtect;
    log_line("GuardArmed surface=%p base=%p bytes=%lu",surface,base,(unsigned long)bytes);
}

static void restore_surface_write_guard(LPDIRECTDRAWSURFACE surface)
{
    DWORD ignored; LONG count=InterlockedCompareExchange(&g_guardActive,0,0)-1; int index;
    if (!g_guardBase || surface != g_guardSurface) return;
    VirtualProtect(g_guardBase,g_guardBytes,g_guardProtect,&ignored);
    g_guardSurface=NULL; g_guardBase=NULL; g_guardBytes=0; g_guardActive=0;
    log_line("GuardWriteCount=%ld",count<0?0:count);
    for(index=0; index<count && index<(int)(sizeof(g_guardWrites)/sizeof(g_guardWrites[0])); ++index)
        log_line("GuardWrite page=%p eip=%p address=%p",g_guardWrites[index].page,g_guardWrites[index].instruction,g_guardWrites[index].address);
}
#endif

__attribute__((used, noinline)) static void log_compositor_caller_once(void *caller)
{
    int index;
    for (index = 0; index < (int)(sizeof(g_compositorCallers) / sizeof(g_compositorCallers[0])); ++index) {
        if (g_compositorCallers[index] == caller) return;
        if (g_compositorCallers[index] == NULL) {
            g_compositorCallers[index] = caller;
            log_line("TiledCompositorCaller=%p", caller);
            return;
        }
    }
}

__attribute__((used, noinline)) static void log_object_blitter_caller_once(void *caller)
{
    int index;
    for (index = 0; index < (int)(sizeof(g_objectBlitterCallers) / sizeof(g_objectBlitterCallers[0])); ++index) {
        if (g_objectBlitterCallers[index] == caller) return;
        if (g_objectBlitterCallers[index] == NULL) {
            g_objectBlitterCallers[index] = caller;
            log_line("ObjectBlitterCaller=%p", caller);
            return;
        }
    }
}

__attribute__((used, noinline)) static void log_pixel_copy_caller_once(void *caller)
{
    int index;
    for (index = 0; index < (int)(sizeof(g_pixelCopyCallers) / sizeof(g_pixelCopyCallers[0])); ++index) {
        if (g_pixelCopyCallers[index] == caller) return;
        if (g_pixelCopyCallers[index] == NULL) {
            g_pixelCopyCallers[index] = caller;
            log_line("PixelCopyCaller=%p", caller);
            return;
        }
    }
}

#if ABOM_BACKGROUND_PROBE
/* FUN_004239C5 is cdecl: source, pixel width, height, source row adjustment,
 * destination, destination row adjustment.  Convert the adjustments back to
 * full row strides for a readable, non-mutating trace. */
__attribute__((used, noinline)) static void log_pixel_copy_geometry_once(void *caller, const void *source,
                                                                           int width, int height,
                                                                           int sourceAdjust, const void *destination,
                                                                           int destinationAdjust)
{
    int index;
    int sourceStride = sourceAdjust + width * 2;
    int destinationStride = destinationAdjust + width * 2;
    /* V12 attribution: retain every distinct geometry written to the widened
     * target.  This remains observational; the bounded 256-entry table keeps
     * replay logging practical while exposing live terrain versus startup/UI
     * sources. */
    if (InterlockedCompareExchange(&g_backgroundProbeLive, 0, 0) == 0 ||
        destinationStride != 1708) return;
    (void)source;
    (void)destination;
    for (index = 0; index < (int)(sizeof(g_pixelCopyGeometries) / sizeof(g_pixelCopyGeometries[0])); ++index) {
        PixelCopyGeometry *entry = &g_pixelCopyGeometries[index];
        if (entry->caller == caller && entry->width == width && entry->height == height &&
            entry->sourceStride == sourceStride && entry->destinationStride == destinationStride) return;
        if (entry->caller == NULL) {
            entry->caller = caller;
            entry->width = width;
            entry->height = height;
            entry->sourceStride = sourceStride;
            entry->destinationStride = destinationStride;
            log_line("PixelCopyGeometry caller=%p size=%dx%d srcStride=%d dstStride=%d",
                     caller, width, height, sourceStride, destinationStride);
            return;
        }
    }
}

/* FUN_00422A98 is a separate opaque 16-bit row copier. Its width argument is
 * bytes, unlike FUN_004239C5's pixel width. */
__attribute__((used, noinline)) static void log_opaque_copy_geometry_once(void *caller, const void *source,
                                                                            int widthBytes, int height,
                                                                            int sourceAdjust, const void *destination,
                                                                            int destinationAdjust)
{
    int index;
    int sourceStride = sourceAdjust + widthBytes;
    int destinationStride = destinationAdjust + widthBytes;
    if (InterlockedCompareExchange(&g_backgroundProbeLive, 0, 0) == 0 ||
        destinationStride != 1708) return;
    (void)source;
    (void)destination;
    for (index = 0; index < (int)(sizeof(g_opaqueCopyGeometries) / sizeof(g_opaqueCopyGeometries[0])); ++index) {
        PixelCopyGeometry *entry = &g_opaqueCopyGeometries[index];
        if (entry->caller == caller && entry->width == widthBytes && entry->height == height &&
            entry->sourceStride == sourceStride && entry->destinationStride == destinationStride) return;
        if (entry->caller == NULL) {
            entry->caller = caller;
            entry->width = widthBytes;
            entry->height = height;
            entry->sourceStride = sourceStride;
            entry->destinationStride = destinationStride;
            log_line("OpaqueCopyGeometry caller=%p bytes=%d height=%d srcStride=%d dstStride=%d",
                     caller, widthBytes, height, sourceStride, destinationStride);
            return;
        }
    }
}
#endif

__attribute__((used, noinline)) static void log_world_renderer_caller_once(void *caller)
{
    int index;
    for (index = 0; index < (int)(sizeof(g_worldRendererCallers) / sizeof(g_worldRendererCallers[0])); ++index) {
        if (g_worldRendererCallers[index] == caller) return;
        if (g_worldRendererCallers[index] == NULL) {
            g_worldRendererCallers[index] = caller;
            log_line("WorldRendererCaller=%p", caller);
            return;
        }
    }
}

/* FUN_005B18F0 receives the active mission state in ECX.  The hook only reads
 * camera coordinates and the shared internal viewport dimensions. */
__attribute__((used, noinline)) static void log_camera_caller_once(void *caller, const BYTE *mission)
{
    int index;
    const BYTE *base = (const BYTE *)GetModuleHandleA(NULL);
    if (!mission || !base) return;
#if ABOM_TIMING_PROBE
    {
        LONG xBits = *(const LONG *)(mission + 0x19c);
        LONG yBits = *(const LONG *)(mission + 0x1a4);
        LONG oldX = InterlockedExchange(&g_cameraProbeLastX, xBits);
        LONG oldY = InterlockedExchange(&g_cameraProbeLastY, yBits);
        InterlockedIncrement(&g_cameraProbeCalls);
        if (InterlockedCompareExchange(&g_cameraProbeInitialized, 1, 0) != 0 &&
            (xBits != oldX || yBits != oldY))
            InterlockedIncrement(&g_cameraProbeChanges);
        InterlockedExchange(&g_renderProbeStartTick, (LONG)GetTickCount());
    }
#endif
    for (index = 0; index < (int)(sizeof(g_cameraCallers) / sizeof(g_cameraCallers[0])); ++index) {
        if (g_cameraCallers[index] == caller) return;
        if (g_cameraCallers[index] == NULL) {
            float cameraX = *(const float *)(mission + 0x19c);
            float cameraY = *(const float *)(mission + 0x1a4);
            int viewportHeight = *(const int *)(base + 0x244ef0);
            int viewportWidth = *(const int *)(base + 0x244eec);
            g_cameraCallers[index] = caller;
            log_line("CameraCaller=%p mission=%p viewport=%dx%d camera=(%.3f,%.3f)",
                     caller, mission, viewportWidth, viewportHeight, cameraX, cameraY);
            return;
        }
    }
}

/* FUN_0059F0A0 requests renderer setup. Its stack arguments are width,
 * height, bpp, and force; ECX is the shared renderer. */
__attribute__((used, noinline)) static void log_mode_request_once(void *caller, int *args, void *renderer)
{
    int index;
    if (!args) return;
#if ABOM_EXPERIMENTAL_WIDESCREEN
    /* This mutation must run for every mission transition.  Caller logging is
     * deliberately one-shot, but renderer setup is repeated for each map. */
    if ((caller == (void *)0x005bb5bd || caller == (void *)0x005bcae8) &&
        args[0] == 640 && args[1] == 480 && args[2] == 16 && args[3] == 0) {
        args[0] = ABOM_WIDESCREEN_WIDTH;
        args[1] = ABOM_WIDESCREEN_HEIGHT;
        log_line("  experimental mission mode replacement=%dx%d caller=%p",
                 args[0], args[1], caller);
        if (caller == (void *)0x005bcae8) {
#if ABOM_EXPERIMENTAL_TERRAIN_REFRESH
            InterlockedExchange(&g_highResLive, 1);
            InterlockedExchange(&g_livePresentCopies, 0);
            log_line("  1920x1080 post-transition capture rearmed");
#endif
#if ABOM_BACKGROUND_PROBE
            InterlockedExchange(&g_backgroundProbeLive, 1);
            log_line("  background geometry probe armed for live mission");
#endif
        }
    }
#endif
    for (index = 0; index < (int)(sizeof(g_modeRequestCallers) / sizeof(g_modeRequestCallers[0])); ++index) {
        if (g_modeRequestCallers[index] == caller) return;
        if (g_modeRequestCallers[index] == NULL) {
            g_modeRequestCallers[index] = caller;
            log_line("ModeRequestCaller=%p renderer=%p request=%dx%dx%d force=%d",
                     caller, renderer, args[0], args[1], args[2], args[3]);
            return;
        }
    }
}

__attribute__((used, noinline)) static void log_presentation_once(void *caller, const BYTE *renderer)
{
    int index;
    unsigned int state;
    int width, height, bpp, flags, windowed, flip;
    if (!renderer) return;
    width = *(const int *)(renderer + 0x14);
    height = *(const int *)(renderer + 0x18);
    bpp = *(const int *)(renderer + 0x24);
    flags = *(const int *)(renderer + 0x30);
    windowed = *(const int *)(renderer + 0x38);
    flip = *(const int *)(renderer + 0x3c);
    state = ((unsigned int)(width & 0xffff) << 16) ^ (unsigned int)(height & 0xffff) ^
            ((unsigned int)(bpp & 0xff) << 24) ^ ((unsigned int)flags * 2654435761u) ^
            ((unsigned int)windowed << 1) ^ ((unsigned int)flip << 2);
    for (index = 0; index < (int)(sizeof(g_presentationCallers) / sizeof(g_presentationCallers[0])); ++index) {
        if (g_presentationCallers[index] == caller) return;
        if (g_presentationCallers[index] == NULL) {
            g_presentationCallers[index] = caller;
            log_line("PresentationCaller=%p renderer=%p mode=%dx%dx%d flags=0x%08x windowed=%d flip=%d",
                     caller, renderer, width, height, bpp, (unsigned int)flags, windowed, flip);
            return;
        }
    }
}

#if ABOM_EXPERIMENTAL_WIDESCREEN
/* Average two RGB565 pixels without allowing carries to cross component
 * boundaries.  Applying this twice produces the 1/4 and 3/4 phases. */
#if ABOM_FRAME_INTERPOLATION_PROBE
static __inline WORD average_rgb565(WORD first, WORD second)
{
    return (WORD)(((first & 0xf7deu) >> 1) +
                  ((second & 0xf7deu) >> 1) +
                  (first & second & 0x0821u));
}

static void copy_tight_rgb565_to_surface(const WORD *source, BYTE *destination,
                                          int destinationPitch)
{
    int y;
    for (y = 0; y < ABOM_WIDESCREEN_HEIGHT; ++y) {
        volatile DWORD *out = (volatile DWORD *)(destination + y * destinationPitch);
        const DWORD *in = (const DWORD *)(source + y * ABOM_WIDESCREEN_WIDTH);
        int x;
        for (x = 0; x < ABOM_WIDESCREEN_WIDTH / 2; ++x) out[x] = in[x];
    }
}

static DWORD WINAPI interpolation_probe_thread(LPVOID unused)
{
    LONG observedSerial = 0;
    (void)unused;
    for (;;) {
        LONG serial;
        LONG phase;
        DWORD elapsed;
        int y;
        Sleep(2);
        if (InterlockedCompareExchange(&g_interpolationPairReady, 0, 0) == 0) continue;
        serial = InterlockedCompareExchange(&g_interpolationFrameSerial, 0, 0);
        elapsed = GetTickCount() - g_interpolationFrameStart;
        phase = (LONG)(elapsed / 16u) + 1;
        if (phase > 4) phase = 4;
        if (serial == observedSerial &&
            phase == InterlockedCompareExchange(&g_interpolationLastPhase, 0, 0)) continue;

        EnterCriticalSection(&g_interpolationLock);
        if (serial != g_interpolationFrameSerial || !g_interpolationPrevious ||
            !g_interpolationCurrent || !g_interpolationDestination) {
            LeaveCriticalSection(&g_interpolationLock);
            continue;
        }
        if (phase == 4) {
            copy_tight_rgb565_to_surface(g_interpolationCurrent,
                                         g_interpolationDestination,
                                         g_interpolationDestinationPitch);
        } else {
            for (y = 0; y < ABOM_WIDESCREEN_HEIGHT; ++y) {
                const WORD *previous = g_interpolationPrevious + y * ABOM_WIDESCREEN_WIDTH;
                const WORD *current = g_interpolationCurrent + y * ABOM_WIDESCREEN_WIDTH;
                volatile WORD *destination =
                    (volatile WORD *)(g_interpolationDestination +
                                      y * g_interpolationDestinationPitch);
                int x;
                for (x = 0; x < ABOM_WIDESCREEN_WIDTH; ++x) {
                    WORD middle = average_rgb565(previous[x], current[x]);
                    destination[x] = phase == 1 ? average_rgb565(previous[x], middle) :
                                     phase == 2 ? middle : average_rgb565(middle, current[x]);
                }
            }
        }
        observedSerial = serial;
        InterlockedExchange(&g_interpolationLastPhase, phase);
        LeaveCriticalSection(&g_interpolationLock);
    }
}
#endif

/* The scanner is authored as one complete 640x480 frame in the upper-left of
 * the mission render target.  Expand it only after every scanner layer has
 * been composed, immediately before the normal high-resolution presentation
 * copy.  Bottom-up/right-to-left traversal makes the in-place enlargement
 * safe because no unread legacy pixel is overwritten. */
#if ABOM_EXPERIMENTAL_HUD_ANCHORS
static void scale_scanner_present_source(BYTE *buffer, int pitch)
{
    int y;
    if (!buffer || pitch < ABOM_WIDESCREEN_WIDTH * 2) return;
    for (y = ABOM_WIDESCREEN_HEIGHT - 1; y >= 0; --y) {
        int x;
        USHORT *destinationRow = (USHORT *)(buffer + y * pitch);
        const USHORT *sourceRow = (const USHORT *)(buffer +
            (y * 480 / ABOM_WIDESCREEN_HEIGHT) * pitch);
        for (x = ABOM_WIDESCREEN_WIDTH - 1; x >= 0; --x)
            destinationRow[x] = sourceRow[x * 640 / ABOM_WIDESCREEN_WIDTH];
    }
}
#endif

/* The original 640-wide conversion receives source/destination pointers plus
 * row adjustments after subtracting 1280.  Preserve that contract, but copy
 * the actual experimental row width. */
__attribute__((used, noinline)) static void experimental_present_copy(const BYTE *source, int sourceAdjust,
                                                                        BYTE *destination, int destinationAdjust)
{
    int row;
    const int sourcePitch = sourceAdjust + 1280;
    const int destinationPitch = destinationAdjust + 1280;
    g_latestPresentPitch = sourcePitch;
#if ABOM_EXPERIMENTAL_HUD_ANCHORS
    if (InterlockedExchange(&g_overlayFrameActive, 0) == 1) {
        LONG scaleNumber;
        scale_scanner_present_source((BYTE *)source, sourcePitch);
        scaleNumber = InterlockedIncrement(&g_overlayPresentScaleCount);
        if (scaleNumber <= 4 || (scaleNumber % 60) == 0) {
            log_line("scanner present scale #%ld source=%p pitch=%d viewport=%dx%d",
                     scaleNumber, source, sourcePitch,
                     ABOM_WIDESCREEN_WIDTH, ABOM_WIDESCREEN_HEIGHT);
        }
    }
#endif
#if ABOM_TIMING_PROBE
    {
        LONG start = InterlockedCompareExchange(&g_renderProbeStartTick, 0, 0);
        if (start != 0) {
            LONG latency = (LONG)(GetTickCount() - (DWORD)start);
            LONG maximum = InterlockedCompareExchange(&g_renderProbeLatencyMaximum, 0, 0);
            InterlockedExchangeAdd(&g_renderProbeLatencyTotal, latency);
            InterlockedIncrement(&g_renderProbeLatencyCount);
            while (latency > maximum) {
                LONG observed = InterlockedCompareExchange(&g_renderProbeLatencyMaximum,
                                                           latency, maximum);
                if (observed == maximum) break;
                maximum = observed;
            }
        }
    }
#endif
#if ABOM_UI_PROBE
    if (InterlockedCompareExchange(&g_overlayCaptureArmed, 0, 1) == 1 &&
        InterlockedCompareExchange(&g_overlayCaptureDone, 1, 0) == 0) {
        BOOL dumped = dump_terrain_source_bmp("abom_1920_overlay_frame.bmp",
                                              source, sourcePitch,
                                              ABOM_WIDESCREEN_WIDTH,
                                              ABOM_WIDESCREEN_HEIGHT);
        log_line("overlay frame dump %s", dumped ? "written" : "failed");
    }
#endif
#if ABOM_EXPERIMENTAL_TERRAIN_REFRESH
    if (InterlockedCompareExchange(&g_highResLive, 0, 0) != 0) {
        LONG copyNumber = InterlockedIncrement(&g_livePresentCopies);
        if (copyNumber <= 4 || (copyNumber % 60) == 0) {
            log_line("live present copy #%ld sourcePitch=%d destinationPitch=%d source=%p destination=%p",
                     copyNumber, sourcePitch, destinationPitch, source, destination);
        }
        if (copyNumber == 1) {
            BOOL dumped = dump_terrain_source_bmp("abom_1920_live_present_source.bmp",
                                                  source, sourcePitch,
                                                  ABOM_WIDESCREEN_WIDTH,
                                                  ABOM_WIDESCREEN_HEIGHT);
            log_line("live present source dump %s", dumped ? "written" : "failed");
        }
    }
#endif
#if ABOM_FRAME_INTERPOLATION_PROBE
    EnterCriticalSection(&g_interpolationLock);
    if (!g_interpolationCurrent) {
        SIZE_T bytes = (SIZE_T)ABOM_WIDESCREEN_WIDTH * ABOM_WIDESCREEN_HEIGHT * 2u;
        g_interpolationPrevious = (WORD *)VirtualAlloc(NULL, bytes, MEM_COMMIT | MEM_RESERVE,
                                                       PAGE_READWRITE);
        g_interpolationCurrent = (WORD *)VirtualAlloc(NULL, bytes, MEM_COMMIT | MEM_RESERVE,
                                                      PAGE_READWRITE);
        if (!g_interpolationPrevious || !g_interpolationCurrent) {
            if (g_interpolationPrevious) VirtualFree(g_interpolationPrevious, 0, MEM_RELEASE);
            if (g_interpolationCurrent) VirtualFree(g_interpolationCurrent, 0, MEM_RELEASE);
            g_interpolationPrevious = g_interpolationCurrent = NULL;
            log_line("frame interpolation allocation failed: %lu", GetLastError());
        }
    } else {
        WORD *swap = g_interpolationPrevious;
        g_interpolationPrevious = g_interpolationCurrent;
        g_interpolationCurrent = swap;
    }
    if (g_interpolationCurrent) {
        for (row = 0; row < ABOM_WIDESCREEN_HEIGHT; ++row) {
            const DWORD *sourcePixels = (const DWORD *)(source + row * sourcePitch);
            DWORD *savedPixels = (DWORD *)(g_interpolationCurrent +
                                           row * ABOM_WIDESCREEN_WIDTH);
            int pixel;
            for (pixel = 0; pixel < ABOM_WIDESCREEN_WIDTH / 2; ++pixel)
                savedPixels[pixel] = sourcePixels[pixel];
        }
        g_interpolationDestination = destination;
        g_interpolationDestinationPitch = destinationPitch;
        if (InterlockedCompareExchange(&g_interpolationPairReady, 0, 0) != 0) {
            copy_tight_rgb565_to_surface(g_interpolationPrevious, destination,
                                         destinationPitch);
        } else if (InterlockedCompareExchange(&g_interpolationFrameSerial, 0, 0) != 0) {
            InterlockedExchange(&g_interpolationPairReady, 1);
            copy_tight_rgb565_to_surface(g_interpolationPrevious, destination,
                                         destinationPitch);
        } else {
            copy_tight_rgb565_to_surface(g_interpolationCurrent, destination,
                                         destinationPitch);
        }
        g_interpolationFrameStart = GetTickCount();
        InterlockedExchange(&g_interpolationLastPhase, 0);
        InterlockedIncrement(&g_interpolationFrameSerial);
        LeaveCriticalSection(&g_interpolationLock);
        return;
    }
    LeaveCriticalSection(&g_interpolationLock);
#endif
    for (row = 0; row < ABOM_WIDESCREEN_HEIGHT; ++row) {
        int pixel;
        /* Volatile prevents the compiler from replacing this with an imported
         * memcpy call.  The hook executes below the game's CRT assumptions,
         * so it must not rely on an optional runtime import. */
        volatile const DWORD *sourcePixels = (volatile const DWORD *)source;
        volatile DWORD *destinationPixels = (volatile DWORD *)destination;
        for (pixel = 0; pixel < ABOM_WIDESCREEN_WIDTH / 2; ++pixel) {
            destinationPixels[pixel] = sourcePixels[pixel];
        }
        source += sourcePitch;
        destination += destinationPitch;
    }
}
#endif

#if ABOM_EXPERIMENTAL_TERRAIN_COPY
/* FUN_0052C0A0 passes asymmetric row remainders in widened modes:
 *   source remainder      = source pitch - live viewport width * 2
 *   destination remainder = destination pitch - 640 * 2
 * The source calculation already follows the live view width, while the
 * destination side is coupled to FUN_00422ADA's fixed 640-pixel row. Recover
 * each real pitch from its own contract, then copy the live dimensions.
 * The source pointer is already camera-relative and the existing camera clamp
 * uses the same live viewport width/height globals. */
__attribute__((used, noinline)) static void __cdecl
hooked_widescreen_terrain_copy(const void *source, int sourceRemainder,
                               void *destination, int destinationRemainder)
{
    const BYTE *sourceRow;
    BYTE *destinationRow;
    int sourcePitch;
    int destinationPitch;
    int width;
    int height;
    int y;
    BOOL firstCopy;

    width = *(volatile int *)(ULONG_PTR)0x00644eec;
    height = *(volatile int *)(ULONG_PTR)0x00644ef0;
    if (width != ABOM_WIDESCREEN_WIDTH || height != ABOM_WIDESCREEN_HEIGHT ||
        !source || !destination) {
        g_fixedTerrainCopy(source, sourceRemainder, destination, destinationRemainder);
        return;
    }

    sourcePitch = sourceRemainder + width * 2;
    destinationPitch = destinationRemainder + 640 * 2;
    firstCopy = InterlockedCompareExchange(&g_terrainCopyLogged, 1, 0) == 0;
    if (firstCopy) {
        log_line("terrain copy viewport=%dx%d sourcePitch=%d destinationPitch=%d source=%p destination=%p",
                 width, height, sourcePitch, destinationPitch, source, destination);
    }
    if (sourcePitch < width * 2 || destinationPitch < width * 2 ||
        (width & 1) != 0 || height <= 0) {
        g_fixedTerrainCopy(source, sourceRemainder, destination, destinationRemainder);
        return;
    }

#if ABOM_TERRAIN_SOURCE_DUMP
    if (firstCopy) {
        BOOL dumped = dump_terrain_source_bmp("abom_v27_terrain_source.bmp",
                                              (const BYTE *)source, sourcePitch,
                                              width, height);
        log_line("terrain source dump %s", dumped ? "written" : "failed");
    }
#endif

    sourceRow = (const BYTE *)source;
    destinationRow = (BYTE *)destination;
    for (y = 0; y < height; ++y) {
        volatile const DWORD *sourcePixel = (volatile const DWORD *)sourceRow;
        volatile DWORD *destinationPixel = (volatile DWORD *)destinationRow;
        int pairs = width / 2;
        while (pairs-- > 0) {
            *destinationPixel++ = *sourcePixel++;
        }
        sourceRow += sourcePitch;
        destinationRow += destinationPitch;
    }
#if ABOM_EXPERIMENTAL_TERRAIN_REFRESH
    if (InterlockedCompareExchange(&g_highResLive, 0, 0) != 0) {
        LONG copyNumber = InterlockedIncrement(&g_liveTerrainCopies);
        if (copyNumber <= 4 || (copyNumber % 60) == 0) {
            log_line("live terrain copy #%ld source=%p destination=%p",
                     copyNumber, source, destination);
        }
        if (copyNumber == 1) {
            BOOL sourceDumped = dump_terrain_source_bmp("abom_1920_live_terrain_source.bmp",
                                                        (const BYTE *)source, sourcePitch,
                                                        width, height);
            BOOL destinationDumped = dump_terrain_source_bmp("abom_1920_live_terrain_destination.bmp",
                                                             (const BYTE *)destination,
                                                             destinationPitch, width, height);
            log_line("live terrain dumps source=%s destination=%s",
                     sourceDumped ? "written" : "failed",
                     destinationDumped ? "written" : "failed");
        }
    }
#endif
#if ABOM_EXPERIMENTAL_HUD_ANCHORS
    /* The pause controller temporarily confines the physical cursor to its
     * centered legacy box.  The pause loop does not execute this world copy;
     * therefore the first resumed gameplay frame is a safe, deterministic
     * place to restore full-viewport cursor bounds. */
    if (InterlockedExchange(&g_pauseBoundsShifted, 0) != 0) {
        BYTE *input = *(BYTE **)(ULONG_PTR)0x006df058;
        if (input) {
            *(LONG *)(input + 0x8b8) = 0;
            *(LONG *)(input + 0x8ac) = width - 1;
            *(LONG *)(input + 0x8bc) = 0;
            *(LONG *)(input + 0x8b0) = height - 1;
            log_line("restored full viewport cursor bounds %dx%d after pause", width, height);
        }
    }
#endif
}
#endif

#if ABOM_EXPERIMENTAL_HUD_ANCHORS
#if ABOM_ANIMATION_PROBE
__attribute__((naked)) static void hooked_animation_frame_select(void)
{
    __asm__ volatile (
        "pushfl\n\t"
        "pushal\n\t"
        "movl 44(%esp), %eax\n\t"
        "movl 40(%esp), %edx\n\t"
        "movl 24(%esp), %ecx\n\t"
        "pushl %eax\n\t"
        "pushl %edx\n\t"
        "pushl %ecx\n\t"
        "call _record_animation_frame\n\t"
        "addl $12, %esp\n\t"
        "popal\n\t"
        "popfl\n\t"
        "movl _g_animationFrameOriginal, %eax\n\t"
        "jmp *%eax\n\t"
    );
}
#endif

__attribute__((naked)) static void hooked_right_hud_sprite_call(void)
{
    __asm__ volatile (
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_WIDTH) ", 0x00644eec\n\t"
        "jne 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_HEIGHT) ", 0x00644ef0\n\t"
        "jne 1f\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_VIEWPORT_X_DELTA) ", 4(%esp)\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_VIEWPORT_Y_DELTA) ", 8(%esp)\n\t"
        "1:\n\t"
        "movl _g_hudSpriteOriginal, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

__attribute__((naked)) static void hooked_right_hud_clip_call(void)
{
    __asm__ volatile (
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_WIDTH) ", 0x00644eec\n\t"
        "jne 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_HEIGHT) ", 0x00644ef0\n\t"
        "jne 1f\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_VIEWPORT_X_DELTA) ", 4(%esp)\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_VIEWPORT_Y_DELTA) ", 8(%esp)\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_VIEWPORT_X_DELTA) ", 12(%esp)\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_VIEWPORT_Y_DELTA) ", 16(%esp)\n\t"
        "1:\n\t"
        "movl _g_hudClipOriginal, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

/* Scale the complete legacy scanner composition while FUN_0058D980 still has
 * its destination surface locked.  The marker sprite has already been
 * blended at this point, so the scan grid and processed live map are included
 * as well.  Bottom-up/right-to-left traversal is overlap-safe. */
__attribute__((used, noinline)) static void scale_locked_overlay_composition(USHORT *buffer)
{
    const int pitch = ABOM_WIDESCREEN_WIDTH * 2;
    LONG scaleNumber;
    int y;
    if (InterlockedExchange(&g_overlayRasterScaleArmed, 0) != 1 ||
        !buffer || !g_overlayLegacySnapshot) return;
    /* Preserve the newly completed legacy scanner before touching the live
     * target.  Every expanded pixel below comes from this isolated snapshot,
     * never from pixels enlarged during a previous scanner frame. */
    for (y = 0; y < 480; ++y) {
        CopyMemory(g_overlayLegacySnapshot + y * 640,
                   (const BYTE *)buffer + y * pitch, 640 * sizeof(WORD));
    }
    InterlockedExchange(&g_overlayLegacySnapshotValid, 1);
    InterlockedExchange(&g_overlaySourceRestorePending, 1);
    scaleNumber = InterlockedIncrement(&g_overlayUnlockScaleCount);
    if (scaleNumber <= 4 || (scaleNumber % 60) == 0) {
        log_line("scanner unlock scale #%ld buffer=%p pitch=%d viewport=%dx%d",
                 scaleNumber, buffer, pitch,
                 ABOM_WIDESCREEN_WIDTH, ABOM_WIDESCREEN_HEIGHT);
    }
    for (y = ABOM_WIDESCREEN_HEIGHT - 1; y >= 0; --y) {
        int x;
        USHORT *destinationRow = (USHORT *)((BYTE *)buffer + y * pitch);
        const USHORT *sourceRow = g_overlayLegacySnapshot +
            (y * 480 / ABOM_WIDESCREEN_HEIGHT) * 640;
        for (x = ABOM_WIDESCREEN_WIDTH - 1; x >= 0; --x)
            destinationRow[x] = sourceRow[x * 640 / ABOM_WIDESCREEN_WIDTH];
    }
}

typedef void *(__attribute__((thiscall)) *OverlaySurfaceLockFn)(void *, int);
typedef void (__attribute__((thiscall)) *OverlaySurfaceUnlockFn)(void *, int);

/* FUN_0054E0D0 is the complete live-world/scanner frame.  Scanner drawing
 * reuses the same mission target that held the preceding expanded frame.
 * Restore the previous clean 640x480 scanner while the target is explicitly
 * locked, before any current-frame map/grid/overlay operations begin. */
__attribute__((used, noinline)) static void restore_overlay_legacy_before_frame(void)
{
    void *manager;
    WORD *buffer;
    int y;
    /* The draw-time UI state is not stable at this earlier frame boundary.
     * Each completed scanner unlock therefore arms exactly one restore for
     * the following world frame.  A non-scanner frame does not re-arm it. */
    if (InterlockedExchange(&g_overlaySourceRestorePending, 0) != 1 ||
        InterlockedCompareExchange(&g_overlayLegacySnapshotValid, 0, 0) == 0 ||
        !g_overlayLegacySnapshot)
        return;
    manager = *(void **)0x006df054;
    if (!manager) return;
    buffer = (WORD *)((OverlaySurfaceLockFn)((BYTE *)GetModuleHandleA(NULL) +
                                              0x1a00e0))(manager, -2);
    if (!buffer) return;
    for (y = 0; y < 480; ++y) {
        CopyMemory((BYTE *)buffer + y * ABOM_WIDESCREEN_WIDTH * 2,
                   g_overlayLegacySnapshot + y * 640, 640 * sizeof(WORD));
    }
    {
        LONG restoreNumber = InterlockedIncrement(&g_overlaySourceRestoreCount);
        if (restoreNumber <= 4 || (restoreNumber % 60) == 0) {
            log_line("scanner source restore #%ld buffer=%p pitch=%d",
                     restoreNumber, buffer, ABOM_WIDESCREEN_WIDTH * 2);
        }
    }
    ((OverlaySurfaceUnlockFn)((BYTE *)GetModuleHandleA(NULL) + 0x1a0210))(
        manager, -2);
}

__attribute__((naked)) static void hooked_overlay_world_frame(void)
{
    __asm__ volatile (
        "pushfl\n\t"
        "pushal\n\t"
        "call _restore_overlay_legacy_before_frame\n\t"
        "popal\n\t"
        "popfl\n\t"
        "movl _g_overlayWorldFrameOriginal, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

/* Arm the scanner's exact sprite-helper unlock.  The caller has already
 * composed its processed map and scan primitives before this final 640x480
 * overlay sprite, so expanding the locked target here captures the complete
 * scanner without depending on the normal terrain presentation path. */
__attribute__((naked)) static void hooked_scaled_overlay_sprite_call(void)
{
    __asm__ volatile (
        "movl $1, _g_overlayRasterScaleArmed\n\t"
        "movl _g_hudSpriteOriginal, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

/* FUN_0058D980 keeps the locked destination pointer in ESI at this exact
 * unlock call.  Scale it before forwarding the original renderer unlock. */
__attribute__((naked)) static void hooked_overlay_surface_unlock(void)
{
    __asm__ volatile (
        "pushal\n\t"
        "pushl 4(%esp)\n\t"
        "call _scale_locked_overlay_composition\n\t"
        "addl $4, %esp\n\t"
        "popal\n\t"
        "movl _g_overlayUnlockOriginal, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

__attribute__((naked)) static void hooked_bottom_left_hud_sprite_call(void)
{
    __asm__ volatile (
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_WIDTH) ", 0x00644eec\n\t"
        "jne 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_HEIGHT) ", 0x00644ef0\n\t"
        "jne 1f\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_VIEWPORT_Y_DELTA) ", 8(%esp)\n\t"
        "1:\n\t"
        "movl _g_hudSpriteOriginal, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

__attribute__((naked)) static void hooked_centered_loading_sprite_call(void)
{
    __asm__ volatile (
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_WIDTH) ", 0x00644eec\n\t"
        "jne 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_HEIGHT) ", 0x00644ef0\n\t"
        "jne 1f\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_UI_X_OFFSET) ", 4(%esp)\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_UI_Y_OFFSET) ", 8(%esp)\n\t"
        "1:\n\t"
        "movl _g_hudSpriteOriginal, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

__attribute__((naked)) static void hooked_centered_interface_sprite_call(void)
{
    __asm__ volatile (
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_WIDTH) ", 0x00644eec\n\t"
        "jne 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_HEIGHT) ", 0x00644ef0\n\t"
        "jne 1f\n\t"
        /* The pause controller is constructed before this DLL's asynchronous
         * patches are installed.  Translate its restored legacy confinement
         * rectangle on the first live pause draw instead. */
        "movl 0x006df058, %eax\n\t"
        "cmpl $186, 0x8b8(%eax)\n\t"
        "jne 2f\n\t"
        "cmpl $460, 0x8ac(%eax)\n\t"
        "jne 2f\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_UI_Y_OFFSET) ", 0x8bc(%eax)\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_UI_Y_OFFSET) ", 0x8b0(%eax)\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_UI_X_OFFSET) ", 0x8b8(%eax)\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_UI_X_OFFSET) ", 0x8ac(%eax)\n\t"
        "2:\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_UI_X_OFFSET) ", 4(%esp)\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_UI_Y_OFFSET) ", 8(%esp)\n\t"
        "1:\n\t"
        "movl _g_hudSpriteOriginal, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

__attribute__((naked)) static void hooked_centered_interface_clip_call(void)
{
    __asm__ volatile (
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_WIDTH) ", 0x00644eec\n\t"
        "jne 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_HEIGHT) ", 0x00644ef0\n\t"
        "jne 1f\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_UI_X_OFFSET) ", 4(%esp)\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_UI_Y_OFFSET) ", 8(%esp)\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_UI_X_OFFSET) ", 12(%esp)\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_UI_Y_OFFSET) ", 16(%esp)\n\t"
        "1:\n\t"
        "movl _g_hudClipOriginal, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

/* The loading worker's copy at 0059EC08 draws the animated red bar.  Its
 * fifth cdecl argument is the display destination pointer; moving only that
 * pointer keeps the source animation and row geometry byte-for-byte intact. */
__attribute__((naked)) static void hooked_centered_loading_bar_copy(void)
{
    __asm__ volatile (
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_WIDTH) ", 0x00644eec\n\t"
        "jne 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_HEIGHT) ", 0x00644ef0\n\t"
        "jne 1f\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_LOADING_POINTER_DELTA) ", 20(%esp)\n\t"
        "1:\n\t"
        /* This renderer dispatch is initialized after our DLL.  Resolve it
         * at draw time rather than caching its startup value. */
        "movl 0x00644e6c, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

/* FUN_0054DF70 draws the flashing map-limit outline.  Its horizontal calls
 * always end at the legacy right edge; only that endpoint expands. */
__attribute__((naked)) static void hooked_widescreen_edge_horizontal_line(void)
{
    __asm__ volatile (
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_WIDTH) ", 0x00644eec\n\t"
        "jne 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_HEIGHT) ", 0x00644ef0\n\t"
        "jne 1f\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_VIEWPORT_X_DELTA) ", 12(%esp)\n\t"
        "1:\n\t"
        "movl _g_edgeLineOriginal, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

/* State 2 is the left vertical flash and stays at x=0.  Other nonzero states
 * select the right flash, whose two x coordinates move from 639 to 853. */
__attribute__((naked)) static void hooked_widescreen_edge_vertical_line(void)
{
    __asm__ volatile (
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_WIDTH) ", 0x00644eec\n\t"
        "jne 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_HEIGHT) ", 0x00644ef0\n\t"
        "jne 1f\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_VIEWPORT_Y_DELTA) ", 16(%esp)\n\t"
        "movl 0x00693870, %eax\n\t"
        "cmpl $2, 0x2428(%eax)\n\t"
        "je 1f\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_VIEWPORT_X_DELTA) ", 4(%esp)\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_VIEWPORT_X_DELTA) ", 12(%esp)\n\t"
        "1:\n\t"
        "movl _g_edgeLineOriginal, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

#if ABOM_UI_PROBE
/* Keep the UI manager's cursor in physical widescreen coordinates, then
 * convert only FUN_005E0290's saved x local for the legacy pause hit tests.
 * Altering the getter itself also shifted UI state consumed outside those
 * tests, which is why the first attempt did not move interaction correctly. */
__attribute__((naked)) static void hooked_centered_pause_cursor_update(void)
{
    __asm__ volatile (
        "movl 4(%esp), %eax\n\t"
        "movl 8(%esp), %edx\n\t"
        "movl %eax, _g_pauseMouseRawX\n\t"
        "movl %eax, 0x87d4(%ecx)\n\t"
        "movl %edx, 0x87d8(%ecx)\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_WIDTH) ", 0x00644eec\n\t"
        "jne 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_HEIGHT) ", 0x00644ef0\n\t"
        "jne 1f\n\t"
        "subl $" ABOM_STRINGIFY(ABOM_UI_X_OFFSET) ", %eax\n\t"
        "subl $" ABOM_STRINGIFY(ABOM_UI_X_OFFSET) ", 28(%esp)\n\t"
        "subl $" ABOM_STRINGIFY(ABOM_UI_Y_OFFSET) ", 32(%esp)\n\t"
        "1:\n\t"
        "movl %eax, _g_pauseMouseAdjustedX\n\t"
        "ret $8\n\t"
    );
}

/* Diagnostic-only state attribution for the five pause interaction handlers.
 * ESI is FUN_005E0290's controller at this caller; the original getter is
 * tail-called without changing its return value or any input state. */
__attribute__((naked)) static void hooked_pause_state_probe_mouse_x(void)
{
    __asm__ volatile (
        "movl 0x74(%esi), %eax\n\t"
        "movl %eax, _g_pauseControllerState\n\t"
        "movl _g_pauseMouseXOriginal, %eax\n\t"
        "jmp *%eax\n\t"
    );
}
#endif

/* The pause controller keeps physical cursor coordinates in the UI manager,
 * but its state handlers compare caller locals against legacy 640x480 bounds.
 * Translate only those two saved locals so all existing hit tests follow the
 * centered high-resolution artwork without rewriting every state table. */
__attribute__((naked)) static void hooked_centered_pause_coordinates(void)
{
    __asm__ volatile (
        "movl 4(%esp), %eax\n\t"
        "movl 8(%esp), %edx\n\t"
        "movl %eax, 0x87d4(%ecx)\n\t"
        "movl %edx, 0x87d8(%ecx)\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_WIDTH) ", 0x00644eec\n\t"
        "jne 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_HEIGHT) ", 0x00644ef0\n\t"
        "jne 1f\n\t"
        "subl $" ABOM_STRINGIFY(ABOM_UI_X_OFFSET) ", 28(%esp)\n\t"
        "subl $" ABOM_STRINGIFY(ABOM_UI_Y_OFFSET) ", 32(%esp)\n\t"
        "1:\n\t"
        "ret $8\n\t"
    );
}

/* The scanner remains a self-contained 640x480 interface which is scaled at
 * presentation time.  Feed only scanner state 8 the inverse coordinate
 * transform so its original hit tests continue to match the stretched view. */
__attribute__((naked)) static void hooked_scaled_overlay_coordinates(void)
{
    __asm__ volatile (
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_WIDTH) ", 0x00644eec\n\t"
        "jne 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_HEIGHT) ", 0x00644ef0\n\t"
        "jne 1f\n\t"
        "cmpl $8, 0x0c(%ecx)\n\t"
        "jne 1f\n\t"
        "movl 4(%esp), %eax\n\t"
        "imull $640, %eax\n\t"
        "xorl %edx, %edx\n\t"
        "divl 0x00644eec\n\t"
        "movl %eax, 4(%esp)\n\t"
        "movl 8(%esp), %eax\n\t"
        "imull $480, %eax\n\t"
        "xorl %edx, %edx\n\t"
        "divl 0x00644ef0\n\t"
        "movl %eax, 8(%esp)\n\t"
        "1:\n\t"
        "movl _g_overlayCoordinatesOriginal, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

__attribute__((naked)) static void hooked_centered_pause_briefing(void)
{
    __asm__ volatile (
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_WIDTH) ", 0x00644eec\n\t"
        "jne 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_HEIGHT) ", 0x00644ef0\n\t"
        "jne 1f\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_UI_X_OFFSET) ", 4(%esp)\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_UI_Y_OFFSET) ", 8(%esp)\n\t"
        "1:\n\t"
        "movl _g_pauseBriefingOriginal, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

/* The pause constructor installs a mouse confinement rectangle through
 * FUN_005A3100.  Reproduce that tiny six-argument setter at its single pause
 * call site and translate only the horizontal min/max by the menu's +107. */
__attribute__((naked)) static void hooked_centered_pause_input_bounds(void)
{
    __asm__ volatile (
        "movl $1, _g_pauseBoundsShifted\n\t"
        "movl 4(%esp), %eax\n\t"
        "movl 12(%esp), %edx\n\t"
        "cmpl %edx, %eax\n\t"
        "jge 1f\n\t"
        "movl %eax, 0x8b8(%ecx)\n\t"
        "movl %edx, 0x8ac(%ecx)\n\t"
        "jmp 2f\n\t"
        "1:\n\t"
        "movl %edx, 0x8b8(%ecx)\n\t"
        "movl %eax, 0x8ac(%ecx)\n\t"
        "2:\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_UI_X_OFFSET) ", 0x8b8(%ecx)\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_UI_X_OFFSET) ", 0x8ac(%ecx)\n\t"
        "movl 8(%esp), %eax\n\t"
        "movl 16(%esp), %edx\n\t"
        "cmpl %edx, %eax\n\t"
        "jge 4f\n\t"
        "movl %eax, 0x8bc(%ecx)\n\t"
        "movl %edx, 0x8b0(%ecx)\n\t"
        "jmp 5f\n\t"
        "4:\n\t"
        "movl %edx, 0x8bc(%ecx)\n\t"
        "movl %eax, 0x8b0(%ecx)\n\t"
        "5:\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_UI_Y_OFFSET) ", 0x8bc(%ecx)\n\t"
        "addl $" ABOM_STRINGIFY(ABOM_UI_Y_OFFSET) ", 0x8b0(%ecx)\n\t"
        "movl 20(%esp), %eax\n\t"
        "movl 24(%esp), %edx\n\t"
        "cmpl %edx, %eax\n\t"
        "jge 6f\n\t"
        "movl %eax, 0x8c0(%ecx)\n\t"
        "movl %edx, 0x8b4(%ecx)\n\t"
        "ret $24\n\t"
        "6:\n\t"
        "movl %edx, 0x8c0(%ecx)\n\t"
        "movl %eax, 0x8b4(%ecx)\n\t"
        "ret $24\n\t"
    );
}

/* The quick-action slot writer receives a legacy-clamped x=574 whenever the
 * pointer is farther right than a 66-pixel box can fit in 640 pixels.  Rebuild
 * that one capped value from the live pointer and the 854-pixel right edge;
 * all ordinary (unclamped) positions remain byte-for-byte unchanged. */
__attribute__((naked)) static void hooked_widescreen_quick_action_x_store(void)
{
    __asm__ volatile (
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_WIDTH) ", 0x00644eec\n\t"
        "jne 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_HEIGHT) ", 0x00644ef0\n\t"
        "jne 1f\n\t"
        "cmpl $574, %edx\n\t"
        "jne 1f\n\t"
        "cmpl $574, 0x87d4(%esi)\n\t"
        "jle 1f\n\t"
        "movl 0x87d4(%esi), %edx\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_QUICK_ACTION_MAX_X) ", %edx\n\t"
        "jle 1f\n\t"
        "movl $" ABOM_STRINGIFY(ABOM_QUICK_ACTION_MAX_X) ", %edx\n\t"
        "1:\n\t"
        "movl %edx, 0x330(%esi,%ecx,4)\n\t"
        "ret\n\t"
    );
}

/* FUN_0054D6F0 is the definitive quick-action draw path.  Correct the live
 * slot immediately before it loads x, then persist that value so the next
 * frame's hit testing uses the same rectangle as rendering. */
__attribute__((naked)) static void hooked_widescreen_quick_action_x_load(void)
{
    __asm__ volatile (
        "movl 0x330(%esi,%ecx,4), %ebx\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_WIDTH) ", 0x00644eec\n\t"
        "jne 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_HEIGHT) ", 0x00644ef0\n\t"
        "jne 1f\n\t"
        "cmpl $574, %ebx\n\t"
        "jne 1f\n\t"
        "cmpl $574, 0x87d4(%esi)\n\t"
        "jle 1f\n\t"
        "movl 0x87d4(%esi), %ebx\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_QUICK_ACTION_MAX_X) ", %ebx\n\t"
        "jle 2f\n\t"
        "movl $" ABOM_STRINGIFY(ABOM_QUICK_ACTION_MAX_X) ", %ebx\n\t"
        "2:\n\t"
        "movl %ebx, 0x330(%esi,%ecx,4)\n\t"
        "1:\n\t"
        "ret\n\t"
    );
}

__attribute__((naked)) static void hooked_widescreen_quick_action_y_store(void)
{
    __asm__ volatile (
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_WIDTH) ", 0x00644eec\n\t"
        "jne 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_HEIGHT) ", 0x00644ef0\n\t"
        "jne 1f\n\t"
        /* In the high-resolution path the legacy drag/hotspot field can
         * retain a large vertical delta (279 was observed at 1080p), even
         * though ordinary 854x480 popup origins equal the physical cursor. */
        "movl 0x006df07c, %eax\n\t"
        "movl 0x87d8(%eax), %edx\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_QUICK_ACTION_MAX_Y) ", %edx\n\t"
        "jle 1f\n\t"
        "movl $" ABOM_STRINGIFY(ABOM_QUICK_ACTION_MAX_Y) ", %edx\n\t"
        "1:\n\t"
        "movl %edx, 0x334(%esi,%ecx,4)\n\t"
        "ret\n\t"
    );
}

__attribute__((naked)) static void hooked_widescreen_quick_action_y_load(void)
{
    __asm__ volatile (
        "movl 0x334(%esi,%ecx,4), %edi\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_WIDTH) ", 0x00644eec\n\t"
        "jne 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_HEIGHT) ", 0x00644ef0\n\t"
        "jne 1f\n\t"
        /* The stock 640x480 draw path clamps the 66x87 popup to y=393.
         * Rebuild that sentinel from the active UI manager's physical cursor;
         * ESI is the draw object here and is not always the active manager. */
        "cmpl $393, %edi\n\t"
        "jne 1f\n\t"
        "movl 0x006df07c, %eax\n\t"
        "cmpl $393, 0x87d8(%eax)\n\t"
        "jle 1f\n\t"
        "movl 0x87d8(%eax), %edi\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_QUICK_ACTION_MAX_Y) ", %edi\n\t"
        "jle 2f\n\t"
        "movl $" ABOM_STRINGIFY(ABOM_QUICK_ACTION_MAX_Y) ", %edi\n\t"
        "2:\n\t"
        "movl %edi, 0x334(%esi,%ecx,4)\n\t"
        "1:\n\t"
        "ret\n\t"
    );
}
#endif

__attribute__((naked)) static void hooked_sprite_blitter(void)
{
    __asm__ volatile (
        "pushfl\n\t"
        "pushal\n\t"
        "movl 52(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 52(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 52(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 52(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 52(%esp), %edx\n\t"
        "pushl %edx\n\t"
#if ABOM_BACKGROUND_PROBE
        "call _log_live_sprite_geometry_once\n\t"
#else
        "call _log_sprite_blitter_caller_once\n\t"
#endif
        "addl $20, %esp\n\t"
        "popal\n\t"
        "popfl\n\t"
        "movl _g_spriteBlitterTrampoline, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

__attribute__((naked)) static void hooked_tiled_compositor(void)
{
    __asm__ volatile (
        "pushfl\n\t"
        "pushal\n\t"
        "movl 36(%esp), %eax\n\t"
        "pushl %eax\n\t"
        "call _log_compositor_caller_once\n\t"
        "addl $4, %esp\n\t"
        "popal\n\t"
        "popfl\n\t"
        "movl _g_compositorTrampoline, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

__attribute__((naked)) static void hooked_object_blitter(void)
{
    __asm__ volatile (
        "pushfl\n\t"
        "pushal\n\t"
        "movl 36(%esp), %eax\n\t"
        "pushl %eax\n\t"
        "call _log_object_blitter_caller_once\n\t"
        "addl $4, %esp\n\t"
        "popal\n\t"
        "popfl\n\t"
        "movl _g_objectBlitterTrampoline, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

__attribute__((naked)) static void hooked_pixel_copy(void)
{
    __asm__ volatile (
        "pushfl\n\t"
        "pushal\n\t"
#if ABOM_BACKGROUND_PROBE
        /* At entry, after saved flags/registers, original return is +36 and
         * the six cdecl arguments begin at +40.  Reusing +60 while pushing
         * walks backward through those original stack slots. */
        "movl 60(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 60(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 60(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 60(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 60(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 60(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 60(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "call _log_pixel_copy_geometry_once\n\t"
        "addl $28, %esp\n\t"
#endif
        "movl 36(%esp), %eax\n\t"
        "pushl %eax\n\t"
        "call _log_pixel_copy_caller_once\n\t"
        "addl $4, %esp\n\t"
        "popal\n\t"
        "popfl\n\t"
        "movl _g_pixelCopyTrampoline, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

#if ABOM_BACKGROUND_PROBE
__attribute__((naked)) static void hooked_opaque_copy(void)
{
    __asm__ volatile (
        /* Six cdecl arguments: source, byte width, height, source row
         * adjustment, destination, destination row adjustment. */
        "pushfl\n\t"
        "pushal\n\t"
        "movl 60(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 60(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 60(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 60(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 60(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 60(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 60(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "call _log_opaque_copy_geometry_once\n\t"
        "addl $28, %esp\n\t"
        "popal\n\t"
        "popfl\n\t"
        "movl _g_opaqueCopyTrampoline, %eax\n\t"
        "jmp *%eax\n\t"
    );
}
#endif

#if ABOM_BACKGROUND_PROBE
/* FUN_0058DC00 has `this` in ECX and four stack parameters: x, y, sprite,
 * route. Preserve every register and flags before resuming the original code. */
__attribute__((naked)) static void hooked_surface_dispatch(void)
{
    __asm__ volatile (
        "pushfl\n\t"
        "pushal\n\t"
        /* Reverse-push route, sprite, y, x, saved ECX/this, return address. */
        "movl 52(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 52(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 52(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 52(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 40(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "movl 56(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "call _log_surface_dispatch_once\n\t"
        "addl $24, %esp\n\t"
        "popal\n\t"
        "popfl\n\t"
        "movl _g_surfaceDispatchTrampoline, %eax\n\t"
        "jmp *%eax\n\t"
    );
}
#endif

#if ABOM_BACKGROUND_PROBE
__attribute__((naked)) static void hooked_surface_unlock(void)
{
    __asm__ volatile (
        "pushfl\n\t"
        "pushal\n\t"
        "movl 36(%esp), %eax\n\t"
        "pushl %eax\n\t"
        "call _log_surface_unlock_caller_once\n\t"
        "addl $4, %esp\n\t"
        "popal\n\t"
        "popfl\n\t"
        "movl _g_surfaceUnlockTrampoline, %eax\n\t"
        "jmp *%eax\n\t"
    );
}
#endif

#if ABOM_BACKGROUND_PROBE
__attribute__((naked)) static void hooked_surface_lock_helper(void)
{
    __asm__ volatile (
        "pushfl\n\t"
        "pushal\n\t"
        /* reverse-push selector, saved ECX/this, and original return address */
        "movl 40(%esp), %eax\n\t"
        "pushl %eax\n\t"
        "movl 28(%esp), %eax\n\t"
        "pushl %eax\n\t"
        "movl 44(%esp), %eax\n\t"
        "pushl %eax\n\t"
        "call _log_surface_lock_helper_caller_once\n\t"
        "addl $12, %esp\n\t"
        "popal\n\t"
        "popfl\n\t"
        "movl _g_surfaceLockHelperTrampoline, %eax\n\t"
        "jmp *%eax\n\t"
    );
}
#endif

__attribute__((naked)) static void hooked_world_renderer(void)
{
    __asm__ volatile (
        "pushfl\n\t"
        "pushal\n\t"
        "movl 36(%esp), %eax\n\t"
        "pushl %eax\n\t"
        "call _log_world_renderer_caller_once\n\t"
        "addl $4, %esp\n\t"
        "popal\n\t"
        "popfl\n\t"
        "movl _g_worldRendererTrampoline, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

__attribute__((naked)) static void hooked_camera_controller(void)
{
    __asm__ volatile (
        "pushfl\n\t"
        "pushal\n\t"
        "movl 36(%esp), %eax\n\t"
        "movl 24(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "pushl %eax\n\t"
        "call _log_camera_caller_once\n\t"
        "addl $8, %esp\n\t"
        "popal\n\t"
        "popfl\n\t"
        "movl _g_cameraTrampoline, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

__attribute__((naked)) static void hooked_mode_request(void)
{
    __asm__ volatile (
        "pushfl\n\t"
        "pushal\n\t"
        "movl 24(%esp), %eax\n\t"
        "leal 36(%esp), %edx\n\t"
        "leal 4(%edx), %ecx\n\t"
        "pushl %eax\n\t"
        "pushl %ecx\n\t"
        "pushl (%edx)\n\t"
        "call _log_mode_request_once\n\t"
        "addl $12, %esp\n\t"
        "popal\n\t"
        "popfl\n\t"
        "movl _g_modeRequestTrampoline, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

__attribute__((naked)) static void hooked_presentation(void)
{
    __asm__ volatile (
        "pushfl\n\t"
        "pushal\n\t"
        "movl 36(%esp), %eax\n\t"
        "movl 24(%esp), %edx\n\t"
        "pushl %edx\n\t"
        "pushl %eax\n\t"
        "call _log_presentation_once\n\t"
        "addl $8, %esp\n\t"
        "popal\n\t"
        "popfl\n\t"
        "movl _g_presentationTrampoline, %eax\n\t"
        "jmp *%eax\n\t"
    );
}

#if ABOM_EXPERIMENTAL_WIDESCREEN
__attribute__((naked, used)) void hooked_widescreen_present_copy(void)
{
    __asm__ volatile (
        "emms\n\t"
        "pushfl\n\t"
        "pushal\n\t"
        "movl 40(%esp), %eax\n\t"
        "movl 44(%esp), %edx\n\t"
        "movl 48(%esp), %ecx\n\t"
        "movl 52(%esp), %ebx\n\t"
        "pushl %ebx\n\t"
        "pushl %ecx\n\t"
        "pushl %edx\n\t"
        "pushl %eax\n\t"
        "call _experimental_present_copy\n\t"
        "addl $16, %esp\n\t"
        "popal\n\t"
        "popfl\n\t"
        /* The original FUN_00422ADA/FUN_00422BCE routines are cdecl: their
         * caller owns the four stack arguments, so this detour must not pop
         * them on return. */
        "ret\n\t"
    );
}

/* The original presenter has two fixed 640-wide branches.  The width
 * dispatchers preserve the original 640 and 800 routes, passing only the
 * selected experimental width into the original height/bpp checks. */
__attribute__((naked)) static void hooked_ada_width_dispatch(void)
{
    __asm__ volatile (
        "cmpl $0x280, %ecx\n\t"
        "je 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_WIDTH) ", %ecx\n\t"
        "je 1f\n\t"
        "pushl _g_adaWidthFallback\n\t"
        "ret\n\t"
        "1:\n\t"
        "pushl _g_adaWidthSuccess\n\t"
        "ret\n\t"
    );
}

__attribute__((naked)) static void hooked_bce_width_dispatch(void)
{
    __asm__ volatile (
        "cmpl $0x280, %ecx\n\t"
        "je 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_WIDTH) ", %ecx\n\t"
        "je 1f\n\t"
        "pushl _g_bceWidthFallback\n\t"
        "ret\n\t"
        "1:\n\t"
        "pushl _g_bceWidthSuccess\n\t"
        "ret\n\t"
    );
}

/* Width and height are separate checks in the original presenter.  Accepting
 * an experimental width without also accepting its height reaches the legacy
 * 800x600 fallback and silently skips the full-frame copy.  Keep 640x480
 * available for movies and menus while adding the selected internal height. */
__attribute__((naked)) static void hooked_ada_height_dispatch(void)
{
    __asm__ volatile (
        "cmpl $0x1e0, 0x18(%esi)\n\t"
        "je 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_HEIGHT) ", 0x18(%esi)\n\t"
        "je 1f\n\t"
        "pushl _g_adaHeightFallback\n\t"
        "ret\n\t"
        "1:\n\t"
        "pushl _g_adaHeightSuccess\n\t"
        "ret\n\t"
    );
}

__attribute__((naked)) static void hooked_bce_height_dispatch(void)
{
    __asm__ volatile (
        "cmpl $0x1e0, 0x18(%esi)\n\t"
        "je 1f\n\t"
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_HEIGHT) ", 0x18(%esi)\n\t"
        "je 1f\n\t"
        "pushl _g_bceHeightFallback\n\t"
        "ret\n\t"
        "1:\n\t"
        "pushl _g_bceHeightSuccess\n\t"
        "ret\n\t"
    );
}

/* Retain the game's original routine for every non-selected pitch.  The
 * caller owns the cdecl arguments, so a tail jump preserves its complete ABI. */
__attribute__((naked)) static void hooked_ada_copy_dispatch(void)
{
    __asm__ volatile (
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_PRESENT_ADJUST) ", 8(%esp)\n\t"
        "je 1f\n\t"
        "pushl _g_adaCopyOriginal\n\t"
        "ret\n\t"
        "1:\n\t"
        "jmp _hooked_widescreen_present_copy\n\t"
    );
}

__attribute__((naked)) static void hooked_bce_copy_dispatch(void)
{
    __asm__ volatile (
        "cmpl $" ABOM_STRINGIFY(ABOM_WIDESCREEN_PRESENT_ADJUST) ", 8(%esp)\n\t"
        "je 1f\n\t"
        "pushl _g_bceCopyOriginal\n\t"
        "ret\n\t"
        "1:\n\t"
        "jmp _hooked_widescreen_present_copy\n\t"
    );
}
#endif

static void hook_sprite_blitter(void)
{
    BYTE *target = (BYTE *)GetModuleHandleA(NULL) + 0x18d980;
    BYTE *trampoline;
    DWORD oldProtect;
    INT32 relative;

    /* The original entry is: A1 64 38 69 00 83 EC 14.  Copying all eight
     * bytes preserves whole instructions before the trampoline returns at
     * target+8.  Refuse unknown builds rather than patching blindly. */
    if (target[0] != 0xa1 || target[1] != 0x64 || target[2] != 0x38 ||
        target[3] != 0x69 || target[4] != 0x00 || target[5] != 0x83 ||
        target[6] != 0xec || target[7] != 0x14) {
        log_line("sprite blitter signature mismatch at %p; hook not installed", target);
        return;
    }
    trampoline = (BYTE *)VirtualAlloc(NULL, 13, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline) {
        log_line("sprite blitter trampoline allocation failed: %lu", GetLastError());
        return;
    }
    CopyMemory(trampoline, target, 8);
    trampoline[8] = 0xe9;
    relative = (INT32)((target + 8) - (trampoline + 13));
    CopyMemory(trampoline + 9, &relative, sizeof(relative));
    if (!VirtualProtect(target, 8, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        log_line("sprite blitter patch protection failed: %lu", GetLastError());
        VirtualFree(trampoline, 0, MEM_RELEASE);
        return;
    }
    g_spriteBlitterTrampoline = trampoline;
    target[0] = 0xe9;
    relative = (INT32)((BYTE *)hooked_sprite_blitter - (target + 5));
    CopyMemory(target + 1, &relative, sizeof(relative));
    target[5] = 0x90;
    target[6] = 0x90;
    target[7] = 0x90;
    FlushInstructionCache(GetCurrentProcess(), target, 8);
    VirtualProtect(target, 8, oldProtect, &oldProtect);
    log_line("installed sprite blitter caller hook target=%p trampoline=%p", target, trampoline);
}

static void hook_tiled_compositor(void)
{
    BYTE *target = (BYTE *)GetModuleHandleA(NULL) + 0x11ad40;
    BYTE *trampoline;
    DWORD oldProtect;
    INT32 relative;

    /* The original entry is "sub esp,0x488".  Keep the hook build-specific. */
    if (target[0] != 0x81 || target[1] != 0xec || target[2] != 0x88 ||
        target[3] != 0x04 || target[4] != 0x00 || target[5] != 0x00) {
        log_line("tiled compositor signature mismatch at %p; hook not installed", target);
        return;
    }
    trampoline = (BYTE *)VirtualAlloc(NULL, 11, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline) {
        log_line("tiled compositor trampoline allocation failed: %lu", GetLastError());
        return;
    }
    CopyMemory(trampoline, target, 6);
    trampoline[6] = 0xe9;
    relative = (INT32)((target + 6) - (trampoline + 11));
    CopyMemory(trampoline + 7, &relative, sizeof(relative));
    if (!VirtualProtect(target, 6, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        log_line("tiled compositor patch protection failed: %lu", GetLastError());
        VirtualFree(trampoline, 0, MEM_RELEASE);
        return;
    }
    g_compositorTrampoline = trampoline;
    target[0] = 0xe9;
    relative = (INT32)((BYTE *)hooked_tiled_compositor - (target + 5));
    CopyMemory(target + 1, &relative, sizeof(relative));
    target[5] = 0x90;
    FlushInstructionCache(GetCurrentProcess(), target, 6);
    VirtualProtect(target, 6, oldProtect, &oldProtect);
    log_line("installed tiled compositor caller hook target=%p trampoline=%p", target, trampoline);
}

static void hook_object_blitter(void)
{
    BYTE *target = (BYTE *)GetModuleHandleA(NULL) + 0x18e400;
    BYTE *trampoline;
    DWORD oldProtect;
    INT32 relative;

    /* Original entry: sub esp,0x18; mov edx,[esp+0x1c]. */
    if (target[0] != 0x83 || target[1] != 0xec || target[2] != 0x18 ||
        target[3] != 0x8b || target[4] != 0x54 || target[5] != 0x24 || target[6] != 0x1c) {
        log_line("object blitter signature mismatch at %p; hook not installed", target);
        return;
    }
    trampoline = (BYTE *)VirtualAlloc(NULL, 12, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline) {
        log_line("object blitter trampoline allocation failed: %lu", GetLastError());
        return;
    }
    CopyMemory(trampoline, target, 7);
    trampoline[7] = 0xe9;
    relative = (INT32)((target + 7) - (trampoline + 12));
    CopyMemory(trampoline + 8, &relative, sizeof(relative));
    if (!VirtualProtect(target, 7, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        log_line("object blitter patch protection failed: %lu", GetLastError());
        VirtualFree(trampoline, 0, MEM_RELEASE);
        return;
    }
    g_objectBlitterTrampoline = trampoline;
    target[0] = 0xe9;
    relative = (INT32)((BYTE *)hooked_object_blitter - (target + 5));
    CopyMemory(target + 1, &relative, sizeof(relative));
    target[5] = 0x90;
    target[6] = 0x90;
    FlushInstructionCache(GetCurrentProcess(), target, 7);
    VirtualProtect(target, 7, oldProtect, &oldProtect);
    log_line("installed object blitter caller hook target=%p trampoline=%p", target, trampoline);
}

static void hook_pixel_copy(void)
{
    BYTE *target = (BYTE *)GetModuleHandleA(NULL) + 0x239c5;
    BYTE *trampoline;
    DWORD oldProtect;
    INT32 relative;

    /* Active 16-bit copy implementation: push ebp; mov ebp,esp; push esi; push edi. */
    if (target[0] != 0x55 || target[1] != 0x8b || target[2] != 0xec ||
        target[3] != 0x56 || target[4] != 0x57) {
        log_line("pixel copy signature mismatch at %p; hook not installed", target);
        return;
    }
    trampoline = (BYTE *)VirtualAlloc(NULL, 10, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline) {
        log_line("pixel copy trampoline allocation failed: %lu", GetLastError());
        return;
    }
    CopyMemory(trampoline, target, 5);
    trampoline[5] = 0xe9;
    relative = (INT32)((target + 5) - (trampoline + 10));
    CopyMemory(trampoline + 6, &relative, sizeof(relative));
    if (!VirtualProtect(target, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        log_line("pixel copy patch protection failed: %lu", GetLastError());
        VirtualFree(trampoline, 0, MEM_RELEASE);
        return;
    }
    g_pixelCopyTrampoline = trampoline;
    target[0] = 0xe9;
    relative = (INT32)((BYTE *)hooked_pixel_copy - (target + 5));
    CopyMemory(target + 1, &relative, sizeof(relative));
    FlushInstructionCache(GetCurrentProcess(), target, 5);
    VirtualProtect(target, 5, oldProtect, &oldProtect);
    log_line("installed pixel copy caller hook target=%p trampoline=%p", target, trampoline);
}

#if ABOM_BACKGROUND_PROBE
static void hook_opaque_copy(void)
{
    BYTE *target = (BYTE *)GetModuleHandleA(NULL) + 0x22a98;
    BYTE *trampoline;
    DWORD oldProtect;
    INT32 relative;
    /* push ebp; mov ebp,esp; push esi; push edi. */
    if (target[0] != 0x55 || target[1] != 0x8b || target[2] != 0xec ||
        target[3] != 0x56 || target[4] != 0x57) {
        log_line("opaque copy signature mismatch at %p; hook not installed", target);
        return;
    }
    trampoline = (BYTE *)VirtualAlloc(NULL, 10, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline) { log_line("opaque copy trampoline allocation failed: %lu", GetLastError()); return; }
    CopyMemory(trampoline, target, 5);
    trampoline[5] = 0xe9;
    relative = (INT32)((target + 5) - (trampoline + 10));
    CopyMemory(trampoline + 6, &relative, sizeof(relative));
    if (!VirtualProtect(target, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        VirtualFree(trampoline, 0, MEM_RELEASE);
        log_line("opaque copy patch protection failed: %lu", GetLastError());
        return;
    }
    g_opaqueCopyTrampoline = trampoline;
    target[0] = 0xe9;
    relative = (INT32)((BYTE *)hooked_opaque_copy - (target + 5));
    CopyMemory(target + 1, &relative, sizeof(relative));
    FlushInstructionCache(GetCurrentProcess(), target, 5);
    VirtualProtect(target, 5, oldProtect, &oldProtect);
    log_line("installed opaque copy hook target=%p trampoline=%p", target, trampoline);
}
#endif

#if ABOM_BACKGROUND_PROBE
static void hook_surface_dispatch(void)
{
    BYTE *target = (BYTE *)GetModuleHandleA(NULL) + 0x18dc00;
    BYTE *trampoline;
    DWORD oldProtect;
    INT32 relative;
    /* mov eax,[00693864]; sub esp,14: two complete instructions. */
    if (target[0] != 0xa1 || target[1] != 0x64 || target[2] != 0x38 || target[3] != 0x69 ||
        target[4] != 0x00 || target[5] != 0x83 || target[6] != 0xec || target[7] != 0x14) {
        log_line("surface dispatcher signature mismatch at %p; hook not installed", target);
        return;
    }
    trampoline = (BYTE *)VirtualAlloc(NULL, 13, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline) { log_line("surface dispatcher trampoline allocation failed: %lu", GetLastError()); return; }
    CopyMemory(trampoline, target, 8);
    trampoline[8] = 0xe9;
    relative = (INT32)((target + 8) - (trampoline + 13));
    CopyMemory(trampoline + 9, &relative, sizeof(relative));
    if (!VirtualProtect(target, 8, PAGE_EXECUTE_READWRITE, &oldProtect)) { VirtualFree(trampoline, 0, MEM_RELEASE); return; }
    g_surfaceDispatchTrampoline = trampoline;
    target[0] = 0xe9;
    relative = (INT32)((BYTE *)hooked_surface_dispatch - (target + 5));
    CopyMemory(target + 1, &relative, sizeof(relative));
    target[5] = target[6] = target[7] = 0x90;
    FlushInstructionCache(GetCurrentProcess(), target, 8);
    VirtualProtect(target, 8, oldProtect, &oldProtect);
    log_line("installed surface dispatcher hook target=%p trampoline=%p", target, trampoline);
}
#endif

#if ABOM_BACKGROUND_PROBE
static void hook_surface_unlock(void)
{
    BYTE *target = (BYTE *)GetModuleHandleA(NULL) + 0x1a0210;
    BYTE *trampoline;
    DWORD oldProtect;
    INT32 relative;
    /* mov eax,[esp+4]; push esi; cmp eax,-1; push edi. */
    if (target[0] != 0x8b || target[1] != 0x44 || target[2] != 0x24 || target[3] != 0x04 ||
        target[4] != 0x56 || target[5] != 0x83 || target[6] != 0xf8 || target[7] != 0xff ||
        target[8] != 0x57) {
        log_line("surface unlock signature mismatch at %p; hook not installed", target);
        return;
    }
    trampoline = (BYTE *)VirtualAlloc(NULL, 14, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline) { log_line("surface unlock trampoline allocation failed: %lu", GetLastError()); return; }
    CopyMemory(trampoline, target, 9);
    trampoline[9] = 0xe9;
    relative = (INT32)((target + 9) - (trampoline + 14));
    CopyMemory(trampoline + 10, &relative, sizeof(relative));
    if (!VirtualProtect(target, 9, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        VirtualFree(trampoline, 0, MEM_RELEASE);
        log_line("surface unlock patch protection failed: %lu", GetLastError());
        return;
    }
    g_surfaceUnlockTrampoline = trampoline;
    target[0] = 0xe9;
    relative = (INT32)((BYTE *)hooked_surface_unlock - (target + 5));
    CopyMemory(target + 1, &relative, sizeof(relative));
    target[5] = target[6] = target[7] = target[8] = 0x90;
    FlushInstructionCache(GetCurrentProcess(), target, 9);
    VirtualProtect(target, 9, oldProtect, &oldProtect);
    log_line("installed surface unlock caller hook target=%p trampoline=%p", target, trampoline);
}
#endif

#if ABOM_BACKGROUND_PROBE
static void hook_surface_lock_helper(void)
{
    BYTE *target = (BYTE *)GetModuleHandleA(NULL) + 0x1a00e0;
    BYTE *trampoline;
    DWORD oldProtect;
    INT32 relative;
    /* mov eax,[esp+4]; sub esp,0x6c: two complete instructions. */
    if (target[0] != 0x8b || target[1] != 0x44 || target[2] != 0x24 || target[3] != 0x04 ||
        target[4] != 0x83 || target[5] != 0xec || target[6] != 0x6c) {
        log_line("surface lock helper signature mismatch at %p; hook not installed", target);
        return;
    }
    trampoline = (BYTE *)VirtualAlloc(NULL, 12, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline) { log_line("surface lock helper trampoline allocation failed: %lu", GetLastError()); return; }
    CopyMemory(trampoline, target, 7);
    trampoline[7] = 0xe9;
    relative = (INT32)((target + 7) - (trampoline + 12));
    CopyMemory(trampoline + 8, &relative, sizeof(relative));
    if (!VirtualProtect(target, 7, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        VirtualFree(trampoline, 0, MEM_RELEASE);
        log_line("surface lock helper patch protection failed: %lu", GetLastError());
        return;
    }
    g_surfaceLockHelperTrampoline = trampoline;
    target[0] = 0xe9;
    relative = (INT32)((BYTE *)hooked_surface_lock_helper - (target + 5));
    CopyMemory(target + 1, &relative, sizeof(relative));
    target[5] = target[6] = 0x90;
    FlushInstructionCache(GetCurrentProcess(), target, 7);
    VirtualProtect(target, 7, oldProtect, &oldProtect);
    log_line("installed surface lock helper caller hook target=%p trampoline=%p", target, trampoline);
}
#endif

static void hook_world_renderer(void)
{
    BYTE *target = (BYTE *)GetModuleHandleA(NULL) + 0xe7cc0;
    BYTE *trampoline;
    DWORD oldProtect;
    INT32 relative;

    /* Original entry: push ebp; mov ebp,esp; mov eax,0x1c94. */
    if (target[0] != 0x55 || target[1] != 0x8b || target[2] != 0xec ||
        target[3] != 0xb8 || target[4] != 0x94 || target[5] != 0x1c ||
        target[6] != 0x00 || target[7] != 0x00) {
        log_line("world renderer signature mismatch at %p; hook not installed", target);
        return;
    }
    trampoline = (BYTE *)VirtualAlloc(NULL, 13, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline) {
        log_line("world renderer trampoline allocation failed: %lu", GetLastError());
        return;
    }
    CopyMemory(trampoline, target, 8);
    trampoline[8] = 0xe9;
    relative = (INT32)((target + 8) - (trampoline + 13));
    CopyMemory(trampoline + 9, &relative, sizeof(relative));
    if (!VirtualProtect(target, 8, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        log_line("world renderer patch protection failed: %lu", GetLastError());
        VirtualFree(trampoline, 0, MEM_RELEASE);
        return;
    }
    g_worldRendererTrampoline = trampoline;
    target[0] = 0xe9;
    relative = (INT32)((BYTE *)hooked_world_renderer - (target + 5));
    CopyMemory(target + 1, &relative, sizeof(relative));
    target[5] = 0x90;
    target[6] = 0x90;
    target[7] = 0x90;
    FlushInstructionCache(GetCurrentProcess(), target, 8);
    VirtualProtect(target, 8, oldProtect, &oldProtect);
    log_line("installed world renderer caller hook target=%p trampoline=%p", target, trampoline);
}

static void hook_camera_controller(void)
{
    BYTE *target = (BYTE *)GetModuleHandleA(NULL) + 0x1b18f0;
    BYTE *trampoline;
    DWORD oldProtect;
    INT32 relative;

    /* mov eax,[0x006df07c]; sub esp,0x28; push ebp; push esi */
    if (target[0] != 0xa1 || target[1] != 0x7c || target[2] != 0xf0 ||
        target[3] != 0x6d || target[4] != 0x00 || target[5] != 0x83 ||
        target[6] != 0xec || target[7] != 0x28 || target[8] != 0x55 ||
        target[9] != 0x56) {
        log_line("camera controller signature mismatch at %p; hook not installed", target);
        return;
    }
    trampoline = (BYTE *)VirtualAlloc(NULL, 15, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline) {
        log_line("camera controller trampoline allocation failed: %lu", GetLastError());
        return;
    }
    CopyMemory(trampoline, target, 10);
    trampoline[10] = 0xe9;
    relative = (INT32)((target + 10) - (trampoline + 15));
    CopyMemory(trampoline + 11, &relative, sizeof(relative));
    if (!VirtualProtect(target, 10, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        log_line("camera controller patch protection failed: %lu", GetLastError());
        VirtualFree(trampoline, 0, MEM_RELEASE);
        return;
    }
    g_cameraTrampoline = trampoline;
    target[0] = 0xe9;
    relative = (INT32)((BYTE *)hooked_camera_controller - (target + 5));
    CopyMemory(target + 1, &relative, sizeof(relative));
    target[5] = 0x90;
    target[6] = 0x90;
    target[7] = 0x90;
    target[8] = 0x90;
    target[9] = 0x90;
    FlushInstructionCache(GetCurrentProcess(), target, 10);
    VirtualProtect(target, 10, oldProtect, &oldProtect);
    log_line("installed camera controller hook target=%p trampoline=%p", target, trampoline);
}

static void hook_mode_request(void)
{
    BYTE *target = (BYTE *)GetModuleHandleA(NULL) + 0x19f0a0;
    BYTE *trampoline;
    DWORD oldProtect;
    INT32 relative;

    /* mov eax,[esp+0x10]; mov edx,[esp+0x08]. Copy both whole instructions. */
    if (target[0] != 0x8b || target[1] != 0x44 || target[2] != 0x24 ||
        target[3] != 0x10 || target[4] != 0x8b || target[5] != 0x54 ||
        target[6] != 0x24 || target[7] != 0x08) {
        log_line("mode request signature mismatch at %p; hook not installed", target);
        return;
    }
    trampoline = (BYTE *)VirtualAlloc(NULL, 13, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline) {
        log_line("mode request trampoline allocation failed: %lu", GetLastError());
        return;
    }
    CopyMemory(trampoline, target, 8);
    trampoline[8] = 0xe9;
    relative = (INT32)((target + 8) - (trampoline + 13));
    CopyMemory(trampoline + 9, &relative, sizeof(relative));
    if (!VirtualProtect(target, 8, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        log_line("mode request patch protection failed: %lu", GetLastError());
        VirtualFree(trampoline, 0, MEM_RELEASE);
        return;
    }
    g_modeRequestTrampoline = trampoline;
    target[0] = 0xe9;
    relative = (INT32)((BYTE *)hooked_mode_request - (target + 5));
    CopyMemory(target + 1, &relative, sizeof(relative));
    target[5] = 0x90;
    target[6] = 0x90;
    target[7] = 0x90;
    FlushInstructionCache(GetCurrentProcess(), target, 8);
    VirtualProtect(target, 8, oldProtect, &oldProtect);
    log_line("installed mode request hook target=%p trampoline=%p", target, trampoline);
}

static void hook_presentation(void)
{
    BYTE *target = (BYTE *)GetModuleHandleA(NULL) + 0x19f950;
    BYTE *trampoline;
    DWORD oldProtect;
    INT32 relative;
    /* mov eax,[0x00693864]; sub esp,0x1c */
    if (target[0] != 0xa1 || target[1] != 0x64 || target[2] != 0x38 ||
        target[3] != 0x69 || target[4] != 0x00 || target[5] != 0x83 ||
        target[6] != 0xec || target[7] != 0x1c) {
        log_line("presentation signature mismatch at %p; hook not installed", target);
        return;
    }
    trampoline = (BYTE *)VirtualAlloc(NULL, 13, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!trampoline) { log_line("presentation trampoline allocation failed: %lu", GetLastError()); return; }
    CopyMemory(trampoline, target, 8);
    trampoline[8] = 0xe9;
    relative = (INT32)((target + 8) - (trampoline + 13));
    CopyMemory(trampoline + 9, &relative, sizeof(relative));
    if (!VirtualProtect(target, 8, PAGE_EXECUTE_READWRITE, &oldProtect)) { VirtualFree(trampoline, 0, MEM_RELEASE); log_line("presentation patch protection failed: %lu", GetLastError()); return; }
    g_presentationTrampoline = trampoline;
    target[0] = 0xe9;
    relative = (INT32)((BYTE *)hooked_presentation - (target + 5));
    CopyMemory(target + 1, &relative, sizeof(relative));
    target[5] = target[6] = target[7] = 0x90;
    FlushInstructionCache(GetCurrentProcess(), target, 8);
    VirtualProtect(target, 8, oldProtect, &oldProtect);
    log_line("installed presentation hook target=%p trampoline=%p", target, trampoline);
}

#if ABOM_EXPERIMENTAL_WIDESCREEN
static void hook_widescreen_present_copy(void)
{
    BYTE *base = (BYTE *)GetModuleHandleA(NULL);
    BYTE *presentChecks[2] = { base + 0x19fc0a, base + 0x19fca7 };
    BYTE *heightChecks[2] = { base + 0x19fc12, base + 0x19fcaf };
    BYTE *copyCalls[2] = {
        base + 0x19fc30, base + 0x19fccd
    };
    void *widthDispatchers[2] = { hooked_ada_width_dispatch, hooked_bce_width_dispatch };
    void *heightDispatchers[2] = { hooked_ada_height_dispatch, hooked_bce_height_dispatch };
    void *copyDispatchers[2] = { hooked_ada_copy_dispatch, hooked_bce_copy_dispatch };
    DWORD oldProtect;
    INT32 relative;
    int index;
    /* Two runtime states were observed: flags 0x11 selects FUN_00422ADA and
     * flags 0x23 selects FUN_00422BCE.  Each has its own 640-wide check and
     * call site.  Leave their 640/800 behavior intact and add only 854. */
    for (index = 0; index < 2; ++index) {
        BYTE *presentCheck = presentChecks[index];
        BYTE *heightCheck = heightChecks[index];
        BYTE *copyCall = copyCalls[index];
        if (presentCheck[0] != 0x81 || presentCheck[1] != 0xf9 || presentCheck[2] != 0x80 ||
            presentCheck[3] != 0x02 || presentCheck[4] != 0x00 || presentCheck[5] != 0x00 ||
            presentCheck[6] != 0x75 || copyCall[0] != 0xe8 || copyCall[3] != 0xe8 ||
            copyCall[4] != 0xff || heightCheck[0] != 0x81 || heightCheck[1] != 0x7e ||
            heightCheck[2] != 0x18 || heightCheck[3] != 0xe0 || heightCheck[4] != 0x01 ||
            heightCheck[5] != 0x00 || heightCheck[6] != 0x00 || heightCheck[7] != 0x75 ||
            (index == 0 && heightCheck[8] != 0x1f) ||
            (index == 1 && heightCheck[8] != 0x1c) ||
            (index == 0 && (copyCall[1] != 0xa5 || copyCall[2] != 0x2e)) ||
            (index == 1 && (copyCall[1] != 0xfc || copyCall[2] != 0x2e))) {
            log_line("widescreen presenter signature mismatch at branch %d", index);
            return;
        }
    }
    g_adaCopyOriginal = base + 0x22ada;
    g_bceCopyOriginal = base + 0x22bce;
    g_adaWidthSuccess = base + 0x19fc12;
    g_adaWidthFallback = base + 0x19fc3a;
    g_bceWidthSuccess = base + 0x19fcaf;
    g_bceWidthFallback = base + 0x19fcd4;
    g_adaHeightSuccess = base + 0x19fc1b;
    g_adaHeightFallback = base + 0x19fc3a;
    g_bceHeightSuccess = base + 0x19fcb8;
    g_bceHeightFallback = base + 0x19fcd4;
    for (index = 0; index < 2; ++index) {
        BYTE *presentCheck = presentChecks[index];
        BYTE *heightCheck = heightChecks[index];
        BYTE *copyCall = copyCalls[index];
        if (!VirtualProtect(presentCheck, 8, PAGE_EXECUTE_READWRITE, &oldProtect)) return;
        presentCheck[0] = 0xe9;
        relative = (INT32)((BYTE *)widthDispatchers[index] - (presentCheck + 5));
        CopyMemory(presentCheck + 1, &relative, sizeof(relative));
        presentCheck[5] = presentCheck[6] = presentCheck[7] = 0x90;
        FlushInstructionCache(GetCurrentProcess(), presentCheck, 8);
        VirtualProtect(presentCheck, 8, oldProtect, &oldProtect);
        if (!VirtualProtect(heightCheck, 9, PAGE_EXECUTE_READWRITE, &oldProtect)) return;
        heightCheck[0] = 0xe9;
        relative = (INT32)((BYTE *)heightDispatchers[index] - (heightCheck + 5));
        CopyMemory(heightCheck + 1, &relative, sizeof(relative));
        heightCheck[5] = heightCheck[6] = heightCheck[7] = heightCheck[8] = 0x90;
        FlushInstructionCache(GetCurrentProcess(), heightCheck, 9);
        VirtualProtect(heightCheck, 9, oldProtect, &oldProtect);
        if (!VirtualProtect(copyCall, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) return;
        copyCall[0] = 0xe8;
        relative = (INT32)((BYTE *)copyDispatchers[index] - (copyCall + 5));
        CopyMemory(copyCall + 1, &relative, sizeof(relative));
        FlushInstructionCache(GetCurrentProcess(), copyCall, 5);
        VirtualProtect(copyCall, 5, oldProtect, &oldProtect);
    }
    g_widescreenPresentPatchInstalled = TRUE;
    log_line("installed experimental %dx%d presentation copy (row adjust=%d)",
             ABOM_WIDESCREEN_WIDTH, ABOM_WIDESCREEN_HEIGHT,
             ABOM_WIDESCREEN_PRESENT_ADJUST);
}
#endif

#if ABOM_EXPERIMENTAL_TERRAIN_COPY
static void hook_widescreen_terrain_copy(void)
{
    BYTE *base = (BYTE *)GetModuleHandleA(NULL);
    BYTE *copyCall = base + 0x12c0f9;
    static const BYTE expected[6] = { 0xff, 0x15, 0x38, 0x4e, 0x64, 0x00 };
    DWORD oldProtect;
    INT32 relative;

    if (memcmp(copyCall, expected, sizeof(expected)) != 0) {
        log_line("widescreen terrain-copy signature mismatch at %p", copyCall);
        return;
    }
    g_fixedTerrainCopy = (FixedTerrainCopyFn)(base + 0x22ada);
    if (!VirtualProtect(copyCall, sizeof(expected), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        log_line("widescreen terrain-copy patch protection failed: %lu", GetLastError());
        return;
    }
    copyCall[0] = 0xe8;
    relative = (INT32)((BYTE *)hooked_widescreen_terrain_copy - (copyCall + 5));
    CopyMemory(copyCall + 1, &relative, sizeof(relative));
    copyCall[5] = 0x90;
    FlushInstructionCache(GetCurrentProcess(), copyCall, sizeof(expected));
    VirtualProtect(copyCall, sizeof(expected), oldProtect, &oldProtect);
    g_widescreenTerrainPatchInstalled = TRUE;
    log_line("installed experimental %dx%d terrain copy at %p",
             ABOM_WIDESCREEN_WIDTH, ABOM_WIDESCREEN_HEIGHT, copyCall);
}
#endif

#if ABOM_EXPERIMENTAL_TERRAIN_REFRESH
static void hook_widescreen_terrain_refresh(void)
{
    BYTE *base = (BYTE *)GetModuleHandleA(NULL);
    BYTE *suppressionBranch = base + 0x14e22f;
    static const BYTE expected[2] = { 0x75, 0x25 };
    DWORD oldProtect;

    if (ABOM_WIDESCREEN_WIDTH != 1920 || ABOM_WIDESCREEN_HEIGHT != 1080) {
        log_line("terrain-refresh patch refused for non-1920x1080 build");
        return;
    }
    if (memcmp(suppressionBranch, expected, sizeof(expected)) != 0) {
        log_line("terrain-refresh signature mismatch at %p", suppressionBranch);
        return;
    }
    if (!VirtualProtect(suppressionBranch, sizeof(expected),
                        PAGE_EXECUTE_READWRITE, &oldProtect)) {
        log_line("terrain-refresh patch protection failed: %lu", GetLastError());
        return;
    }
    suppressionBranch[0] = 0x90;
    suppressionBranch[1] = 0x90;
    FlushInstructionCache(GetCurrentProcess(), suppressionBranch,
                          sizeof(expected));
    VirtualProtect(suppressionBranch, sizeof(expected), oldProtect, &oldProtect);
    log_line("installed experimental 1920x1080 per-frame terrain refresh at %p",
             suppressionBranch);
}
#endif

#if ABOM_EXPERIMENTAL_HUD_ANCHORS
static BOOL patch_checked_relative_call(BYTE *call, void *expectedTarget, void *replacement)
{
    DWORD oldProtect;
    INT32 relative;
    INT32 existingRelative;
    BYTE *actualTarget;

    if (call[0] != 0xe8) return FALSE;
    CopyMemory(&existingRelative, call + 1, sizeof(existingRelative));
    actualTarget = call + 5 + existingRelative;
    if (actualTarget != (BYTE *)expectedTarget) return FALSE;
    if (!VirtualProtect(call, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) return FALSE;
    relative = (INT32)((BYTE *)replacement - (call + 5));
    CopyMemory(call + 1, &relative, sizeof(relative));
    FlushInstructionCache(GetCurrentProcess(), call, 5);
    VirtualProtect(call, 5, oldProtect, &oldProtect);
    return TRUE;
}

static BOOL patch_checked_indirect_call(BYTE *call, const BYTE expected[6],
                                        void *replacement)
{
    DWORD oldProtect;
    INT32 relative;
    if (memcmp(call, expected, 6) != 0) return FALSE;
    if (!VirtualProtect(call, 6, PAGE_EXECUTE_READWRITE, &oldProtect)) return FALSE;
    call[0] = 0xe8;
    relative = (INT32)((BYTE *)replacement - (call + 5));
    CopyMemory(call + 1, &relative, sizeof(relative));
    call[5] = 0x90;
    FlushInstructionCache(GetCurrentProcess(), call, 6);
    VirtualProtect(call, 6, oldProtect, &oldProtect);
    return TRUE;
}

static void hook_widescreen_hud_anchors(void)
{
    BYTE *base = (BYTE *)GetModuleHandleA(NULL);
    BYTE *spriteTarget = base + 0x18d980;
    BYTE *clipTarget = base + 0x18d660;
    BYTE *spriteCalls[] = {
        base + 0x1573b9, base + 0x157406, base + 0x157453,
        base + 0x157492, base + 0x1574f2, base + 0x15753f
    };
    BYTE *clipCalls[] = {
        base + 0x1573da, base + 0x157427, base + 0x157474,
        base + 0x1574c6, base + 0x157513
    };
    BYTE *loadingCall = base + 0x1bb608;
    BYTE *loadingBarCall = base + 0x19ec08;
    BYTE *bottomLeftHudCall = base + 0x14a28b;
    BYTE *overlayWorldFrameCall = base + 0x149e91;
    BYTE *overlaySpriteCall = base + 0x14ea01;
    BYTE *overlayUnlockCall = base + 0x18dbca;
    BYTE *bottomHudHitTop = base + 0x147fbe;
    BYTE *bottomHudHitBottom = base + 0x147fc9;
    static const BYTE loadingBarExpected[6] = {
        0xff, 0x15, 0x6c, 0x4e, 0x64, 0x00
    };
    static const DWORD fixedBottomHudHitTop = 0x000001c2;
    static const DWORD fixedBottomHudHitBottom = 0x000001d4;
    const DWORD wideBottomHudHitTop = 0x000001c2 + ABOM_VIEWPORT_Y_DELTA;
    const DWORD wideBottomHudHitBottom = 0x000001d4 + ABOM_VIEWPORT_Y_DELTA;
    BYTE *pauseSpriteCalls[] = {
        base + 0x1dfdd2, base + 0x1dfe08, base + 0x1e0422,
        base + 0x1e051a, base + 0x1e0566,
        base + 0x1e05a3, base + 0x1e0627, base + 0x1e0659,
        base + 0x1e068b, base + 0x1e06ee, base + 0x1e0737,
        base + 0x1e0781, base + 0x1e07cb, base + 0x1e0815,
        base + 0x1e085f, base + 0x1e08a9, base + 0x1e08f3,
        base + 0x1e0925, base + 0x1e096f, base + 0x1e098d,
        base + 0x1e0a73
    };
    BYTE *pauseClipCalls[] = {
        base + 0x1e040b, base + 0x1e0440,
        base + 0x1e0548, base + 0x1e05d5
    };
    BYTE *edgeHorizontalCall = base + 0x14dfe9;
    BYTE *edgeVerticalCall = base + 0x14e049;
#if ABOM_ANIMATION_PROBE
    BYTE *animationFrameCall = base + 0x18d208;
#endif
#if ABOM_UI_PROBE
    BYTE *pauseMouseXCall = base + 0x1e02f8;
    BYTE *overlayCoordinatesCall = base + 0x1b3dcf;
#endif
    BYTE *pauseInputBoundsCall = base + 0x1dff45;
    BYTE *pauseCoordinatesCall = base + 0x1e031a;
    BYTE *pauseBriefingCall = base + 0x1e049c;
    /* State 4's briefing bitmap is drawn directly rather than through the
     * interface-sprite helper.  Its x origin must follow the centered clip. */
    BYTE *pauseBriefingTextXImmediate = base + 0x1e0498;
    BYTE *lineWidthCompare = base + 0x1a063e;
    BYTE *lineHeightCompare = base + 0x1a064a;
    BYTE *quickActionXStore = base + 0x1498e3;
    BYTE *quickActionXLoad = base + 0x14d71f;
    BYTE *quickActionYStore = base + 0x149913;
    BYTE *quickActionYLoad = base + 0x14d726;
    BYTE *pauseMainXBounds[] = {
        base + 0x1e0b68, base + 0x1e0b79,
        base + 0x1e14a4, base + 0x1e14ab,
        base + 0x1e14cb, base + 0x1e14d2,
        base + 0x1e1526, base + 0x1e152d,
        base + 0x1e1581, base + 0x1e1588
    };
    BYTE *pauseSubmenuXImmediates[] = {
        /* State 1: restart confirmation. */
        base + 0x1e1d96, base + 0x1e1d9e,
        base + 0x1e1dc6, base + 0x1e1dce,
        /* State 3: abandon confirmation. */
        base + 0x1e1c96, base + 0x1e1c9e,
        base + 0x1e1cc6, base + 0x1e1cce,
        /* State 2: sound/settings controls. */
        base + 0x1e19b7, base + 0x1e19bf,
        base + 0x1e19fd, base + 0x1e1a05,
        base + 0x1e1a65, base + 0x1e1a6d,
        base + 0x1e1ab8, base + 0x1e1ac0,
        base + 0x1e1b28, base + 0x1e1b30,
        base + 0x1e1b7a, base + 0x1e1b82,
        /* State 4: review/scroll controls. */
        base + 0x1e1608, base + 0x1e1610,
        base + 0x1e1638, base + 0x1e1640,
        base + 0x1e16d7, base + 0x1e16e3,
        base + 0x1e1834, base + 0x1e1840
    };
    BYTE *quickActionRightBounds[] = {
        base + 0x148116, base + 0x149153
    };
    BYTE *quickActionBottomBounds[] = {
        base + 0x148128, base + 0x149165
    };
    static const BYTE fixedLineWidthCompare[6] = {
        0x81, 0xfe, 0x80, 0x02, 0x00, 0x00
    };
    static const BYTE liveLineWidthCompare[6] = {
        0x3b, 0x35, 0xec, 0x4e, 0x64, 0x00
    };
    static const BYTE fixedLineHeightCompare[6] = {
        0x81, 0xf9, 0xe0, 0x01, 0x00, 0x00
    };
    static const BYTE liveLineHeightCompare[6] = {
        0x3b, 0x0d, 0xf0, 0x4e, 0x64, 0x00
    };
    static const BYTE fixedQuickActionRightBound[5] = {
        0x3d, 0xf0, 0x01, 0x00, 0x00
    };
    BYTE wideQuickActionRightBound[5] = { 0x3d, 0, 0, 0, 0 };
    static const BYTE fixedQuickActionBottomBound[5] = {
        0x3d, 0x7b, 0x01, 0x00, 0x00
    };
    BYTE wideQuickActionBottomBound[5] = { 0x3d, 0, 0, 0, 0 };
    static const BYTE fixedQuickActionXStore[7] = {
        0x89, 0x94, 0x8e, 0x30, 0x03, 0x00, 0x00
    };
    static const BYTE fixedQuickActionXLoad[7] = {
        0x8b, 0x9c, 0x8e, 0x30, 0x03, 0x00, 0x00
    };
    static const BYTE fixedQuickActionYStore[7] = {
        0x89, 0x94, 0x8e, 0x34, 0x03, 0x00, 0x00
    };
    static const BYTE fixedQuickActionYLoad[7] = {
        0x8b, 0xbc, 0x8e, 0x34, 0x03, 0x00, 0x00
    };
    static const DWORD fixedPauseMainXBounds[] = {
        0x000000c8, 0x000001bc,
        0x000000c3, 0x00000100,
        0x00000103, 0x0000013f,
        0x00000141, 0x0000017b,
        0x0000017e, 0x000001bd
    };
    static const DWORD widePauseMainXBounds[] = {
        0x00000133, 0x00000227,
        0x0000012e, 0x0000016b,
        0x0000016e, 0x000001aa,
        0x000001ac, 0x000001e6,
        0x000001e9, 0x00000228
    };
    static const DWORD fixedPauseSubmenuXBounds[] = {
        0x000000d2, 0x00000146, 0x0000014b, 0x000001c2,
        0x000000d2, 0x00000146, 0x0000014b, 0x000001c2,
        0x00000103, 0x0000013f, 0x00000141, 0x0000017b,
        0x000000c8, 0x000000e2, 0x0000019f, 0x000001b9,
        0x000000c8, 0x000000e2, 0x0000019f, 0x000001b9,
        0x00000103, 0x0000013f, 0x00000141, 0x0000017b,
        0x000000c3, 0x00000100, 0x0000017e, 0x000001bd
    };
    static const DWORD widePauseSubmenuXBounds[] = {
        0x0000013d, 0x000001b1, 0x000001b6, 0x0000022d,
        0x0000013d, 0x000001b1, 0x000001b6, 0x0000022d,
        0x0000016e, 0x000001aa, 0x000001ac, 0x000001e6,
        0x00000133, 0x0000014d, 0x0000020a, 0x00000224,
        0x00000133, 0x0000014d, 0x0000020a, 0x00000224,
        0x0000016e, 0x000001aa, 0x000001ac, 0x000001e6,
        0x0000012e, 0x0000016b, 0x000001e9, 0x00000228
    };
    static const DWORD fixedPauseBriefingTextX = 0x000000d2;
    static const DWORD widePauseBriefingTextX = 0x0000013d;
    DWORD quickActionRightBoundValue = ABOM_QUICK_ACTION_RIGHT_BOUND;
    DWORD quickActionBottomBoundValue = ABOM_QUICK_ACTION_BOTTOM_BOUND;
    int index;

    CopyMemory(wideQuickActionRightBound + 1, &quickActionRightBoundValue,
               sizeof(quickActionRightBoundValue));
    CopyMemory(wideQuickActionBottomBound + 1, &quickActionBottomBoundValue,
               sizeof(quickActionBottomBoundValue));

    g_hudSpriteOriginal = spriteTarget;
    g_hudClipOriginal = clipTarget;
    g_edgeLineOriginal = base + 0x1a03a0;
    g_pauseBriefingOriginal = base + 0x18d760;
    g_overlayUnlockOriginal = base + 0x1a0210;
    g_overlayWorldFrameOriginal = base + 0x14e0d0;
    if (!g_overlayLegacySnapshot) {
        g_overlayLegacySnapshot = (WORD *)VirtualAlloc(
            NULL, 640u * 480u * sizeof(WORD),
            MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!g_overlayLegacySnapshot) {
            log_line("scanner legacy snapshot allocation failed: %lu",
                     GetLastError());
            return;
        }
    }
#if ABOM_ANIMATION_PROBE
    g_animationFrameOriginal = base + 0x18ab40;
    if (!patch_checked_relative_call(animationFrameCall, g_animationFrameOriginal,
                                     hooked_animation_frame_select)) {
        log_line("animation frame-select signature mismatch at %p", animationFrameCall);
        return;
    }
#endif
#if ABOM_UI_PROBE
    g_pauseMouseXOriginal = base + 0x1a38f0;
    g_overlayCoordinatesOriginal = base + 0x14ef90;
#endif
    for (index = 0; index < (int)(sizeof(spriteCalls) / sizeof(spriteCalls[0])); ++index) {
        if (!patch_checked_relative_call(spriteCalls[index], spriteTarget,
                                         hooked_right_hud_sprite_call)) {
            log_line("right HUD sprite signature mismatch at %p", spriteCalls[index]);
            return;
        }
    }
    for (index = 0; index < (int)(sizeof(clipCalls) / sizeof(clipCalls[0])); ++index) {
        if (!patch_checked_relative_call(clipCalls[index], clipTarget,
                                         hooked_right_hud_clip_call)) {
            log_line("right HUD clip signature mismatch at %p", clipCalls[index]);
            return;
        }
    }
    if (!patch_checked_relative_call(bottomLeftHudCall, spriteTarget,
                                     hooked_bottom_left_hud_sprite_call)) {
        log_line("bottom-left HUD sprite signature mismatch at %p", bottomLeftHudCall);
        return;
    }
    if (!patch_checked_relative_call(overlayWorldFrameCall,
                                     g_overlayWorldFrameOriginal,
                                     hooked_overlay_world_frame)) {
        log_line("scanner world-frame signature mismatch at %p",
                 overlayWorldFrameCall);
        return;
    }
    if (!patch_checked_relative_call(overlaySpriteCall, spriteTarget,
                                     hooked_scaled_overlay_sprite_call)) {
        log_line("scanner overlay sprite signature mismatch at %p", overlaySpriteCall);
        return;
    }
    if (!patch_checked_relative_call(overlayUnlockCall, g_overlayUnlockOriginal,
                                     hooked_overlay_surface_unlock)) {
        log_line("scanner surface-unlock signature mismatch at %p", overlayUnlockCall);
        return;
    }
    {
        DWORD oldProtect;
        if (memcmp(bottomHudHitTop, &fixedBottomHudHitTop, sizeof(DWORD)) != 0 ||
            memcmp(bottomHudHitBottom, &fixedBottomHudHitBottom, sizeof(DWORD)) != 0) {
            log_line("bottom HUD hit-bound signature mismatch at %p/%p",
                     bottomHudHitTop, bottomHudHitBottom);
            return;
        }
        if (!VirtualProtect(bottomHudHitTop, sizeof(DWORD), PAGE_EXECUTE_READWRITE,
                            &oldProtect)) {
            log_line("bottom HUD top hit-bound protection failed at %p", bottomHudHitTop);
            return;
        }
        CopyMemory(bottomHudHitTop, &wideBottomHudHitTop, sizeof(DWORD));
        FlushInstructionCache(GetCurrentProcess(), bottomHudHitTop, sizeof(DWORD));
        VirtualProtect(bottomHudHitTop, sizeof(DWORD), oldProtect, &oldProtect);
        if (!VirtualProtect(bottomHudHitBottom, sizeof(DWORD), PAGE_EXECUTE_READWRITE,
                            &oldProtect)) {
            log_line("bottom HUD bottom hit-bound protection failed at %p", bottomHudHitBottom);
            return;
        }
        CopyMemory(bottomHudHitBottom, &wideBottomHudHitBottom, sizeof(DWORD));
        FlushInstructionCache(GetCurrentProcess(), bottomHudHitBottom, sizeof(DWORD));
        VirtualProtect(bottomHudHitBottom, sizeof(DWORD), oldProtect, &oldProtect);
    }
    if (!patch_checked_relative_call(loadingCall, spriteTarget,
                                     hooked_centered_loading_sprite_call)) {
        log_line("loading-art signature mismatch at %p", loadingCall);
        return;
    }
    if (!patch_checked_indirect_call(loadingBarCall, loadingBarExpected,
                                     hooked_centered_loading_bar_copy)) {
        log_line("loading-bar signature mismatch at %p", loadingBarCall);
        return;
    }
#if ABOM_EXPERIMENTAL_SECONDARY_UI
    for (index = 0; index < (int)(sizeof(pauseSpriteCalls) / sizeof(pauseSpriteCalls[0])); ++index) {
        if (!patch_checked_relative_call(pauseSpriteCalls[index], spriteTarget,
                                         hooked_centered_interface_sprite_call)) {
            log_line("pause sprite signature mismatch at %p", pauseSpriteCalls[index]);
            return;
        }
    }
    for (index = 0; index < (int)(sizeof(pauseClipCalls) / sizeof(pauseClipCalls[0])); ++index) {
        if (!patch_checked_relative_call(pauseClipCalls[index], clipTarget,
                                         hooked_centered_interface_clip_call)) {
            log_line("pause clip signature mismatch at %p", pauseClipCalls[index]);
            return;
        }
    }
#if ABOM_WIDESCREEN_HEIGHT == 480
    for (index = 0; index < (int)(sizeof(pauseMainXBounds) / sizeof(pauseMainXBounds[0])); ++index) {
        BYTE *instruction = pauseMainXBounds[index];
        DWORD oldProtect;
        if (instruction[0] != 0x3d ||
            memcmp(instruction + 1, &fixedPauseMainXBounds[index], sizeof(DWORD)) != 0) {
            log_line("pause main x-bound signature mismatch at %p", instruction);
            return;
        }
        if (!VirtualProtect(instruction + 1, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &oldProtect)) {
            log_line("pause main x-bound protection failed at %p", instruction);
            return;
        }
        CopyMemory(instruction + 1, &widePauseMainXBounds[index], sizeof(DWORD));
        FlushInstructionCache(GetCurrentProcess(), instruction + 1, sizeof(DWORD));
        VirtualProtect(instruction + 1, sizeof(DWORD), oldProtect, &oldProtect);
    }
    for (index = 0; index < (int)(sizeof(pauseSubmenuXImmediates) / sizeof(pauseSubmenuXImmediates[0])); ++index) {
        BYTE *immediate = pauseSubmenuXImmediates[index];
        DWORD oldProtect;
        if (memcmp(immediate, &fixedPauseSubmenuXBounds[index], sizeof(DWORD)) != 0) {
            log_line("pause submenu x-bound signature mismatch at %p", immediate);
            return;
        }
        if (!VirtualProtect(immediate, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &oldProtect)) {
            log_line("pause submenu x-bound protection failed at %p", immediate);
            return;
        }
        CopyMemory(immediate, &widePauseSubmenuXBounds[index], sizeof(DWORD));
        FlushInstructionCache(GetCurrentProcess(), immediate, sizeof(DWORD));
        VirtualProtect(immediate, sizeof(DWORD), oldProtect, &oldProtect);
    }
    {
        DWORD oldProtect;
        if (memcmp(pauseBriefingTextXImmediate, &fixedPauseBriefingTextX,
                   sizeof(DWORD)) != 0) {
            log_line("pause briefing x-origin signature mismatch at %p",
                     pauseBriefingTextXImmediate);
            return;
        }
        if (!VirtualProtect(pauseBriefingTextXImmediate, sizeof(DWORD),
                            PAGE_EXECUTE_READWRITE, &oldProtect)) {
            log_line("pause briefing x-origin protection failed at %p",
                     pauseBriefingTextXImmediate);
            return;
        }
        CopyMemory(pauseBriefingTextXImmediate, &widePauseBriefingTextX,
                   sizeof(DWORD));
        FlushInstructionCache(GetCurrentProcess(), pauseBriefingTextXImmediate,
                              sizeof(DWORD));
        VirtualProtect(pauseBriefingTextXImmediate, sizeof(DWORD), oldProtect,
                       &oldProtect);
    }
#else
    if (!patch_checked_relative_call(pauseCoordinatesCall, base + 0x14eff0,
                                     hooked_centered_pause_coordinates)) {
        log_line("pause coordinate-transform signature mismatch at %p",
                 pauseCoordinatesCall);
        return;
    }
    if (!patch_checked_relative_call(pauseBriefingCall, g_pauseBriefingOriginal,
                                     hooked_centered_pause_briefing)) {
        log_line("pause briefing-call signature mismatch at %p", pauseBriefingCall);
        return;
    }
#endif
#if ABOM_UI_PROBE
    if (!patch_checked_relative_call(pauseMouseXCall, g_pauseMouseXOriginal,
                                     hooked_pause_state_probe_mouse_x)) {
        log_line("pause state-probe signature mismatch at %p", pauseMouseXCall);
        return;
    }
    if (!patch_checked_relative_call(overlayCoordinatesCall,
                                     g_overlayCoordinatesOriginal,
                                     hooked_scaled_overlay_coordinates)) {
        log_line("overlay coordinate-transform signature mismatch at %p",
                 overlayCoordinatesCall);
        return;
    }
#endif
    if (!patch_checked_relative_call(pauseInputBoundsCall, base + 0x1a3100,
                                     hooked_centered_pause_input_bounds)) {
        log_line("pause input-bounds signature mismatch at %p", pauseInputBoundsCall);
        return;
    }
    if (!patch_checked_relative_call(edgeHorizontalCall, g_edgeLineOriginal,
                                     hooked_widescreen_edge_horizontal_line)) {
        log_line("horizontal edge-flash signature mismatch at %p", edgeHorizontalCall);
        return;
    }
    if (!patch_checked_relative_call(edgeVerticalCall, g_edgeLineOriginal,
                                     hooked_widescreen_edge_vertical_line)) {
        log_line("vertical edge-flash signature mismatch at %p", edgeVerticalCall);
        return;
    }
    if (memcmp(lineWidthCompare, fixedLineWidthCompare, 6) == 0) {
        DWORD oldProtect;
        if (!VirtualProtect(lineWidthCompare, 6, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            log_line("line-raster width protection failed at %p", lineWidthCompare);
            return;
        }
        CopyMemory(lineWidthCompare, liveLineWidthCompare, 6);
        FlushInstructionCache(GetCurrentProcess(), lineWidthCompare, 6);
        VirtualProtect(lineWidthCompare, 6, oldProtect, &oldProtect);
    } else {
        log_line("line-raster width signature mismatch at %p", lineWidthCompare);
        return;
    }
    if (memcmp(lineHeightCompare, fixedLineHeightCompare, 6) == 0) {
        DWORD oldProtect;
        if (!VirtualProtect(lineHeightCompare, 6, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            log_line("line-raster height protection failed at %p", lineHeightCompare);
            return;
        }
        CopyMemory(lineHeightCompare, liveLineHeightCompare, 6);
        FlushInstructionCache(GetCurrentProcess(), lineHeightCompare, 6);
        VirtualProtect(lineHeightCompare, 6, oldProtect, &oldProtect);
    } else {
        log_line("line-raster height signature mismatch at %p", lineHeightCompare);
        return;
    }
    for (index = 0; index < (int)(sizeof(quickActionRightBounds) / sizeof(quickActionRightBounds[0])); ++index) {
        BYTE *quickActionRightBound = quickActionRightBounds[index];
        if (memcmp(quickActionRightBound, fixedQuickActionRightBound, 5) == 0) {
            DWORD oldProtect;
            if (!VirtualProtect(quickActionRightBound, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
                log_line("quick-action right-bound protection failed at %p", quickActionRightBound);
                return;
            }
            CopyMemory(quickActionRightBound, wideQuickActionRightBound, 5);
            FlushInstructionCache(GetCurrentProcess(), quickActionRightBound, 5);
            VirtualProtect(quickActionRightBound, 5, oldProtect, &oldProtect);
        } else {
            log_line("quick-action right-bound signature mismatch at %p", quickActionRightBound);
            return;
        }
    }
    for (index = 0; index < (int)(sizeof(quickActionBottomBounds) / sizeof(quickActionBottomBounds[0])); ++index) {
        BYTE *quickActionBottomBound = quickActionBottomBounds[index];
        if (memcmp(quickActionBottomBound, fixedQuickActionBottomBound, 5) == 0) {
            DWORD oldProtect;
            if (!VirtualProtect(quickActionBottomBound, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
                log_line("quick-action bottom-bound protection failed at %p", quickActionBottomBound);
                return;
            }
            CopyMemory(quickActionBottomBound, wideQuickActionBottomBound, 5);
            FlushInstructionCache(GetCurrentProcess(), quickActionBottomBound, 5);
            VirtualProtect(quickActionBottomBound, 5, oldProtect, &oldProtect);
        } else {
            log_line("quick-action bottom-bound signature mismatch at %p", quickActionBottomBound);
            return;
        }
    }
    if (memcmp(quickActionXStore, fixedQuickActionXStore, 7) == 0) {
        DWORD oldProtect;
        INT32 displacement = (INT32)((BYTE *)hooked_widescreen_quick_action_x_store -
                                     (quickActionXStore + 5));
        BYTE replacement[7] = { 0xe8, 0, 0, 0, 0, 0x90, 0x90 };
        CopyMemory(replacement + 1, &displacement, sizeof(displacement));
        if (!VirtualProtect(quickActionXStore, 7, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            log_line("quick-action x-store protection failed at %p", quickActionXStore);
            return;
        }
        CopyMemory(quickActionXStore, replacement, 7);
        FlushInstructionCache(GetCurrentProcess(), quickActionXStore, 7);
        VirtualProtect(quickActionXStore, 7, oldProtect, &oldProtect);
    } else {
        log_line("quick-action x-store signature mismatch at %p", quickActionXStore);
        return;
    }
    if (memcmp(quickActionXLoad, fixedQuickActionXLoad, 7) == 0) {
        DWORD oldProtect;
        INT32 displacement = (INT32)((BYTE *)hooked_widescreen_quick_action_x_load -
                                     (quickActionXLoad + 5));
        BYTE replacement[7] = { 0xe8, 0, 0, 0, 0, 0x90, 0x90 };
        CopyMemory(replacement + 1, &displacement, sizeof(displacement));
        if (!VirtualProtect(quickActionXLoad, 7, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            log_line("quick-action x-load protection failed at %p", quickActionXLoad);
            return;
        }
        CopyMemory(quickActionXLoad, replacement, 7);
        FlushInstructionCache(GetCurrentProcess(), quickActionXLoad, 7);
        VirtualProtect(quickActionXLoad, 7, oldProtect, &oldProtect);
    } else {
        log_line("quick-action x-load signature mismatch at %p", quickActionXLoad);
        return;
    }
    if (memcmp(quickActionYStore, fixedQuickActionYStore, 7) == 0) {
        DWORD oldProtect;
        INT32 displacement = (INT32)((BYTE *)hooked_widescreen_quick_action_y_store -
                                     (quickActionYStore + 5));
        BYTE replacement[7] = { 0xe8, 0, 0, 0, 0, 0x90, 0x90 };
        CopyMemory(replacement + 1, &displacement, sizeof(displacement));
        if (!VirtualProtect(quickActionYStore, 7, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            log_line("quick-action y-store protection failed at %p", quickActionYStore);
            return;
        }
        CopyMemory(quickActionYStore, replacement, 7);
        FlushInstructionCache(GetCurrentProcess(), quickActionYStore, 7);
        VirtualProtect(quickActionYStore, 7, oldProtect, &oldProtect);
    } else {
        log_line("quick-action y-store signature mismatch at %p", quickActionYStore);
        return;
    }
    if (memcmp(quickActionYLoad, fixedQuickActionYLoad, 7) == 0) {
        DWORD oldProtect;
        INT32 displacement = (INT32)((BYTE *)hooked_widescreen_quick_action_y_load -
                                     (quickActionYLoad + 5));
        BYTE replacement[7] = { 0xe8, 0, 0, 0, 0, 0x90, 0x90 };
        CopyMemory(replacement + 1, &displacement, sizeof(displacement));
        if (!VirtualProtect(quickActionYLoad, 7, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            log_line("quick-action y-load protection failed at %p", quickActionYLoad);
            return;
        }
        CopyMemory(quickActionYLoad, replacement, 7);
        FlushInstructionCache(GetCurrentProcess(), quickActionYLoad, 7);
        VirtualProtect(quickActionYLoad, 7, oldProtect, &oldProtect);
    } else {
        log_line("quick-action y-load signature mismatch at %p", quickActionYLoad);
        return;
    }
#endif
    log_line("installed caller-scoped 854x480 HUD and loading anchors");
}
#endif

static void patch_slot(void **slot, void *replacement, void **original, const char *name)
{
    DWORD oldProtect;
    if (!slot || !*slot || *original) return;
    if (!VirtualProtect(slot, sizeof(*slot), PAGE_READWRITE, &oldProtect)) {
        log_line("patch %s failed: VirtualProtect=%lu", name, GetLastError());
        return;
    }
    *original = *slot;
    *slot = replacement;
    VirtualProtect(slot, sizeof(*slot), oldProtect, &oldProtect);
    log_line("patched %s slot=%p original=%p", name, slot, *original);
}

#if ABOM_TIMING_PROBE
typedef VOID (WINAPI *SleepFn)(DWORD);
typedef DWORD (WINAPI *WaitForSingleObjectFn)(HANDLE, DWORD);
typedef DWORD (WINAPI *MsgWaitForMultipleObjectsFn)(DWORD, const HANDLE *, BOOL,
                                                     DWORD, DWORD);
typedef DWORD (WINAPI *GetTickCountFn)(void);
typedef DWORD (WINAPI *TimeGetTimeFn)(void);
static SleepFn g_realSleep;
static WaitForSingleObjectFn g_realWaitForSingleObject;
static MsgWaitForMultipleObjectsFn g_realMsgWaitForMultipleObjects;
static GetTickCountFn g_realGameGetTickCount;
static TimeGetTimeFn g_realGameTimeGetTime;

static VOID WINAPI hooked_sleep(DWORD milliseconds)
{
    DWORD start = GetTickCount();
    g_realSleep(milliseconds);
    {
        LONG duration = (LONG)(GetTickCount() - start);
        record_timing_duration(&g_sleepDurationTotal, &g_sleepDurationCount,
                               &g_sleepDurationMaximum, duration);
        if (InterlockedCompareExchange(&g_masterDispatchActive, 0, 0) != 0 &&
            (DWORD)InterlockedCompareExchange(&g_masterDispatchThreadId, 0, 0) ==
                GetCurrentThreadId()) {
            record_timing_duration(&g_dispatchSleepDurationTotal,
                                   &g_dispatchSleepDurationCount,
                                   &g_dispatchSleepDurationMaximum, duration);
            InterlockedExchangeAdd(&g_dispatchSleepRequestedTotal,
                                   (LONG)milliseconds);
        }
    }
}

#if ABOM_RENDER_ONLY_PROBE
static void run_render_only_pass(void)
{
    BYTE *base = (BYTE *)GetModuleHandleA(NULL);
    BYTE *mission = *(BYTE **)0x00693870;
    BYTE *renderState = *(BYTE **)0x006df058;
    BYTE *worldState = *(BYTE **)0x006df07c;
    BYTE *presentation = *(BYTE **)0x006df054;
    int index;
    int count;
    BYTE **objects;
    typedef void (__attribute__((thiscall)) *ThisVoidFn)(void *);
    typedef void (__attribute__((thiscall)) *ThisIntFn)(void *, int);
    ThisVoidFn beginFrame;
    ThisVoidFn drawObject;
    ThisIntFn drawWorld;
    ThisVoidFn drawMissionUi;
    ThisVoidFn drawOverlay;
    ThisVoidFn drawCursor;
    ThisVoidFn present;
    if (!base || !mission || !renderState || !worldState || !presentation) return;
    if (*(volatile LONG *)0x0065b140 != 7 ||
        *(volatile LONG *)0x00644eec != ABOM_WIDESCREEN_WIDTH ||
        *(volatile LONG *)0x00644ef0 != ABOM_WIDESCREEN_HEIGHT) return;
    beginFrame = (ThisVoidFn)(base + 0x1a3180);
    drawObject = (ThisVoidFn)(base + 0x12e8a0);
    drawWorld = (ThisIntFn)(base + 0x149d40);
    drawMissionUi = (ThisVoidFn)(base + 0x1b2450);
    drawOverlay = (ThisVoidFn)(base + 0x1d3f30);
    drawCursor = (ThisVoidFn)(base + 0x17c820);
    present = (ThisVoidFn)(base + 0x19f950);
    beginFrame(renderState);
    count = *(int *)(mission + 0x2378);
    objects = *(BYTE ***)(mission + 0x236c);
    if (count < 0 || count > 4096 || (count && !objects)) return;
    for (index = 0; index < count; ++index) {
        if (objects[index]) drawObject(objects[index]);
    }
    drawWorld(worldState, 0);
    drawMissionUi(mission);
    drawOverlay(*(void **)(mission + 0x23c4));
    drawCursor(*(void **)(mission + 0x23c0));
    present(presentation);
    InterlockedIncrement(&g_renderOnlyProbeCalls);
}
#endif

static DWORD WINAPI hooked_game_get_tick_count(void)
{
    DWORD now = g_realGameGetTickCount();
    if (InterlockedCompareExchange(&g_masterDispatchActive, 0, 0) != 0 &&
        (DWORD)InterlockedCompareExchange(&g_masterDispatchThreadId, 0, 0) ==
            GetCurrentThreadId()) {
        BYTE *base = (BYTE *)GetModuleHandleA(NULL);
        BYTE *caller = (BYTE *)__builtin_return_address(0);
        InterlockedIncrement(&g_dispatchGetTickCalls);
        if (caller == base + 0x12b054)
            InterlockedIncrement(&g_dispatchGetTickFrameCounter);
        else if (caller == base + 0x199196)
            InterlockedIncrement(&g_dispatchGetTickInitCounter);
        else if (caller == base + 0x1ea419)
            InterlockedIncrement(&g_dispatchGetTickPacketStampCounter);
        else if (caller == base + 0x1ea4b1)
            InterlockedIncrement(&g_dispatchGetTickPacketPollCounter);
        else {
            InterlockedIncrement(&g_dispatchGetTickOtherCounter);
            InterlockedCompareExchangePointer(&g_dispatchGetTickUnknownCaller,
                                              caller, NULL);
        }
#if ABOM_RENDER_ONLY_PROBE
        if (caller == base + 0x12afe2 &&
            (DWORD)(now - (DWORD)InterlockedCompareExchange(
                &g_renderOnlyProbeLastTick, 0, 0)) >= 15 &&
            InterlockedCompareExchange(&g_renderOnlyProbeActive, 1, 0) == 0) {
            InterlockedExchange(&g_renderOnlyProbeLastTick, (LONG)now);
            run_render_only_pass();
            InterlockedExchange(&g_renderOnlyProbeActive, 0);
        }
#endif
    }
    return now;
}

static DWORD WINAPI hooked_game_time_get_time(void)
{
    if (InterlockedCompareExchange(&g_masterDispatchActive, 0, 0) != 0 &&
        (DWORD)InterlockedCompareExchange(&g_masterDispatchThreadId, 0, 0) ==
            GetCurrentThreadId())
        InterlockedIncrement(&g_dispatchTimeGetTimeCalls);
    return g_realGameTimeGetTime();
}

static DWORD WINAPI hooked_wait_for_single_object(HANDLE handle, DWORD milliseconds)
{
    DWORD start = GetTickCount();
    DWORD result = g_realWaitForSingleObject(handle, milliseconds);
    record_timing_duration(&g_waitDurationTotal, &g_waitDurationCount,
                           &g_waitDurationMaximum,
                           (LONG)(GetTickCount() - start));
    return result;
}

static DWORD WINAPI hooked_msg_wait_for_multiple_objects(DWORD count,
                                                          const HANDLE *handles,
                                                          BOOL waitAll,
                                                          DWORD milliseconds,
                                                          DWORD wakeMask)
{
    DWORD start = GetTickCount();
    DWORD result = g_realMsgWaitForMultipleObjects(count, handles, waitAll,
                                                    milliseconds, wakeMask);
    record_timing_duration(&g_msgWaitDurationTotal, &g_msgWaitDurationCount,
                           &g_msgWaitDurationMaximum,
                           (LONG)(GetTickCount() - start));
    return result;
}

static void hook_timing_wait_imports(void)
{
    HMODULE module = GetModuleHandleA(NULL);
    IMAGE_DOS_HEADER *dos;
    IMAGE_NT_HEADERS *nt;
    IMAGE_IMPORT_DESCRIPTOR *imports;
    if (!module) return;
    dos = (IMAGE_DOS_HEADER *)module;
    nt = (IMAGE_NT_HEADERS *)((BYTE *)module + dos->e_lfanew);
    imports = (IMAGE_IMPORT_DESCRIPTOR *)((BYTE *)module +
        nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    for (; imports->Name; ++imports) {
        IMAGE_THUNK_DATA *lookup = (IMAGE_THUNK_DATA *)((BYTE *)module + imports->OriginalFirstThunk);
        IMAGE_THUNK_DATA *iat = (IMAGE_THUNK_DATA *)((BYTE *)module + imports->FirstThunk);
        if (!imports->OriginalFirstThunk) lookup = iat;
        for (; lookup->u1.AddressOfData; ++lookup, ++iat) {
            IMAGE_IMPORT_BY_NAME *entry;
            const char *name;
            if (IMAGE_SNAP_BY_ORDINAL(lookup->u1.Ordinal)) continue;
            entry = (IMAGE_IMPORT_BY_NAME *)((BYTE *)module + lookup->u1.AddressOfData);
            name = (const char *)entry->Name;
            if (lstrcmpiA(name, "Sleep") == 0) {
                patch_slot((void **)&iat->u1.Function, hooked_sleep,
                           (void **)&g_realSleep, "abomb.exe!Sleep");
            } else if (lstrcmpiA(name, "WaitForSingleObject") == 0) {
                patch_slot((void **)&iat->u1.Function, hooked_wait_for_single_object,
                           (void **)&g_realWaitForSingleObject,
                           "abomb.exe!WaitForSingleObject");
            } else if (lstrcmpiA(name, "MsgWaitForMultipleObjects") == 0) {
                patch_slot((void **)&iat->u1.Function,
                           hooked_msg_wait_for_multiple_objects,
                           (void **)&g_realMsgWaitForMultipleObjects,
                           "abomb.exe!MsgWaitForMultipleObjects");
            } else if (lstrcmpiA(name, "GetTickCount") == 0) {
                patch_slot((void **)&iat->u1.Function, hooked_game_get_tick_count,
                           (void **)&g_realGameGetTickCount,
                           "abomb.exe!GetTickCount");
            } else if (lstrcmpiA(name, "timeGetTime") == 0) {
                patch_slot((void **)&iat->u1.Function, hooked_game_time_get_time,
                           (void **)&g_realGameTimeGetTime,
                           "abomb.exe!timeGetTime");
            }
        }
    }
}
#endif

static HRESULT WINAPI hooked_flip(LPDIRECTDRAWSURFACE surface, LPDIRECTDRAWSURFACE target, DWORD flags)
{
    log_line("Flip surface=%p target=%p flags=0x%08lx", surface, target, flags);
    return g_realFlip ? g_realFlip(surface, target, flags) : DDERR_GENERIC;
}

static HRESULT WINAPI hooked_blt_fast(LPDIRECTDRAWSURFACE destination, DWORD x, DWORD y,
                                      LPDIRECTDRAWSURFACE source, LPRECT sourceRect, DWORD flags)
{
    void *caller = __builtin_return_address(0);
    log_line("BltFast dst=%p src=%p at=(%lu,%lu) flags=0x%08lx", destination, source, x, y, flags);
    log_rect("  srcRect", sourceRect);
    log_blt_fast_caller_once(caller, x, y, sourceRect);
    if (!sourceRect && x == 0 && y == 0) {
        log_full_copy_caller_once(caller);
    }
    return g_realBltFast ? g_realBltFast(destination, x, y, source, sourceRect, flags) : DDERR_GENERIC;
}

static HRESULT WINAPI hooked_lock(LPDIRECTDRAWSURFACE surface, LPRECT rect,
                                  LPDDSURFACEDESC desc, DWORD flags, HANDLE event)
{
    HRESULT result;
    log_lock_caller_once(__builtin_return_address(0), rect, flags);
    result = g_realLock ? g_realLock(surface, rect, desc, flags, event) : DDERR_GENERIC;
#if ABOM_BACKGROUND_PROBE
    if (SUCCEEDED(result)) {
        log_live_lock_stack_once(surface, desc, flags);
#if ABOM_GUARD_WRITE_PROBE
        arm_surface_write_guard(surface, desc, flags);
#endif
    }
#endif
    return result;
}

static HRESULT WINAPI hooked_unlock(LPDIRECTDRAWSURFACE surface, LPVOID data)
{
    log_unlock_caller_once(__builtin_return_address(0));
#if ABOM_BACKGROUND_PROBE && ABOM_GUARD_WRITE_PROBE
    restore_surface_write_guard(surface);
#endif
    return g_realUnlock ? g_realUnlock(surface, data) : DDERR_GENERIC;
}

static HRESULT WINAPI hooked_get_dc(LPDIRECTDRAWSURFACE surface, HDC *dc)
{
    log_get_dc_caller_once(__builtin_return_address(0));
    return g_realGetDC ? g_realGetDC(surface, dc) : DDERR_GENERIC;
}

static void log_actual_surface_desc(LPDIRECTDRAWSURFACE surface)
{
    DDSURFACEDESC actual;
    HRESULT result;
    if (!surface) return;
    ZeroMemory(&actual, sizeof(actual));
    actual.dwSize = sizeof(actual);
    result = IDirectDrawSurface_GetSurfaceDesc(surface, &actual);
    if (SUCCEEDED(result)) {
        log_line("  actualSurface flags=0x%08lx caps=0x%08lx width=%lu height=%lu pitch=%ld bpp=%lu",
                 actual.dwFlags, actual.ddsCaps.dwCaps, actual.dwWidth, actual.dwHeight,
                 actual.lPitch, actual.ddpfPixelFormat.dwRGBBitCount);
    } else {
        log_line("  GetSurfaceDesc result=0x%08lx", result);
    }
}

static HRESULT WINAPI hooked_blt(LPDIRECTDRAWSURFACE destination, LPRECT destinationRect,
                                 LPDIRECTDRAWSURFACE source, LPRECT sourceRect,
                                 DWORD flags, LPDDBLTFX effects)
{
    log_line("Blt dst=%p src=%p flags=0x%08lx", destination, source, flags);
    log_rect("  dstRect", destinationRect);
    log_rect("  srcRect", sourceRect);
    return g_realBlt ? g_realBlt(destination, destinationRect, source, sourceRect, flags, effects) : DDERR_GENERIC;
}

static void hook_surface(LPDIRECTDRAWSURFACE surface)
{
    void **vtable;
    if (!surface) return;
    vtable = *(void ***)surface;
    /* IDirectDrawSurface v1: Blt 5, BltFast 7, Flip 11, GetDC 17,
     * Lock 25, Unlock 32. */
    patch_slot(&vtable[5], hooked_blt, (void **)&g_realBlt, "IDirectDrawSurface::Blt");
    patch_slot(&vtable[7], hooked_blt_fast, (void **)&g_realBltFast, "IDirectDrawSurface::BltFast");
    patch_slot(&vtable[11], hooked_flip, (void **)&g_realFlip, "IDirectDrawSurface::Flip");
    patch_slot(&vtable[17], hooked_get_dc, (void **)&g_realGetDC, "IDirectDrawSurface::GetDC");
    patch_slot(&vtable[25], hooked_lock, (void **)&g_realLock, "IDirectDrawSurface::Lock");
    patch_slot(&vtable[32], hooked_unlock, (void **)&g_realUnlock, "IDirectDrawSurface::Unlock");
}

static HRESULT WINAPI hooked_create_surface(LPDIRECTDRAW dd, LPDDSURFACEDESC desc,
                                             LPDIRECTDRAWSURFACE *surface, IUnknown *outer)
{
    HRESULT result;
    void *caller = __builtin_return_address(0);
    if (desc) {
        log_line("CreateSurface caller=%p flags=0x%08lx caps=0x%08lx width=%lu height=%lu pitch=%ld",
                 caller, desc->dwFlags, desc->ddsCaps.dwCaps, desc->dwWidth, desc->dwHeight, desc->lPitch);
    } else {
        log_line("CreateSurface caller=%p desc=<null>", caller);
    }
    result = g_realCreateSurface ? g_realCreateSurface(dd, desc, surface, outer) : DDERR_GENERIC;
    log_line("CreateSurface result=0x%08lx surface=%p", result, surface ? *surface : NULL);
    if (SUCCEEDED(result) && surface) {
        hook_surface(*surface);
        log_actual_surface_desc(*surface);
    }
    return result;
}

static HRESULT WINAPI hooked_set_cooperative_level(LPDIRECTDRAW dd, HWND window, DWORD flags)
{
    log_line("SetCooperativeLevel hwnd=%p flags=0x%08lx", window, flags);
    return g_realSetCooperativeLevel ? g_realSetCooperativeLevel(dd, window, flags) : DDERR_GENERIC;
}

static HRESULT WINAPI hooked_set_display_mode(LPDIRECTDRAW dd, DWORD width, DWORD height, DWORD bpp)
{
    log_line("SetDisplayMode %lux%lux%lu", width, height, bpp);
    return g_realSetDisplayMode ? g_realSetDisplayMode(dd, width, height, bpp) : DDERR_GENERIC;
}

static void hook_directdraw(LPDIRECTDRAW dd)
{
    void **vtable;
    if (!dd) return;
    vtable = *(void ***)dd;
    /* IDirectDraw v1: CreateSurface 6, SetCooperativeLevel 20, SetDisplayMode 21. */
    patch_slot(&vtable[6], hooked_create_surface, (void **)&g_realCreateSurface, "IDirectDraw::CreateSurface");
    patch_slot(&vtable[20], hooked_set_cooperative_level, (void **)&g_realSetCooperativeLevel, "IDirectDraw::SetCooperativeLevel");
    patch_slot(&vtable[21], hooked_set_display_mode, (void **)&g_realSetDisplayMode, "IDirectDraw::SetDisplayMode");
}

static HRESULT WINAPI hooked_direct_draw_create(GUID *guid, LPDIRECTDRAW *dd, IUnknown *outer)
{
    HRESULT result = g_realDirectDrawCreate ? g_realDirectDrawCreate(guid, dd, outer) : DDERR_GENERIC;
    log_line("DirectDrawCreate result=0x%08lx interface=%p", result, dd ? *dd : NULL);
    if (SUCCEEDED(result) && dd) hook_directdraw(*dd);
    return result;
}

static void patch_abomb_iat(void)
{
    HMODULE module = GetModuleHandleA(NULL);
    IMAGE_DOS_HEADER *dos;
    IMAGE_NT_HEADERS *nt;
    IMAGE_IMPORT_DESCRIPTOR *imports;
    if (!module) return;
    dos = (IMAGE_DOS_HEADER *)module;
    nt = (IMAGE_NT_HEADERS *)((BYTE *)module + dos->e_lfanew);
    imports = (IMAGE_IMPORT_DESCRIPTOR *)((BYTE *)module + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    for (; imports->Name; ++imports) {
        IMAGE_THUNK_DATA *lookup = (IMAGE_THUNK_DATA *)((BYTE *)module + imports->OriginalFirstThunk);
        IMAGE_THUNK_DATA *iat = (IMAGE_THUNK_DATA *)((BYTE *)module + imports->FirstThunk);
        if (!imports->OriginalFirstThunk) lookup = iat;
        for (; lookup->u1.AddressOfData; ++lookup, ++iat) {
            IMAGE_IMPORT_BY_NAME *name;
            if (IMAGE_SNAP_BY_ORDINAL(lookup->u1.Ordinal)) continue;
            name = (IMAGE_IMPORT_BY_NAME *)((BYTE *)module + lookup->u1.AddressOfData);
            if (lstrcmpiA((LPCSTR)name->Name, "DirectDrawCreate") == 0) {
                patch_slot((void **)&iat->u1.Function, hooked_direct_draw_create,
                           (void **)&g_realDirectDrawCreate, "abomb.exe!DirectDrawCreate");
                return;
            }
        }
    }
    log_line("DirectDrawCreate import not found in abomb.exe");
}

static DWORD WINAPI initialize_hook(LPVOID unused)
{
    char executable[MAX_PATH];
    (void)unused;
    GetModuleFileNameA(NULL, executable, sizeof(executable));
    GetModuleFileNameA(GetModuleHandleA("abom_diagnostics.dll"), g_logPath, sizeof(g_logPath));
    {
        char *slash = strrchr(g_logPath, '\\');
        if (slash) {
#if ABOM_TIMING_PROBE
            lstrcpyA(slash + 1, "abom_timing_probe.log");
#elif ABOM_MODEL_SUPERSAMPLE_PROBE
            lstrcpyA(slash + 1, "abom_model_supersample_probe.log");
#elif ABOM_MODEL_RASTER_PROBE
            lstrcpyA(slash + 1, "abom_model_raster_probe.log");
#elif ABOM_UI_PROBE
            lstrcpyA(slash + 1, "abom_diagnostics.rc3-ui-probe.log");
#elif ABOM_EXPERIMENTAL_TERRAIN_COPY
            lstrcpyA(slash + 1, "abom_diagnostics.widescreen-854-v27pitch.log");
#elif ABOM_BACKGROUND_PROBE
            lstrcpyA(slash + 1, "abom_diagnostics.widescreen-854-v24guardwrites.log");
#else
            lstrcpyA(slash + 1, "abom_diagnostics.log");
#endif
        }
    }
    InitializeCriticalSection(&g_logLock);
#if ABOM_FRAME_INTERPOLATION_PROBE
    InitializeCriticalSection(&g_interpolationLock);
#endif
    g_initialized = 1;
    log_line("Abomination DirectDraw diagnostics loaded for %s", executable);
#if ABOM_BACKGROUND_PROBE && ABOM_GUARD_WRITE_PROBE
    g_guardHandler = AddVectoredExceptionHandler(1, guarded_surface_exception);
    log_line("GuardHandler=%p", g_guardHandler);
#endif
#if !ABOM_PRODUCTION_WIDESCREEN
    patch_abomb_iat();
    hook_sprite_blitter();
    hook_tiled_compositor();
    hook_object_blitter();
    hook_pixel_copy();
#if ABOM_BACKGROUND_PROBE
    hook_opaque_copy();
    hook_surface_dispatch();
    hook_surface_lock_helper();
#endif
    hook_world_renderer();
    hook_camera_controller();
    hook_presentation();
#endif
#if ABOM_UI_PROBE && ABOM_PRODUCTION_WIDESCREEN
    hook_sprite_blitter();
#endif
    hook_mode_request();
#if ABOM_EXPERIMENTAL_WIDESCREEN
    hook_widescreen_present_copy();
#endif
#if ABOM_EXPERIMENTAL_TERRAIN_COPY
    hook_widescreen_terrain_copy();
#endif
#if ABOM_EXPERIMENTAL_TERRAIN_REFRESH
    hook_widescreen_terrain_refresh();
#endif
#if ABOM_EXPERIMENTAL_HUD_ANCHORS
    hook_widescreen_hud_anchors();
#endif
#if ABOM_TIMING_PROBE
    hook_master_dispatch_call();
    hook_lockstep_wait_call();
    hook_status_call();
    hook_timing_wait_imports();
    {
        HANDLE thread = CreateThread(NULL, 0, timing_probe_thread, NULL, 0, NULL);
        if (thread) {
            CloseHandle(thread);
            log_line("timing probe started");
        } else {
            log_line("timing probe start failed: %lu", GetLastError());
        }
    }
#endif
#if ABOM_FRAME_INTERPOLATION_PROBE
    {
        HANDLE thread = CreateThread(NULL, 0, interpolation_probe_thread, NULL, 0, NULL);
        if (thread) {
            CloseHandle(thread);
            log_line("presentation interpolation probe started");
        } else {
            log_line("presentation interpolation probe start failed: %lu", GetLastError());
        }
    }
#endif
#if ABOM_MODEL_RASTER_PROBE
    hook_model_raster_dispatch();
#if ABOM_MODEL_SUPERSAMPLE_PROBE
    hook_model_render();
#endif
#endif
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    (void)instance;
    (void)reserved;
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(instance);
        if (!CreateThread(NULL, 0, initialize_hook, NULL, 0, NULL)) return FALSE;
    }
    return TRUE;
}
