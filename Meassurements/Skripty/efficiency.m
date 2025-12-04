%% Analýza účinnosti v DC režimu s vyhlazením regresí
clear; close all; clc;

% Data z tabulky
U_vstup = [12, 31]; % Vstupní napětí [V]
R_zatez = 1.2; % Zátěžový odpor [ohm]

% Naměřená data [1A, 4A, 8A]
I_nastaveno = [1.000, 4.000, 8.000]; % Nastavený proud [A]

% Data pro 12V
I_vyst_12V = [0.897, 3.925, 8.060]; % Výstupní proud RMS [A]
P_vyst_12V = [0.966, 18.487, 77.956]; % Výstupní výkon [W]
P_vstup_12V = [1.988, 22.280, 88.800]; % Vstupní výkon [W]
ucinnost_12V = [48.568, 82.975, 87.789]; % Účinnost [%]

% Data pro 31V
I_vyst_31V = [0.896, 4.300, 8.360]; % Výstupní proud DC [A]
P_vyst_31V = [0.963, 22.188, 83.868]; % Výstupní výkon [W]
P_vstup_31V = [2.333, 27.000, 91.500]; % Vstupní výkon [W]
ucinnost_31V = [41.294, 82.178, 91.658]; % Účinnost [%]

%% Vytvoření vyhlazených křivek pomocí regrese
I_interp = linspace(min(I_nastaveno), max(I_nastaveno), 100); % Jemná mřížka pro hladké křivky

% Vyhlazovací funkce (kubická interpolace)
vyhladit_krivku = @(x, y) interp1(x, y, I_interp, 'pchip');

% Vyhlazení všech křivek
ucinnost_12V_vyhlazena = vyhladit_krivku(I_nastaveno, ucinnost_12V);
ucinnost_31V_vyhlazena = vyhladit_krivku(I_nastaveno, ucinnost_31V);

P_vyst_12V_vyhlazena = vyhladit_krivku(I_nastaveno, P_vyst_12V);
P_vyst_31V_vyhlazena = vyhladit_krivku(I_nastaveno, P_vyst_31V);

P_vstup_12V_vyhlazena = vyhladit_krivku(I_nastaveno, P_vstup_12V);
P_vstup_31V_vyhlazena = vyhladit_krivku(I_nastaveno, P_vstup_31V);

%% Graf závislosti účinnosti na proudu
figure('Name', 'Účinnost v DC režimu', 'Position', [100 100 800 600], 'Color', 'w');

hold on; grid on; box on;
plot(I_interp, ucinnost_12V_vyhlazena, '-', 'LineWidth', 2, 'Color', [0 0.4470 0.7410]);
plot(I_interp, ucinnost_31V_vyhlazena, '-', 'LineWidth', 2, 'Color', [0.8500 0.3250 0.0980]);
scatter(I_nastaveno, ucinnost_12V, 70, 'filled', 'MarkerFaceColor', [0 0.4470 0.7410]);
scatter(I_nastaveno, ucinnost_31V, 70, 'filled', 'MarkerFaceColor', [0.8500 0.3250 0.0980]);
title('Závislost účinnosti na výstupním proudu');
xlabel('Výstupní proud [A]'); ylabel('Účinnost [%]');
legend({'12V vstup', '31V vstup'}, 'Location', 'southeast');
set(gca, 'FontSize', 12, 'GridAlpha', 0.3);
ylim([30 100]);

%% Uložení grafů
saveas(gcf, '../Grafy/Analyza_ucinnosti_DC.png');
saveas(gcf, 'Analyza_ucinnosti_DC.fig');

disp('Analýza účinnosti v DC režimu dokončena a uložena.');