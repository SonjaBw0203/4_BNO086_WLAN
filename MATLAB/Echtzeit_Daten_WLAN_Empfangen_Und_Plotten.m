 % fclose('all'); clear s


%% WLAN-Rohdaten vom ESP empfangen, Verarbeiten, Plotten und ROhdaten als .log speichern
% Vorher Laptop mit WLAN "ESP32S3_BNO" verbinden. Zum Beenden: Fenster schliessen
clear; clc;

%% "Hartes" ZUPT: Geschwindigkeit bei erkanntem Stillstand auf 0 zurücksetzen
% Stillstand erkennen: 
% Annahme: bei Stillstand schwanken die Beschleunigungs- und Drehrate-Werte nicht so stark 
% -> Standardabweichung über ein kurzes Zeitfenster berechnen 
% gewähltes Zeitfenster: 0,2s (!!! Ausprobieren !!!)
N = 20; % für Berechnung der Standardabweichung 20 Werte merken (weil 20 Werte bei 100 Hz = 0,2 s)
                    clc
% Vergleich Standardabweichung mit einer Schwelle
% gewählte Schwellen: Beschl. 0,05 m/s2, Drehrate 0,01 rad/s (!!! Ausprobieren !!!)
sa = 0.05;
sw = 0.01;
%%

%TCP-Verbindung zum ESP aufbauen
% 5000: Port auf dem ESP
s   = tcpclient("192.168.4.1", 5000); 

% Datei zum schreiben der Daten öffnen
fid = fopen("WLAN_" + string(datetime('now', 'Format', 'yyyy-MM-dd_HH-mm-ss')) + ".log", 'w');

%Fenster zum Anzeigen dass Messung läuft und zum Beenden der Messung
f   = figure('Name', 'WLAN-Aufnahme laeuft - Fenster schliessen zum Beenden');


% zwei Linien im Fenster f, blau = BNO01, rot = BNO2, werden Punkt für
% Punkt verlaengert
h =[animatedline('Color','b'), animatedline('Color','r')];

% Gitter, gleiche Skalierung von x und y, Achsenbeschriftung, Legende
grid on; axis equal, xlabel('x in m'); ylabel('y in m'); legend('BNO1','BNO2');

% zweites Fenster für Trajektorie mit ZUPT
f2 = figure('Name','Trajektorie mit "Echtzeit"-ZUPT');
hz = [animatedline('Color','b'), animatedline('Color','r')];
grid on; axis equal, xlabel('x in m'); ylabel('y in m'); legend('BNO1', 'BNO2'); title('mit "Echtzeit"-ZUPT');

% Spalte des Zeitstempels tA je Sensor in einer Zeile (BNO1: Spalte 1,
% BNO2: Spalte 14), direkt danach folgen ax, ay und az
sp = [1 14];

% letzter Zeitstempel je Sensor, NaN = noch kein Wert empfangen
tL = nan(1,2);

% letzte Beschleunigung, Geschwindigkeit, Position je Sensor (Zeile 1 = BNO1, Zeile 2 = BNO2; Spalte 1 = x, Spalte 2 = y), Start bei 0
aL = zeros(2);
v = zeros(2);
p = zeros(2);

%% für ZUPT 
% Geschwindigkeit und Position
vz = zeros(2);
pz = zeros(2);

% Speicher für die letzten N Beträge von a und Drehrate je Sensor, Am Anfang Werte = 0
bufA = zeros(2,N);
bufW = zeros(2,N);
%%


% Zwischenspeicher für eine noch unvollständig angekommende Zeile
puffer = "";


