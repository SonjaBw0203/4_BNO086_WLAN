%% Auswertung Logfiles Ruhemessungen - zwei Sensoren
% Format der Logdatei (26 Werte pro Zeile):
% tA1 ax1 ay1 az1 | tG1 gx1 gy1 gz1 | tQ1 qr1 qi1 qj1 qk1 | tA2 ax2 ay2 az2 | tG2 gx2 gy2 gz2 | tQ2 qr2 qi2 qj2 qk2
% lineare Beschleunigung in m/s^2 ohne g, Drehrate in rad/s, Zeitstempel in us
%
% Die Analyse wird fuer jeden Sensor einzeln durchgefuehrt (eigene Abbildungen).
% Jeder Sensor verwendet seine eigenen Zeitstempel (tA), beide bezogen auf denselben Nullpunkt.

% Es koennen mehrere Logdateien auf einmal ausgewaehlt werden (Strg + Klick).

clear; clc; close all;

%% Einstellungen
ruhedauer = 2.0;    % s, Ruhephase
zeitpunkte = [10 60 300 600 900 1800 3600 7200 14400]; % s, zu diesem Zeiten wird der Positionsfehler ausgegeben

%% Logdateien auswaehlen
[dateien, pfad] = uigetfile('*.log', 'Logdatei(en) auswaehlen', 'MultiSelect', 'on');
if isequal(dateien, 0)
    error('Keine Datei ausgewaehlt.');
end
dateien = cellstr(dateien);          % auch bei nur einer Datei eine Liste
nDat = numel(dateien);

namen = {'ohne Offsetkorrektur', 'Offset aus 2 s', 'Offset aus ganzer Messung'};

