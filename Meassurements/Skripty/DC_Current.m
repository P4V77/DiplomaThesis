%% Komplexní analýza DC charakteristik zdroje - Samostatné grafy
clear; close all; clc;

% ==============================================
% 1. Načtení dat z Excelu - DC Current měření
% ==============================================
sheet_dc = 'DC Current';
filename = '/home/pavel/modular-current-source/Meassurements/Mereni/MoSeZ_meassurements.xlsx';
data_dc = readtable(filename, 'Sheet', sheet_dc);

% Vstupní data
nastaveny_proud = [-8 -4 -1 -0.1 0.1 1 4 8];

% Výstupní proudy
Iout_12V_1R = data_dc{3,3:10};
Iout_48V_1R = data_dc{10,3:10};
Iout_48V_3R = data_dc{17,3:10};

% Zvlnění (peak-to-peak)
Iripple_12V_1R = data_dc{4,3:10};
Iripple_48V_1R = data_dc{11,3:10};
Iripple_48V_3R = data_dc{18,3:10};

% Zvlnění v procentech
rip_perc_12V_1R = data_dc{5,3:10};
rip_perc_48V_1R = data_dc{12,3:10};
rip_perc_48V_3R = data_dc{19,3:10};

% IRMS
irms_12V_1R = data_dc{6,3:10};
irms_48V_1R = data_dc{13,3:10};
irms_48V_3R = data_dc{20,3:10};

% ==============================================
% 2. Načtení dat pro zátěžové charakteristiky
% ==============================================
% Data pro 12V
Uzat_12V_1A = [3.22 2.61 1.9552 1.24 0.75 0.4 0.1 0.0717];
Uzat_12V_8A = [7.2776 5.872 5.376 4.5 3.4 1.558 0.9254 0.4839];
Ivy_12V_1A = [1.135 1.173 1.225 1.268 1.315 1.356 1.373 1.367];
Ivy_12V_8A = [8.23 8.6 8.69 8.81 8.89 9.06 9.12 9.17];

% Data pro 48V
Uzat_48V_1A = [2.45 2.046 1.61 1.29 0.9628 0.5598 0.1444 0.042];
Uzat_48V_8A = [22.034 17.633 14.442 11.469 8.6 5.798 1.4188 0.4];
Ivy_48V_1A = [0.9 0.9 0.91 0.92 0.942 0.942 0.958 0.951];
Ivy_48V_8A = [8.02 8.05 8.09 8.29 8.55 8.91 9.37 9.46];

% Odpory zátěže pro různé případy
R_load = [3.22/1.135, 2.61/1.173, 1.9552/1.225, 1.24/1.268, 0.75/1.315, 0.4/1.356, 0.1/1.373, 0.0717/1.367];

% ==============================================
% 3. Vytvoření samostatných grafů
% ==============================================
barvy = lines(4); % Barevné schéma

%% Graf 1: Výstupní charakteristika
figure(1);
set(gcf, 'Name','Výstupní charakteristika','Position',[100 100 800 600],'Color','w');
hold on; grid on; box on;

% Data sady
datasets = {
    Iout_12V_1R, '--', barvy(1,:), '12V / 1Ω';
    Iout_48V_1R, '--', barvy(2,:), '48V / 1Ω';
    Iout_48V_3R, '--', barvy(3,:), '48V / 3Ω'
};

for i = 1:size(datasets, 1)
    y = datasets{i,1};
    plot(nastaveny_proud, y, '-o', 'LineWidth', 2, 'Color', datasets{i,3}, 'MarkerSize', 8);
    
    % Lineární regrese
    [fit, coeffs, R2] = linear_regression(nastaveny_proud, y);
    plot(nastaveny_proud, fit, datasets{i,2}, 'Color', datasets{i,3}, 'LineWidth', 1.5);
end

