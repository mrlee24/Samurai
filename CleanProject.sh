# Set the path to your Unreal project directory
project_path="/Users/sayohyeah/Desktop/Samurai"

# Remove Binaries, Intermediate, and Saved folders
rm -rf "${project_path}/Binaries"
rm -rf "${project_path}/Intermediate"
rm -rf "${project_path}/Saved"

echo "Binaries, Intermediate, and Saved folders removed successfully."
exit 0
