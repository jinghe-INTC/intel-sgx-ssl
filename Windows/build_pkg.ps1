Param(
	[string]$Platform = "x64",
    [string]$VS_CMD_PLFM = "amd64",
    [string]$OPENSSL_CFG_PLFM ="sgx-VC-WIN64A",
    [Parameter(mandatory=$true)][string]$my_Configuration,
    [Parameter(mandatory=$true)][string]$OPENSSL_VERSION,
    [Parameter(mandatory=$false)][bool] $Clean = 0,
    [Parameter(mandatory=$false)][string] $TEST_MODE ="",
    [Parameter(mandatory=$false)][bool] $Pause_at_end = 0    
)

function Invoke-Environment {
    param
    (
        # Any cmd shell command, normally a configuration batch file.
        [Parameter(Mandatory=$true)][string] $Command
    )
    $Command = "`"" + $Command + "`""
    cmd /c "$Command > nul 2>&1 && set" | . { process {
        if ($_ -match '^([^=]+)=(.*)') {
            [System.Environment]::SetEnvironmentVariable($matches[1], $matches[2])
        }
    }}
}

try {
    $SGXSSL_ROOT = Get-Location
    perl svn_revision.pl > sgx\libsgx_tsgxssl\tsgxssl_version.h
    Set-Location ..\openssl_source
    if (Test-Path $OPENSSL_VERSION -PathType Container)
    {
        Remove-Item $OPENSSL_VERSION -Recurse -Force
    }
    tar -xf "$OPENSSL_VERSION.tar.gz"
    (get-content $OPENSSL_VERSION\Configure) -replace ('BSAES_ASM','') | out-file $OPENSSL_VERSION\Configure
    Copy-Item rand_lib.c $OPENSSL_VERSION\crypto\rand\
    Copy-Item sgx_config.conf $OPENSSL_VERSION\
    Copy-Item x86_64-xlate.pl $OPENSSL_VERSION\crypto\perlasm
    Copy-Item threads_win.c $OPENSSL_VERSION\crypto\

    Set-Location $OPENSSL_VERSION
    Copy-Item  $Env:SGXSDKInstallPath\scripts\sgx-asm-pp.py .
    
    if ($my_Configuration -eq "debug")
    {
        $OPENSSL_CFG_PLFM += " --debug"
    }

    $CVE_2020_0551_MITIGATIONS= ""
    $ADDITIONAL_CONF = ""
    Start-Process perl -ArgumentList "Configure --config=sgx_config.conf $OPENSSL_CFG_PLFM  $CVE_2020_0551_MITIGATIONS $ADDITIONAL_CONF no-dtls no-idea no-mdc2 no-rc5 no-rc4 no-bf no-ec2m no-camellia no-cast no-srp no-padlockeng no-dso no-shared no-ui-console no-ssl3 no-md2 no-md4 no-stdio -FI$SGXSSL_ROOT\..\openssl_source\bypass_to_sgxssl.h -D_NO_CRT_STDIO_INLINE -DOPENSSL_NO_SOCK -DOPENSSL_NO_DGRAM -DOPENSSL_NO_ASYNC " -wait
    (Get-Content crypto\engine\tb_rand.c) |  Foreach-Object {$_ -replace 'ENGINE_set_default_RAND', 'dummy_ENGINE_set_default_RAND'} | Out-File crypto\engine\tb_rand.c
    Invoke-Environment "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"
    nmake build_generated libcrypto.lib

    New-Item -Path $SGXSSL_ROOT\package\lib\$Platform\$my_Configuration -ItemType Directory -Force
    Copy-Item  libcrypto.lib $SGXSSL_ROOT\package\lib\$Platform\$my_Configuration\libsgx_tsgxssl_crypto.lib -Force
    Copy-Item  ossl_static.pdb $SGXSSL_ROOT\package\lib\$Platform\$my_Configuration\ -Force
    Copy-Item  include\openssl\* $SGXSSL_ROOT\package\include\openssl\ -Recurse -Force
    Copy-Item  include\* $SGXSSL_ROOT\sgx\test_app\enclave\ -Recurse -Force

    set-location $SGXSSL_ROOT\sgx
    MSBUILD SGXOpenSSLLibrary.sln /p:Configuration=$my_Configuration /p:Platform=$Platform /t:Rebuild
    Copy-Item $Platform\$my_Configuration\libsgx_tsgxssl.lib $SGXSSL_ROOT\package\lib\$Platform\$my_Configuration\ -Force
    Copy-Item $Platform\$my_Configuration\libsgx_usgxssl.lib $SGXSSL_ROOT\package\lib\$Platform\$my_Configuration\ -Force
    if ($my_Configuration -eq "debug")
    {
        Copy-Item  libsgx_tsgxssl\$Platform\$my_Configuration\libsgx_tsgxssl.pdb $SGXSSL_ROOT\package\lib\$Platform\$my_Configuration\ -Force
        Copy-Item  libsgx_usgxssl\$Platform\$my_Configuration\libsgx_usgxssl.pdb $SGXSSL_ROOT\package\lib\$Platform\$my_Configuration\ -Force
    }
} catch {
    Write-Error "Failed!"
    Exit 1
} finally  {
    set-location $SGXSSL_ROOT
}
Exit 0