@echo on
cd windows-build

cd Examples\CI
dir

python -m junit2htmlreport --summary-matrix cicd-pass-plain.exe-Results.xml cicd-pass-setupfuncs.exe-Results.xml cicd-multi-suite.exe-Results.xml || exit /b

