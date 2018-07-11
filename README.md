CAProfiler - C++ tool to scan installed smartcard applets 
=======
[![Latest release](https://img.shields.io/github/release/petrs/CAProfiler.svg)](https://github.com/petrs/CAProfiler/releases/latest)
[![Build status](https://ci.appveyor.com/api/projects/status/4a953152ftvd044a?svg=true)](https://ci.appveyor.com/project/petrs/CAProfiler)

The applications installed on smartcard are identified by its unique Application ID (AID). The list of installed applications can be directly retrieved via [GlobalPlatform API](https://globalplatform.org/) using a suitable tool, e.g., amazing [GlobalPlatformPro tool](https://github.com/martinpaljak/GlobalPlatformPro) by Martin Paljak. However, the command requires successful authentication against so-called CardManager (CM) application using cryptographic keys. While the required keys are set to well-known values for developer cards, production cards are usually locked with keys known only to given card issuer. As a result, a list of installed applications cannot be readily retrieved.

The CAProfiler tool iterates over a large number of known AIDs from standards (e.g., EMV or OpenPGP application), try to SELECT the application for use and detects response status. The successful SELECT command signalize installed application.

## Important notes
The codebase for this project was developed already around 2007, is written in C++ and uses Microsoft proprietary extensions (MFC). I do plan to rewrite utility to Java (for better portability) and a separate list of known AIDs together with human-readable names into the separate configuration file (for easier addition of new AIDs).

##  Usage
If run without command line parameters, interactive menu selection is displayed.
```
CAProfiler.exe 
```

Example output:
```bash
Available readers:
1. Generic EMV Smartcard Reader 0

...

Available operations:
1. Test installed applets (/oper:"1")
2. Get EMV ACC challenge (/oper:"2")
3. Get SIM challenge (/oper:"3")

Select index of target operation: 1
```


## Scan for installed smarcard applications
```
CAProfiler.exe /OPER:"1"
```

Example output:
```bash
Available readers:
1. Generic EMV Smartcard Reader 0

Select index of target reader: 1

INFO: You may use /reader:"reader_name" to provide this value
OpenSession(Generic EMV Smartcard Reader 0) finish with 0 SUCCESS

Testing installed applets...

-> 00 a4 04 00 05  a0 00 00 00 03   00
   APPLET_AID_VISA_PREFIX applet found
<- 6f 10 84 08 a0 00 00 00 03 00 00 00 a5 04 9f 65 01 ff  90 00

...

-> 00 a4 04 00 08  a0 00 00 00 03 00 00 00   00
   APPLET_AID_JCOP_CM applet found
<- 6f 10 84 08 a0 00 00 00 03 00 00 00 a5 04 9f 65 01 ff  90 00

The following applets were detected:
APPLET_AID_JCOP_CM             : a0 00 00 00 03 00 00 00
APPLET_AID_VISA_PREFIX         : a0 00 00 00 03
```

## Obtain EMV ACC challenge

```
CAProfiler.exe /OPER:"2" /APPLET_AID:"a0000000043060" /PIN:"1234" /CHALLENGE:"11223344"
```

## Obtain GSM SIM session key
```
CAProfiler.exe /OPER:"3" /CHALLENGE:"0102030405060708090a0b0c0d0e0f10" /PIN:"1234"
```

