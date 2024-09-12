@echo off
echo Configuring Source SDK environment variables for Counter-Strike: Source

:: Set local environment variable that will only persist in processes
:: created by the current command processor shell.  We use this CD
:: trick so that we can set the variable to the canonical, absolute
:: name, without any ..'s in it.
PUSHD .
CD "%~d0%~p0..\cstrike"
SET VPROJECT=%CD%
POPD

:: Commentthis out if you ant to set it permanently as a system variable,
:: too
:: echo Attempding to set VPROJECT in system environment
:: SETX VPROJECT %VPROJECT%
:: echo.
echo VPROJECT=%VPROJECT%