% Schleife, die so lange läuft wie das Fenster f existiert
while ishandle(f) && ishandle(f2)
    
    % Prüft, ob seit dem letzten Durchlauf neue Bytes vom ESP angekommen
    if s.NumBytesAvailable > 0
        
        % alle vorhandenen Bytes aus dem Empfangspuffer holen und in b
        % merken (werden zweimal gebraucht)
        b = read(s, s.NumBytesAvailable, "uint8");

        % schreibt Daten unverändert in die Datei
        fwrite(fid,b);
        
        % Bytes in Text unwandeln und an den Rest vom letzten Durchlauf
        % anhängen
        puffer = puffer + char(b(:)');

        % Text an den Zeilenumbrüchen in einzelne Zeilen aufteilen
        z = split(puffer, newline);

        % letzte Zeile kann unvollständig sein -> für den nächsten
        % Durchlauf aufheben
        puffer = z(end);

        % alle vollständigen Zeilen nacheinander bearbeiten
        for zeile = z(1:end-1)'
            
            %Trennstriche durch Leerzeichen ersetzen und alle Zahlen der
            %Zeile einlesen
            w = sscanf(replace(zeile, "|", " "), '%f');

            % fehlerhafte Zeile (nicht genau 26 Werte) ueberspringen
            if numel(w) ~= 26, continue; end

            % Sensor 1 und Sensor 2 nacheinander bearbeiten
            for k = 1:2

                %Zeitstempel des Sensors von Mikrosekunden in Sekunden
                %umrechnen (damit es zur Einheit der Beschleunigung passt)
                t = w(sp(k))/1e6;

                % Beschleunigung [ax, ay, az] als Spaltenvektor holen
                aSensor = w(sp(k) + (1:3));

                % Drehrate [gx, gy, gz] als Spaltenvektor holen 
                gSensor = w(sp(k) + (5:7));

                % Quaternion des Sensor holen (Realteil zuerst)
                qw = w(sp(k)+9); qx = w(sp(k)+10); qy = w(sp(k)+11); qz = w(sp(k)+12);

                % dreht einen Vektor vom Sensor-System ins Welt-System
                R = [1-2*(qy^2+qz^2),  2*(qx*qy-qw*qz),  2*(qx*qz+qw*qy);
                     2*(qx*qy+qw*qz),  1-2*(qx^2+qz^2),  2*(qy*qz-qw*qx);
                     2*(qx*qz-qw*qy),  2*(qy*qz+qw*qx),  1-2*(qx^2+qy^2)];

                % Beschleunigung ins Welt-System drehen
                % Welt-z zeigt nach oben, Welt -x/y liegen horizontal
                aWelt = R * aSensor;

                % nur die horizontalen Anteile [ax ay] als Zeilenvektor
                % weiterverwenden 
                % z Komponente wurde nur für die Drehung ins Weltsystem
                % gebraucht, wird für 2D Darstellung verworfen
                a =aWelt(1:2)';
                
                % Zeit seit dem letzten Messwert dieses Sensors
                dt = t - tL(k);

                % nur integrierem, wenn es ein neuer Messwert ist (dt = 0:
                % Wert wiederholt, dt = NANA: erster Wert)
                if dt>0

                    % Trapezregel a -> v: 
                    % neue Geschwindigkeit = alte + dt* Mittelwert
                    % aus alter und neuer Beschleunigung
                    vNeu = v(k,:) + dt * (aL(k,:)+a)/2;

                    % Trapezregel v -> x: 
                    % neue Position = alte + dt *Mittelwert
                    % aus alter und neuer Geschwindigkeit
                    p(k,:) = p(k,:) + dt * (v(k,:) + vNeu)/2;

                    % neue Geschwindigkeit für den nächsten Schritt merken
                    v(k,:) = vNeu;

                    % neuen Punkt an die Trajektorie dieses Sensors
                    % anhängen
                    addpoints(h(k), p(k,1), p(k,2));

                    %% für ZUPT 
                    % ältesten Wert aus Puffer entfernen und aktuellen Betrag anhängen
                    bufA(k,:) = [bufA(k,2:end), norm(aSensor)];
                    bufW(k,:) = [bufW(k,2:end), norm(gSensor)];

                    % Stillstand erkannt, wenn beide Werte im Fenster kaum schwanken
                    if std(bufA(k,:)) < sa && std(bufW(k,:)) < sw     
                        % dann v = 0 setzen
                        vz(k,:) = [0 0];
                    else
                        % Bewegung -> Trapezregel zur Positionsbestimmung
                        vzNeu   = vz(k,:) + dt * (aL(k,:) + a)/2;
                        pz(k,:) = pz(k,:) + dt * (vz(k,:) + vzNeu)/2;
                        vz(k,:) = vzNeu;
                    end

                    % Punkt an Trajektorie anhängen
                    addpoints(hz(k), pz(k,1), pz(k,2));

                   %% ZUPT mit Positionsfehlerabzug für den Zeitraum N/100
                   % Beim Abbremsen misst der Sensor die
                   % Bremsbeschleunigung etwas zu groß (Offset kleiner 0)
                   % Dadurch fällt die berechnete Geschwindigkeit nicht nur
                   % auf null, sondern darüber hinaus und zeigt nach
                   % hinten.
                   % solange bis ZUPT den Stillstand erkennt und die
                   % Geschwindigkeit auf Null setzt, läuft die Position in
                   % die entgegengesetzte Richtung

                    %% ZUPT mit nachträglicher Korrektur des Postionsfehlers
                    % noch zu implementieren

                end

                % bei neuem oder erstem Wert Zeit und Beschleunigung für
                % den nächsten Schritt merken
                if isnan(dt) || dt > 0, tL(k) = t; aL(k,:) = a; end
            end

        end

    end

    %MATLAB kurz Zeit geben um Ereignisse zu verarbeiten -> z.B Schließen
    %des Fensters
    drawnow limitrate;
end

% Datei schließen, clear trennt Verbindung zum ESP
fclose(fid); clear s

