From http://www.uvnc.com/downloads/ultravnc/95-downlaod-10961.html
Ultravnc source

Sourcecode UltraVnc 10961.
VS2010 & VS2008 project files, require nasm and directx sdk
GNU/GPL 2011-06-19 Windows 0 B 8257

Changes

server
*crash chat x64
*AuthRequired=0, passwd=NULL: Warning message block service
*About x64 say win32
*viewer 1082 and server 1096 with localcursor ( no connect, 100% cpu)
*serversite scaling and multiple viewers, framebuffer size get overwritten
(Scale is now lock when multiple viewers are connected to avoid a
framebuffer change, first connected viewer set scale. Site effect is that
the viewer report the unused, incorrect scale, but at least it doesn't crash anymore)
*-connect ip, passed to winvnc running as service is not remembered for
autoreconnect.
*-stopreconnect
stop the autoreconnect function of the server.
*server mouse moves jump on viewer when screen is idle.
*old plugin zrle crash


viewer
*plugin (SecureVNCPlugin) used by viewer
server without plugin
give incorrect viewer message. And doesn't ask to reject the connection.
*monitor value is saved, but vncviewer read it as bool (true/false)
Only 0/1 are correct imported
*old plugins fail when zrle encoding is used
*old plugins give incorrect info in statusbox
*-autoreconnect timeout, -reconnectcounter number
(available from gui and commandline)
-autoreconnect timeout was incorrect, updated
*old plugin detection
*Messagebox was sometimes displayed on invisable desktop
*auth dll error messages for missing dll's incorrect
*mslogon and no groups, didn't checked admin account for access
*lock /logout screen on exit viewer option blocked shutdown server.

javaviewer
*mslogon fixed

added special build: only one port for javaviewer (rfb port is used for java download and rfb data)
