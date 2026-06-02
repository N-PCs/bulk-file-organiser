# Package urFiles for Windows download
# Run from project root: .\scripts\package-release.ps1

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$ReleaseDir = Join-Path $Root "release"
$PublicDir = Join-Path $Root "website\public"
$ZipName = "urFiles-windows.zip"
$ZipPath = Join-Path $PublicDir $ZipName

Write-Host "=== urFiles Release Packager ===" -ForegroundColor Cyan

# Build if organizer.exe missing
$ExePath = Join-Path $Root "organizer.exe"
if (-not (Test-Path $ExePath)) {
    Write-Host "organizer.exe not found. Running build.bat..." -ForegroundColor Yellow
    Push-Location $Root
    & cmd /c "build.bat"
    Pop-Location
    if (-not (Test-Path $ExePath)) {
        Write-Error "Build failed - organizer.exe still missing."
    }
}

# Prepare staging folder
$Stage = Join-Path $env:TEMP "urFiles-stage-$(Get-Random)"
New-Item -ItemType Directory -Path $Stage -Force | Out-Null

$Files = @(
    @{ Src = "organizer.exe"; Dst = "organizer.exe" },
    @{ Src = "config.json"; Dst = "config.json" },
    @{ Src = "run.bat"; Dst = "run.bat" },
    @{ Src = "organizer.bat"; Dst = "organizer.bat" },
    @{ Src = "release\README.txt"; Dst = "README.txt" }
)

foreach ($f in $Files) {
    $src = Join-Path $Root $f.Src
    if (-not (Test-Path $src)) {
        Write-Error "Missing required file: $($f.Src)"
    }
    Copy-Item $src (Join-Path $Stage $f.Dst)
}

# Create zip
New-Item -ItemType Directory -Path $PublicDir -Force | Out-Null
if (Test-Path $ZipPath) { Remove-Item $ZipPath -Force }
Compress-Archive -Path (Join-Path $Stage "*") -DestinationPath $ZipPath -CompressionLevel Optimal

Remove-Item $Stage -Recurse -Force

$SizeMb = [math]::Round((Get-Item $ZipPath).Length / 1MB, 2)
Write-Host "Created: $ZipPath - $SizeMb megabytes" -ForegroundColor Green
Write-Host "The website download button will serve this file." -ForegroundColor Green
