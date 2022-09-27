Param (
    [UInt32]$size
)

$i = 1

while ($i -le $size) {
    Write-Host "Compute to size k =" $i
    ./hilbert_gen.exe $i
    ./float_solver.exe
    ./scalar_solver.exe
    ./double_solver.exe
    
    $destPath = "./outForK_" + $i

    New-Item -Path $destPath -ItemType Directory -Force > $null
    Move-Item -Path "./*output.txt" -Destination $destPath -Force
    Remove-Item -Path "./*.txt" -Force
    $i += 1
}