for d = 1:nDat
    %% Logdatei einlesen (NEU: 5 Trennstriche, 26 Werte)
    txt = strtrim(readlines(fullfile(pfad, dateien{d})));
    zeilen = txt(count(txt, "|") == 5);
    daten = nan(numel(zeilen),26);
    for i = 1:numel(zeilen)
        w = str2double(split(strtrim(replace(zeilen(i),"|"," "))));
        if numel(w) == 26
            daten(i,:) = w';
        end
    end
    daten = daten(all(~isnan(daten), 2), :);
    % Neustart: Zeitstempel springt zurueck, aber nicht vom Ende des Zaehlbereichs
    % (Ueberlauf von micros() nach 2^32 us = 71,6 min ist ein Sprung um fast -2^32)
    dt1 = diff(daten(:,1));
    sprung = find(dt1 < 0 & dt1 > -2^31, 1, 'last');
    if ~isempty(sprung)
        fprintf('%s: Neustart erkannt, %d Zeilen davor verworfen\n', dateien{d}, sprung);
        daten = daten(sprung+1:end, :);
    end
    % NEU: Zeilen am Anfang verwerfen, in denen ein Sensor noch keine Werte hatte (Zeitstempel 0)
    daten = daten(all(daten(:, [1 5 9 14 18 22]) > 0, 2), :);
        % Ueberlauf von micros() korrigieren: bei jedem Sprung um fast -2^32 us
    % werden 2^32 us zu allen folgenden Werten addiert (jede Zeitstempel-Spalte einzeln)
    for c = [1 5 9 14 18 22]
        ueberlauf = [0; diff(daten(:,c)) < -2^31];
        daten(:,c) = daten(:,c) + cumsum(ueberlauf) * 2^32;
    end
    if size(daten, 1) < 10
        warning('%s: keine gueltigen Daten - uebersprungen.', dateien{d});
        continue;
    end
    t0 = daten(1,1);                          % NEU: gemeinsamer Zeitnullpunkt fuer beide Sensoren

    %% NEU: gleiche Analyse fuer Sensor 1 und Sensor 2
    for s = 1:2
    o = 13*(s-1);                             % Spaltenversatz: Sensor 2 beginnt in Spalte 14
    sName = sprintf('Sensor %d', s);

    % NEU: eigene Zeitstempel des Sensors; doppelte Werte entfernen
    % (ein Messwert von Sensor 2 kann in zwei Zeilen stehen, wenn er langsamer kam)
    [~, iu] = unique(daten(:, o+1), 'stable');
    ds = daten(iu, :);

    t  = (ds(:,o+1) - t0) / 1e6;              % s
    ax = ds(:,o+2);  ay = ds(:,o+3);          % m/s^2
    w  = sqrt(sum(ds(:,o+(6:8)).^2, 2));      % Betrag Drehrate in rad/s
    n  = numel(t);
    rate = (n - 1) / (t(end) - t(1));

        qr = ds(:,o+10); qi = ds(:,o+11); qj = ds(:,o+12); qk = ds(:,o+13);
    roll = rad2deg(atan2(2*(qr.*qi + qj.*qk), 1 - 2*(qi.^2 + qj.^2)));
    nick = rad2deg(asin(2*(qr.*qj - qk.*qi)));
    gier = rad2deg(unwrap(atan2(2*(qr.*qk + qi.*qj), 1 - 2*(qj.^2 + qk.^2))));
    roll = roll - roll(1);  nick = nick - nick(1);  gier = gier - gier(1);
    aufl = rad2deg(2*1e-4);   % Aufloesung durch 4 Nachkommastellen im Quaternion: ca. 0,0115 Grad
    winkel = {roll, nick, gier};  wName = {'Roll', 'Nick', 'Gier'};

    fprintf('\n===== %s: %s =====\n', dateien{d}, sName);
    fprintf('%d Messwerte, Dauer %.1f s, Abtastrate %.1f Hz\n', n, t(end), rate);

    %% 3) Kennwerte des Rauschens
    fprintf('          Mittelwert      Streuung um Mittelwert    groeßte Abweichung vom Mittelwert\n');
    fprintf('ax     %10.4f           %9.4f                       %10.4f   m/s^2\n', mean(ax), std(ax), max(abs(ax-mean(ax))));
    fprintf('ay     %10.4f           %9.4f                       %10.4f   m/s^2\n', mean(ay), std(ay), max(abs(ay-mean(ay))));
    fprintf('|w|    %10.4f           %9.4f                       %10.4f   rad/s\n', mean(w),  std(w),  max(abs(w-mean(w))));

    ruhe0 = t < ruhedauer;
    fprintf('Offset aus 2 s:     ax %.4f, ay %.4f m/s^2\n', mean(ax(ruhe0)), mean(ay(ruhe0)));
    fprintf('Offset gesamt:      ax %.4f, ay %.4f m/s^2\n', mean(ax), mean(ay));

        %% Orientierung (Aenderung gegenueber Start)
    fprintf('\nOrientierung (Aenderung gegenueber Start, 1 Aufloesungsstufe = %.4f Grad):\n', aufl);
    fprintf('           Ende       Min       Max   Spannweite\n');
    for k = 1:3
        wk = winkel{k};
        fprintf('%-6s %+9.3f %+9.3f %+9.3f %9.3f   Grad\n', ...
                wName{k}, wk(end), min(wk), max(wk), max(wk) - min(wk));
    end

    %% 4) Drift fuer die drei Varianten
    axV = {ax, ax - mean(ax(ruhe0)), ax - mean(ax)};
    ayV = {ay, ay - mean(ay(ruhe0)), ay - mean(ay)};
    fehler  = zeros(n, 3);   % Positionsfehler
    vfehler = zeros(n, 3);   % Geschwindigkeitsfehler
    for k = 1:3
        vx = cumtrapz(t, axV{k});           % 1. Integration: a -> v
        vy = cumtrapz(t, ayV{k});
        x  = cumtrapz(t, vx);               % 2. Integration: v -> x
        y  = cumtrapz(t, vy);
        vfehler(:, k) = hypot(vx, vy);      % Betrag der Geschwindigkeit
        fehler(:, k)  = hypot(x, y);        % Abstand vom Startpunkt
    end

        fprintf('\nGeschwindigkeitsfehler [m/s]:\n%-28s', 'Variante');
    fprintf('%10s', compose("%g s", zeitpunkte)); fprintf('%10s%10s\n', 'Ende', 'Max');
    for k = 1:3
        fprintf('%-28s', namen{k});
        for z = zeitpunkte
            if t(end) >= z
                fprintf('%10.4f', vfehler(find(t >= z, 1), k));
            else
                fprintf('%10s', '-');
            end
        end
        fprintf('%10.4f%10.4f\n', vfehler(end, k), max(vfehler(:, k)));
    end

    fprintf('\nPositionsfehler [m]:\n%-28s', 'Variante');
    fprintf('%10s', compose("%g s", zeitpunkte)); fprintf('%10s\n', 'Ende');

    for k = 1:3
        fprintf('%-28s', namen{k});
        for z = zeitpunkte
            if t(end) >= z
                wert = fehler(find(t >= z, 1), k);
                fprintf('%10.3f', wert);
            else
                wert = NaN;
                fprintf('%10s', '-');
            end
        end
        fprintf('%10.3f\n', fehler(end, k));
    end

    %% 5) Abbildungen je Datei und Sensor
    figure('Name', [sName ': ' dateien{d}]);

    subplot(3,1,1);
    plot(t, ax); grid on;
    ylabel('a in m/s^2'); legend('a_x');
    title(['Rohdaten in Ruhe, ' sName], 'Interpreter', 'none');

    subplot(3,1,2);
    plot(t, ay, 'Color', [0.85 0.33 0.10]); grid on;
    ylabel('a in m/s^2'); legend('a_y');
    title(['Rohdaten in Ruhe, ' sName], 'Interpreter', 'none');

    subplot(3,1,3);
    semilogy(t, fehler); grid on;
    xlabel('t in s'); ylabel('Positionsfehler in m');
    legend(namen, 'Location', 'southeast');
    title(['Drift bei ruhendem Sensor (logarithmische Achse), ' sName]);

    %% Verteilung der Rohdaten
    figure('Name', ['Verteilung ' sName ': ' dateien{d}]);
    aV = {ax, ay};  aName = {'a_x', 'a_y'};
    for k = 1:2
        subplot(1,2,k);
        histogram(aV{k}, 'BinWidth', 1/256); hold on; grid on;
        xline(mean(aV{k}), 'k-', 'LineWidth', 1.5);
        xlabel([aName{k} ' in m/s^2']); ylabel('Anzahl');
        title(sprintf('%s: Verteilung %s (Mittelwert = %.4f m/s^2)', sName, aName{k}, mean(aV{k})));
        legend('Messwerte', 'Mittelwert');
    end

        %% 6) Abbildung Orientierung je Datei
    figure('Name', ['Orientierung ' sName ': ' dateien{d}]);
    for k = 1:3
        subplot(3,1,k);
        plot(t, winkel{k}); hold on;
        yline(aufl*[-2 -1 1 2], ':', 'Color', [0.5 0.5 0.5]);   % Aufloesungsstufen
        grid on; ylabel(['\Delta ' wName{k} ' in Grad']);
        if k == 1
            title(['Orientierung in Ruhe, ' sName ' (gepunktet: Aufloesungsstufen): ' dateien{d}], ...
                  'Interpreter', 'none');
        end
    end
    xlabel('t in s');

    end   % NEU: Ende Schleife ueber die Sensoren
end