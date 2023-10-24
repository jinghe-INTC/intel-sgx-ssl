Param(
	[Parameter(mandatory=$false)][string]$OPENSSL_VERSION = "openssl-3.0.0",
    [Parameter(mandatory=$false)][bool] $PSW_available=1
)
function ExecuteCommand() {
    param
    (
        [Parameter(Mandatory=$true)][string] $Command,
        [Parameter(Mandatory=$true)][string] $Cmdarg
    )
    try {
        & $command $cmdarg
        if ($LASTEXITCODE -ne 0) {
            throw 'Execution failed'
            Exit 1
        }
    } catch {
        Write-Error "cannot find $command"
        Exit 1
    }
}

try {
    #Write-out "Validating Prerequisites (perl, nasm)"
    ExecuteCommand "nasm" "-v"
    ExecuteCommand "perl" "-v"
} catch {
    Write-Error "cannot find nasm or perl, exiting"
    Exit 1
}

try {
    $SGXSSL_ROOT = Get-Location
    if (-not (Test-Path "../openssl_source/$OpenSSL_version.tar.gz" -PathType Leaf))
    {
        Write-Output "$OpenSSL_version source code package not available, exiting"
        Exit 1 
    }
    Write-Output "Building SGXSSL with: $OpenSSL_version"
    ForEach ($Config in ("debug", "release", "cve-2020-0551-load-release", "cve-2020-0551-cf-release")) {
        Write-Output "  Building libraries in x64, $Config..."
        $BUILD_LEVEL = "ALL"
        if ( $PSW_available -ne 1)
        {
            $BUILD_LEVEL = "SKIP_TEST"
        }
        $Build_proc = Start-Process powershell -ArgumentList ".\build_pkg.ps1 -my_Configuration $Config -OPENSSL_version $OpenSSL_version -BUILD_LEVEL $BUILD_LEVEL -Clean 0" -PassThru
        $Build_proc.WaitForExit()
        if ($Build_proc.HasExited) {
            # Write-Host "The build process has exited."
        }
        if ($Build_proc.ExitCode  -ne 0) {
            Write-Output "  Failed building config $Config, exiting..."
            Exit 1
        } else {
            Write-Output "  Successfully built config $Config"
        }
    }

    $currentTime = Get-Date -format "dd-MMM-yyyy HH:mm:ss"
    Write-Output "Build completed: Zipping package $currentTime"
    $SGXSSL_version_numbers = ($OpenSSL_version -split '-')[1]
    if ( $PSW_available -eq 1) 
    {
        $SGXSSL_version_numbers = (Get-ItemProperty -Path "HKLM:\SOFTWARE\Intel\SGX_PSW" -Name "Version")."Version" + "_" + $SGXSSL_version_numbers
    }
    Set-Location package
    Compress-Archive -Path docs, include, lib  -DestinationPath ..\sgxssl.$SGXSSL_version_numbers.zip -Update
    
} catch {
    Write-Output $_.ToString()
    Write-Output $_.ScriptStackTrace
    Exit 1
} finally  {
    set-location $SGXSSL_ROOT
}
Exit 0