[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('PlayBordered', 'PlayBorderless', 'PlayWidescreen', 'SetupRegistry', 'RestoreRegistry')]
    [string]$Action,
    [switch]$Elevated
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version 2.0

$GameDirectory = $PSScriptRoot
$GameExecutable = Join-Path $GameDirectory 'abomb.exe'
$RegistrySubKey = 'SOFTWARE\Hothouse Creations - Abomination\Abomination: The Nemesis Project'
$BackupPath = Join-Path $GameDirectory '.abominable-registry-backup.json'

function Test-Administrator {
    $identity = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = [Security.Principal.WindowsPrincipal]::new($identity)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

function Assert-GameDirectory {
    if (-not (Test-Path -LiteralPath $GameExecutable -PathType Leaf)) {
        throw 'abomb.exe was not found beside this script. Extract Abominable into the game directory.'
    }
}

function Assert-GameStopped {
    $running = @(Get-Process -Name 'abomb', 'ReBenchmark' -ErrorAction SilentlyContinue)
    if ($running.Count -gt 0) {
        throw 'Close Abomination and ReBenchmark before changing the registry.'
    }
}

function Start-ElevatedAction {
    param([Parameter(Mandatory = $true)][string]$RequestedAction)

    $arguments = @(
        '-NoLogo', '-NoProfile', '-ExecutionPolicy', 'Bypass',
        '-File', ('"{0}"' -f $PSCommandPath), '-Action', $RequestedAction, '-Elevated'
    )
    # Registry restore may require a typed confirmation, so this elevated
    # process is intentionally visible and interactive.
    $process = Start-Process -FilePath 'powershell.exe' -Verb RunAs -ArgumentList $arguments -Wait -PassThru
    if ($process.ExitCode -ne 0) {
        throw "The elevated registry action failed or was cancelled (exit code $($process.ExitCode))."
    }
}

function ConvertTo-BackupValue {
    param(
        [Parameter(Mandatory = $true)][Microsoft.Win32.RegistryKey]$Key,
        [Parameter(Mandatory = $true)][AllowEmptyString()][string]$Name
    )

    $kind = $Key.GetValueKind($Name)
    $value = $Key.GetValue($Name, $null, [Microsoft.Win32.RegistryValueOptions]::DoNotExpandEnvironmentNames)
    if ($kind -eq [Microsoft.Win32.RegistryValueKind]::Binary) {
        $value = [Convert]::ToBase64String([byte[]]$value)
    }
    [pscustomobject]@{ Name = $Name; Kind = $kind.ToString(); Value = $value }
}

function Save-RegistryBackup {
    param([Parameter(Mandatory = $true)][Microsoft.Win32.RegistryKey]$BaseKey)

    if (Test-Path -LiteralPath $BackupPath) { return }

    $key = $BaseKey.OpenSubKey($RegistrySubKey, $false)
    try {
        $values = @()
        if ($null -ne $key) {
            foreach ($name in $key.GetValueNames()) {
                $values += ConvertTo-BackupValue -Key $key -Name $name
            }
        }
        [pscustomobject]@{
            FormatVersion = 1
            RegistryView = 'Registry32'
            Key = $RegistrySubKey
            KeyExisted = ($null -ne $key)
            CreatedUtc = [DateTime]::UtcNow.ToString('o')
            Values = $values
        } | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $BackupPath -Encoding UTF8
    } finally {
        if ($null -ne $key) { $key.Dispose() }
    }
}

function Set-GameRegistryPath {
    Assert-GameDirectory
    Assert-GameStopped
    $base = [Microsoft.Win32.RegistryKey]::OpenBaseKey(
        [Microsoft.Win32.RegistryHive]::LocalMachine,
        [Microsoft.Win32.RegistryView]::Registry32
    )
    try {
        Save-RegistryBackup -BaseKey $base
        $key = $base.CreateSubKey($RegistrySubKey, $true)
        if ($null -eq $key) { throw 'Windows did not create or open the game registry key.' }
        try {
            $key.SetValue('PATH', $GameDirectory, [Microsoft.Win32.RegistryValueKind]::String)
        } finally {
            $key.Dispose()
        }
    } finally {
        $base.Dispose()
    }
    Write-Host "Registry PATH set to: $GameDirectory" -ForegroundColor Green
    Write-Host 'If this is a fresh or copied installation, run ReBenchmark.exe once before playing.'
}

function ConvertFrom-BackupValue {
    param([Parameter(Mandatory = $true)]$Entry)

    $kind = [Microsoft.Win32.RegistryValueKind]([Enum]::Parse(
        [Microsoft.Win32.RegistryValueKind], [string]$Entry.Kind, $true
    ))
    $value = switch ($kind) {
        ([Microsoft.Win32.RegistryValueKind]::Binary) { [Convert]::FromBase64String([string]$Entry.Value); break }
        ([Microsoft.Win32.RegistryValueKind]::DWord) { [int]$Entry.Value; break }
        ([Microsoft.Win32.RegistryValueKind]::QWord) { [long]$Entry.Value; break }
        ([Microsoft.Win32.RegistryValueKind]::MultiString) { [string[]]$Entry.Value; break }
        default { [string]$Entry.Value }
    }
    return [pscustomobject]@{ Kind = $kind; Value = $value }
}

function Restore-GameRegistry {
    Assert-GameStopped
    if (-not (Test-Path -LiteralPath $BackupPath -PathType Leaf)) {
        throw 'No .abominable-registry-backup.json file was found. Nothing can be restored safely.'
    }
    $backup = Get-Content -LiteralPath $BackupPath -Raw | ConvertFrom-Json
    if ([int]$backup.FormatVersion -ne 1 -or [string]$backup.Key -ne $RegistrySubKey -or
        [string]$backup.RegistryView -ne 'Registry32') {
        throw 'The registry backup is not a supported Abominable backup.'
    }
    if (-not [bool]$backup.KeyExisted) {
        $answer = Read-Host 'The key did not exist before setup. Remove it and all settings created since then? Type RESTORE'
        if ($answer -cne 'RESTORE') {
            Write-Host 'Registry restoration cancelled.'
            return
        }
    }

    $base = [Microsoft.Win32.RegistryKey]::OpenBaseKey(
        [Microsoft.Win32.RegistryHive]::LocalMachine,
        [Microsoft.Win32.RegistryView]::Registry32
    )
    try {
        if ([bool]$backup.KeyExisted) {
            $key = $base.CreateSubKey($RegistrySubKey, $true)
            if ($null -eq $key) { throw 'Windows did not create or open the game registry key.' }
            try {
                foreach ($name in $key.GetValueNames()) { $key.DeleteValue($name, $false) }
                foreach ($entry in @($backup.Values)) {
                    $decoded = ConvertFrom-BackupValue -Entry $entry
                    $key.SetValue([string]$entry.Name, $decoded.Value, $decoded.Kind)
                }
            } finally {
                $key.Dispose()
            }
        } else {
            $existing = $base.OpenSubKey($RegistrySubKey, $false)
            if ($null -ne $existing) {
                $existing.Dispose()
                $base.DeleteSubKeyTree($RegistrySubKey, $false)
            }
        }
    } finally {
        $base.Dispose()
    }
    Remove-Item -LiteralPath $BackupPath -Force
    Write-Host 'The original 32-bit registry state was restored.' -ForegroundColor Green
}

function Start-GameWithProfile {
    param([Parameter(Mandatory = $true)][string]$ProfileName)

    Assert-GameDirectory
    if (@(Get-Process -Name 'abomb' -ErrorAction SilentlyContinue).Count -gt 0) {
        throw 'Abomination is already running.'
    }
    $profile = Join-Path (Join-Path $GameDirectory 'Profiles') $ProfileName
    if (-not (Test-Path -LiteralPath $profile -PathType Leaf)) {
        throw "Required profile not found: $profile"
    }
    foreach ($required in @('ddraw.dll', 'dxwrapper.dll')) {
        if (-not (Test-Path -LiteralPath (Join-Path $GameDirectory $required) -PathType Leaf)) {
            throw "Required wrapper file not found: $required"
        }
    }
    $customDllMatch = Get-Content -LiteralPath $profile |
        Where-Object { $_ -match '^\s*LoadCustomDllPath\s*=\s*(\S+)\s*$' } |
        Select-Object -First 1
    if ($customDllMatch -and $customDllMatch -match '^\s*LoadCustomDllPath\s*=\s*(\S+)\s*$') {
        $customDll = $Matches[1]
        if (-not (Test-Path -LiteralPath (Join-Path $GameDirectory $customDll) -PathType Leaf)) {
            throw "Required custom renderer not found: $customDll"
        }
    }
    Copy-Item -LiteralPath $profile -Destination (Join-Path $GameDirectory 'dxwrapper.ini') -Force
    Start-Process -FilePath $GameExecutable -WorkingDirectory $GameDirectory
}

try {
    switch ($Action) {
        'PlayBordered' { Start-GameWithProfile -ProfileName 'Bordered.ini' }
        'PlayBorderless' { Start-GameWithProfile -ProfileName 'Borderless-4x3.ini' }
        'PlayWidescreen' { Start-GameWithProfile -ProfileName 'Widescreen-854x480.ini' }
        'SetupRegistry' {
            if (-not (Test-Administrator)) {
                if ($Elevated) { throw 'Administrator permission was not granted.' }
                Start-ElevatedAction -RequestedAction $Action
            } else { Set-GameRegistryPath }
        }
        'RestoreRegistry' {
            if (-not (Test-Administrator)) {
                if ($Elevated) { throw 'Administrator permission was not granted.' }
                Start-ElevatedAction -RequestedAction $Action
            } else { Restore-GameRegistry }
        }
    }
} catch {
    Write-Host "Abominable: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}
