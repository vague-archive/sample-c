param (
    [string]$SourceFile = "src/*.c",
    [string]$OutputFile = "modules/sample-c.dll"
)

# Function to check if cl.exe is available
function Find-MSVC {
    return $null -ne (Get-Command cl -ErrorAction SilentlyContinue)
}

# Function to setup MSVC environment and compile
function Compile {
    Write-Host "Checking for MSVC installation..."
    
    # Locate vswhere.exe (Visual Studio installation locator)
    $vswherePath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (!(Test-Path $vswherePath)) {
        Write-Host "Error: vswhere.exe not found. Please install Visual Studio Build Tools."
        exit 1
    }

    # Get the latest installed Visual Studio path
    $vsPath = & $vswherePath -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (!$vsPath) {
        Write-Host "Error: No MSVC installation found."
        exit 1
    }

    $vcvarsall = "$vsPath\VC\Auxiliary\Build\vcvars64.bat"
    if (!(Test-Path $vcvarsall)) {
        Write-Host "Error: Could not find vcvars64.bat."
        exit 1
    }

    Write-Host "Setting up MSVC environment..."
    cmd.exe /c "`"$vcvarsall`" && cl /I src /LD $SourceFile /Fe$OutputFile"

    if ($?) {
        Write-Host "Compilation successful: $OutputFile"
    } else {
        Write-Host "Compilation failed."
    }
}

# Check if MSVC is already in the environment
if (Find-MSVC) {
    Write-Host "MSVC found in system PATH."
} else {
    Write-Host "MSVC not in PATH. Attempting to locate it..."
}

Compile
