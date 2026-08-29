#Requires -Version 5
$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot

Write-Host "[1/4] Building engine..."
g++ converter.cpp -std=c++17 -O3 -march=native -flto -w -o converter.exe

Write-Host "[2/4] Fetching lichess-bot..."
if (-not (Test-Path lichess-bot)) { git clone --depth 1 https://github.com/lichess-bot-devs/lichess-bot.git }
Set-Location lichess-bot

Write-Host "[3/4] Creating venv and installing dependencies..."
python -m venv venv
venv\Scripts\python.exe -m pip install --upgrade pip
venv\Scripts\python.exe -m pip install -r requirements.txt

Write-Host "[4/4] Installing engine and config..."
New-Item -ItemType Directory -Force engines | Out-Null
Copy-Item ..\converter.exe engines\ -Force
if (-not (Test-Path config.yml)) { Copy-Item config.yml.default config.yml }

Write-Host ""
Write-Host "Done. Next steps:"
Write-Host "  1. Edit lichess-bot\config.yml -> paste your BOT token;"
Write-Host "     set engine { dir: './engines/', name: 'converter.exe' }"
Write-Host "  2. Run:  cd lichess-bot; venv\Scripts\python.exe lichess-bot.py"
