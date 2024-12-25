# Set up a virtual environment (optional)
echo "Creating a virtual environment..."
python3 -m venv SmartTerrarium
source SmartTerrarium/bin/activate

echo "Update Pip"
pip install --upgrade pip

# Install required packages
echo "Installing required packages..."
pip install -r setup/requirements.txt