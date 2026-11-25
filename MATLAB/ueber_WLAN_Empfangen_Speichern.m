%% WLAN-Rohdaten vom ESP empfangen und als .log speichern
% Vorher Laptop mit WLAN "ESP32S3_BNO" verbinden. Beenden: Fenster schliessen
clear; clc;
s   = tcpclient("192.168.4.1", 5000);
fid = fopen("WLAN_" + string(datetime('now', 'Format', 'yyyy-MM-dd_HH-mm-ss')) + ".log", 'w');
f   = figure('Name', 'Aufnahme laeuft - Fenster schliessen zum Beenden');
while ishandle(f)
    if s.NumBytesAvailable > 0
        fwrite(fid, read(s, s.NumBytesAvailable, "uint8"));
    end
    drawnow limitrate;
end
fclose(fid); clear s