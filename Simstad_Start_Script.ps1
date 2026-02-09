param($AutoRestart = $true)

function Shutdown-SimStad {
    param($SimStadProcess)

    if (-not $SimStadProcess -or $SimStadProcess.HasExited) {
        Exit -1
    } else {
        Stop-Process -InputObject $SimStadProcess -Force
        Exit 0
    }
}

function Run-SimStad {
    param($SimStadPath, $AutoRestart)

    # Check for active SimStad Instances
    $existing = Get-Process SimStad -ErrorAction SilentlyContinue
    if ($existing) {
        Write-Host "SimStad is already running on this machine, currently multiple instances are not supported.
                    >>> Please close all other instances of SimStad first! <<<" -ForegroundColor Red
        Exit -1
    }

    # Checking that all binaries can be located
    if (-Not (Test-Path "$SimStadPath\DigitalTwin.exe")) {
        Write-Error "Could not find SimStad binary at $SimStadPath, did you forget building?"
        Exit -1
    }

    # Main Loop
    while ($true) {
        Write-Host "Starting SimStad..."

        $argsList = @(
            "-SunIntensity=4"
            "-MoonIntensity=3"
            "-game"
            "-messaging"
            "-dc_cluster"
            "-nosplash"
            "-fixedseed"
            "-NoVerifyGC"
            "-RemoteControlIsHeadless"
            "-StageFriendlyName=`"Test_2`""
            "-dc_cfg=`"Path\to\nDisplayConfig1_1.ndisplay`""
            "-dx12"
            "-dc_dev_mono"
            "-dc_node=Test_2"
            "Log=Test_2.log"
            "-ini:Game:[/Script/EngineSettings.GeneralProjectSettings]:bUseBorderlessWindow=True"
			"-ini:Engine:[/Script/Engine.Engine]:GameEngine=/Script/DisplayCluster.DisplayClusterGameEngine,[/Script/Engine.Engine]:GameViewportClientClassName=/Script/DisplayCluster.DisplayClusterViewportClient,[/Script/Engine.UserInterfaceSettings]:bAllowHighDPIInGameMode=True"
            "-unattended"
            "-handleensurepercent=0"
            "-UDPMESSAGING_TRANSPORT_MULTICAST=`"ip_1`""
            "-UDPMESSAGING_TRANSPORT_UNICAST=`"ip_2`""
            "-UDPMESSAGING_TRANSPORT_STATIC=`"ip_3`""
            "-ExecCmds=`"DisableAllScreenMessages`""
            "-forceres"
            "WinX=0"
            "WinY=0"
            "ResX=3200"
            "ResY=4800"
        )

        $SimStadProcess = Start-Process -FilePath "$SimStadPath\DigitalTwin.exe" -ArgumentList $argsList -PassThru -Wait

        Write-Host "Started SimStad Main Loop, waiting for exit command [ESC]..."
        while (-not $SimStadProcess.HasExited) {
            try {
                if ([console]::KeyAvailable) {
                    $key = [console]::ReadKey($true)
                    if ($key.Key -eq [ConsoleKey]::Escape) {
                        Write-Host "_Got Exit command [ESC], shutting down in 3 seconds..."
                        Start-Sleep -s 3
                        Shutdown-SimStad -SimStadProcess $SimStadProcess
                    }
                }
            }
            catch [System.InvalidOperationException] {
                # No key console available (like in CI), so just ignore
            }
            Start-Sleep -Milliseconds 200
        }

        if (-not $AutoRestart) {
            Write-Host "SimStad exited - shutting down everything"
            Shutdown-SimStad -SimStadProcess $SimStadProcess
        } else {
            Write-Host "SimStad exited - restarting in 5 seconds..."
            Start-Sleep -Seconds 5
        }
    }
}

cd "C:\Users\auto_lvl3-simstad-01\Desktop\TrafikLabWebNDIServer"
Start-Process "trafiklabbServer.exe"
Start-Sleep -Seconds 5
Start-Process "stream_webpage_to_ndi.exe"
Start-Sleep -Seconds 5

$SimStadPath = "C:\Users\auto_lvl3-simstad-01\Desktop\DigitalTwinVersions\DigitalTwin_2025-12-16"
Run-SimStad -SimStadPath $SimStadPath -AutoRestart $AutoRestart