xlabel('Nastavený proud [A]', 'FontSize', 11);
ylabel('Výstupní proud [A]', 'FontSize', 11);
title('Výstupní charakteristika zdroje', 'FontSize', 12, 'FontWeight','bold');
legend({'12V/1Ω','Regrese','48V/1Ω','Regrese','48V/3Ω','Regrese'}, 'Location', 'northwest');
set(gca, 'FontSize', 10, 'GridAlpha', 0.3);

% Uložení
saveas(gcf, '/home/pavel/modular-current-source/LaTeX/kapitola5/Figures/Graf1_Vystupni_charakteristika.png');
saveas(gcf, '/home/pavel/modular-current-source/LaTeX/kapitola5/Figures/Graf1_Vystupni_charakteristika.fig');

%% Graf 2: Zátěžová charakteristika - proud
figure(2);
set(gcf, 'Name','Zátěžová charakteristika - proud','Position',[100 100 800 600],'Color','w');
hold on; grid on; box on;

% 12V - 1A
plot(R_load, Ivy_12V_1A, '-s', 'Color', barvy(1,:), 'LineWidth', 2, 'MarkerSize', 8);
% 12V - 8A
plot(R_load, Ivy_12V_8A, '-o', 'Color', barvy(2,:), 'LineWidth', 2, 'MarkerSize', 8);
% 48V - 1A
plot(R_load, Ivy_48V_1A, '-^', 'Color', barvy(3,:), 'LineWidth', 2, 'MarkerSize', 8);
% 48V - 8A
plot(R_load, Ivy_48V_8A, '-d', 'Color', barvy(4,:), 'LineWidth', 2, 'MarkerSize', 8);

xlabel('Odpor zátěže [Ω]', 'FontSize', 11);
ylabel('Výstupní proud [A]', 'FontSize', 11);
title('Zátěžová charakteristika - proud', 'FontSize', 12, 'FontWeight','bold');
legend({'12V/1A','12V/8A','48V/1A','48V/8A'}, 'Location', 'northeast');
set(gca, 'FontSize', 10, 'GridAlpha', 0.3, 'XScale', 'log');

% Uložení
saveas(gcf, '/home/pavel/modular-current-source/LaTeX/kapitola5/Figures/Graf2_Zatezova_char_proud.png');
saveas(gcf, '/home/pavel/modular-current-source/LaTeX/kapitola5/Figures/Graf2_Zatezova_char_proud.fig');

%% Graf 3: Zvlnění proudu
figure(3);
set(gcf, 'Name','Relativní zvlnění proudu','Position',[100 100 800 600],'Color','w');
hold on; grid on; box on;

datasets = {
    rip_perc_12V_1R, '--', barvy(1,:), '12V/1Ω';
    rip_perc_48V_1R, '--', barvy(2,:), '48V/1Ω';
    rip_perc_48V_3R, '--', barvy(3,:), '48V/3Ω'
};

for i = 1:size(datasets,1)
    y = datasets{i,1};
    plot(nastaveny_proud, y, '-o', 'LineWidth', 2, 'Color', datasets{i,3}, 'MarkerSize', 8);

    % [fit, coeffs, R2] = linear_regression(nastaveny_proud, y);
    % plot(nastaveny_proud, fit, datasets{i,2}, 'Color', datasets{i,3}, 'LineWidth', 1.5);
end

xlabel('Nastavený proud [A]', 'FontSize', 11);
ylabel('Zvlnění [%]', 'FontSize', 11);
title('Relativní zvlnění výstupního proudu', 'FontSize', 12, 'FontWeight','bold');
legend({'12V/1Ω','Regrese','48V/1Ω','Regrese','48V/3Ω','Regrese'}, 'Location', 'northwest');
set(gca, 'FontSize', 10, 'GridAlpha', 0.3);

