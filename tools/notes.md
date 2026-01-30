# setup.iss
## Test Signing
Comment out #define TEST_SIGNED_DRIVERS

## Code Signing
```
[Setup]
...
SignTool=sign_util -c C:\config.json -t pecoff -o $f $f
SignedUninstaller=yes
```

## Files
```
; Source: {#SolutionDir + "signed\hlk\*"}; DestDir: "{tmp}"; Components: main
; Source: {#SolutionDir + "signed\attestation\*"}; DestDir: "{tmp}"; Components: main
; Source: {#BuildDir + "package\*"}; DestDir: "{tmp}"; Components: main

```

Add Flags: sign or Flags: signonce

## Project Config - innosetup

/Ssign_util="C:\Signing\ossign.exe $p"


# Attestation signing
## usbip2_filter identifier
Change from c1e0d1fe-d564-596e-4526-71da1249702a to c1e0d1fe-d564-596e-4526-71da12497000