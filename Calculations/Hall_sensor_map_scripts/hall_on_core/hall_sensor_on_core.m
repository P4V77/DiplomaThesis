%%%%% RESOLUTION %%%%%
% Define the desired resolution for the lookup table
h_resolution = 0.01; % mm
I_resolution = 0.05; % A
B_resolution = 0.525; % mV

% Specify the filename and range
filename = 'vibr_hall_drv5053OA_in_coil.xlsx';
range_mV = 'A24:H33';

% Read the specified range from the XLSX file
data_BmV = readtable(filename, 'Range', range_mV);

% Convert table to arrays for compatibility with interp2
h_values = data_BmV.Var1(2:end);           % Assuming Var1 holds h values
I_values = data_BmV{1, 2:end};             % Assuming the first row after header holds I values
B_values = data_BmV{2:end, 2:end};         % B data matrix

% Create a grid of h and I values within the range of h_values and I_values
h_grid = 0:h_resolution:8;
I_grid = -2:I_resolution:2;

% Perform 2D interpolation with compatible ranges
[B_interp] = interp2(I_values, h_values, B_values, I_grid', h_grid, 'linear');

% Define the desired B and I ranges for the new table
B_target_grid = min(B_values(:)):B_resolution:max(B_values(:)); % Adjust the step size as needed
I_target_grid = I_grid;  % Assuming you want the same I range as in your original data

% Preallocate the new lookup table for h values
h_interp_array = zeros(length(B_target_grid), length(I_target_grid));

% Loop over each target B and I pair to find the corresponding h
for i = 1:length(B_target_grid)
    for j = 1:length(I_target_grid)
        % Set the target B and I for this cell
        B_target = B_target_grid(i);
        I_target = I_target_grid(j);
        
        % Find the closest index in the original I_grid
        [~, I_index] = min(abs(I_grid - I_target));

        % Extract the corresponding B values column for this I
        B_column = B_interp(:, I_index);

        % Interpolate to find the h value for the target B within this column
        h_interp_array(i, j) = interp1(B_column, h_grid, B_target, 'linear', 'extrap');
    end
end

% Create a new figure for the plot
figure('Units', 'centimeters', 'Position', [0 0 15 10]);

% Display the h_interp_array as a heatmap
imagesc(I_target_grid, B_target_grid, h_interp_array);

% Add color bar to show the scale of h values
c = colorbar;
c.Label.String = 'Výška (h) [mm]';

% Set labels and title in Czech
xlabel('Proud (I) [A]');
ylabel('Magnetické pole (B) [mV]');
title('Interpolované hodnoty polohy jádra');

% Adjust the color map and axis
colormap(jet);
axis xy;

% Set the output path and save as EPS
output_path = '~/modular-current-source/LaTeX/kapitola6/Figures/hall_on_core_map.eps';

% Ensure the directory exists
[filepath,~,~] = fileparts(output_path);
if ~exist(filepath, 'dir')
    mkdir(filepath);
end

% Save the figure in EPS format
print(gcf, '-depsc', output_path);
disp(['Graf byl uložen jako: ' output_path]);