% Uložení
saveas(gcf, '/home/pavel/modular-current-source/LaTeX/kapitola5/Figures/Graf3_Zvlneni_proudu.png');
saveas(gcf, '/home/pavel/modular-current-source/LaTeX/kapitola5/Figures/Graf3_Zvlneni_proudu.fig');

%% Graf 4: Zátěžová charakteristika - napětí
figure(4);
set(gcf, 'Name','Zátěžová charakteristika - napětí','Position',[100 100 800 600],'Color','w');
hold on; grid on; box on;

% 12V - 1A
plot(R_load, Uzat_12V_1A, '-s', 'Color', barvy(1,:), 'LineWidth', 2, 'MarkerSize', 8);
% 12V - 8A
plot(R_load, Uzat_12V_8A, '-o', 'Color', barvy(2,:), 'LineWidth', 2, 'MarkerSize', 8);
% 48V - 1A
plot(R_load, Uzat_48V_1A, '-^', 'Color', barvy(3,:), 'LineWidth', 2, 'MarkerSize', 8);
% 48V - 8A
plot(R_load, Uzat_48V_8A, '-d', 'Color', barvy(4,:), 'LineWidth', 2, 'MarkerSize', 8);

xlabel('Odpor zátěže [Ω]', 'FontSize', 11);
ylabel('Napětí na zátěži [V]', 'FontSize', 11);
title('Zátěžová charakteristika - napětí', 'FontSize', 12, 'FontWeight','bold');
legend({'12V/1A','12V/8A','48V/1A','48V/8A'}, 'Location', 'northeast');
set(gca, 'FontSize', 10, 'GridAlpha', 0.3, 'XScale', 'log');

% Uložení
saveas(gcf, '/home/pavel/modular-current-source/LaTeX/kapitola5/Figures/Graf4_Zatezova_char_napeti.png');
saveas(gcf, '/home/pavel/modular-current-source/LaTeX/kapitola5/Figures/Graf4_Zatezova_char_napeti.fig');

%% Graf 5: Efektivní hodnota proudu (IRMS)
figure(5);
set(gcf, 'Name','Efektivní hodnota proudu','Position',[100 100 800 600],'Color','w');
hold on; grid on; box on;

datasets = {
    irms_12V_1R, '--', barvy(1,:), '12V/1Ω';
    irms_48V_1R, '--', barvy(2,:), '48V/1Ω';
    irms_48V_3R, '--', barvy(3,:), '48V/3Ω'
};

for i = 1:size(datasets,1)
    y = datasets{i,1};
    plot(nastaveny_proud, y, '-o', 'LineWidth', 2, 'Color', datasets{i,3}, 'MarkerSize', 8);
    
    % [fit, coeffs, R2] = linear_regression(nastaveny_proud, y);
    % plot(nastaveny_proud, fit, datasets{i,2}, 'Color', datasets{i,3}, 'LineWidth', 1.5);
end

xlabel('Nastavený proud [A]', 'FontSize', 11);
ylabel('I_{RMS} [A]', 'FontSize', 11);
title('Efektivní hodnota výstupního proudu', 'FontSize', 12, 'FontWeight','bold');
legend({'12V/1Ω','Regrese','48V/1Ω','Regrese','48V/3Ω','Regrese'}, 'Location', 'northwest');
set(gca, 'FontSize', 10, 'GridAlpha', 0.3);

% Uložení
saveas(gcf, '/home/pavel/modular-current-source/LaTeX/kapitola5/Figures/Graf5_IRMS.png');
saveas(gcf, '/home/pavel/modular-current-source/LaTeX/kapitola5/Figures/Graf5_IRMS.fig');

%% Pomocná funkce pro lineární regresi
function [fit_y, coeffs, R2] = linear_regression(x, y)
    coeffs = polyfit(x, y, 1);
    fit_y = polyval(coeffs, x);
    SS_res = sum((y - fit_y).^2);
    SS_tot = sum((y - mean(y)).^2);
    R2 = 1 - SS_res / SS_tot;
end