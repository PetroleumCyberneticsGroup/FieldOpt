mrstPath = '/home/einar/Documents/MATLAB/mrst-2013a/';
run(fullfile(mrstPath,'startup.m'))

% change controls
i1 = [200, 200]*meter^3/day;
p1 = [200, 200]*barsa;
u  = [i1; p1];

try
 %write control file:
 fid = fopen('SIMPLE10x5x10_CONTROLS.TXT', 'w');
 fprintf(fid, '%+12.6e %+12.6e\n',  [i1; p1]);
 fclose(fid);

 runSim2;
catch err
 warning('something went wrong with MRST');
 warning(getReport(err));
 exit(3)
end
warning('Simulation Completed')
quit
