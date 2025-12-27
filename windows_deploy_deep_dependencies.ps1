param(
    [Parameter(Mandatory, Position = 0)]
    [String]$directory
)

# checked dll files, for which all required dll-s are imported
$checked = @()

function AssertInstalled-Tool {
    param([String]$toolName)
    return (Get-Command $toolName -ErrorAction SilentlyContinue) -ne $null
}

function CheckShouldImport-Item {
    param(
        [Parameter(Mandatory, Position = 0)]
        [String]$filePath
    )
    if (!(Test-Path -PathType Leaf $filePath)) {
        throw "Invalid file"
    }
    $filePath = Resolve-Path $filePath

    $parentDir = Split-Path $filePath -Parent

    return (!($parentDir -ieq (Resolve-Path $directory) -or $parentDir -ieq "C:\Windows\System32"))
}

# returns list of imported dlls in this iteration.
function ImportDependencies-Item {
    param(
        [Parameter(Mandatory, Position = 0)]
        [String]$filePath
    )
    $imported = @()

    if (!(Test-Path -PathType Leaf $filePath)) {
        throw "Invalid file"
    }

    $list = (ntldd.exe $filePath)
    foreach ($entry in $list) {
        $dllPath = $entry.Split(" ")[2]
        if ($dllPath -eq $null) {
            throw "Invalid ntldd output!"
        }
        if ((CheckShouldImport-Item $dllPath) -and
            (! $(Test-Path -PathType Leaf $(Join-Path $directory $(Split-Path -Leaf $dllPath))))
            ) {
            Write-Host "Copying $dllPath to project binary directory"
            Copy-Item $dllPath $directory
            $imported += $dllPath
        }
    }

    return $imported
}

if (!(AssertInstalled-Tool "ntldd.exe")) {
    throw "ntldd.exe must be installed"
}

if (!(Test-Path -PathType Container $directory)) {
    throw "Invalid path"
}

$directory = Resolve-Path $directory

$toCheck = $(Get-ChildItem -Path $directory -Filter "*.dll" -Recurse | select -ExpandProperty FullName)

while ($toCheck.Count -gt 0) {
    $newToCheck = @()
    foreach ($dllFile in $toCheck) {
        $importedFiles = ImportDependencies-Item $dllFile
        if (!($checked -Contains $dllFile)) {
            $checked += $dllFile
        }
        foreach ($importedFile in $importedFiles) {
            if (!($checked -Contains $importedFile)) {
                $newToCheck += $importedFile
            }
        }
    }

    # Write-Host "old $toCheck"
    # Write-Host "new $newToCheck"
    # Write-Host "checked $checked"
    $toCheck = $newToCheck
}
