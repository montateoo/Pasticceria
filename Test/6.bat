@echo off
cd /d "C:\Users\monta\CLionProjects\Pasticceria\cmake-build-debug"

set startTime=%TIME%
Pasticceria.exe < open6.txt > TestOut.txt
set endTime=%TIME%

for /F "tokens=1-4 delims=:.," %%a in ("%startTime%") do (
    set /A "startInSeconds=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)"
)

for /F "tokens=1-4 delims=:.," %%a in ("%endTime%") do (
    set /A "endInSeconds=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)"
)

set /A duration=endInSeconds-startInSeconds
echo Il programma ha impiegato %duration% secondi.

pause
