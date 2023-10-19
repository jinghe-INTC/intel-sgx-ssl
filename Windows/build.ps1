Param(
	[Parameter(mandatory=$true)][string] $OSSL_version,
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
    Write-Error "cannot find nasm or perl, exit"
    Exit 1
}

try {
    if (-not (Test-Path "../openssl_source/$OSSL_version.tar.gz" -PathType Leaf))
    {
        Write-Error "OpenSSL source code package not available"
        throw 
    }
    Write-Output "Building SGXSSL with: $OSSL_version"
    ForEach ($Config in ("debug", "release", "release_cve-2020-0551-load", "release_cve-2020-0551-cf")) {
        Write-Output "  Building libraries in x64, $Config..."
        $SKIP_test = ""
        if ( $PSW_available -ne 1)
        {
            $SKIP_test = "SIM"
        }
        Start-Process .\build_package.cmd -ArgumentList "x64_$Config $OSSL_version $SKIP_test no-clean" -Wait
        if ($LASTEXITCODE -ne 0) {
            throw "  Failed building in config $Config, exiting..."
            Exit 1
        } else {
            Write-Output "  Successfully built config $Config"
        }
    }

    $currentTime = Get-Date -format "dd-MMM-yyyy HH:mm:ss"
    Write-Output "Build completed: Zipping package $currentTime"
    $SGXSSL_version_numbers = ($OSSL_version -split '-')[1]
    if ( $PSW_available -eq 1) 
    {
        $SGXSSL_version_numbers = (Get-ItemProperty -Path "HKLM:\SOFTWARE\Intel\SGX_PSW" -Name "Version")."Version" + "_" + $SGXSSL_version_numbers
    }
    Set-Location package
    Compress-Archive -Path docs, include, lib  -DestinationPath ..\sgxssl.$SGXSSL_version_numbers.zip -Update
    Set-Location ..
} catch {
    Write-Output $_.ToString()
    Write-Output $_.ScriptStackTrace
    Exit 1
}
Exit 0