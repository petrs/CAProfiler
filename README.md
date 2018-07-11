CAProfiler - a tool for analysis of installed smartcard applets (and some basic SIM/EMV functionality)
=======
[![Latest release](https://img.shields.io/github/release/petrs/CAProfiler.svg)](https://github.com/petrs/CAProfiler/releases/latest)

[![Build status](https://ci.appveyor.com/api/projects/status/4a953152ftvd044a?svg=true)](https://ci.appveyor.com/project/petrs/CAProfiler)

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