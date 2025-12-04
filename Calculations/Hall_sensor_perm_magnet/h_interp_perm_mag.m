% Načtení dat z Excelu
filename = 'meassurement.xlsx'; % Název Excel souboru
data = readtable(filename);

% Extrakce sloupců s výškou a napětím
heights = str2double(data{:, 1}); % Výška v prvním sloupci
voltages = str2double(data{:, 2}); % Napětí ve druhém sloupci

% Interpolace pro vytvoření funkce výšky
interpFunc = @(V) interp1(voltages, heights, V, 'makima'); 

% Definice rozsahu napětí pro LUT
range = 500;
voltage_range = linspace(min(voltages), max(voltages), range);
resolution = (max(voltages)-min(voltages))/range;

% Interpolace výšek
h_interp = arrayfun(interpFunc, voltage_range);

% Vytvoření a uložení hlavičkového souboru
relative_path = "h_interp.hpp";
fileID = fopen(relative_path, 'w');

fprintf(fileID, '#ifndef    H_INTERP_HPP\n');
fprintf(fileID, '#define H_INTERP_HPP\n\n');
fprintf(fileID, 'namespace position_LUT\n{\n\n');
fprintf(fileID, 'constexpr float lut_voltage_min = %ff; \n',min(voltages));
fprintf(fileID, 'constexpr float lut_voltage_max = %ff; \n',max(voltages));
fprintf(fileID, 'constexpr float lut_voltage_resolution = %ff; \n',resolution);
fprintf(fileID, 'constexpr uint16_t lut_voltage_range = %d; \n',range-1);

fprintf(fileID, 'const float LUT[] = {\n');

for i = 1:length(h_interp)
    fprintf(fileID, '    %ff,\n', h_interp(i));
end

fprintf(fileID, '};\n}\n\n');
fprintf(fileID, '#endif // H_INTERP_HPP\n');
fclose(fileID);

disp('Look-up tabulka uložena jako h_interp.hpp');

% Vytvoření grafů s českými popisky
figure(1)
plot(heights, voltages, 'b-', 'LineWidth', 1.5);
xlabel('Výška [mm]');
ylabel('Napětí [V]');
title('Naměřená závislost napětí Hallovy sondy na výšce magnetu');
grid on;

% Uložení prvního grafu
output_path1 = '~/modular-current-source/LaTeX/kapitola6/Figures/hall_perm_magnet1.eps';
print(gcf, '-depsc', output_path1);

figure(2)
plot(voltage_range, h_interp, 'r-', 'LineWidth', 1.5);  
xlabel('Napětí [V]');
ylabel('Výška [mm]');
title('Interpolovaná závislost výšky magnetu na napětí');
grid on;

% Uložení druhého grafu
output_path2 = '~/modular-current-source/LaTeX/kapitola6/Figures/hall_perm_magnet2.eps';
print(gcf, '-depsc', output_path2);

disp(['Grafy uloženy jako: ' output_path1 ' a ' output_path2]);