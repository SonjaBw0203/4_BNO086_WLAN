%% Vergleich BLE- und USB-Log anhand der Zeitstempel
% erst BLE, dann serialMonitor Log
clear; clc; close all;
[fb, pb] = uigetfile('*.log', '1. WLAN-Log waehlen');
[fu, pu] = uigetfile('*.log', '2. Serial-Monitor-Log waehlen');
B = lies(fullfile(pb, fb));
U = lies(fullfile(pu, fu));

U  = U(U(:,1) >= B(1,1) & U(:,1) <= B(end,1), :);   % nur gemeinsamer Zeitraum
da = ismember(U(:,1), B(:,1));                      % Zeitstempel auch im BLE-Log?
t  = (U(:,1) - U(1,1)) / 1e6;

figure('Name', 'Vergleich WLAN / USB');
plot(t, U(:,2), '-', 'Color', [0.7 0.7 0.7]); hold on; grid on;
plot(t(da), U(da,2), 'b.');
plot(t(~da), U(~da,2), 'rx', 'MarkerSize', 4);
xlabel('t in s'); ylabel('a_x BNO1 in m/s^2');
legend('USB (vollstaendig)', 'ueber WLAN angekommen', 'ueber WLAN fehlend');
title(sprintf('WLAN: %d von %d Zeilen angekommen (%.0f %%)', sum(da), numel(da), 100*mean(da)));

function d = lies(datei)                  % Spalten: Zeitstempel tA1, ax1
txt = readlines(datei);
txt = txt(count(txt, "|") == 5);
w   = str2double(split(extractBefore(txt, " |"), " "));
d   = unique(w(all(~isnan(w), 2), 1:2), 'rows');
end