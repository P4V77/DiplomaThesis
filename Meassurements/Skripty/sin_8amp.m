% =============================================
% MATLAB SCRIPT: IMPORT CSV AND PLOT DATA (1000 vzorků + peaky)
% =============================================

%% 1. Load CSV File
filename = '~/modular-current-source/Meassurements/Mereni/SQ8A12V.CSV';
% filename = '~/modular-current-source/Meassurements/Mereni/SIN8A12.CSV';
hasHeader = true;

if hasHeader
    data = readtable(filename);
else
    data = readtable(filename, 'ReadVariableNames', false);
end

disp('First 5 rows of data:');
disp(head(data, 5));

%% 2. Extract Data Columns
if hasHeader
    xData = data.inHz;           % Frekvence v Hz
    yData = data.SpectrumInDBA;  % Hladina v dBA
else
    xData = data{:, 1};
    yData = data{:, 2};
end

%% 3. Limit to first 1000 samples
numSamples = min(1000, height(data));
xData = xData(1:numSamples);
yData = yData(1:numSamples);

%% 4. Find Two Largest Peaks
[sortedVals, sortedIdx] = maxk(yData, 1);    % Najdi 2 nejvyšší hodnoty
peakFrequencies = xData(sortedIdx);
peakValues = yData(sortedIdx);

fprintf('Peak 1: %.2f dBA at %.2f Hz\n', peakValues(1), peakFrequencies(1));
% fprintf('Peak 2: %.2f dBA at %.2f Hz\n', peakValues(2), peakFrequencies(2));

%% 5. Plot the Data
figure(1);
plot(xData, yData, ...
    'LineWidth', 2, ...
    'Color', [0 0.5 0.8], ...
    'Marker', 'o', ...
    'MarkerSize', 4);
hold on;

% Mark the peaks
plot(peakFrequencies, peakValues, 'r*', 'MarkerSize', 16, 'LineWidth', 2);
text(peakFrequencies + 5, peakValues, ...
    arrayfun(@(f,v) sprintf('%.1f Hz\n%.1f dBA', f, v), peakFrequencies, peakValues, 'UniformOutput', false), ...
    'FontSize', 10, 'Color', 'red');

xlabel('Frekvence [Hz]', 'FontSize', 12);
ylabel('Hladina [dBA]', 'FontSize', 12);
title('Spektrum - prvních 1000 vzorků', 'FontSize', 14);
grid on;
set(gca, 'FontSize', 11);

% saveas(gcf, '/home/pavel/modular-current-source/LaTeX/kapitola5/Figures/sin10hz_8amp_12V.png');
saveas(gcf, '/home/pavel/modular-current-source/LaTeX/kapitola5/Figures/SQ10HZ_8A_12V.png');
disp('Plot generated successfully!');

