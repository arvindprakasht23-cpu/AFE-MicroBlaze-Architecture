@echo off
echo =========================================
echo       AUTOMATED GITHUB UPDATER
echo =========================================
echo.
echo 1. Scanning for changed files...
git add .
echo.

echo 2. Packaging the update...
git commit -m "Automated code update from Windows PC"
echo.

echo 3. Pushing to GitHub...
git push origin main
echo.

echo =========================================
echo       UPDATE COMPLETE!
echo =========================================
pause