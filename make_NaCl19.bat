@C:\nacl_sdk\pepper_19\tools\make.exe %*


C:\nacl_sdk\pepper_19\toolchain\win_x86_newlib\bin\x86_64-nacl-strip morirt_x86_32.nexe -o morirt_x86_32.nexe
C:\nacl_sdk\pepper_19\toolchain\win_x86_newlib\bin\x86_64-nacl-strip morirt_x86_64.nexe -o morirt_x86_64.nexe

mkdir C:\nacl_sdk\pepper_19\examples\morirt
copy PlatformNACL\MoriRT.html C:\nacl_sdk\pepper_19\examples\morirt
copy PlatformNACL\morirt.nmf C:\nacl_sdk\pepper_19\examples\morirt

copy morirt_x86_32.nexe C:\nacl_sdk\pepper_19\examples\morirt

copy morirt_x86_64.nexe C:\nacl_sdk\pepper_19\examples\morirt

pause