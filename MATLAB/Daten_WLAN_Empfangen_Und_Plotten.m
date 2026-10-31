%% WLAN-Rohdaten vom ESP empfangen und als .log speichern
% Vorher Laptop mit WLAN "ESP32S3_BNO" verbinden. Beenden: Fenster schliessen
clear; clc;

%TCP-Verbindung zum ESP aufbauen
% 5000: Port auf dem ESP
s   = tcpclient("192.168.4.1", 5000); 

% Datei zum schreiben der Daten öffnen
fid = fopen("WLAN_" + string(datetime('now', 'Format', 'yyyy-MM-dd_HH-mm-ss')) + ".log", 'w');

%Fenster zum Anzeigen dass Messung läuft und zum Beenden der Messung
f   = figure('Name', 'WLAN-Aufnahme laeuft - Fenster schliessen zum Beenden');

% Schleife, die so lange läuft wie das Fenster f existiert
while ishandle(f)

    % Prüft, ob seit dem letzten Durchlauf neue Bytes vom ESP angekommen
    if s.NumBytesAvailable > 0

        % read holt alle vorhandenen Bytes aus dem Empfangspuffer, fwrite
        % schreibt Daten unverändert in die Datei
        fwrite(fid, read(s, s.NumBytesAvailable, "uint8"));
    end

    %MATLAB kurz Zeit geben um Ereignisse zu verarbeiten -> z.B Schließen
    %des Fensters
    drawnow limitrate;
end

% Datei schließen, clear trennt Verbindung zum ESP
fclose(fid); clear s

