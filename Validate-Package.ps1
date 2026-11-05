[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version 2.0
$Root = $PSScriptRoot
$Failures = [System.Collections.Generic.List[string]]::new()

function Add-Failure([string]$Message) { $Failures.Add($Message) }

$required = @(
    'Abominable.ps1', 'ABOMINABLE-MANIFEST.txt', 'README.md',
    'ddraw.dll', 'dxwrapper.dll', 'dxwrapper.ini',
    'Profiles\Bordered.ini', 'Profiles\Borderless-4x3.ini',
    'Profiles\Widescreen-854x480.ini', 'Profiles\Widescreen-Desktop.ini',
    'abom_widescreen_854.dll', 'Play - Native Widescreen 854x480.cmd',
    'abom_renderer_2560x1440.dll',
    'Play - High Resolution Widescreen.cmd',
    'patches\abom-renderer.c', 'patches\README.md',
    'patches\dxwrapper-mode-aware-native-resolution.patch'
)
foreach ($relative in $required) {
    if (-not (Test-Path -LiteralPath (Join-Path $Root $relative) -PathType Leaf)) {
        Add-Failure "Missing required file: $relative"
    }
}

$expectedHashes = @{
    'ddraw.dll' = 'B8B878E29C2931D6A8D064665C4C93EDB28A0105152E7247F8F543C1603985C3'
    'dxwrapper.dll' = '18491B4196717F59ADCA9A91AA329A5A9FED76B6A877105C37BC0325008D412D'
    'abom_widescreen_854.dll' = '89F16CA3F84B591B6128BEB687A654C1A748951DFC07FBE06F01B03E9A5D1839'
    'abom_renderer_2560x1440.dll' = '884ACF9C822B4ECC3BF4E406319CF465DFAA0E584FE863DDA5611F593D6EFA6E'
}
foreach ($relative in $expectedHashes.Keys) {
    $path = Join-Path $Root $relative
    if (Test-Path -LiteralPath $path -PathType Leaf) {
        $actual = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash
        if ($actual -ne $expectedHashes[$relative]) {
            Add-Failure "$relative has an unexpected SHA-256 hash: $actual"
        }
    }
}

$profileExpectations = @{
    'Profiles\Bordered.ini' = @{
        Dd7to9 = '1'; DdrawUseNativeResolution = '0'; DdrawMaintainAspectRatio = '0'
        EnableWindowMode = '1'; WindowModeBorder = '1'; FullscreenWindowMode = '0'
        FullScreen = '0'; LimitPerFrameFPS = '0'; ShowFPSCounter = '0'
        Dinputto8 = '0'; EnableDsoundWrapper = '0'
    }
    'Profiles\Borderless-4x3.ini' = @{
        Dd7to9 = '1'; DdrawUseNativeResolution = '1'; DdrawMaintainAspectRatio = '1'
        EnableWindowMode = '1'; WindowModeBorder = '0'; FullscreenWindowMode = '0'
        FullScreen = '0'; LimitPerFrameFPS = '0'; ShowFPSCounter = '0'
        Dinputto8 = '0'; EnableDsoundWrapper = '0'
    }
    'Profiles\Widescreen-854x480.ini' = @{
        LoadCustomDllPath = 'abom_widescreen_854.dll'
        Dd7to9 = '1'; DdrawUseNativeResolution = '0'; DdrawMaintainAspectRatio = '0'
        EnableWindowMode = '1'; WindowModeBorder = '1'; FullscreenWindowMode = '0'
        FullScreen = '0'; LimitPerFrameFPS = '0'; ShowFPSCounter = '0'
        Dinputto8 = '0'; EnableDsoundWrapper = '0'
    }
    'Profiles\Widescreen-Desktop.ini' = @{
        LoadCustomDllPath = 'abom_renderer_2560x1440.dll'
        Dd7to9 = '1'; DdrawUseNativeResolution = '1'; DdrawMaintainAspectRatio = '1'
        EnableWindowMode = '1'; WindowModeBorder = '0'; FullscreenWindowMode = '0'
        FullScreen = '0'; LimitPerFrameFPS = '0'; ShowFPSCounter = '0'
        Dinputto8 = '0'; EnableDsoundWrapper = '0'
    }
}
foreach ($relative in $profileExpectations.Keys) {
    $path = Join-Path $Root $relative
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { continue }
    $lines = Get-Content -LiteralPath $path
    foreach ($setting in $profileExpectations[$relative].Keys) {
        $pattern = '^\s*{0}\s*=\s*(\S+)' -f [regex]::Escape($setting)
        $matches = @($lines | Where-Object { $_ -match $pattern })
        $expectedPattern = '=\s*{0}\s*$' -f $profileExpectations[$relative][$setting]
        if ($matches.Count -ne 1 -or $matches[0] -notmatch $expectedPattern) {
            Add-Failure "$relative does not have the expected $setting setting."
        }
    }
}

$defaultPath = Join-Path $Root 'dxwrapper.ini'
$borderedPath = Join-Path $Root 'Profiles\Bordered.ini'
if ((Test-Path -LiteralPath $defaultPath) -and (Test-Path -LiteralPath $borderedPath)) {
    if ((Get-FileHash $defaultPath).Hash -ne (Get-FileHash $borderedPath).Hash) {
        Add-Failure 'dxwrapper.ini is not identical to the default bordered profile.'
    }
}

$allFiles = @(Get-ChildItem -LiteralPath $Root -Recurse -File -ErrorAction SilentlyContinue |
    Where-Object { $_.FullName -notlike (Join-Path $Root '.git\*') })
foreach ($name in @('abomb.exe', 'ABOMB.ICD', 'ReBenchmark.exe', 'SoundFX.awf')) {
    if (@($allFiles | Where-Object { $_.Name -ieq $name }).Count -gt 0) {
        Add-Failure "Game file must not be distributed: $name"
    }
}

$tokens = $null
$parseErrors = $null
[void][System.Management.Automation.Language.Parser]::ParseFile(
    (Join-Path $Root 'Abominable.ps1'), [ref]$tokens, [ref]$parseErrors
)
foreach ($parseError in @($parseErrors)) {
    Add-Failure "PowerShell parse error: $($parseError.Message)"
}

if ($Failures.Count -gt 0) {
    Write-Host 'Abominable validation FAILED:' -ForegroundColor Red
    foreach ($failure in $Failures) { Write-Host " - $failure" -ForegroundColor Red }
    exit 1
}

Write-Host 'Abominable validation passed.' -ForegroundColor Green
Write-Host 'Wrapper binaries, profiles, default mode, scripts, and game-file exclusions are valid.'
