*** Win98, Pentium III Only ***

Debugging DirectX applications on Windows 98 with a Intel Pentium III processor:

The file contained in this directory (VMCPD.VXD) is necessary when debugging 
applications on Win98 Pentium III machines with DX6.1 and DX7.  Exceptions are 
unmasked on the Pentium III when debugging.  On Win98, this can produce 
"Unhandled exceptions" or "Illegal Instructions".  To correct this problem, 
you will need to copy vmcpd.vxd to your \windows\system\vmm32 directory. 
