

%%

close all; clear all;

% mandrill

figure();
hold on;

entropy = [5.540663 5.537652 5.529445 5.476813 5.178717 4.265658 2.49727];
psnr = [39.688141 38.638695 38.60318 38.26778 32.315872 21.209816 13.428112];

plot(entropy, psnr);

% mona lisa

entropy = [4.441130 4.420294 4.417159 4.377809 4.091026 3.184715 1.475442];
psnr = [40.627651 39.267334 38.234413 34.163448 24.817711 16.815660 11.832013];

plot(entropy, psnr, 'r');

xlabel('entropy');
ylabel('PSNR');
legend('mandrill', 'mona lisa');


% quality: 0.000000
% PSNR: 40.627651
% entropy: 4.441130
% 
% quality: 0.100000
% PSNR: 39.267334
% entropy: 4.420294
% 
% quality: 0.200000
% PSNR: 38.234413
% entropy: 4.417159
% 
% quality: 0.300000
% PSNR: 34.163448
% entropy: 4.377809
% 
% quality: 0.500000
% PSNR: 24.817711
% entropy: 4.091026
% 
% quality: 0.750000
% PSNR: 16.815660
% entropy: 3.184715
% 
% quality: 1.000000
% PSNR: 11.832013
% entropy: 1.475442
