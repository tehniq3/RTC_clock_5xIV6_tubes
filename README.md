# RTC_clock_5xIV6_tubes
based on 
- http://rcl-radio.ru/?p=67405
- http://forum.rcl-radio.ru/viewtopic.php?id=87&amp;p=5
- https://oshwlab.com/allexsemenow/iv-6-clock_copy

My article: https://nicuflorica.blogspot.com/2026/03/ceas-rtc-cu-5-tuburi-vfd-de-tip-iv-6.html

![generall](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEi62x9-MGa-x30f8BnnxMatmyy1NLhIFwfAfkGuMqXd6Bt7SkWj2ki2AbpYmP4R1NacOzAMFa-cX733rKVw4AfEHITK93iQiLxipWlMTp49RrPTby2Evy0QhGC1U59je2RbqYIJ4VXXTemk5sPNuWZeVkTBQFLOH8jtPXHj9fE6IGEKk81ow3orsaEGdz59/w200-h93/ceasRTCcu5tuburiVFDtipIV6_0.jpeg)
![real](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEhbzkVAgmDZWv09GJEbYKI2XDzDVAM8Vp7NRL0KNYiNMFpjTTUoKNS-906Nv5EZM7r_wgGS2sq0lHE0MmVqoT347byK3hBXOxn5zFnIjFB8W56EvuW45B5Px6RGNDxc6-ittNq1gz2GeTJ1bXCsIry1SoUpxnE_Vl9UMawjkceh2x7eotykaZzAmPRlxfJN/w200-h93/ceasRTCcu5tuburiVFDtipIV6_27.jpeg)

![schema](https://blogger.googleusercontent.com/img/b/R29vZ2xl/AVvXsEgfJJfLFE2jovYve-YcGFPc4-jAYeuJfFI0WPrd9OsMeMtW7bfolSU-pRxMCVF-DLPmdD2-SIyhThdIwynVOJuS2M3x1KES_vfwvM11jh3Oxd7NVBhjricaHnyURK_soRdkCaPuQT5bcAlCqJ82TfJbE8XnCnNVjRdX7c-eAjaonJJw5k5_RTB8UCcRSpVw/w200-h141/Schematic_Ceas-cu-tuburi-Nixie_niqro_2026-02-03.jpg)


History (versions of software):
- v.0 - changed for DS3231 by niq_ro (Nicu FLORICA)
- v.0a - v.0a - translate the comments in english and move the commands as one per line
- v.1 - replaced iarduino_RTC.h with RTClib.h library  // https://github.com/adafruit/rtclib
- v.2 - added temperature on the display (internal sensor from DS3231 chip)
- v.2a - added a correction for temperature (decreased with 3 degree in my case)
- v.2b - show the temprature whe push the SW2 button
