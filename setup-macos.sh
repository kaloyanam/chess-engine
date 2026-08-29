set -euo pipefail
cd "$(dirname "$0")"

echo "[1/4] Building engine..."
g++ converter.cpp -std=c++17 -O3 -march=native -flto -w -o converter

echo "[2/4] Fetching lichess-bot..."
[ -d lichess-bot ] || git clone --depth 1 https://github.com/lichess-bot-devs/lichess-bot.git
cd lichess-bot

echo "[3/4] Creating venv and installing dependencies..."
python3 -m venv venv
venv/bin/python -m pip install --upgrade pip
venv/bin/python -m pip install -r requirements.txt

echo "[4/4] Installing engine and config..."
mkdir -p engines && cp ../converter engines/
[ -f config.yml ] || cp config.yml.default config.yml

echo
echo "Done. Next steps:"
echo "  1. Edit lichess-bot/config.yml -> paste your BOT token;"
echo "     set engine { dir: \"./engines/\", name: \"converter\" }"
echo "  2. Run:  cd lichess-bot && venv/bin/python lichess-bot.py"
