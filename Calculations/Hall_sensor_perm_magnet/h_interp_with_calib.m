% Import dat z Excelu
filename = 'meassurement.xlsx'; % Název vašeho Excel souboru
data = readtable(filename);

% Extrakce sloupců s výškou a napětím
heights = str2double(data{:, 1}); % Výška je v prvním sloupci
voltages = str2double(data{:, 2}); % Napětí je v druhém sloupci

% Interpolace pro vytvoření funkce výšky
interpFunc = @(V) interp1(voltages, heights, V, 'makima'); 

% Definice rozsahu napětí pro LUT
range = 500;
voltage_range = linspace(min(voltages), max(voltages), range);
resolution = (max(voltages) - min(voltages)) / range;

% Interpolace výšek pomocí rozsahu napětí
h_interp = arrayfun(interpFunc, voltage_range);

% Proložení polynomem 2. řádu
p = polyfit(voltages, heights, 2);

% Generování h_interp z proloženého polynomu
h_interp = polyval(p, voltage_range);

% Vytvoření grafu
figure('Units', 'centimeters', 'Position', [0 0 15 10]);
hold on;
plot(voltage_range, h_interp, 'b-', 'LineWidth', 1.5);
grid on;

% České popisky
xlabel('Napětí Hallovy sondy [mV]');
ylabel('Výška magnetu [mm]');
title('Závislost výšky magnetu na výstupním napětí Hallovy sondy');

% Uložení grafu
output_path = '~/modular-current-source/LaTeX/kapitola5/Figures/hall_perm_magnet.eps';

% Zajištění existence adresáře
[filepath,~,~] = fileparts(output_path);
if ~exist(filepath, 'dir')
    mkdir(filepath);
end

% Uložení ve formátu EPS
print(gcf, '-depsc', output_path);
disp(['Graf byl uložen jako: ' output_path]);